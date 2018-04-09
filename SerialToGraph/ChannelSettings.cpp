#include "ChannelSettings.h"
#include <Axis.h>
#include <AxisChooseDialog.h>
#include <AxisSettings.h>
#include <ChannelBase.h>
#include <ChannelWidget.h>
#include <GlobalSettings.h>
#include <graphics/ChannelProxyBase.h>
#include <graphics/GraphicsContainer.h>
#include <graphics/HwChannelProxy.h>
#include <graphics/SampleChannelProxy.h>
#include <hw/Sensor.h>
#include <hw/SensorManager.h>
#include <hw/SensorQuantity.h>
#include <HwChannel.h>
#include <Measurement.h>
#include <memory>
#include <MainWindow.h>
#include <MyMessageBox.h>
#include <Plot.h>
#include <bases/ComboBox.h>
#include <QCheckBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QLocale>
#include <QPushButton>
#include <QSettings>
#include <QString>
#include <SampleChannel.h>

QString ChannelSettings::_GetQuantityString(hw::SensorQuantity *quantity)
{
    if (quantity->GetName() == "")
        return "";
    if (quantity->GetName() == "Current")
        return tr("Current");
    if (quantity->GetName() == "Humidity")
        return tr("Humidity");
    if (quantity->GetName() == "Pressure")
        return tr("Pressure");
    if (quantity->GetName() == "Temperature")
        return tr("Temperature");
    if (quantity->GetName() == "Voltage")
        return tr("Voltage");

    return quantity->GetName(); //TODO: TFs mod: Remove!!!

    qWarning() << "unknown quantity to translate " << quantity->GetName();
}

ChannelSettings::ChannelSettings(
    QVector<Measurement *> measurements,
    GraphicsContainer *graphicsContainer,
    ChannelProxyBase *channelProxy,
    hw::SensorManager *sensorManager
) :
    bases::FormDialogColor (channelProxy->GetWidget(), tr("Channel settings"), GlobalSettings::GetInstance().GetAcceptChangesByDialogClosing()),
    m_measurements(measurements),
    m_graphicsContainer(graphicsContainer),
    m_channelProxy(channelProxy),
    m_currentValueControl(NULL),
    m_name(NULL),
    m_units(NULL),
    m_measurementCombo(NULL),
    m_channelCombo(NULL),
    m_shapeComboBox(NULL),
    m_axisComboBox(NULL),
    m_style(NULL),
    m_timeUnits(NULL),
    m_format(NULL),
    m_penStyle(NULL),
    m_sensorQuantityComboBox(NULL),
    m_sensorNameComboBox(NULL),
    m_sensorPortComboBox(NULL),
    m_currentValueChanged(false),
    m_currentValue(ChannelBase::GetNaValue()),
    m_sensorManager(sensorManager)
{
    if (m_channelProxy->isGhost())
    {
        _InitializeGhostCombos();
        AddSeparator();
    }

    if (m_channelProxy->isGhost())
    {
        _FillMeasurementCombo();
        connect(m_measurementCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(fillChannelCombo(int)));
        connect(m_channelCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(loadFromOriginalWidget(int)));
    }

    m_name = new QLineEdit(m_channelProxy->GetName(), this);
    m_units = new QLineEdit(m_channelProxy->GetUnits(), this);

    if (dynamic_cast<SampleChannelProxy*>(channelProxy))
    {
        m_name->setVisible(false);
        _InitializeTimeFeatures();
        AddSeparator();
    }
    else
    {
        if (dynamic_cast<HwChannelProxy*>(channelProxy))
        {
            _InitializeValueLine(channelProxy);
            AddSeparator();
            _InitializeSensorItems();
            AddSeparator();
        }
        m_formLayout->addRow(new QLabel(tr("Title"), this), m_name);
        m_formLayout->addRow(new QLabel(tr("Units"), this), m_units);
    }
    AddColorButtonRow(channelProxy->GetForeColor());
    _InitializeAxisCombo();
    _InitializeShapeCombo(channelProxy);
    _InitializePenStyle(channelProxy->GetPenStyle());
}

void ChannelSettings::_InitializeSensorItem(bases::ComboBox **item, QString const &label, const char* slot)
{
    (*item) = new bases::ComboBox(this);
    (*item)->setEnabled(m_channelProxy->GetChannelMeasurement()->GetState() == Measurement::Ready);
    m_formLayout->addRow(new QLabel(label, this), (*item));
    connect((*item), SIGNAL(currentIndexChanged(int)), this, slot);
}

void ChannelSettings::_FillSensorPortCB()
{
    m_sensorPortComboBox->addItem(tr("Undefined"), hw::SensorManager::nonePortId);
    if (m_sensorManager->GetSensors().size() == 1)
        return; //sensors are not defined

    for (unsigned i = 1; i <= hw::SensorManager::sensorPortCount; ++i)
    {
        m_sensorPortComboBox->addItem(_GetPortName(i), i);
        if (i == dynamic_cast<HwChannelProxy*>(m_channelProxy)->GetSensorPort())
            m_sensorPortComboBox->setCurrentIndex(m_sensorPortComboBox->count() - 1);
    }
}

void ChannelSettings::_FillSensorNameCB()
{
    unsigned currentSensorId = dynamic_cast<HwChannelProxy*>(m_channelProxy)->GetSensor()->GetId();
    m_sensorNameComboBox->clear();
    foreach (hw::Sensor *sensor, m_sensorManager->GetSensors())
    {

        if (sensor->GetId() == hw::Sensor::noSensorId)
        {
            if (m_sensorPortComboBox->currentData().toInt() == hw::SensorManager::nonePortId)
            {
                m_sensorNameComboBox->addItem(sensor->GetName(), sensor->GetId());
                m_sensorNameComboBox->setEnabled(false);
                return;
            }
        }
        else
        {
            m_sensorNameComboBox->addItem(sensor->GetName(), sensor->GetId());
        }

        if (currentSensorId == sensor->GetId())
        {
            m_sensorNameComboBox->setCurrentIndex(m_sensorNameComboBox->count() - 1);
        }
    }
    m_sensorNameComboBox->setEnabled(m_channelProxy->GetChannelMeasurement()->GetState() == Measurement::Ready);
}

void ChannelSettings::_FillSensorQuanitityCB()
{
    hw::SensorQuantity *currentSensorQuanity = ((HwChannelProxy*)m_channelProxy)->GetSensorQuantity();
    m_sensorQuantityComboBox->clear();
    unsigned currentSensorOrder = m_sensorNameComboBox->currentData().toInt();

    foreach (hw::Sensor *sensor, m_sensorManager->GetSensors())
    {
        if (sensor->GetId() == currentSensorOrder)
        {
            foreach (hw::SensorQuantity *quantity, sensor->GetQuantities())
            {
                m_sensorQuantityComboBox->addItem(_GetQuantityString(quantity), quantity->GetId());
                if (currentSensorQuanity == quantity)
                {
                    m_sensorQuantityComboBox->setCurrentIndex(m_sensorQuantityComboBox->count() - 1);
                }
            }
            break;
        }
    }
    m_sensorQuantityComboBox->setEnabled(
        m_channelProxy->GetChannelMeasurement()->GetState() == Measurement::Ready &&
        m_sensorQuantityComboBox->count() > 1
    );
}

void ChannelSettings::_InitializeSensorItems()
{
    _InitializeSensorItem(&m_sensorPortComboBox, tr("Sensor Port"), SLOT(sensorPortChanged(int)));
    _InitializeSensorItem(&m_sensorNameComboBox, tr("Sensor Name"), SLOT(sensorNameChanged(int)));
    _InitializeSensorItem(&m_sensorQuantityComboBox, tr("Sensor Quantity"), SLOT(sensorQualityChanged(int)));

    _FillSensorPortCB();
    _FillSensorNameCB();
    _FillSensorQuanitityCB();

}

void ChannelSettings::sensorPortChanged(int index)
{
    Q_UNUSED(index)
    if (
        (m_sensorPortComboBox->currentData().toInt() == hw::SensorManager::nonePortId) ==
        !(m_sensorNameComboBox->currentData().toInt() == hw::Sensor::noSensorId))
    {
        _FillSensorNameCB();
    }
}

void ChannelSettings::sensorNameChanged(int index)
{
    Q_UNUSED(index)
    _FillSensorQuanitityCB();
}

void ChannelSettings::sensorQualityChanged(int index)
{
    Q_UNUSED(index)

}

void ChannelSettings::_InitializeGhostCombos()
{
    m_measurementCombo = new bases::ComboBox(this);
    m_formLayout->addRow(new QLabel(tr("Measurement"), this), m_measurementCombo);

    m_channelCombo = new bases::ComboBox(this);
    m_formLayout->addRow(new QLabel(tr("Channel"), this), m_channelCombo);
}

void ChannelSettings::_FillMeasurementCombo()
{
    Measurement *originalMeasurement = m_channelProxy->GetChannelMeasurement();

    unsigned comboIndex = 0;
    unsigned measurementIndex = ~0;
    foreach (Measurement *item, m_measurements)
    {
        measurementIndex++;
        if (item->GetWidget() == m_graphicsContainer)
            continue; //it is not possible to select channels from current gc

        m_measurementCombo->addItem(item->GetWidget()->GetName(), measurementIndex);
        if (item == originalMeasurement)
        {
            m_measurementCombo->setCurrentIndex(comboIndex);
            fillChannelCombo(comboIndex);
        }
        comboIndex++;
    }
}

void ChannelSettings::fillChannelCombo(int measurementComboIndex)
{
    Q_UNUSED(measurementComboIndex)
    Measurement *currentMeasurement = m_measurements[m_measurementCombo->currentData().toInt()];
    GraphicsContainer *currentGC = currentMeasurement->GetWidget();


    bool channelFound = false;
    for (unsigned index = 0; index < currentMeasurement->GetChannelCount(); ++index)
    {
        ChannelBase * iteratingChannel = currentMeasurement->GetChannel(index);
        m_channelCombo->addItem(currentGC->GetChannelWidget(iteratingChannel)->GetName(), index);
        if (iteratingChannel == m_channelProxy->GetChannel())
        {
            m_channelCombo->setCurrentIndex(index);
            channelFound = true;
        }
    }
    if (!channelFound)
    {
        m_channelCombo->setCurrentIndex(1); //skip samples
    }
    loadFromOriginalWidget(m_channelCombo->currentIndex());
}

void ChannelSettings::loadFromOriginalWidget(int channelComboIndex)
{
    Q_UNUSED(channelComboIndex)
    Measurement *originalMeasurement = m_measurements[m_measurementCombo->currentData().toInt()];
    GraphicsContainer *originalGC = originalMeasurement->GetWidget();
    ChannelBase *originalChannel = originalMeasurement->GetChannel(m_channelCombo->currentData().toInt());
    ChannelProxyBase *originalChannelProxy = originalGC->GetChannelProxy(originalChannel);

    if (m_name)
    {
        m_name->setText(m_graphicsContainer->GetGhostName(originalGC, originalChannelProxy));
    }
    if (m_units)
    {
        m_units->setText(originalChannelProxy->GetUnits());
    }
    if (m_colorButtonWidget)
    {
        SetColorButtonColor(originalChannelProxy->GetForeColor());
    }
    if (m_shapeComboBox)
    {
        m_shapeComboBox->setCurrentIndex(originalChannelProxy->GetChannelGraph()->GetShapeIndex());
    }
}

unsigned ChannelSettings::_GetCurrentValueIndex(ChannelProxyBase *channelProxy)
{
    double currentHorizontalValue =
        m_graphicsContainer->GetHorizontalValueBySliderPos(m_graphicsContainer->GetCurrentIndex());
    return m_graphicsContainer->GetHorizontalChannelProxy(
        channelProxy->GetChannelMeasurement())->GetLastValueIndex(currentHorizontalValue);
}

void ChannelSettings::_InitializeValueLine(ChannelProxyBase *channelProxy)
{
    QHBoxLayout *curValLayout = new QHBoxLayout();
    m_currentValueControl = new QLineEdit(this);
    curValLayout->addWidget(m_currentValueControl);

    QPushButton *originlValue = new QPushButton(tr("Original"), this);
    connect(originlValue, SIGNAL(clicked(bool)), this, SLOT(setOriginalValue(bool)));
    curValLayout->addWidget(originlValue);

    QPushButton *naValue = new QPushButton(tr("n/a"), this);
    connect(naValue, SIGNAL(clicked(bool)), this, SLOT(setNaValue(bool)));
    curValLayout->addWidget(naValue);

    unsigned currentIndex = _GetCurrentValueIndex(channelProxy);
    if (currentIndex < (int)channelProxy->GetValueCount())
    {
        QLocale locale(QLocale::system());
        QString currentValueStr = (channelProxy->IsValueNA(currentIndex)) ?
            channelProxy->GetNAValueString() : locale.toString(channelProxy->GetValue(currentIndex));

        m_currentValueControl->setText(currentValueStr);
    }
    else
    {
        m_currentValueControl->setDisabled(true);
        originlValue->setDisabled(true);
        naValue->setDisabled(true);
    }
    //must be defined after setTest
    connect(m_currentValueControl, SIGNAL(textChanged(QString)), this, SLOT(currentValueChanged(QString)));


    m_formLayout->addRow(new QLabel(tr("Current Value"), this), curValLayout);
}

void ChannelSettings::setOriginalValue(bool checked)
{
    Q_UNUSED(checked);
    //this method is called just in a case the original value box is  enabled and
    //it is just in the case index is in range of this channel
    QLocale locale(QLocale::system());
    double currentValue = ((HwChannelProxy*)m_channelProxy)->GetOriginalValue(_GetCurrentValueIndex(m_channelProxy));
    m_currentValueControl->setText(
        (currentValue == ChannelBase::GetNaValue()) ?
            ChannelWidget::GetNAValueString() :
            locale.toString(currentValue)
    );

    //was changed in currentValueChanged
    m_currentValue = currentValue;
}

void ChannelSettings::setNaValue(bool)
{
    m_currentValueControl->setText(ChannelWidget::GetNAValueString());
    m_currentValue = ChannelBase::GetNaValue();
}

void ChannelSettings::currentValueChanged(QString const &content)
{
    Q_UNUSED(content)
    QLocale locale(QLocale::system());
    m_currentValue = locale.toDouble(m_currentValueControl->text());
    m_currentValueChanged = true;
}

void ChannelSettings::_InitializePenStyle(Qt::PenStyle selected)
{
    m_penStyle = new bases::ComboBox(this);
    m_penStyle->addItem(tr("No Line"));
    m_penStyle->addItem(tr("Solid Line"));
    m_penStyle->addItem(tr("Dash Line"));
    m_penStyle->addItem(tr("Dot Line"));
    m_penStyle->addItem(tr("Dash Dot Line"));
    m_penStyle->addItem(tr("Dash Dot Dot Line"));
    m_penStyle->setCurrentIndex((int)selected);

    m_formLayout->addRow(new QLabel(tr("Pen Style"), this), m_penStyle);
}

void ChannelSettings::_InitializeTimeFeatures()
{
    SampleChannelProxy * channelProxy = (SampleChannelProxy*)m_channelProxy;

    m_style = new bases::ComboBox(this);
    m_style->addItem(SampleChannelProxy::GetSampleChannelStyleText(SampleChannelProxy::Samples), false);
    m_style->addItem(SampleChannelProxy::GetSampleChannelStyleText(SampleChannelProxy::TimeOffset), false);
    m_style->addItem(SampleChannelProxy::GetSampleChannelStyleText(SampleChannelProxy::RealTime), true); //RealTime state as data
    m_style->setCurrentIndex(channelProxy->GetStyle());//unfortunately I cant use a template with a Qt class
    connect(m_style, SIGNAL(currentIndexChanged(int)), this, SLOT(styleChanged(int)));
    m_formLayout->addRow(new QLabel(tr("Style"), this), m_style);

    m_units->setVisible(false); //there will be diplayed timeUnits combo box instead units
    m_timeUnits = new bases::ComboBox(this);
    m_timeUnits->addItem(tr("Microseconds"));
    m_timeUnits->addItem(tr("Miliseconds"));
    m_timeUnits->addItem(tr("Seconds"));
    m_timeUnits->addItem(tr("Minuts"));
    m_timeUnits->addItem(tr("Hours"));
    m_timeUnits->addItem(tr("Days"));
    m_timeUnits->setCurrentIndex(channelProxy->GetTimeUnits());
    m_timeUnits->setEnabled(channelProxy->GetStyle() == SampleChannelProxy::TimeOffset);
    m_formLayout->addRow(new QLabel(tr("Units"), this), m_timeUnits);

    m_format = new bases::ComboBox(this);
    m_format->addItem(tr("day.month.year"));
    m_format->addItem(tr("day.month.hour:minute"));
    m_format->addItem(tr("hour:minute:second"));
    m_format->addItem(tr("minute:second.milisecond"));
    m_format->setCurrentIndex(channelProxy->GetRealTimeFormat());
    m_format->setEnabled(channelProxy->GetStyle() == SampleChannelProxy::RealTime);
    m_formLayout->addRow(new QLabel(tr("Format"), this), m_format);
}

void ChannelSettings::styleChanged(int index)
{
    m_timeUnits->setEnabled((SampleChannelProxy::Style)index == SampleChannelProxy::TimeOffset);
    m_format->setEnabled((SampleChannelProxy::Style)index == SampleChannelProxy::RealTime);
    _RefillAxisCombo(); //on axis with RealTime channel must not be another channel
}

bool ChannelSettings::_AxisCheckForRealTimeMode()
{
    if (m_style != NULL && m_style->currentData().toBool()) //channel with realtime style
    {
        Axis *axis = ((Axis *)m_axisComboBox->currentData().toLongLong());
        if (!axis->IsHorizontal() && !axis->IsEmptyExcept(m_channelProxy))
        {
            MyMessageBox::critical(
                this,
                tr("Time format channel must be placed on a separate axis. Please, choose one.")
            );
            return false;
        }
    }
    return true;
}
bool ChannelSettings::BeforeAccept()
{
    if (!_AxisCheckForRealTimeMode())
        return false;

    bool changed = false;
    bool changedHorizontal = false;
    bool rescaleAxis = false;

    Axis *axis = (Axis *)m_axisComboBox->currentData().toLongLong();
    if (m_channelProxy->GetChannelGraph()->GetValuleAxis() != axis)
    {
        if (axis->IsHorizontal())
        {
            if (!_MoveLastHorizontalToVertical())
                return false; //no axis has been selected

            changedHorizontal = true;
        }

        Axis *lastAxis = m_channelProxy->GetChannelGraph()->GetValuleAxis();
        m_channelProxy->GetChannelGraph()->AssignToAxis(axis);
        lastAxis->UpdateGraphAxisName();
        lastAxis->UpdateGraphAxisStyle();
        lastAxis->UpdateVisiblility();
        m_graphicsContainer->GetPlot()->RescaleAxis(lastAxis->GetGraphAxis());

        rescaleAxis = true;
        changed = true;
    }

    HwChannelProxy *hwChannelProxy = dynamic_cast<HwChannelProxy*>(m_channelProxy);
    if (hwChannelProxy && m_currentValueChanged)
    {
        QLocale locale(QLocale::system());
        bool ok;
        locale.toDouble(m_currentValueControl->text(), &ok);
        if (m_currentValue != ChannelBase::GetNaValue() && !ok)
        {
            MyMessageBox::information(this, tr("Current value is not a number."));
            return false;
        }
        changed = true;
        unsigned currentIndex = _GetCurrentValueIndex(m_channelProxy);
        hwChannelProxy->ChangeValue(currentIndex, m_currentValue);
    }
    if (m_channelProxy->GetName() != m_name->text() && m_channelProxy->GetType() != ChannelBase::Type_Sample)
    {
        changed = true;
        m_channelProxy->SetName(m_name->text());
    }

    if (m_channelProxy->GetForeColor() != m_color)
    {
        changed = true;
        m_channelProxy->SetForeColor(m_color);
    }

    if (m_channelProxy->GetChannelGraph()->GetShapeIndex() != (unsigned)m_shapeComboBox->currentData().toInt())
    {
        changed = true;
        m_channelProxy->SetShapeIndex(m_shapeComboBox->currentData().toInt());
        rescaleAxis = true;

    }

    SampleChannelProxy *sampleChannelProxy = dynamic_cast<SampleChannelProxy*>(m_channelProxy);
    if (sampleChannelProxy)
    {
        if ((int)sampleChannelProxy->GetTimeUnits() != m_timeUnits->currentIndex())
        {
            changed = true;
            sampleChannelProxy->SetTimeUnits((SampleChannelProxy::TimeUnits)m_timeUnits->currentIndex());
        }

        if ((int)sampleChannelProxy->GetRealTimeFormat() != m_format->currentIndex())
        {
            changed = true;
            sampleChannelProxy->SetRealTimeFormat((SampleChannelProxy::RealTimeFormat)m_format->currentIndex());
        }

        if ((int)sampleChannelProxy->GetStyle() != m_style->currentIndex())
        {
            changed = true;
            sampleChannelProxy->SetStyle((SampleChannelProxy::Style)m_style->currentIndex());
        }
    }
    else
    {
        if (m_channelProxy->GetUnits() != m_units->text())
        {
            changed = true;
            m_channelProxy->SetUnits(m_units->text());
        }
    }

    if (m_penStyle->currentIndex() != (int)m_channelProxy->GetPenStyle())
    {
        changed = true;
        m_channelProxy->SetPenStyle((Qt::PenStyle)m_penStyle->currentIndex());
        rescaleAxis = true;
    }

    if (m_channelProxy->isGhost())
    {
        Measurement *originalMeasurement = m_measurements[m_measurementCombo->currentData().toInt()];
        GraphicsContainer *originalGC = originalMeasurement->GetWidget();
        ChannelBase *originalChannel = originalMeasurement->GetChannel(m_channelCombo->currentData().toInt());

        if (m_channelProxy->GetChannel() != originalChannel)
        {
            //FIXME: is it removed somewhere?
            m_graphicsContainer->SetHorizontalChannel(originalMeasurement, originalGC->GetHorizontalChannelProxy(originalMeasurement)->GetChannel());
            m_graphicsContainer->ReplaceChannelForWidget(originalChannel, m_channelProxy);
        }
    }

    if (hwChannelProxy)
    {

        hw::Sensor *sensor = m_sensorManager->GetSensor(m_sensorNameComboBox->currentData().toInt());
        if (NULL != sensor && sensor != hwChannelProxy->GetSensor())
        {
          hwChannelProxy->SetSensor(sensor);
          changed = true;
        }

        unsigned port = m_sensorPortComboBox->currentData().toInt();
        if (port != hwChannelProxy->GetSensorPort())
        {
            hwChannelProxy->SetSensorPort(port);
            changed = true;
        }

        hw::SensorQuantity *quantity = m_sensorManager->GetSensorQuantity(m_sensorQuantityComboBox->currentData().toInt());
        if (quantity != hwChannelProxy->GetSensorQuantity())
        {
            hwChannelProxy->SetSensorQuantity(quantity, m_sensorQuantityComboBox->currentIndex());
            changed = true;
        }
    }

    //NOTE: must be last!
    if (changed)
    {
        if (rescaleAxis)
        {
           m_graphicsContainer->GetPlot()->RescaleAxis(axis->GetGraphAxis());
        }

        GlobalSettings::GetInstance().SetSavedState(false);
        if (changedHorizontal)
        {
            m_channelProxy->UpdateTitle();
        }

        m_channelProxy->GetPlot()->ReplotIfNotDisabled();

    }

    return true;
}

bool ChannelSettings::_MoveLastHorizontalToVertical()
{
    foreach (ChannelProxyBase *proxy, m_graphicsContainer->GetChannelProxies())
    {
        //find last horizontal axis
        if (proxy->GetChannelGraph()->GetValuleAxis()->IsHorizontal())
        {
            AxisChooseDialog dialog(this, m_graphicsContainer, proxy, m_channelProxy);
            if (QDialog::Rejected != dialog.exec())
            {
                m_channelProxy->ShowGraph(false);
                m_graphicsContainer->SetHorizontalChannel(
                    m_channelProxy->GetChannelMeasurement(), m_channelProxy->GetChannel(), proxy);
                return true;
            }
            return false;
        }
    }
    qWarning() << "horizontal axes has not been found";

    return false; //it should never reach this point
}

void ChannelSettings::_InitializeShapeCombo(ChannelProxyBase *channelProxy)
{
    m_shapeComboBox = new bases::ComboBox(this);
    m_shapeComboBox->addItem(tr("None"), 0);
    m_shapeComboBox->addItem(tr("Cross"), 2);
    m_shapeComboBox->addItem(tr("Plus"), 3);
    m_shapeComboBox->addItem(tr("Circle"), 4);
    m_shapeComboBox->addItem(tr("Disc"), 5);
    m_shapeComboBox->addItem(tr("Square"), 6);
    m_shapeComboBox->addItem(tr("Diamond"), 7);
    m_shapeComboBox->addItem(tr("Star"), 8);
    m_shapeComboBox->addItem(tr("Triangle"), 9);
    m_shapeComboBox->addItem(tr("Inverted Triangle"), 10);
    m_shapeComboBox->addItem(tr("Cross and Square"), 11);
    m_shapeComboBox->addItem(tr("Plus and Square"), 12);
    m_shapeComboBox->addItem(tr("Cross and Circle"), 13);
    m_shapeComboBox->addItem(tr("Plus and Circle"), 14);
    m_shapeComboBox->addItem(tr("Peace"), 15);
    if (channelProxy->GetChannelGraph()->GetShapeIndex() == 0)
    {
        m_shapeComboBox->setCurrentIndex(0);
    }
    else
    {
        m_shapeComboBox->setCurrentIndex(channelProxy->GetChannelGraph()->GetShapeIndex() - 1); //skip dot
    }

    m_shapeComboBox->setEnabled(!channelProxy->IsOnHorizontalAxis());
    m_formLayout->addRow(new QLabel(tr("Shape"), this), m_shapeComboBox);
}

void ChannelSettings::_RefillAxisCombo()
{
    disconnect(m_axisComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(axisChanged(int)));
    m_axisComboBox->clear();
    m_axisComboBox->addItem(tr("New Axis..."));
    foreach (Axis *axis, m_graphicsContainer->GetAxes())
    {
        bool valid =
                m_channelProxy->GetChannelGraph()->GetValuleAxis() == axis || //I should be able to switch back to original axis
                axis->IsHorizontal(); //as same as to horizontal


        if (!valid)
        {
            if (dynamic_cast<SampleChannelProxy *>(m_channelProxy) && m_style->currentData().toBool())
                valid = axis->IsEmptyExcept(NULL); //channel with real time style might be moved only on empty vertical axis because of differet graphic axis style
            else
                valid = !axis->ContainsChannelWithRealTimeStyle();//but on DateTime axis might be only one channel
        }

        if (valid && axis->IsHorizontal() && m_channelProxy->isGhost())
            valid = false;

        if (valid)
            m_axisComboBox->addItem(axis->GetTitle(), (qlonglong)axis);
    }

    m_axisComboBox->setCurrentIndex(
        m_axisComboBox->findData((qlonglong)(m_channelProxy->GetChannelGraph()->GetValuleAxis())));
    connect(m_axisComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(axisChanged(int)));
}
void ChannelSettings::_InitializeAxisCombo()
{
    m_axisComboBox = new bases::ComboBox(this);
    _RefillAxisCombo();

    if (m_channelProxy->IsOnHorizontalAxis())
        m_axisComboBox->setEnabled(false);
    else
        m_axisComboBox->setEnabled(true);
    m_formLayout->addRow(new QLabel(tr("Axis"), this), m_axisComboBox);
}

void ChannelSettings::axisChanged(int index)
{
    if (0 == index) //New Axis...
    {
        Axis*newAxis = m_graphicsContainer->CreateYAxis(m_channelProxy->GetForeColor());

        AxisSettings dialog(this, newAxis, GlobalSettings::GetInstance().GetAcceptChangesByDialogClosing());
        if (QDialog::Accepted == dialog.exec())
        {
            GlobalSettings::GetInstance().SetSavedState(false);
            m_axisComboBox->addItem(newAxis->GetTitle(), (qlonglong)newAxis);
            m_axisComboBox->setCurrentIndex(m_axisComboBox->findData((qlonglong)(newAxis)));
            m_shapeComboBox->setEnabled(true); //new axis might be just a horizontal one
        }
        else
            m_graphicsContainer->RemoveAxis(newAxis);
    }
    else
    {
        Axis *axis = (Axis *)m_axisComboBox->currentData().toLongLong();
        m_shapeComboBox->setEnabled(!axis->IsHorizontal());
    }
}

QString ChannelSettings::_GetPortName(int port)
{
    return tr("Port %1").arg(port);
}

GraphicsContainer *ChannelSettings::GetGraphicsContainer()
{
    return m_graphicsContainer;
}
