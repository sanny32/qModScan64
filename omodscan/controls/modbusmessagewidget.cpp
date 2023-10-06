#include "formatutils.h"
#include "htmldelegate.h"
#include "modbusmessagewidget.h"
#include "modbusmessages.h"

///
/// \brief ModbusMessageWidget::ModbusMessageWidget
/// \param parent
///
ModbusMessageWidget::ModbusMessageWidget(QWidget *parent)
    : QListWidget(parent)
    ,_byteOrder(ByteOrder::LittleEndian)
    ,_dataDisplayMode(DataDisplayMode::Decimal)
    ,_showTimestamp(true)
    ,_msg(nullptr)

{
    setItemDelegate(new HtmlDelegate(this));
    setEditTriggers(QAbstractItemView::NoEditTriggers);
}

///
/// \brief ModbusMessageWidget::clear
///
void ModbusMessageWidget::clear()
{
    _msg = nullptr;
    update();
}

///
/// \brief ModbusMessageWidget::dataDisplayMode
/// \return
///
DataDisplayMode ModbusMessageWidget::dataDisplayMode() const
{
    return _dataDisplayMode;
}

///
/// \brief ModbusMessageWidget::setDataDisplayMode
/// \param mode
///
void ModbusMessageWidget::setDataDisplayMode(DataDisplayMode mode)
{
    _dataDisplayMode = mode;
    update();
}

///
/// \brief ModbusMessageWidget::byteOrder
/// \return
///
ByteOrder ModbusMessageWidget::byteOrder() const
{
    return _byteOrder;
}

///
/// \brief ModbusMessageWidget::setByteOrder
/// \param order
///
void ModbusMessageWidget::setByteOrder(ByteOrder order)
{
    _byteOrder = order;
    update();
}

///
/// \brief ModbusMessageWidget::showTimestamp
/// \return
///
bool ModbusMessageWidget::showTimestamp() const
{
    return _showTimestamp;
}

///
/// \brief ModbusMessageWidget::setShowTimestamp
/// \param on
///
void ModbusMessageWidget::setShowTimestamp(bool on)
{
    _showTimestamp = on;
    update();
}

///
/// \brief ModbusMessageWidget::modbusMessage
/// \return
///
const ModbusMessage* ModbusMessageWidget::modbusMessage() const
{
    return _msg;
}

///
/// \brief ModbusMessageWidget::setModbusMessage
/// \param msg
///
void ModbusMessageWidget::setModbusMessage(const ModbusMessage* msg)
{
    _msg = msg;
    update();
}

///
/// \brief ModbusMessageWidget::update
///
void ModbusMessageWidget::update()
{
    QListWidget::clear();

    if(_msg == nullptr)
        return;

    if(!_msg->isValid())
    {
        if(_msg->isRequest())
            addItem(tr("<span style='color:red'>*** INVALID MODBUS REQUEST ***</span>"));
        else if(!_msg->isException())
            addItem(tr("<span style='color:red'>*** INVALID MODBUS RESPONSE ***</span>"));
    }

    addItem(tr("<b>Type:</b> %1").arg(_msg->isRequest() ? tr("Tx Message") : tr("Rx Message")));
    if(_showTimestamp) addItem(tr("<b>Timestamp:</b> %1").arg(_msg->timestamp().toString(Qt::ISODateWithMs)));
    addItem(tr("<b>Device ID:</b> %1").arg(formatByteValue(_dataDisplayMode, _msg->deviceId())));

    if(_msg->isException())
    {
        const auto exception = QString("%1 (%2)").arg(formatByteValue(_dataDisplayMode, _msg->exception()), _msg->exception());
        addItem(tr("<b>Error Code:</b> %1").arg(formatByteValue(_dataDisplayMode, _msg->function())));
        addItem(tr("<b>Exception Code:</b> %1").arg(exception));
        return;
    }

    const auto func = _msg->function();
    const auto function = func.isValid() ?
                              QString("%1 (%2)").arg(formatByteValue(_dataDisplayMode, func), func) :
                              formatByteValue(_dataDisplayMode, func);
    addItem(tr("<b>Function Code:</b> %1").arg(function));

    switch(_msg->function())
    {
        case QModbusPdu::ReadCoils:
            if(_msg->isRequest())
            {
                auto req = reinterpret_cast<const ReadCoilsRequest*>(_msg);
                const auto startAddress = req->isValid() ? formatWordValue(_dataDisplayMode, req->startAddress()) : "??";
                const auto length = req->isValid() ? formatWordValue(_dataDisplayMode, req->length()): "??";
                addItem(tr("<b>Start Address:</b> %1").arg(startAddress));
                addItem(tr("<b>Length:</b> %1").arg(length));
            }
            else
            {
                auto resp = reinterpret_cast<const ReadCoilsResponse*>(_msg);
                const auto byteCount = resp->isValid() ? formatByteValue(_dataDisplayMode, resp->byteCount()) : "?";
                const auto coilStatus = resp->isValid() ? formatByteArray(_dataDisplayMode, resp->coilStatus()) : "???";
                addItem(tr("<b>Byte Count:</b> %1").arg(byteCount));
                addItem(tr("<b>Coil Status:</b> %1").arg(coilStatus));
            }
        break;

        case QModbusPdu::ReadDiscreteInputs:
            if(_msg->isRequest())
            {
                auto req = reinterpret_cast<const ReadDiscreteInputsRequest*>(_msg);
                const auto startAddress = req->isValid() ? formatWordValue(_dataDisplayMode, req->startAddress()) : "??";
                const auto length = req->isValid() ? formatWordValue(_dataDisplayMode, req->length()): "??";
                addItem(tr("<b>Start Address:</b> %1").arg(startAddress));
                addItem(tr("<b>Length:</b> %1").arg(length));
            }
            else
            {
                auto resp = reinterpret_cast<const ReadDiscreteInputsResponse*>(_msg);
                const auto byteCount = resp->isValid() ? formatByteValue(_dataDisplayMode, resp->byteCount()) : "?";
                const auto inputStatus = resp->isValid() ? formatByteArray(_dataDisplayMode, resp->inputStatus()) : "???";
                addItem(tr("<b>Byte Count:</b> %1").arg(byteCount));
                addItem(tr("<b>Input Status:</b> %1").arg(inputStatus));
            }
        break;

        case QModbusPdu::ReadHoldingRegisters:
            if(_msg->isRequest())
            {
                auto req = reinterpret_cast<const ReadHoldingRegistersRequest*>(_msg);
                const auto startAddress = req->isValid() ? formatWordValue(_dataDisplayMode, req->startAddress()) : "??";
                const auto length = req->isValid() ? formatWordValue(_dataDisplayMode, req->length()): "??";
                addItem(tr("<b>Start Address:</b> %1").arg(startAddress));
                addItem(tr("<b>Length:</b> %1").arg(length));
            }
            else
            {
                auto resp = reinterpret_cast<const ReadHoldingRegistersResponse*>(_msg);
                const auto byteCount = resp->isValid() ? formatByteValue(_dataDisplayMode, resp->byteCount()) : "?";
                const auto registerValue = resp->isValid() ? formatWordArray(_dataDisplayMode, resp->registerValue(), _byteOrder) : "???";
                addItem(tr("<b>Byte Count:</b> %1").arg(byteCount));
                addItem(tr("<b>Register Value:</b> %1").arg(registerValue));
            }
        break;

        case QModbusPdu::ReadInputRegisters:
            if(_msg->isRequest())
            {
                auto req = reinterpret_cast<const ReadInputRegistersRequest*>(_msg);
                const auto startAddress = req->isValid() ? formatWordValue(_dataDisplayMode, req->startAddress()) : "??";
                const auto length = req->isValid() ? formatWordValue(_dataDisplayMode, req->length()): "??";
                addItem(tr("<b>Start Address:</b> %1").arg(startAddress));
                addItem(tr("<b>Length:</b> %1").arg(length));
            }
            else
            {
                auto resp = reinterpret_cast<const ReadInputRegistersResponse*>(_msg);
                const auto byteCount = resp->isValid() ? formatByteValue(_dataDisplayMode, resp->byteCount()) : "?";
                const auto registerValue = resp->isValid() ? formatWordArray(_dataDisplayMode, resp->registerValue(), _byteOrder) : "???";
                addItem(tr("<b>Byte Count:</b> %1").arg(byteCount));
                addItem(tr("<b>Input Registers:</b> %1").arg(registerValue));
            }
        break;

        case QModbusPdu::WriteSingleCoil:
            if(_msg->isRequest())
            {
                auto req = reinterpret_cast<const WriteSingleCoilRequest*>(_msg);
                const auto outputAddress = req->isValid() ? formatWordValue(_dataDisplayMode, req->address()) : "??";
                const auto outputValue = req->isValid() ? formatWordValue(_dataDisplayMode, req->value()) : "??";
                addItem(tr("<b>Output Address:</b> %1").arg(outputAddress));
                addItem(tr("<b>Output Value:</b> %1").arg(outputValue));
            }
            else
            {
                auto resp = reinterpret_cast<const WriteSingleCoilResponse*>(_msg);
                const auto outputAddress = resp->isValid() ? formatWordValue(_dataDisplayMode, resp->address()) : "??";
                const auto outputValue = resp->isValid() ? formatWordValue(_dataDisplayMode, resp->value()) : "??";
                addItem(tr("<b>Output Address:</b> %1").arg(outputAddress));
                addItem(tr("<b>Output Value:</b> %1").arg(outputValue));
            }
        break;

        case QModbusPdu::WriteSingleRegister:
            if(_msg->isRequest())
            {
                auto req = reinterpret_cast<const WriteSingleRegisterRequest*>(_msg);
                const auto registerAddress = req->isValid() ? formatWordValue(_dataDisplayMode, req->address()) : "??";
                const auto registerValue = req->isValid() ? formatWordValue(_dataDisplayMode, req->value()) : "??";
                addItem(tr("<b>Register Address:</b> %1").arg(registerAddress));
                addItem(tr("<b>Register Value:</b> %1").arg(registerValue));
            }
            else
            {
                auto resp = reinterpret_cast<const WriteSingleRegisterResponse*>(_msg);
                const auto registerAddress = resp->isValid() ? formatWordValue(_dataDisplayMode, resp->address()) : "??";
                const auto registerValue = resp->isValid() ? formatWordValue(_dataDisplayMode, resp->value()) : "??";
                addItem(tr("<b>Register Address:</b> %1").arg(registerAddress));
                addItem(tr("<b>Register Value:</b> %1").arg(registerValue));
            }
            break;

        case QModbusPdu::ReadExceptionStatus:
            if(!_msg->isRequest())
            {
                auto resp = reinterpret_cast<const ReadExceptionStatusResponse*>(_msg);
                const auto outputData = resp->isValid() ? formatByteValue(_dataDisplayMode, resp->outputData()) : "?";
                addItem(tr("<b>Output Data:</b> %1").arg(outputData));
            }
            break;

        case QModbusPdu::Diagnostics:
            if(_msg->isRequest())
            {
                auto req = reinterpret_cast<const DiagnosticsRequest*>(_msg);
                const auto subFunc = req->isValid() ? formatWordValue(_dataDisplayMode, req->subfunc()) : "??";
                const auto data = req->isValid() ? formatByteArray(_dataDisplayMode, req->data()) : "???";
                addItem(tr("<b>Sub-function:</b> %1").arg(subFunc));
                addItem(tr("<b>Data:</b> %1").arg(data));
            }
            else
            {
                auto resp = reinterpret_cast<const DiagnosticsResponse*>(_msg);
                const auto subFunc = resp->isValid() ? formatWordValue(_dataDisplayMode, resp->subfunc()) : "??";
                const auto data = resp->isValid() ? formatByteArray(_dataDisplayMode, resp->data()) : "???";
                addItem(tr("<b>Sub-function:</b> %1").arg(subFunc));
                addItem(tr("<b>Data:</b> %1").arg(data));
            }
            break;

        case QModbusPdu::GetCommEventCounter:
            if(!_msg->isRequest())
            {
                auto resp = reinterpret_cast<const GetCommEventCounterResponse*>(_msg);
                const auto status = resp->isValid() ? formatWordValue(_dataDisplayMode, resp->status()) : "??";
                const auto eventCount = resp->isValid() ? formatWordValue(_dataDisplayMode, resp->eventCount()) : "??";
                addItem(tr("<b>Status:</b> %1").arg(status));
                addItem(tr("<b>Event Count:</b> %1").arg(eventCount));
            }
            break;

        case QModbusPdu::GetCommEventLog:
            if(!_msg->isRequest())
            {
                auto resp = reinterpret_cast<const GetCommEventLogResponse*>(_msg);
                const auto byteCount = resp->isValid() ? formatByteValue(_dataDisplayMode, resp->byteCount()) : "?";
                const auto status = resp->isValid() ? formatWordValue(_dataDisplayMode, resp->status()) : "??";
                const auto eventCount = resp->isValid() ? formatWordValue(_dataDisplayMode, resp->eventCount()) : "??";
                const auto messageCount = resp->isValid() ? formatWordValue(_dataDisplayMode, resp->messageCount()) : "??";
                const auto events = resp->isValid() ? formatByteArray(_dataDisplayMode, resp->events()) : "???";
                addItem(tr("<b>Byte Count:</b> %1").arg(byteCount));
                addItem(tr("<b>Status:</b> %1").arg(status));
                addItem(tr("<b>Event Count:</b> %1").arg(eventCount));
                addItem(tr("<b>Message Count:</b> %1").arg(messageCount));
                addItem(tr("<b>Events:</b> %1").arg(events));
            }
            break;

        case QModbusPdu::WriteMultipleCoils:
            if(_msg->isRequest())
            {
                auto req = reinterpret_cast<const WriteMultipleCoilsRequest*>(_msg);
                const auto startAddr = req->isValid() ? formatWordValue(_dataDisplayMode, req->startAddress()) : "??";
                const auto quantity = req->isValid() ? formatWordValue(_dataDisplayMode, req->quantity()) : "??";
                const auto byteCount = req->isValid() ? formatByteValue(_dataDisplayMode, req->byteCount()) : "?";
                const auto values = req->isValid() ? formatByteArray(_dataDisplayMode, req->values()) : "???";
                addItem(tr("<b>Starting Address:</b> %1").arg(startAddr));
                addItem(tr("<b>Quantity of Outputs:</b> %1").arg(quantity));
                addItem(tr("<b>Byte Count:</b> %1").arg(byteCount));
                addItem(tr("<b>Output Value:</b> %1").arg(values));
            }
            else
            {
                auto resp = reinterpret_cast<const WriteMultipleCoilsResponse*>(_msg);
                const auto startAddr = resp->isValid() ? formatWordValue(_dataDisplayMode, resp->startAddress()) : "??";
                const auto quantity = resp->isValid() ? formatWordValue(_dataDisplayMode, resp->quantity()) : "??";
                addItem(tr("<b>Starting Address:</b> %1").arg(startAddr));
                addItem(tr("<b>Quantity of Outputs:</b> %1").arg(quantity));
            }
            break;

        case QModbusPdu::WriteMultipleRegisters:
            if(_msg->isRequest())
            {
                auto req = reinterpret_cast<const WriteMultipleRegistersRequest*>(_msg);
                const auto startAddr = req->isValid() ? formatWordValue(_dataDisplayMode, req->startAddress()) : "??";
                const auto quantity = req->isValid() ? formatWordValue(_dataDisplayMode, req->quantity()) : "??";
                const auto byteCount = req->isValid() ? formatByteValue(_dataDisplayMode, req->byteCount()) : "?";
                const auto values = req->isValid() ? formatWordArray(_dataDisplayMode, req->values(), _byteOrder) : "???";
                addItem(tr("<b>Starting Address:</b> %1").arg(startAddr));
                addItem(tr("<b>Quantity of Registers:</b> %1").arg(quantity));
                addItem(tr("<b>Byte Count:</b> %1").arg(byteCount));
                addItem(tr("<b>Registers Value:</b> %1").arg(values));
            }
            else
            {
                auto resp = reinterpret_cast<const WriteMultipleRegistersResponse*>(_msg);
                const auto startAddr = resp->isValid() ? formatWordValue(_dataDisplayMode, resp->startAddress()) : "??";
                const auto quantity = resp->isValid() ? formatWordValue(_dataDisplayMode, resp->quantity()) : "??";
                addItem(tr("<b>Starting Address:</b> %1").arg(startAddr));
                addItem(tr("<b>Quantity of Registers:</b> %1").arg(quantity));
            }
        break;

        case QModbusPdu::ReportServerId:
            if(!_msg->isRequest())
            {
                auto resp = reinterpret_cast<const ReportServerIdResponse*>(_msg);
                const auto byteCount = resp->isValid() ? formatByteValue(_dataDisplayMode, resp->byteCount()) : "?";
                const auto data = resp->isValid() ? formatByteArray(_dataDisplayMode, resp->data()) : "?";
                addItem(tr("<b>Byte Count:</b> %1").arg(byteCount));
                addItem(tr("<b>Data:</b> %1").arg(data));
            }
        break;

        case QModbusPdu::ReadFileRecord:
            if(_msg->isRequest())
            {
                auto req = reinterpret_cast<const ReadFileRecordRequest*>(_msg);
                const auto byteCount = req->isValid() ? formatByteValue(_dataDisplayMode, req->byteCount()) : "?";
                const auto data = req->isValid() ? formatByteArray(_dataDisplayMode, req->data()) : "?";
                addItem(tr("<b>Byte Count:</b> %1").arg(byteCount));
                addItem(tr("<b>Data:</b> %1").arg(data));
            }
            else
            {
                auto resp = reinterpret_cast<const ReadFileRecordResponse*>(_msg);
                const auto byteCount = resp->isValid() ? formatByteValue(_dataDisplayMode, resp->byteCount()) : "?";
                const auto data = resp->isValid() ? formatByteArray(_dataDisplayMode, resp->data()) : "?";
                addItem(tr("<b>Byte Count:</b> %1").arg(byteCount));
                addItem(tr("<b>Data:</b> %1").arg(data));
            }
        break;

        case QModbusPdu::WriteFileRecord:
            if(_msg->isRequest())
            {
                auto req = reinterpret_cast<const WriteFileRecordRequest*>(_msg);
                const auto length = req->isValid() ? formatByteValue(_dataDisplayMode, req->length()) : "?";
                const auto data = req->isValid() ? formatByteArray(_dataDisplayMode, req->data()) : "???";
                addItem(tr("<b>Request Data Length:</b> %1").arg(length));
                addItem(tr("<b>Data:</b> %1").arg(data));
            }
            else
            {
                auto resp = reinterpret_cast<const WriteFileRecordResponse*>(_msg);
                const auto length = resp->isValid() ? formatByteValue(_dataDisplayMode, resp->length()) : "?";
                const auto data = resp->isValid() ? formatByteArray(_dataDisplayMode, resp->data()) : "???";
                addItem(tr("<b>Response Data Length:</b> %1").arg(length));
                addItem(tr("<b>Data:</b> %1").arg(data));
            }
        break;

        case QModbusPdu::MaskWriteRegister:
            if(_msg->isRequest())
            {
                auto req = reinterpret_cast<const MaskWriteRegisterRequest*>(_msg);
                const auto address = req->isValid() ? formatWordValue(_dataDisplayMode, req->address()) : "??";
                const auto andMask = req->isValid() ? formatWordValue(_dataDisplayMode, req->andMask()) : "??";
                const auto orMask = req->isValid() ? formatWordValue(_dataDisplayMode, req->orMask()) : "??";
                addItem(tr("<b>Address:</b> %1").arg(address));
                addItem(tr("<b>And Mask:</b> %1").arg(andMask));
                addItem(tr("<b>Or Mask:</b> %1").arg(orMask));
            }
            else
            {
                auto resp = reinterpret_cast<const MaskWriteRegisterResponse*>(_msg);
                const auto address = resp->isValid() ? formatWordValue(_dataDisplayMode, resp->address()) : "??";
                const auto andMask = resp->isValid() ? formatWordValue(_dataDisplayMode, resp->andMask()) : "??";
                const auto orMask = resp->isValid() ? formatWordValue(_dataDisplayMode, resp->orMask()) : "??";
                addItem(tr("<b>Address:</b> %1").arg(address));
                addItem(tr("<b>And Mask:</b> %1").arg(andMask));
                addItem(tr("<b>Or Mask:</b> %1").arg(orMask));
            }
        break;

        case QModbusPdu::ReadWriteMultipleRegisters:
            if(_msg->isRequest())
            {
                auto req = reinterpret_cast<const ReadWriteMultipleRegistersRequest*>(_msg);
                const auto readStartAddr = req->isValid() ? formatWordValue(_dataDisplayMode, req->readStartAddress()) : "??";
                const auto readLength = req->isValid() ? formatWordValue(_dataDisplayMode, req->readLength()) : "??";
                const auto writeStartAddr = req->isValid() ? formatWordValue(_dataDisplayMode, req->writeStartAddress()) : "??";
                const auto writeLength = req->isValid() ? formatWordValue(_dataDisplayMode, req->writeLength()) : "??";
                const auto writeByteCount = req->isValid() ? formatByteValue(_dataDisplayMode, req->writeByteCount()) : "?";
                const auto writeValues = req->isValid() ? formatWordArray(_dataDisplayMode, req->writeValues(), _byteOrder) : "???";
                addItem(tr("<b>Read Starting Address:</b> %1").arg(readStartAddr));
                addItem(tr("<b>Quantity to Read:</b> %1").arg(readLength));
                addItem(tr("<b>Write Starting Address:</b> %1").arg(writeStartAddr));
                addItem(tr("<b>Quantity to Write:</b> %1").arg(writeLength));
                addItem(tr("<b>Write Byte Count:</b> %1").arg(writeByteCount));
                addItem(tr("<b>Write Registers Value:</b> %1").arg(writeValues));
            }
            else
            {
                auto resp = reinterpret_cast<const ReadWriteMultipleRegistersResponse*>(_msg);
                const auto byteCount = resp->isValid() ? formatByteValue(_dataDisplayMode, resp->byteCount()): "?";
                const auto values = resp->isValid() ? formatWordArray(_dataDisplayMode, resp->values(), _byteOrder) : "???";
                addItem(tr("<b>Byte Count:</b> %1").arg(byteCount));
                addItem(tr("<b>Registers Value:</b> %1").arg(values));
            }
        break;

        case QModbusPdu::ReadFifoQueue:
            if(_msg->isRequest())
            {
                auto req = reinterpret_cast<const ReadFifoQueueRequest*>(_msg);
                const auto fifoAddr = req->isValid() ? formatWordValue(_dataDisplayMode, req->fifoAddress()) : "??";
                addItem(tr("<b>FIFO Point Address:</b> %1").arg(fifoAddr));
            }
            else
            {
                auto resp = reinterpret_cast<const ReadFifoQueueResponse*>(_msg);
                const auto byteCount = resp->isValid() ? formatByteValue(_dataDisplayMode, resp->byteCount()) : "?";
                const auto fifoCount = resp->isValid() ? formatByteValue(_dataDisplayMode, resp->fifoCount()) : "?";
                const auto fifoValue = resp->isValid() ? formatWordArray(_dataDisplayMode, resp->fifoValue(), _byteOrder) : "???";
                addItem(tr("<b>Byte Count:</b> %1").arg(byteCount));
                addItem(tr("<b>FIFO Count:</b> %1").arg(fifoCount));
                addItem(tr("<b>FIFO Value Register:</b> %1").arg(fifoValue));
            }
        break;

        default:
        {
            const auto data = _msg->isValid() ? formatByteArray(_dataDisplayMode, _msg->rawData()) : "???";
            addItem(tr("<b>Data:</b> %1").arg(data));
        }
        break;
    }
}
