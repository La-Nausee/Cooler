#include "scanner.h"

Scanner::Scanner(QObject *parent):
    QObject(parent)
{
    m_localDevices = QBluetoothLocalDevice::allDevices();

    m_deviceDiscoveryAgent = nullptr;
}

Scanner::~Scanner()
{
    if(m_deviceDiscoveryAgent != nullptr)
    {
        m_deviceDiscoveryAgent->stop();
        delete m_deviceDiscoveryAgent;
        m_deviceDiscoveryAgent = nullptr;
    }
}

QString Scanner::allLocalDevices()
{
    QString devs;

    foreach(const QBluetoothHostInfo &dev, m_localDevices)
    {
        qDebug()<<"Found"<<dev.name()<<dev.address().toString();
        devs.append(dev.address().toString());
        devs.append(",");
    }

    return devs;
}

void Scanner::setLocalDevice(QString address)
{
    if(m_deviceDiscoveryAgent != nullptr)
    {
        m_deviceDiscoveryAgent->stop();
        delete m_deviceDiscoveryAgent;
        m_deviceDiscoveryAgent = nullptr;
    }

    m_deviceDiscoveryAgent = new QBluetoothDeviceDiscoveryAgent(QBluetoothAddress(address),this);

    connect(m_deviceDiscoveryAgent, SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo &)),
            this, SLOT(foundDevice(const QBluetoothDeviceInfo &)));
    connect(m_deviceDiscoveryAgent, SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)),
            this, SLOT(deviceScanError(QBluetoothDeviceDiscoveryAgent::Error)));
    connect(m_deviceDiscoveryAgent, SIGNAL(finished()), this, SLOT(scanFinished()));

    connect(m_deviceDiscoveryAgent, SIGNAL(finished()), &m_eventLoop, SLOT(quit()));
}


void Scanner::foundDevice(const QBluetoothDeviceInfo &device) {
    if (device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) {
        qWarning() << "Discovered LE Device name: " << device.name() << " Address: "
                   << device.address().toString()<<QString::number(device.rssi());

        QString dev_str;
        dev_str.append(QString::number(device.rssi()));
        dev_str.append(",");
        dev_str.append(device.name());
        dev_str.append(",");

#if defined (Q_OS_MACOS) || defined (Q_OS_IOS)
        QString uuid = device.deviceUuid().toString();
        dev_str.append(device.deviceUuid().toString());
#else
        dev_str.append(device.address().toString());
#endif
        dev_str.append(",");
//        if(m_filter.isEmpty() && device.rssi() != 0)
        if(m_filter.isEmpty())
        {
            m_remoteDevices.append(dev_str);
        }
        else
        {
//            if(dev_str.contains(m_filter) && device.rssi() != 0)
            if(dev_str.contains(m_filter))
                m_remoteDevices.append(dev_str);
        }
//        m_remoteDeviceList.append(device);
    }
}

void Scanner::scanFinished() {

}

void Scanner::deviceScanError(QBluetoothDeviceDiscoveryAgent::Error error) {
    if (error == QBluetoothDeviceDiscoveryAgent::PoweredOffError)
    {

    }
    else if (error == QBluetoothDeviceDiscoveryAgent::InputOutputError)
    {

    }
    else
    {

    }
}

void Scanner::start(QString filter, int timeout)
{
    if(m_deviceDiscoveryAgent == nullptr)
        return;

    m_filter = filter;
//    m_remoteDeviceList.clear();
    m_remoteDevices.clear();

    m_deviceDiscoveryAgent->setLowEnergyDiscoveryTimeout(timeout);
    m_deviceDiscoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);

    m_eventLoop.exec();
}

QString Scanner::allRemoteDevices()
{
    return m_remoteDevices;
}
