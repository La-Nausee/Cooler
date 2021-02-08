#ifndef SCANNER_H
#define SCANNER_H

#include <QObject>
#include <QDebug>
#include <QEventLoop>

#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothAddress>
#include <QBluetoothHostInfo>
#include <QBluetoothLocalDevice>

class Scanner: public QObject
{
    Q_OBJECT
public:
    Scanner(QObject *parent = nullptr);
    ~Scanner();

    Q_INVOKABLE QString allLocalDevices();
    Q_INVOKABLE void setLocalDevice(QString address);
    Q_INVOKABLE void start(QString filter = QString(), int timeout=3000);
    Q_INVOKABLE QString allRemoteDevices();

private slots:
    void foundDevice(const QBluetoothDeviceInfo&);
    void scanFinished();
    void deviceScanError(QBluetoothDeviceDiscoveryAgent::Error);
private:

    QEventLoop m_eventLoop;

    QList<QBluetoothHostInfo> m_localDevices;

    QBluetoothDeviceDiscoveryAgent *m_deviceDiscoveryAgent;

    QString m_filter;
//    QList<QBluetoothDeviceInfo> m_remoteDeviceList;
    QString              m_remoteDevices;

};

#endif // SCANNER_H
