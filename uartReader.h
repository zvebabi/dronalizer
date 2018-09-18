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
    void setFlyMode(bool mode){
        m_flyMode = mode;
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
private:
    void sendDataToDevice();
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
    void dataProcessingHandler(const QStringList& line);
    void buttonPressHandler(const QStringList& line);

    std::vector<QString> ports;

    QSerialPort* device = NULL;
    QVector<QtCharts::QAbstractSeries *>m_series;
    QVector<QtCharts::QAbstractAxis *>m_axisX;
    int m_serNumber;
    bool m_flyMode;
    std::shared_ptr<QFile> logFile;
    bool isPortOpen, firstLine, serviceMode;
    std::atomic_bool deviceInSleepMode;
    QVector<QString> m_queueCommandsToSend;

    QString documentsPath;
    std::ofstream diagnosticLog;

};

#endif // ANALIZERCDC_H
