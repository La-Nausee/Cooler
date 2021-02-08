#include "cooler.h"
#include <QDebug>
#include <QEventLoop>
#include <QThread>
#include <QTimer>

Cooler::Cooler(QObject *parent)
    : QObject (parent),
      ready(false),
      foundNusService(false),
      rx_led_pwm(0),
      m_control(nullptr)
{
    tx_data.fill(0,BLE_GATT_ATT_MTU_DEFAULT);
}

Cooler::~Cooler() {
    if (m_control != nullptr) {
        m_control->disconnectFromDevice();

        delete m_control;
        m_control = nullptr;
    }
}

bool Cooler::connectToDevice(const QString &address) {
    QEventLoop loop;
    QTimer timer;

    m_remoteAddress = address;

    if (m_control) {
        m_control->disconnectFromDevice();
        delete m_control;
        m_control = nullptr;
    }

    m_control = new QLowEnergyController(QBluetoothAddress(address));
    connect(m_control, SIGNAL(serviceDiscovered(QBluetoothUuid)),
            this, SLOT(serviceDiscovered(QBluetoothUuid)));
    connect(m_control, SIGNAL(discoveryFinished()),
            this, SLOT(serviceScanDone()));
    connect(m_control, SIGNAL(error(QLowEnergyController::Error)),
            this, SLOT(controllerError(QLowEnergyController::Error)));
    connect(m_control, SIGNAL(connected()),
            this, SLOT(deviceConnected()));
    connect(m_control, SIGNAL(disconnected()),
            this, SLOT(deviceDisconnected()));

    m_control->setRemoteAddressType(QLowEnergyController::RandomAddress);


    connect(m_control, SIGNAL(connected()),
            &loop, SLOT(quit()));
    connect(&timer, SIGNAL(timeout()),
            &loop, SLOT(quit()));

    timer.setSingleShot(true);
    timer.start(3000);
    m_control->connectToDevice();

    loop.exec();

    return (timer.remainingTime() > 0);
}

void Cooler::disconnectFromDevice()
{
    ready  = false;

    if (m_control != nullptr) {
        m_control->disconnectFromDevice();
        delete m_control;
        m_control = nullptr;
    }

    m_remoteAddress.clear();
}

void Cooler::deviceConnected() {
    ready = true;
    m_control->discoverServices();
}

void Cooler::deviceDisconnected() {
    ready = false;
    m_remoteAddress.clear();
    qWarning() << "device disconnected";
}

void Cooler::serviceDiscovered(const QBluetoothUuid &gatt) {
    qDebug()<<gatt.toString();
    if (gatt == QBluetoothUuid(QString(CONTROLLER_NUS_SERVICE_UUID))) {
        foundNusService = true;
        qDebug()<<"found nus serice";
    }
}

void Cooler::serviceScanDone() {
    if (foundNusService) {
        m_service = m_control->createServiceObject(QBluetoothUuid(QString(CONTROLLER_NUS_SERVICE_UUID)), this);

        connect(m_service, &QLowEnergyService::stateChanged, this, &Cooler::serviceStateChanged);
        connect(m_service, &QLowEnergyService::characteristicChanged, this, &Cooler::serviceCharacteristicChanged);

        m_service->discoverDetails();
    }
}

void Cooler::controllerError(QLowEnergyController::Error error) {
    Q_UNUSED(error);
}


void Cooler::serviceStateChanged(QLowEnergyService::ServiceState s)
{
    switch (s) {
    case QLowEnergyService::ServiceDiscovered:
    {
        m_rxChar = m_service->characteristic(QBluetoothUuid(QString(NUS_SERVICE_RX_UUID)));
        if (m_rxChar.isValid()) {
            qDebug()<<"rxChar found.";
        }

        m_notificationDesc = m_rxChar.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
        if (m_notificationDesc.isValid()) {
            //enable notification
            qDebug()<<"m_rxChar enable Notification.";
            m_service->writeDescriptor(m_notificationDesc, QByteArray::fromHex("0100"));
        }

        m_txChar = m_service->characteristic(QBluetoothUuid(QString(NUS_SERVICE_TX_UUID)));
        if (m_txChar.isValid()) {
            qDebug()<<"m_txChar found.";
        }

        break;
    }
    default:
        //nothing for now
        break;
    }
}
void Cooler::serviceCharacteristicChanged(const QLowEnergyCharacteristic &c,
                                     const QByteArray &value)
{
    qint16 i = 0;
    Q_UNUSED(c)
//    Q_UNUSED(value)
//    qDebug()<<value.toHex();
    rx_led_pwm = value[i++]<<8;
    rx_led_pwm += value[i++];
    tx_data[LED_PWM_INDEX_H] = rx_led_pwm>>8;
    tx_data[LED_PWM_INDEX_L] = rx_led_pwm & 0xFF;

    rx_fan_pwm = value[i++]<<8;
    rx_fan_pwm += value[i++];
    tx_data[FAN_PWM_INDEX_H] = rx_fan_pwm>>8;
    tx_data[FAN_PWM_INDEX_L] = rx_fan_pwm & 0xFF;

    rx_pel_pwm = value[i++]<<8;
    rx_pel_pwm += value[i++];
    tx_data[PEL_PWM_INDEX_H] = rx_pel_pwm>>8;
    tx_data[PEL_PWM_INDEX_L] = rx_pel_pwm & 0xFF;

    rx_fan_cur = value[i++]<<8;
    rx_fan_cur += value[i++];
    fan_cur = rx_fan_cur * 0.6 /4096.0;

    rx_pel_cur = value[i++]<<8;
    rx_pel_cur += value[i++];
    pel_cur = rx_pel_cur * 0.6 /4096.0;

    rx_bat_lev = value[i++]<<8;
    rx_bat_lev += value[i++];
    battery_votagle = rx_bat_lev * 2.0 * 0.6 /4096.0;

    rx_ain4 = value[i++]<<8;
    rx_ain4 += value[i++];
    ain4_votagle = rx_ain4 * 2.0 * 0.6 /4096.0;

//    qDebug()<<QString::number(value[6],16)<<QString::number(value[7],16)<<rx_fan_cur;
    qDebug()<<rx_fan_cur<<rx_pel_cur<<rx_bat_lev<<rx_ain4;
}

void Cooler::setFanPWM(unsigned int pwm)
{
    pwm = pwm*PWM_TOP_VAL/100;
    pwm = PWM_TOP_VAL - pwm;
    tx_data[FAN_PWM_INDEX_H] = pwm>>8;
    tx_data[FAN_PWM_INDEX_L] = pwm & 0xFF;

    m_service->writeCharacteristic(m_txChar,tx_data);
}

void Cooler::setLEDPWM(unsigned int pwm)
{
    pwm = pwm*PWM_TOP_VAL/100;
//    pwm = PWM_TOP_VAL - pwm;
    tx_data[LED_PWM_INDEX_H] = pwm>>8;
    tx_data[LED_PWM_INDEX_L] = pwm & 0xFF;

    m_service->writeCharacteristic(m_txChar,tx_data);
}

void Cooler::setPelPWM(unsigned int pwm)
{
    pwm = pwm*PWM_TOP_VAL/100;
    pwm = PWM_TOP_VAL - pwm;
    tx_data[PEL_PWM_INDEX_H] = pwm>>8;
    tx_data[PEL_PWM_INDEX_L] = pwm & 0xFF;

    m_service->writeCharacteristic(m_txChar,tx_data, QLowEnergyService::WriteWithoutResponse);
}

void Cooler::shutdown()
{
    ready = false;

    tx_data[ON_OFF_INDEX_H] = 0xFF;
    tx_data[ON_OFF_INDEX_L] = 0xFF;

    m_service->writeCharacteristic(m_txChar,tx_data);

    tx_data[ON_OFF_INDEX_H] = 0;
    tx_data[ON_OFF_INDEX_L] = 0;
}
