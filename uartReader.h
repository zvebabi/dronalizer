#ifndef ANALIZERCDC_H
#define ANALIZERCDC_H

#include <QtCore/QObject>
#include <QVariant>
#include <QVector>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QStringList>
#include <QByteArray>
#include <QPointF>
#include <QAbstractSeries>
#include <QtCharts/QAbstractSeries>
#include <QtCharts/QXYSeries>
#include <QColor>
#include <QVariantList>
#include <QStringList>
#include <QDebug>
#include <QDir>
#include <QPair>
#include <QFile>
#include <vector>
#include <fstream>
#include <typeinfo>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>


class uartReader : public QObject
{
    Q_OBJECT
public:
    explicit uartReader(QObject *parent = 0);
    ~uartReader();
public slots:
    void initDevice(QString port,
                    QVariantList propertiesNames_,
                    QVariantList propertiesValues_);
    void updateProperties(QVariantList propertiesNames_,
                          QVariantList propertiesValues_);
    void getListOfPort();
    QString getDataPath() {return documentsPath;}
    void readData();

    void doMeasurements();
    void selectPath(QString pathForSave);

signals:
    void sendPortName(QString port);
    void sendDebugInfo(QString data, int time=700);
    void sendSerialNumber(QString serNumber);
    void makeSeries(); // run 1 time on startup
    void disableButton();
private:
    void update(QtCharts::QAbstractSeries *series);
    void processLine(const QByteArray& line);
    void serviceModeHandler(const QStringList& line);
    void identityHandler(const QStringList& line);
    void dataAquisitionHandler(const QStringList& line);
    void dataProcessingHandler(const QStringList& line);
    void buttonPressHandler(const QStringList& line);

    std::vector<QString> ports;

    QSerialPort* device = NULL;
    QSerialPort::BaudRate baudRate;

    int m_serNumber;
    bool isPortOpen, firstLine, serviceMode;

    QString documentsPath;
    std::ofstream diagnosticLog;

};

#endif // ANALIZERCDC_H
