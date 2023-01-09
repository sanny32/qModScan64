#include "modbuslimits.h"
#include "mainwindow.h"
#include "formmodsca.h"
#include "ui_formmodsca.h"
#include "dialogwritecoilregister.h"
#include "dialogwriteholdingregister.h"
#include "dialogwriteholdingregisterbits.h"

///
/// \brief FormModSca::FormModSca
/// \param num
/// \param parent
///
FormModSca::FormModSca(int num, ModbusClient& client, MainWindow* parent) :
    QWidget(parent)
    , ui(new Ui::FormModSca)
    ,_formId(num)
    ,_modbusClient(client)
{
    Q_ASSERT(parent != nullptr);

    ui->setupUi(this);
    setWindowTitle(QString("ModSca%1").arg(_formId));

    ui->lineEditAddress->setPaddingZeroes(true);
    ui->lineEditAddress->setInputRange(ModbusLimits::addressRange());
    ui->lineEditAddress->setValue(1);

    ui->lineEditLength->setInputRange(ModbusLimits::lengthRange());
    ui->lineEditLength->setValue(50);

    ui->lineEditDeviceId->setInputRange(ModbusLimits::slaveRange());
    ui->lineEditDeviceId->setValue(1);

    ui->outputWidget->setup(displayDefinition());
    ui->outputWidget->setFocus();

    connect(&_modbusClient, &ModbusClient::modbusRequest, this, &FormModSca::on_modbusRequest);
    connect(&_modbusClient, &ModbusClient::modbusReply, this, &FormModSca::on_modbusReply);
    connect(&_timer, &QTimer::timeout, this, &FormModSca::on_timeout);

    _timer.setInterval(1000);
    _timer.start();
}

///
/// \brief FormModSca::~FormModSca
///
FormModSca::~FormModSca()
{
    delete ui;
}

///
/// \brief FormModSca::data
/// \return
///
QVector<quint16> FormModSca::data() const
{
    return ui->outputWidget->data();
}

///
/// \brief FormModSca::displayDefinition
/// \return
///
DisplayDefinition FormModSca::displayDefinition() const
{
    DisplayDefinition dd;
    dd.ScanRate = _timer.interval();
    dd.DeviceId = ui->lineEditDeviceId->value<int>();
    dd.PointAddress = ui->lineEditAddress->value<int>();
    dd.PointType = ui->comboBoxModbusPointType->currentPointType();
    dd.Length = ui->lineEditLength->value<int>();

    return dd;
}

///
/// \brief FormModSca::setDisplayDefinition
/// \param dd
///
void FormModSca::setDisplayDefinition(const DisplayDefinition& dd)
{
    _timer.setInterval(qBound(20U, dd.ScanRate, 10000U));
    ui->lineEditDeviceId->setValue(dd.DeviceId);
    ui->lineEditAddress->setValue(dd.PointAddress);
    ui->lineEditLength->setValue(dd.Length);
    ui->comboBoxModbusPointType->setCurrentPointType(dd.PointType);

    ui->outputWidget->setStatus("Data Uninitialized");
    ui->outputWidget->setup(dd);
}

///
/// \brief FormModSca::displayMode
/// \return
///
DisplayMode FormModSca::displayMode() const
{
    return ui->outputWidget->displayMode();
}

///
/// \brief FormModSca::setDisplayMode
/// \param mode
///
void FormModSca::setDisplayMode(DisplayMode mode)
{
    ui->outputWidget->setDisplayMode(mode);
}

///
/// \brief FormModSca::dataDisplayMode
/// \return
///
DataDisplayMode FormModSca::dataDisplayMode() const
{
    return ui->outputWidget->dataDisplayMode();
}

///
/// \brief FormModSca::displayHexAddreses
/// \return
///
bool FormModSca::displayHexAddreses() const
{
    return ui->outputWidget->displayHexAddreses();
}

///
/// \brief FormModSca::setDisplayHexAddreses
/// \param on
///
void FormModSca::setDisplayHexAddreses(bool on)
{
    ui->outputWidget->setDisplayHexAddreses(on);
}

///
/// \brief FormModSca::setDataDisplayMode
/// \param mode
///
void FormModSca::setDataDisplayMode(DataDisplayMode mode)
{
    ui->outputWidget->setDataDisplayMode(mode);
}

///
/// \brief FormModSca::captureMode
///
CaptureMode FormModSca::captureMode() const
{
    return ui->outputWidget->captureMode();
}

///
/// \brief FormModSca::startTextCapture
/// \param file
///
void FormModSca::startTextCapture(const QString& file)
{
    ui->outputWidget->startTextCapture(file);
}

///
/// \brief FormModSca::stopTextCapture
///
void FormModSca::stopTextCapture()
{
   ui->outputWidget->stopTextCapture();
}

///
/// \brief FormModSca::backgroundColor
/// \return
///
QColor FormModSca::backgroundColor() const
{
    return ui->outputWidget->backgroundColor();
}

///
/// \brief FormModSca::setBackgroundColor
/// \param clr
///
void FormModSca::setBackgroundColor(QColor clr)
{
    ui->outputWidget->setBackgroundColor(clr);
}

///
/// \brief FormModSca::foregroundColor
/// \return
///
QColor FormModSca::foregroundColor() const
{
    return ui->outputWidget->foregroundColor();
}

///
/// \brief FormModSca::setForegroundColor
/// \param clr
///
void FormModSca::setForegroundColor(QColor clr)
{
    ui->outputWidget->setForegroundColor(clr);
}

///
/// \brief FormModSca::statusColor
/// \return
///
QColor FormModSca::statusColor() const
{
    return ui->outputWidget->statusColor();
}

///
/// \brief FormModSca::setStatusColor
/// \param clr
///
void FormModSca::setStatusColor(QColor clr)
{
    ui->outputWidget->setStatusColor(clr);
}

///
/// \brief FormModSca::font
/// \return
///
QFont FormModSca::font() const
{
   return ui->outputWidget->font();
}

///
/// \brief FormModSca::setFont
/// \param font
///
void FormModSca::setFont(const QFont& font)
{
    ui->outputWidget->setFont(font);
}

///
/// \brief FormModSca::resetCtrls
///
void FormModSca::resetCtrs()
{
    ui->statisticWidget->resetCtrs();
}

///
/// \brief FormModSca::on_timeout
///
void FormModSca::on_timeout()
{
    if(!_modbusClient.isValid()) return;
    if(_modbusClient.state() != QModbusDevice::ConnectedState)
    {
        ui->outputWidget->setStatus("Device NOT CONNECTED!");
        return;
    }

    const auto dd = displayDefinition();
    if(dd.PointAddress + dd.Length - 1 > ModbusLimits::addressRange().to())
    {
        ui->outputWidget->setStatus("Invalid Data Length Specified");
        return;
    }

    _modbusClient.sendReadRequest(dd.PointType, dd.PointAddress - 1, dd.Length, dd.DeviceId, _formId);
}

///
/// \brief FormModSca::on_modbusReply
/// \param reply
///
void FormModSca::on_modbusReply(QModbusReply* reply)
{
    if(!reply) return;

    if(_formId != reply->property("RequestId").toInt())
    {
        return;
    }

    ui->outputWidget->update(reply);

    if(reply->error() == QModbusDevice::NoError)
    {
        ui->statisticWidget->increaseValidSlaveResponses();
    }
}

///
/// \brief FormModSca::on_modbusRequest
/// \param requestId
/// \param request
///
void FormModSca::on_modbusRequest(int requestId, const QModbusRequest& request)
{
    if(requestId != _formId)
    {
        return;
    }

    const auto deviceId = ui->lineEditDeviceId->value<int>();
    ui->outputWidget->update(request, deviceId);
    ui->statisticWidget->increaseNumberOfPolls();
}

///
/// \brief FormModSca::on_lineEditAddress_valueChanged
///
void FormModSca::on_lineEditAddress_valueChanged(const QVariant&)
{
    ui->outputWidget->setup(displayDefinition());
}

///
/// \brief FormModSca::on_lineEditLength_valueChanged
///
void FormModSca::on_lineEditLength_valueChanged(const QVariant&)
{
    ui->outputWidget->setup(displayDefinition());
}

///
/// \brief FormModSca::on_lineEditDeviceId_valueChanged
///
void FormModSca::on_lineEditDeviceId_valueChanged(const QVariant&)
{
}

///
/// \brief FormModSca::on_comboBoxModbusPointType_pointTypeChanged
///
void FormModSca::on_comboBoxModbusPointType_pointTypeChanged(QModbusDataUnit::RegisterType)
{
    ui->outputWidget->setup(displayDefinition());
}

///
/// \brief FormModSca::on_outputWidget_itemDoubleClicked
/// \param addr
/// \param value
///
void FormModSca::on_outputWidget_itemDoubleClicked(quint32 addr, const QVariant& value)
{
    const auto mode = dataDisplayMode();
    const quint32 node = ui->lineEditDeviceId->value<int>();
    const auto pointType = ui->comboBoxModbusPointType->currentPointType();
    switch(pointType)
    {
        case QModbusDataUnit::Coils:
        {
            ModbusWriteParams params = { node, addr, value, mode };
            DialogWriteCoilRegister dlg(params, this);
            if(dlg.exec() == QDialog::Accepted)
                _modbusClient.writeRegister(pointType, params, _formId);
        }
        break;

        case QModbusDataUnit::HoldingRegisters:
        {
            ModbusWriteParams params = { node, addr, value, mode};
            if(mode == DataDisplayMode::Binary)
            {
                DialogWriteHoldingRegisterBits dlg(params, this);
                if(dlg.exec() == QDialog::Accepted)
                    _modbusClient.writeRegister(pointType, params, _formId);
            }
            else
            {
                DialogWriteHoldingRegister dlg(params, mode, this);
                if(dlg.exec() == QDialog::Accepted)
                    _modbusClient.writeRegister(pointType, params, _formId);
            }
        }
        break;

        default:
        break;
    }
}