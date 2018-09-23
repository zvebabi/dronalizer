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
#include <memory>

class uartReader : public QObject
{
    Q_OBJECT
public:
    explicit uartReader(QObject *parent = 0);
    ~uartReader();
public slots:
    void initDevice(QString port, QString baudRate,
                    QVariantList propertiesNames_,
                    QVariantList propertiesValues_);
    void updateProperties(QVariantList propertiesNames_,
                          QVariantList propertiesValues_);
    void getListOfPort();
    QString getDataPath() {return documentsPath;}
    void readData();
    void prepareCommandToSend(QString cmd_);
    void setFlyMode(bool mode, QString _nSamples){
        m_flyMode = mode;
        m_nSamples = _nSamples.toInt();
    }
    void writeToFileOne(bool mode, QString _temp, QString _conc, QString fn1){
        m_writeToFileOne = mode;
        m_currentTemp = _temp.toDouble();
        m_currentConc = _conc.toDouble();
        m_filenameOne = fn1;
    }
    void sendSeriesPointer(QtCharts::QAbstractSeries *series_,
                           QtCharts::QAbstractAxis *Xaxis_);
    void selectPath(QString pathForSave);

signals:
    void sendPortName(QString port);
    void sendDebugInfo(QString data, int time=700);
    void sendSerialNumber(QString serNumber);
    void makeSeries(); // run 1 time on startup
    void disableButton();
    void adjustAxis(QPointF axisYRange_Umeas
                   ,QPointF axisYRange_Uref
                   ,QPointF axisYRange_Upn
                   ,QPointF axisYRange_C);
    void sendDataToUI(QPointF _mean
                     ,QPointF _ref
                     ,QPointF _pn);
private:
    void sendDataToDevice();
    QVector<QVector<QPointF>> allDataHere;
    QVector<QPointF> tempPoint;  //0-meas
                                 //1-ref
                                 //2-pn
                                 //3-C,
    QVector<QPointF> axisYRange; //x-min, y - max
    void update(int graphIdx, QPointF p);
    void processLine(const QByteArray& line);
    void serviceModeHandler(const QStringList& line);
    void identityHandler(const QStringList& line);
    void dataAquisitionHandler(const QStringList& line);
    void dataProcessingHandler(QVector<QPointF> tempPoint);
    void buttonPressHandler(const QStringList& line);

    std::vector<QString> ports;

    QSerialPort* device = NULL;
    QVector<QtCharts::QAbstractSeries *>m_series;
    QVector<QtCharts::QAbstractAxis *>m_axisX;
    int m_serNumber;
    int m_nSamples;
    double m_currentTemp;
    double m_currentConc;
    QString m_filenameOne;
    bool m_flyMode, m_writeToFileOne;
    std::shared_ptr<QFile> logFile;
    bool isPortOpen, firstLine, serviceMode;
    std::atomic_bool deviceInSleepMode;
    QVector<QString> m_queueCommandsToSend;

    QString documentsPath;
    std::ofstream diagnosticLog;

};

#endif // ANALIZERCDC_H
