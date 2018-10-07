#include "uartReader.h"

#include <QtMath>
#include <QDateTime>
#include <QTimer>
#include <thread>
#include <chrono>
#include <ctime>
#include <memory>
#define REAL_DEVICE
//#define SHOW_Debug_

uartReader::uartReader(QObject *parent)
  : QObject(parent), m_serNumber(-1), m_flyMode(false), m_writeToFileOne(false)
  , m_enableLogging(false), isPortOpen(false), firstLine(true)
  , serviceMode(false), deviceInSleepMode(false)
{
    qRegisterMetaType<QtCharts::QAbstractSeries*>();
    qRegisterMetaType<QtCharts::QAbstractAxis*>();
    documentsPath = QDir::homePath()+QString("/Documents/");
    device = new QSerialPort(this);
    connect(device, &QSerialPort::readyRead, this, &uartReader::readData);
    axisYRange.resize(4);
    tempPoint.resize(4);
    for (auto& p: tempPoint)
    {
        p.setX(0.0);
        p.setY(0.0);
    }
    for(auto& p : axisYRange)
        p = QPointF(100000,-100000);
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
    if(logFile != NULL)
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
//        logFile = std::make_shared<QFile>(QString(documentsPath
//                                                  +"/log_"
//                                                  +QDateTime::currentDateTime().toString("yyyyMMdd_hhmm")
//                                                  +".txt"));
//        if (!logFile->open(QIODevice::WriteOnly | QIODevice::Text))
//            emit sendDebugInfo("Cannot create logfile", 2000);
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
//    qDebug() << "in readdata";
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

void uartReader::enableLogging(QString _nSamples, QString _logDelay) {
    m_logWriteDelay = _logDelay.toInt()*1000*60; //convert min to ms
    m_nSamplesLog = _nSamples.toInt();

//    if (!dataForLogging.empty()) {dataForLogging.clear();}
//    dataForLogging.resize(m_nSamplesLog);

    m_enableLogging = true;
}

void uartReader::disableLogging() {
    m_enableLogging =false;
    if(timer)
        timer->stop();
}

void uartReader::sendSeriesPointer(QtCharts::QAbstractSeries* series_
                                   , QtCharts::QAbstractAxis* Xaxis_)
{
    qDebug() << "sendSeriesPointer: " << series_;
    m_series.push_back(series_);
    m_axisX.push_back(Xaxis_);
    qDebug() << "getSeriesPointer: " << *m_series.rbegin();
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

void uartReader::update(int graphIdx, QPointF p)
{
    if (m_series[graphIdx]) {
        QtCharts::QXYSeries *xySeries =
                static_cast<QtCharts::QXYSeries *>(m_series[graphIdx]);
        // Use replace instead of clear + append, it's optimized for performance
        xySeries->append(p);//replace(lines.value(series));
    }
    if(m_axisX[graphIdx]) {
        m_axisX[graphIdx]->setMin(p.rx()-3600);
        m_axisX[graphIdx]->setMax(p.rx());
    }
}

void uartReader::processLine(const QByteArray &_line)
{
#ifdef SHOW_Debug_
    qDebug() << _line;
#endif //SHOW_Debug_
    if (!m_flyMode)
        return;
    QStringList line;
//    logFile->write(_line);

    //appendDataToseries/writeTofile
#if 1 //new data format
    //t C Um Ur Ud a b c n ce c0; 11val;
    //1 2 3  4  5  6 7 8 9 10 11
    for (auto w : _line.split(';'))
    {
        line.append(QString(w));
    }
    if(line.size() > 6)
        deviceInSleepMode = false;
    else
        return;
    //fill points
    for (auto& p: tempPoint)
        p.setX(line[1].toInt());
    processTemppoint(0, line[3].toDouble());
    processTemppoint(1, line[4].toDouble());
    processTemppoint(2, line[5].toDouble());
    processTemppoint(3, line[2].toDouble());

    //create timer thread to hold flag while device unsleep
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

    if(!m_queueCommandsToSend.empty())
        sendDataToDevice();
    update(0, tempPoint[0]); //update meas
    update(1, tempPoint[1]); //update ref
    update(2, tempPoint[2]); //update pn
    update(3, tempPoint[3]); //update conc
    emit adjustAxis(axisYRange[0]
                   ,axisYRange[1]
                   ,axisYRange[2]
                   ,axisYRange[3] );
#ifdef SHOW_Debug_
    qDebug() << "temppoint: " <<  tempPoint;
#endif //SHOW_Debug_
    dataProcessingHandler(tempPoint); //send data to ui
    delayThread.join();



#else
    for (auto w : _line.split(','))
    {
        line.append(QString(w));
    }
    if(line.size()==1)
    {
        QStringList pairOfvalue;
        for (auto w : line.first().split('='))
            pairOfvalue.append(QString(w));

        if(pairOfvalue.size()==2 && pairOfvalue.first().compare("time") == 0 )
        {
            for (auto& p: tempPoint)
                p.setX(pairOfvalue[1].toInt());
//            qDebug() << "Match Time " << pairOfvalue[1].toInt() <<"\n";
        }
        if(pairOfvalue.size()==2 && pairOfvalue.first().compare("C") == 0 )
        {
            deviceInSleepMode = false;
            tempPoint[3].setY(pairOfvalue[1].toDouble());
            if( tempPoint[3].y() < axisYRange[3].x())
                axisYRange[3].setX(tempPoint[3].y());
            if( tempPoint[3].y() > axisYRange[3].y())
                axisYRange[3].setY(tempPoint[3].y());
//            qDebug() << "Match conc " << pairOfvalue[1].toDouble();
        }

        if( pairOfvalue.size()==1) //end of packet line "-------"
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

            if(!m_queueCommandsToSend.empty())
                sendDataToDevice();
            update(0, tempPoint[0]); //update meas
            update(1, tempPoint[1]); //update ref
            update(2, tempPoint[2]); //update pn
            update(3, tempPoint[3]); //update conc
            emit adjustAxis(axisYRange[0]
                           ,axisYRange[1]
                           ,axisYRange[2]
                           ,axisYRange[3] );
#ifdef SHOW_Debug_
            qDebug() << "temppoint: " <<  tempPoint;
#endif //SHOW_Debug_
            dataProcessingHandler(tempPoint); //send data to ui
            delayThread.join();
        }
    }
    else
    {
//        qDebug() << "line size: "<<line.size();
        for (auto val : line)
        {
            QStringList pairOfvalueU;
            for (auto w : val.split('='))
                pairOfvalueU.append(QString(w));

            if(pairOfvalueU.size()==2 && pairOfvalueU.first().compare("u_meas") == 0 )
            {
                tempPoint[0].setY(pairOfvalueU[1].toDouble());
                if( tempPoint[0].y() < axisYRange[0].x())
                    axisYRange[0].setX(tempPoint[0].y());
                if( tempPoint[0].y() > axisYRange[0].y())
                    axisYRange[0].setY(tempPoint[0].y());
//                qDebug() << "Match umeas " << val.right(8).toDouble();
            }
            else if(pairOfvalueU.size()==2 && pairOfvalueU.first().compare(" u_ref") == 0 )
            {
                tempPoint[1].setY(pairOfvalueU[1].toDouble());
                if( tempPoint[1].y() < axisYRange[1].x())
                    axisYRange[1].setX(tempPoint[1].y());
                if( tempPoint[1].y() > axisYRange[1].y())
                    axisYRange[1].setY(tempPoint[1].y());
//                qDebug() << "Match uref " << val.right(8).toDouble();
            }
            else if(pairOfvalueU.size()==2 && pairOfvalueU.first().compare(" u_d") == 0 )
            {
                tempPoint[2].setY(pairOfvalueU[1].toDouble());
                if( tempPoint[2].y() < axisYRange[2].x())
                    axisYRange[2].setX(tempPoint[2].y());
                if( tempPoint[2].y() > axisYRange[2].y())
                    axisYRange[2].setY(tempPoint[2].y());
//                qDebug() << "Match upn " << val.right(8).toDouble();
            }
        }
    }
#endif
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

void uartReader::dataProcessingHandler(QVector<QPointF> tempPoint)
{
    allDataHere.push_back(tempPoint);
    //TODO: rewrite to windowed sd and mean
    //get data for nSamples
    std::vector<Statistics<double> > stat;
    calcMeanDev(stat, m_nSamples);
//    int count=0;
//    std::vector<double> accMeas;
//    std::vector<double> accRef;
//    std::vector<double> accPn;
//    std::vector<double> accConc;
//    for (auto p = allDataHere.rbegin(); p != allDataHere.rend(); p++)
//    {
//        accMeas.push_back(p->at(0).y());
//        accRef.push_back( p->at(1).y());
//        accPn.push_back(  p->at(2).y());
//        accConc.push_back(p->at(3).y());
//        if( ++count >= m_nSamples)
//            break;
//    }
//    //calc mean
//    Statistics<double> measStat(accMeas);
//    Statistics<double>  refStat(accRef);
//    Statistics<double>   pnStat(accPn);
//    Statistics<double>   concStat(accConc);


    //sendDataToUi
    emit sendDataToUI(QPointF(stat[0].getMean(), stat[0].getStdDev())      //mean
                     ,QPointF(stat[1].getMean(), stat[1].getStdDev())      //ref
                     ,QPointF(stat[2].getMean(), stat[2].getStdDev()));    //pn
    //sendDataToFileOnce
    if(m_writeToFileOne)
    {
        qDebug() << "write to file";
        QFile f(QString(documentsPath+"/calibr_"+m_filenameOne+".csv"));
        if (!f.open(QIODevice::Append | QIODevice::Text))
        {
            emit sendDebugInfo("Cannot open file" + m_filenameOne);
            return;
        }
        QTextStream ts(&f);
        //get time
        QString str = QDateTime::currentDateTime().toString("yyyyMMdd_hhmm");
        //write header
        ts << QString("Date\tTemp\tConc_real\tConc_meas\tSD\tU_meas\tSD\t"
                      "U_Ref\tSD\tU_d\tSD\n")
           << str <<"\t" << m_currentTemp <<"\t" << m_currentConc <<"\t"
           << stat[0].getMean() <<"\t" <<  stat[0].getStdDev()   <<"\t"
           << stat[1].getMean() <<"\t" <<  stat[1].getStdDev()   <<"\t"
           << stat[2].getMean() <<"\t" <<  stat[2].getStdDev()    <<"\t"
           << stat[3].getMean() <<"\t" <<  stat[3].getStdDev()     <<"\n";
        f.close();
        m_writeToFileOne=false;
    }
    if(m_enableLogging)
    {
        if(!timer) //first loop only
        {
            qDebug() << "Create timer for Log file2";
            timer = std::make_shared<QTimer>();
            connect(timer.get(), &QTimer::timeout, this, &uartReader::logToFile2);
            //create file for logging
            logFile = std::make_shared<QFile>(QString(documentsPath
                        +QDateTime::currentDateTime().toString("/yyyyMMdd_hhmm_")
                        +m_filenameOne
                        +"_log.csv"));
            if (!logFile->open(QIODevice::Append | QIODevice::Text))
            {
                emit sendDebugInfo("Cannot create logfile", 2000);
                return;
            }
            QTextStream ts(logFile.get());
            ts << QString("Temp\tConc_real\tConc_meas\tSD\tU_meas\tSD\t"
                          "U_Ref\tSD\tU_d\tSD\n");
        }
        if(!timer->isActive())
            timer->start(m_logWriteDelay);
            //update if values were updated
        if( timer->interval() != m_logWriteDelay)
            timer->start(m_logWriteDelay);
    }
}

void uartReader::processTemppoint(int num, double value)
{
    tempPoint[num].setY(value);
    if( tempPoint[num].y() < axisYRange[num].x())
        axisYRange[num].setX(tempPoint[num].y());
    if( tempPoint[num].y() > axisYRange[num].y())
        axisYRange[num].setY(tempPoint[num].y());
}

void uartReader::logToFile2()
{
    qDebug() << "log to file2" << QDateTime::currentDateTime().toString("yyyyMMdd_hh:mm:ss");
    std::vector<Statistics<double> > stat;
    calcMeanDev(stat, m_nSamplesLog);

    QTextStream ts(logFile.get());
//    ts << QString("Temp\tConc_real\tConc_meas\tSD\tU_meas\tSD\t"
//                  "U_Ref\tSD\tU_d\tSD\n");
    ts << m_currentTemp <<"\t" << m_currentConc <<"\t"
       << stat[0].getMean() <<"\t" <<  stat[0].getStdDev()   <<"\t"
       << stat[1].getMean() <<"\t" <<  stat[1].getStdDev()   <<"\t"
       << stat[2].getMean() <<"\t" <<  stat[2].getStdDev()    <<"\t"
       << stat[3].getMean() <<"\t" <<  stat[3].getStdDev()     <<"\n";
}

void uartReader::calcMeanDev(std::vector<Statistics<double> > &data, int numOfSamples)
{
    int count=0;
    std::vector<double> accMeas;
    std::vector<double> accRef;
    std::vector<double> accPn;
    std::vector<double> accConc;
    for (auto p = allDataHere.rbegin(); p != allDataHere.rend(); p++)
    {
        accMeas.push_back(p->at(0).y());
        accRef.push_back( p->at(1).y());
        accPn.push_back(  p->at(2).y());
        accConc.push_back(p->at(3).y());
        if( ++count >= numOfSamples)
            break;
    }
    //calc mean
    data.push_back(Statistics<double>(accMeas));
    data.push_back(Statistics<double>(accRef));
    data.push_back(Statistics<double>(accPn));
    data.push_back(Statistics<double>(accConc));
}

void uartReader::buttonPressHandler(const QStringList &line)
{
    qDebug() << "signal from button";
}


















