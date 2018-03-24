#include "ChannelSettings.h"
#include <Axis.h>
#include <AxisChooseDialog.h>
#include <AxisSettings.h>
#include <ChannelBase.h>
#include <ChannelWidget.h>
#include <GlobalSettings.h>
#include <graphics/GraphicsContainer.h>
#include <graphics/SampleChannelProperties.h>
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
    ChannelWidget *channelWidget,
    hw::SensorManager *sensorManager
) :
    bases::FormDialogColor (channelWidget, tr("Channel settings"), GlobalSettings::GetInstance().GetAcceptChangesByDialogClosing()),
    m_measurements(measurements),
    m_graphicsContainer(graphicsContainer),
    m_channelWidget(channelWidget),
    m_channel(graphicsContainer->GetChannel(channelWidget)),
    m_currentValueControl(NULL),
    m_name(NULL),
    m_units(NULL),
    m_shapeComboBox(NULL),
    m_axisComboBox(NULL),
    m_style(NULL),
    m_timeUnits(NULL),
    m_format(NULL),
    m_penStyle(NULL),
    m_currentValueChanged(false),
    m_currentValue(ChannelBase::GetNaValue()),
    m_measurementCombo(NULL),
    m_channelCombo(NULL),
    m_sensorQuantityComboBox(NULL),
    m_sensorNameComboBox(NULL),
    m_sensorPortComboBox(NULL),
    m_sensorManager(sensorManager)
{
    if (m_channelWidget->isGhost())
    {
        _InitializeGhostCombos();
        AddSeparator();
    }

    if (m_channelWidget->isGhost())
    {
        _FillMeasurementCombo();
        connect(m_measurementCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(fillChannelCombo(int)));
        connect(m_channelCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(loadFromOriginalWidget(int)));
    }

    m_name = new QLineEdit(m_channelWidget->GetName(), this);
    m_units = new QLineEdit(m_channelWidget->GetUnits(), this);

    ChannelBase *channel = graphicsContainer->GetChannel(channelWidget);
    if (channel->GetType() == ChannelBase::Type_Sample)
    {
        m_name->setVisible(false);
        _InitializeTimeFeatures();
        AddSeparator();
    }
    else
    {
        if (channel->GetType() == ChannelBase::Type_Hw)
        {
            _InitializeValueLine(channelWidget);
            AddSeparator();
            _InitializeSensorItems();
            AddSeparator();
        }
        m_formLayout->addRow(new QLabel(tr("Title"), this), m_name);
        m_formLayout->addRow(new QLabel(tr("Units"), this), m_units);
    }
    AddColorButtonRow(channelWidget->GetForeColor());
    _InitializeAxisCombo();
    _InitializeShapeCombo(channelWidget);
    _InitializePenStyle(channelWidget->GetPenStyle());
}

void ChannelSettings::_InitializeSensorItem(bases::ComboBox **item, QString const &label, const char* slot)
{
    (*item) = new bases::ComboBox(this);
    (*item)->setEnabled(m_channel->GetMeasurement()->GetState() == Measurement::Ready);
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
        if (i == ((HwChannel *)m_channel)->GetSensorPort())
            m_sensorPortComboBox->setCurrentIndex(m_sensorPortComboBox->count() - 1);
    }
}

void ChannelSettings::_FillSensorNameCB()
{
    unsigned currentSensorId = ((HwChannel*)m_channel)->GetSensor()->GetId();
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
    m_sensorNameComboBox->setEnabled(m_channel->GetMeasurement()->GetState() == Measurement::Ready);
}

void ChannelSettings::_FillSensorQuanitityCB()
{
    hw::SensorQuantity *currentSensorQuanity = ((HwChannel*)m_channel)->GetSensorQuantity();
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
        m_channel->GetMeasurement()->GetState() == Measurement::Ready &&
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
    ChannelBase *originalChannel = m_graphicsContainer->GetChannel(m_channelWidget);
    Measurement *originalMeasurement = originalChannel->GetMeasurement();

    unsigned comboIndex = 0;
    unsigned measurementIndex = ~0;
    foreach (Measurement *item, m_measurements)
    {
        measurementIndex++;
        if (item->GetWidget() == m_graphicsContainer)
            continue; //it is not possible to select channels from current gc

        QString name = item->GetWidget()->GetName();
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
    ChannelBase *originalChannel = m_graphicsContainer->GetChannel(m_channelWidget);

    bool channelFound = false;
    for (unsigned index = 0; index < currentMeasurement->GetChannelCount(); ++index)
    {
        ChannelBase * iteratingChannel = currentMeasurement->GetChannel(index);
        m_channelCombo->addItem(currentGC->GetChannelWidget(iteratingChannel)->GetName(), index);
        if (iteratingChannel == originalChannel)
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
    ChannelWidget *originalChannelWidget = originalGC->GetChannelWidget(originalChannel);

    if (m_name)
    {
        m_name->setText(m_graphicsContainer->GetGhostWidgetName(originalGC, originalChannelWidget));
    }
    if (m_units)
    {
        m_units->setText(originalChannelWidget->GetUnits());
    }
    if (m_colorButtonWidget)
    {
        SetColorButtonColor(originalChannelWidget->GetForeColor());
    }
    if (m_shapeComboBox)
    {
        m_shapeComboBox->setCurrentIndex(originalChannelWidget->GetChannelGraph()->GetShapeIndex());
    }
}

void ChannelSettings::_InitializeValueLine(ChannelWidget *channelWidget)
{
    ChannelBase *channel = m_graphicsContainer->GetChannel(channelWidget);
    QHBoxLayout *curValLayout = new QHBoxLayout();
    m_currentValueControl = new QLineEdit(this);
    curValLayout->addWidget(m_currentValueControl);

    QPushButton *originlValue = new QPushButton(tr("Original"), this);
    connect(originlValue, SIGNAL(clicked(bool)), this, SLOT(setOriginalValue(bool)));
    curValLayout->addWidget(originlValue);

    QPushButton *naValue = new QPushButton(tr("n/a"), this);
    connect(naValue, SIGNAL(clicked(bool)), this, SLOT(setNaValue(bool)));
    curValLayout->addWidget(naValue);

    double currentHorizontalValue =
        m_graphicsContainer->GetHorizontalValueBySliderPos(m_graphicsContainer->GetCurrentIndex());
    unsigned currentIndex =
        m_graphicsContainer->GetHorizontalChannel(channel->GetMeasurement())->GetLastValueIndex(currentHorizontalValue);
    if (currentIndex < (int)channel->GetValueCount())
    {
        QLocale locale(QLocale::system());
        QString currentValueStr = (channel->IsValueNA(currentIndex)) ?
            channelWidget->GetNAValueString() : locale.toString(channel->GetValue(currentIndex));

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
    double currentValue = ((HwChannel*)m_channel)->GetOriginalValue(m_graphicsContainer->GetCurrentIndex());
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
    SampleChannel * channel = (SampleChannel*)m_channel;

    m_style = new bases::ComboBox(this);
    m_style->addItem(SampleChannelProperties::GetSampleChannelStyleText(SampleChannelProperties::Samples), false);
    m_style->addItem(SampleChannelProperties::GetSampleChannelStyleText(SampleChannelProperties::TimeOffset), false);
    m_style->addItem(SampleChannelProperties::GetSampleChannelStyleText(SampleChannelProperties::RealTime), true); //RealTime state as data
    m_style->setCurrentIndex(channel->m_style);//unfortunately I cant use a template with a Qt class
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
    m_timeUnits->setCurrentIndex(channel->m_timeUnits);
    m_timeUnits->setEnabled(channel->m_style == SampleChannelProperties::TimeOffset);
    m_formLayout->addRow(new QLabel(tr("Units"), this), m_timeUnits);

    m_format = new bases::ComboBox(this);
    m_format->addItem(tr("day.month.year"));
    m_format->addItem(tr("day.month.hour:minute"));
    m_format->addItem(tr("hour:minute:second"));
    m_format->addItem(tr("minute:second.milisecond"));
    m_format->setCurrentIndex(channel->m_realTimeFormat);
    m_format->setEnabled(channel->m_style == SampleChannelProperties::RealTime);
    m_formLayout->addRow(new QLabel(tr("Format"), this), m_format);
}

void ChannelSettings::styleChanged(int index)
{
    m_timeUnits->setEnabled((SampleChannelProperties::Style)index == SampleChannelProperties::TimeOffset);
    m_format->setEnabled((SampleChannelProperties::Style)index == SampleChannelProperties::RealTime);
    _RefillAxisCombo(); //on axis with RealTime channel must not be another channel
}

bool ChannelSettings::_AxisCheckForRealTimeMode()
{
    if (m_style != NULL && m_style->currentData().toBool()) //channel with realtime style
    {
        Axis *axis = ((Axis *)m_axisComboBox->currentData().toLongLong());
        if (!axis->IsHorizontal() && !axis->IsEmptyExcept(m_channelWidget))
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
    if (m_channelWidget->GetChannelGraph()->GetValuleAxis() != axis)
    {
        if (axis->IsHorizontal())
        {
            if (!_MoveLastHorizontalToVertical())
                return false; //no axis has been selected

            changedHorizontal = true;
            m_channelWidget->ShowOrHideGraph(false);
            m_graphicsContainer->SetHorizontalChannel(m_channel->GetMeasurement(), m_channel);
        }

        Axis *lastAxis = m_channelWidget->GetChannelGraph()->GetValuleAxis();
        m_channelWidget->GetChannelGraph()->AssignToAxis(axis);
        lastAxis->UpdateGraphAxisName();
        lastAxis->UpdateGraphAxisStyle();
        lastAxis->UpdateVisiblility();
        m_graphicsContainer->GetPlot()->RescaleAxis(lastAxis->GetGraphAxis());

        rescaleAxis = true;
        changed = true;
    }

    if (m_channel->GetType() == ChannelBase::Type_Hw && m_currentValueChanged)
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
        ((HwChannel *)m_channel)->ChangeValue(
            m_channel->GetMeasurement()->GetCurrentIndex(),
            m_currentValue);
    }
    if (m_channelWidget->GetName() != m_name->text() && m_channel->GetType() != ChannelBase::Type_Sample)
    {
        changed = true;
        m_channelWidget->SetName(m_name->text());
    }

    if (m_channelWidget->GetForeColor() != m_color)
    {
        changed = true;
        m_channelWidget->SetForeColor(m_color);
    }

    if (m_channelWidget->GetChannelGraph()->GetShapeIndex() != (unsigned)m_shapeComboBox->currentData().toInt())
    {
        changed = true;
        m_channelWidget->SetShapeIndex(m_shapeComboBox->currentData().toInt());
        rescaleAxis = true;

    }

    if (m_channel->GetType() == ChannelBase::Type_Sample)
    {
        SampleChannel *channelWithTime = (SampleChannel *)m_channel;
        if ((int)channelWithTime->m_timeUnits != m_timeUnits->currentIndex())
        {
            changed = true;
            channelWithTime->_SetTimeUnits((SampleChannelProperties::TimeUnits)m_timeUnits->currentIndex());
        }

        if ((int)channelWithTime->m_realTimeFormat != m_format->currentIndex())
        {
            changed = true;
            channelWithTime->_SetFormat((SampleChannelProperties::RealTimeFormat)m_format->currentIndex());
        }

        if ((int)channelWithTime->m_style != m_style->currentIndex())
        {
            changed = true;
            channelWithTime->_SetStyle((SampleChannelProperties::Style)m_style->currentIndex());
        }
    }
    else
    {
        if (m_channelWidget->GetUnits() != m_units->text())
        {
            changed = true;
            m_channelWidget->SetUnits(m_units->text());
        }
    }

    if (m_penStyle->currentIndex() != (int)m_channelWidget->GetPenStyle())
    {
        changed = true;
        m_channelWidget->SetPenStyle((Qt::PenStyle)m_penStyle->currentIndex());
        rescaleAxis = true;
    }

    if (m_channelWidget->isGhost())
    {
        Measurement *originalMeasurement = m_measurements[m_measurementCombo->currentData().toInt()];
        GraphicsContainer *originalGC = originalMeasurement->GetWidget();
        ChannelBase *originalChannel = originalMeasurement->GetChannel(m_channelCombo->currentData().toInt());

        if (m_graphicsContainer->GetChannel(m_channelWidget) != originalChannel)
        {
            //FIXME: is it removed somewhere?
            m_graphicsContainer->SetHorizontalChannel(originalMeasurement, originalGC->GetHorizontalChannel(originalMeasurement));
            m_graphicsContainer->ReplaceChannelForWidget(originalChannel, m_channelWidget);
        }
    }

    if (m_channel->GetType() == ChannelBase::Type_Hw)
    {
        HwChannel *hwChannel = (HwChannel *)m_channel;
        hw::Sensor *sensor = m_sensorManager->GetSensor(m_sensorNameComboBox->currentData().toInt());
        if (NULL != sensor && sensor != hwChannel->GetSensor())
        {
          hwChannel->SetSensor(sensor);
          changed = true;
        }

        unsigned port = m_sensorPortComboBox->currentData().toInt();
        if (port != hwChannel->GetSensorPort())
        {
            hwChannel->_SetSensorPort(port);
            changed = true;
        }

        hw::SensorQuantity *quantity = m_sensorManager->GetSensorQuantity(m_sensorQuantityComboBox->currentData().toInt());
        if (quantity != hwChannel->GetSensorQuantity())
        {
            hwChannel->SetSensorQuantity(quantity, m_sensorQuantityComboBox->currentIndex());
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
            m_channelWidget->UpdateTitle();
        }

        m_channelWidget->GetPlot()->ReplotIfNotDisabled();

    }

    return true;
}

bool ChannelSettings::_MoveLastHorizontalToVertical()
{
    foreach (ChannelWidget *channelWidget, m_graphicsContainer->GetChannelWidgets())
    {
        //find last horizontal axis
        if (channelWidget->GetChannelGraph()->GetValuleAxis()->IsHorizontal())
        {
            AxisChooseDialog dialog(this, m_graphicsContainer, channelWidget, m_channelWidget);
            return (QDialog::Rejected != dialog.exec());
        }
    }
    return false; //it should never reach this point
}

void ChannelSettings::_InitializeShapeCombo(ChannelWidget *channelWidget)
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
    if (channelWidget->GetChannelGraph()->GetShapeIndex() == 0)
    {
        m_shapeComboBox->setCurrentIndex(0);
    }
    else
    {
        m_shapeComboBox->setCurrentIndex(channelWidget->GetChannelGraph()->GetShapeIndex() - 1); //skip dot
    }

    m_shapeComboBox->setEnabled(!channelWidget->IsOnHorizontalAxis());
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
                m_channelWidget->GetChannelGraph()->GetValuleAxis() == axis || //I should be able to switch back to original axis
                axis->IsHorizontal(); //as same as to horizontal


        if (!valid)
        {
            if (m_channel->GetType() == ChannelBase::Type_Sample && m_style->currentData().toBool())
                valid = axis->IsEmptyExcept(NULL); //channel with real time style might be moved only on empty vertical axis because of differet graphic axis style
            else
                valid = !axis->ContainsChannelWithRealTimeStyle();//but on DateTime axis might be only one channel
        }

        if (valid && axis->IsHorizontal() && m_channelWidget->isGhost())
            valid = false;

        if (valid)
            m_axisComboBox->addItem(axis->GetTitle(), (qlonglong)axis);
    }

    m_axisComboBox->setCurrentIndex(
        m_axisComboBox->findData((qlonglong)(m_channelWidget->GetChannelGraph()->GetValuleAxis())));
    connect(m_axisComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(axisChanged(int)));
}
void ChannelSettings::_InitializeAxisCombo()
{
    m_axisComboBox = new bases::ComboBox(this);
    _RefillAxisCombo();

    if (m_channelWidget->IsOnHorizontalAxis())
        m_axisComboBox->setEnabled(false);
    else
        m_axisComboBox->setEnabled(true);
    m_formLayout->addRow(new QLabel(tr("Axis"), this), m_axisComboBox);
}

void ChannelSettings::axisChanged(int index)
{
    if (0 == index) //New Axis...
    {
        Axis*newAxis = m_graphicsContainer->CreateYAxis(m_channelWidget->GetForeColor());

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
