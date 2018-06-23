#include "uartReader.h"
#include <QtMath>
#include <memory>

uartReader::uartReader(QObject *parent) : QObject(parent),
    firstLine(true), serviceMode(false), isPortOpen(false),
    m_serNumber(-1)

{
    qRegisterMetaType<QtCharts::QAbstractSeries*>();
    qRegisterMetaType<QtCharts::QAbstractAxis*>();
    documentsPath = QDir::homePath()+QString("/Documents/");
    device = new QSerialPort(this);
    connect(device, &QSerialPort::readyRead, this, &uartReader::readData);

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
}

void uartReader::initDevice(QString port,
                            QVariantList propertiesNames_,
                            QVariantList propertiesValues_)
{
#if 1
    updateProperties(propertiesNames_, propertiesValues_);
#else
    device->setPortName(port);
    device->setBaudRate(QSerialPort::Baud115200);
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
        updateProperties(properties_);
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
    command = QString("%1=%2$\r").arg(propertiesNames_[0].toString())
                                 .arg(propertiesValues_[0].toInt(), 2,10, QChar('0'));
    qDebug() << command;

    for(int i=1; i< propertiesValues_.size(); i++)
    {
        double koef_1=0.0, koef_2=0.0;
        koef_2 = std::modf(propertiesValues_[i].toDouble(), &koef_1);
        int k1 = koef_1;
        int k2 = koef_2 * 100000;

        command = QString("%1=%2.%3$\r").arg(propertiesNames_[i].toString())
                                     .arg(k1)
                                     .arg(QString::number(k2).leftJustified(5,QChar('0')));
        qDebug() << command;
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
    while (device->canReadLine()) processLine(device->readLine());
}

void uartReader::doMeasurements()
{
    emit sendDebugInfo("Start measurement");
    qDebug() << "doMeasurements";

//    if(serviceMode)
//        device->write("d");
//    else
//        device->write("m");
}

void uartReader::selectPath(QString pathForSave)
{
    qDebug() << documentsPath;
    documentsPath = pathForSave;
    qDebug() << pathForSave;
    qDebug() << documentsPath;
}

void uartReader::update(QtCharts::QAbstractSeries *series)
{
    if (series) {
        QtCharts::QXYSeries *xySeries =
                static_cast<QtCharts::QXYSeries *>(series);
//        QVector<QPointF> points = lines.value(series);
        // Use replace instead of clear + append, it's optimized for performance
//        xySeries->append();//replace(lines.value(series));
        qDebug() << "//TODO: append data to series";
    }
    emit sendDebugInfo("Done");
}

void uartReader::processLine(const QByteArray &_line)
{
//    QByteArray line = device->readAll();
    qDebug() << _line;
    QStringList line;//(_line);
    for (auto w : _line.split(','))
    {
        line.append(QString(w));
    }
//identity
    if( line.first().compare("x=i") ==0)
        identityHandler(line);
//service mode parser
    if( line.first().compare("x=s") == 0)
        serviceModeHandler(line);   //parse all comands here
//measure mode
    if( line.first().compare("x=m\n") == 0)
        buttonPressHandler(line);
    if( line.first().compare("x=d") ==0)
        dataAquisitionHandler(line);
    if( line.first().compare("x=e\n") ==0)
        dataProcessingHandler(line);
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

















