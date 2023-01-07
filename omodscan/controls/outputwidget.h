#ifndef OUTPUTWIDGET_H
#define OUTPUTWIDGET_H

#include <QFile>
#include <QWidget>
#include <QListWidgetItem>
#include <QModbusReply>
#include "enums.h"
#include "displaydefinition.h"

namespace Ui {
class OutputWidget;
}

///
/// \brief The OutputWidget class
///
class OutputWidget : public QWidget
{
    Q_OBJECT

public:  
    explicit OutputWidget(QWidget *parent = nullptr);
    ~OutputWidget();

    QVector<quint16> data() const;

    void setup(const DisplayDefinition& dd);

    DisplayMode displayMode() const;
    void setDisplayMode(DisplayMode mode);

    DataDisplayMode dataDisplayMode() const;
    void setDataDisplayMode(DataDisplayMode mode);

    bool displayHexAddreses() const;
    void setDisplayHexAddreses(bool on);

    CaptureMode captureMode() const;
    void startTextCapture(const QString& file);
    void stopTextCapture();

    QColor backgroundColor() const;
    void setBackgroundColor(const QColor& clr);

    QColor foregroundColor() const;
    void setForegroundColor(const QColor& clr);

    QColor statusColor() const;
    void setStatusColor(const QColor& clr);

    void setStatus(const QString& status);

    void update(const QModbusRequest& request, int server);
    void update(QModbusReply* reply);

signals:
    void itemDoubleClicked(quint32 address, const QVariant& value);

private slots:
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

private:
    void captureString(const QString& s);
    void updateDataWidget(const QModbusDataUnit& data);
    void updateTrafficWidget(bool request, int server, const QModbusPdu& pdu);

private:
    Ui::OutputWidget *ui;

private:
    bool _displayHexAddreses;
    DisplayMode _displayMode;
    DataDisplayMode _dataDisplayMode;
    DisplayDefinition _displayDefinition;
    QModbusDataUnit _lastData;
    QFile _fileCapture;
};

#endif // OUTPUTWIDGET_H
