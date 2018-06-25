#include "uartReader.h"
#include <QtMath>
#include <QDateTime>
#include <thread>
#include <chrono>
#include <memory>
#define REAL_DEVICE

uartReader::uartReader(QObject *parent) : QObject(parent),
    firstLine(true), serviceMode(false), isPortOpen(false),
    m_serNumber(-1), deviceInSleepMode(false)
{
    qRegisterMetaType<QtCharts::QAbstractSeries*>();
    qRegisterMetaType<QtCharts::QAbstractAxis*>();
    documentsPath = QDir::homePath()+QString("/Documents/");
    device = new QSerialPort(this);
    connect(device, &QSerialPort::readyRead, this, &uartReader::readData);
    axisYRange = QPointF(100000,0);
}

uartReader::~uartReader()
{
    qDebug() << "analizer destructor";
    if (device != NULL)
    {
        device->disconnect();
        delete device;
        qDebug() << "call disconnect port";
    }
    if(logFile->isOpen())
        logFile->close();
}

void uartReader::initDevice(QString port, QString baudRate,
                            QVariantList propertiesNames_,
                            QVariantList propertiesValues_)
{
#ifndef REAL_DEVICE
    qDebug() << "Selected port: " << port;
    qDebug() << "Selected baudRate: " << baudRate;
    emit makeSeries();
    updateProperties(propertiesNames_, propertiesValues_);
#else
    device->setPortName(port);
    device->setBaudRate(baudRate.toInt());
    device->setDataBits(QSerialPort::Data8);
    device->setParity(QSerialPort::NoParity);
    device->setStopBits(QSerialPort::OneStop);
    device->setFlowControl(QSerialPort::NoFlowControl);
    if(device->open(QIODevice::ReadWrite)){
        qDebug() << "Connected to: " << device->portName();
        device->write("i");
        device->setDataTerminalReady(true);
        device->setRequestToSend(false);
//        while( serNumber == -1 ) {};
        emit sendDebugInfo("Connected to: " + device->portName());
        emit makeSeries();
        logFile = std::make_shared<QFile>(QString(documentsPath
                                                  +"/log_"
                                                  +QDateTime::currentDateTime().toString("yyyyMMdd_hhmm")
                                                  +".txt"));
        if (!logFile->open(QIODevice::WriteOnly | QIODevice::Text))
            emit sendDebugInfo("Cannot create logfile", 2000);
        updateProperties(propertiesNames_, propertiesValues_);
    }
    else {
        qDebug() << "Can't open port" << port;
        emit sendDebugInfo("Can't open port" + port, 2000);
    }
#endif
}

void uartReader::updateProperties(QVariantList propertiesNames_,
                                  QVariantList propertiesValues_)
{
//    qDebug() << properties_;
    //parse properties
    QString command;
    //send period
    command = QString("%1=%2\r").arg(propertiesNames_[0].toString())
                                 .arg(propertiesValues_[0].toInt(), 2,10, QChar('0'));
    qDebug() << "updateProperties: " + command;
    prepareCommandToSend(command);
    //send coeffs
    for(int i=1; i< propertiesValues_.size(); i++)
    {
        double koef_1=0.0, koef_2=0.0;
        koef_2 = std::modf(propertiesValues_[i].toDouble(), &koef_1);
        int k1 = koef_1;
        int k2 = koef_2 * 100000;

        command = QString("%1=%2.%3\r").arg(propertiesNames_[i].toString())
                                     .arg(k1)
                                     .arg(QString::number(k2).leftJustified(5,QChar('0')));
        prepareCommandToSend(command);
        qDebug() << "updateProperties: " + command;
        //TODO: implement sleepFlag and separate write method
//        device->write(command);
    }
}

void uartReader::getListOfPort()
{
    ports.clear();
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts()){
        ports.push_back(info.portName());
        emit sendPortName(info.portName());
        qDebug() << info.portName();
    }
    std::stringstream ss;
    ss << "Found " << ports.size() << " ports";
    emit sendDebugInfo(QString(ss.str().c_str()), 100);
}

void uartReader::readData()
{
    qDebug() << "in readdata";
    //manually readData from file
#ifndef REAL_DEVICE
    std::thread( [&] () {
        QFile input("D:/projects/ascii.log");
        if( !input.open(QIODevice::ReadOnly | QIODevice::Text) )
        {
            qDebug() << "Cannot open file!";
            return;
        }
        int numLinesReaded=0;
        while(!input.atEnd() || numLinesReaded < 10 )
        {
            auto t1 = std::chrono::high_resolution_clock::now();
            deviceInSleepMode=false;
            processLine(input.readLine());
            if(++numLinesReaded %4 ==0)
            {
                auto t2 = std::chrono::high_resolution_clock::now();
                std::chrono::milliseconds delay(1000);
                std::chrono::milliseconds delaySleepMode(100);
                while( (std::chrono::duration<double, std::milli>(t2 - t1) < delay) )
                {
                    t2 = std::chrono::high_resolution_clock::now();
                    if (std::chrono::duration<double, std::milli>(t2 - t1) > delaySleepMode)
                        deviceInSleepMode=true;
                }
            }
        }
    });
#else
    while (device->canReadLine()) processLine(device->readLine());
#endif
}

void uartReader::prepareCommandToSend(QString cmd_)
{
    qDebug() << "prepareCommandToSend: " + cmd_;
    if (cmd_.length()>0)
        m_queueCommandsToSend.push_back(cmd_);
    if(!deviceInSleepMode) { sendDataToDevice(); } //immediately send command
}

void uartReader::sendSeriesPointer(QtCharts::QAbstractSeries* series_
                                  , QtCharts::QAbstractAxis* Xaxis_)
{
    qDebug() << "sendSeriesPointer: " << series_;
    m_series = series_;
    m_axisX = Xaxis_;
    qDebug() << "sendSeriesPointer: " << m_series;
}

void uartReader::selectPath(QString pathForSave)
{
    qDebug() << documentsPath;
    documentsPath = pathForSave;
    qDebug() << pathForSave;
    qDebug() << documentsPath;
}

void uartReader::sendDataToDevice()
{
#ifdef REAL_DEVICE
    if(device->isOpen())
#endif
    {
        while (!m_queueCommandsToSend.empty() && !deviceInSleepMode)
        {
            QString cmd_ = m_queueCommandsToSend[0];
#ifdef REAL_DEVICE
            device->write(cmd_.toStdString().c_str());
#endif
            m_queueCommandsToSend.pop_front();
            emit sendDebugInfo(QString("Send: ") + cmd_);
            qDebug() << "sendDataToDevice: " + cmd_;
        }
    }
    else
    {
        emit sendDebugInfo("Device disconnected or in measurement mode");
    }
}

void uartReader::update(QPointF p)
{
    if (m_series) {
        QtCharts::QXYSeries *xySeries =
                static_cast<QtCharts::QXYSeries *>(m_series);
        // Use replace instead of clear + append, it's optimized for performance
        xySeries->append(p);//replace(lines.value(series));
    }
    if(m_axisX) {
        m_axisX->setMin(p.rx()-180);
        m_axisX->setMax(p.rx());
    }
}

void uartReader::processLine(const QByteArray &_line)
{
//    qDebug() << _line;
    QStringList line;//(_line);
    logFile->write(_line);

    for (auto w : _line.split(','))
    {
        line.append(QString(w));
    }
    //appendDataToseries/writeTofile
    QRegExp concRX("^C=*");
    QRegExp timeRX("^time=*");
    QRegExp endMsgRX("^---*");
    if(line.size()==1)
    {
        QPointF p;
        //TODO:get conc and time
        if( timeRX.indexIn(line.first()) >= 0)
            tempPoint.setX(line.first().right(line.first().length()-5).toInt());
//            qDebug() << "Match Time " << line.first().right(line.first().length()-5).toInt();

        if( concRX.indexIn(line.first()) >= 0)
        {
            deviceInSleepMode = false;
            tempPoint.setY(line.first().right(9).toDouble());
            if( tempPoint.y() < axisYRange.x())
                axisYRange.setX(tempPoint.y());
            if( tempPoint.y() > axisYRange.y())
                axisYRange.setY(tempPoint.y());
//            qDebug() << "Match conc " << line.first().right(9).toDouble();
            emit adjustAxis(axisYRange);
        }

        if( endMsgRX.indexIn(line.first()) >= 0)
        {
            auto t1 = std::chrono::high_resolution_clock::now();
            std::chrono::milliseconds delaySleepMode(100);
            auto delayThread = std::thread([=]() {
                auto t2 = std::chrono::high_resolution_clock::now();
                while( (std::chrono::duration<double, std::milli>(t2 - t1) < delaySleepMode) )
                {
                    t2 = std::chrono::high_resolution_clock::now();
                }
                deviceInSleepMode=true;
            });
            //TODO:send to device
            if(!m_queueCommandsToSend.empty())
                sendDataToDevice();
            update(tempPoint);
            delayThread.join();
        }
//            qDebug() << tempPoint;
    }
    else
    {
        //TODO:work with voltage data
    }

//identity
//    if( line.first().compare("x=i") ==0)
//        identityHandler(line);
////service mode parser
//    if( line.first().compare("x=s") == 0)
//        serviceModeHandler(line);   //parse all comands here
////measure mode
//    if( line.first().compare("x=m\n") == 0)
//        buttonPressHandler(line);
//    if( line.first().compare("x=d") ==0)
//        dataAquisitionHandler(line);
//    if( line.first().compare("x=e\n") ==0)
//        dataProcessingHandler(line);
}

void uartReader::serviceModeHandler(const QStringList &line)
{
//    qDebug() << line.at(1);
//    qDebug() << line.at(1).compare("START");
    if(line.at(1).compare("START") == 0)    //create file
    {
        std::time_t  t = time(0);
        struct std::tm * now = localtime( & t );
        char buf[200];
        std::strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S_raw.csv", now);
        QString filename(buf);
        diagnosticLog.open(QString(documentsPath+"/"+filename).toStdString(),
                       std::fstream::out);
        if (!diagnosticLog.is_open())
            qDebug()<< "Can't open file!";
        qDebug() << "start file";
    }
    if(line.at(1).compare("END") == 0)      //close file
    {
        if (diagnosticLog.is_open())
            diagnosticLog.close();
        qDebug() << "endfile";
    }
    if(line.at(1).compare("LED") == 0)      //insert line with led wavelenght
    {
        diagnosticLog << "\nLed#" << line.at(2).toStdString()
                      << " (" << line.at(3).toFloat() << " um)\n"
                      << "Signal,Background\n";
        qDebug() << "Led#"<<line.at(2) << " (" <<line.at(3) << "um)";
    }
    if(line.at(1).compare("DATA") == 0)     //insert line with data
    {
        diagnosticLog << std::fixed << line.at(2).toFloat() << ", "
                      << std::fixed << line.at(3).toFloat() << "\n";
        qDebug() << "signal: "<<line.at(2) << ", bgd: " <<line.at(3);
    }
}

void uartReader::identityHandler(const QStringList &line)
{
    if (line.at(1).compare("SERIAL") == 0 )
    {
        qDebug() << "Serial# " << line.at(2).toInt();
        m_serNumber = line.at(2).toInt();
        emit sendSerialNumber(QString("%1").arg(m_serNumber, 4, 16,QChar('0')));
    }
    if(line.at(1).compare("TYPE") ==0 )
    {
        qDebug() << "Type: " << line.at(2).toInt();

    }
    emit sendDebugInfo(QString("Click calibration button to perform device calibration"));
}

void uartReader::dataAquisitionHandler(const QStringList &line)
{
    if (line.size() == 4)
    {
                qDebug() << "data " << line.at(1).toFloat() <<" "
                         <<line.at(3).toFloat();
    }
}

void uartReader::dataProcessingHandler(const QStringList &line)
{

}

void uartReader::buttonPressHandler(const QStringList &line)
{
    qDebug() << "signal from button";
}

















