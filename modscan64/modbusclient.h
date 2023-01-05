#ifndef MODBUSCLIENT_H
#define MODBUSCLIENT_H

#include <QObject>
#include <QModbusClient>
#include "connectiondetails.h"
#include "modbuswriteparams.h"

///
/// \brief The ModbusClient class
///
class ModbusClient : public QObject
{
    Q_OBJECT
public:
    explicit ModbusClient(QObject *parent = nullptr);

    void connectDevice(const ConnectionDetails& cd);
    void disconnectDevice();

    bool isValid() const;
    QModbusDevice::State state() const;

    void sendReadRequest(QModbusDataUnit::RegisterType pointType, int startAddress, quint16 valueCount, int server, int requestId);
    void writeRegister(QModbusDataUnit::RegisterType pointType, const ModbusWriteParams& params, int requestId);
    void maskWriteRegister(const ModbusMaskWriteParams& params, int requestId);

signals:
    void modbusRequest(int requestId, const QModbusRequest& request);
    void modbusReply(QModbusReply* reply);
    void modbusWriteError(const QString& error);
    void modbusConnectionError(const QString& error);

private slots:
    void on_readReply();
    void on_writeReply();
    void on_errorOccured(QModbusDevice::Error error);
    void on_stateChanged(QModbusDevice::State state);

private:
    QModbusClient* _modbusClient;
};

#endif // MODBUSCLIENT_H