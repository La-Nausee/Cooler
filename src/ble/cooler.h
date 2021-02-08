#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QString>
#include <QDebug>
#include <QLowEnergyController>
#include <QLowEnergyService>
#include <QTimer>
#include <QEventLoop>
#include <QList>

QT_USE_NAMESPACE

#define CONTROLLER_NUS_SERVICE_UUID     "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define NUS_SERVICE_RX_UUID             "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
#define NUS_SERVICE_TX_UUID             "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"

#define BLE_GATT_ATT_MTU_DEFAULT        (23-3)
#define ON_OFF_INDEX_H                   0
#define ON_OFF_INDEX_L                   1
#define LED_PWM_INDEX_H                  2
#define LED_PWM_INDEX_L                  3
#define PEL_PWM_INDEX_H                  4
#define PEL_PWM_INDEX_L                  5
#define FAN_PWM_INDEX_H                  6
#define FAN_PWM_INDEX_L                  7

#define PWM_TOP_VAL                      12000

class Cooler : public QObject
{
    Q_OBJECT
public:
     Cooler(QObject *parent = nullptr);
     ~Cooler();
     Q_INVOKABLE void disconnectFromDevice();
     Q_INVOKABLE bool connectToDevice(const QString &address);
     Q_INVOKABLE bool isReady(){return ready;}
     Q_INVOKABLE QString remoteAddress() {return m_remoteAddress;}

     Q_INVOKABLE  unsigned int getFanPWM() { return (100 - rx_fan_pwm*100/PWM_TOP_VAL);}
     Q_INVOKABLE  unsigned int getLEDPWM() { return (rx_led_pwm*100/PWM_TOP_VAL);}
     Q_INVOKABLE  unsigned int getPelPWM() { return (100 - rx_pel_pwm*100/PWM_TOP_VAL);}
     Q_INVOKABLE  QString getFanCur() { return QString::number(fan_cur,'g',3);}
     Q_INVOKABLE  QString getPelCur() { return QString::number(pel_cur,'g',3);}
     Q_INVOKABLE  QString getBatLevel() { return QString::number(battery_votagle,'g',3);}
     Q_INVOKABLE  QString getTemVal() { return QString::number(ain4_votagle,'g',3);}

     Q_INVOKABLE  void setFanPWM(unsigned int);
     Q_INVOKABLE  void setLEDPWM(unsigned int);
     Q_INVOKABLE  void setPelPWM(unsigned int);
     Q_INVOKABLE  void shutdown();

private slots:
     //QLowEnergyController
     void serviceDiscovered(const QBluetoothUuid &);
     void serviceScanDone();
     void controllerError(QLowEnergyController::Error);
     void deviceConnected();
     void deviceDisconnected();

     void serviceStateChanged(QLowEnergyService::ServiceState s);
     void serviceCharacteristicChanged(const QLowEnergyCharacteristic &c, const QByteArray &value);

private:
     bool ready;
     bool foundNusService;
     quint16 rx_led_pwm;
     quint16 rx_fan_pwm;
     quint16 rx_pel_pwm;
     qint16  rx_fan_cur;
     qint16  rx_pel_cur;
     qint16  rx_bat_lev;
     qint16  rx_ain4;

     double battery_votagle;
     double fan_cur;
     double pel_cur;
     double ain4_votagle;

     QByteArray tx_data;

     QString m_remoteAddress;

     QLowEnergyController *m_control;

     QLowEnergyDescriptor m_notificationDesc;

     QLowEnergyService *m_service;
     QLowEnergyCharacteristic m_rxChar;
     QLowEnergyCharacteristic m_txChar;
};

#endif
