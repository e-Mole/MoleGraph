#include "ChannelSettings.h"
#include <Axis.h>
#include <AxisChooseDialog.h>
#include <AxisSettings.h>
#include <ChannelWidget.h>
#include <ChannelBase.h>
#include <GhostChannel.h>
#include <Context.h>
#include <HwChannel.h>
#include <Measurement.h>
#include <MainWindow.h>
#include <MyMessageBox.h>
#include <Plot.h>
#include <bases/ComboBox.h>
#include <QCheckBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QString>
#include <SampleChannel.h>

ChannelSettings::ChannelSettings(ChannelBase *channel, const Context &context) :
    bases::FormDialogColor(channel->GetWidget(), tr("Channel settings"), context.m_settings),
    m_context(context),
    m_channel(channel),
    m_currentValueControl(NULL),
    m_name(NULL),
    m_units(NULL),
    m_shapeComboBox(NULL),
    m_axisComboBox(NULL),
    m_style(NULL),
    m_timeUnits(NULL),
    m_format(NULL),
    m_penStyle(NULL),
    m_sourceMeasurements(NULL),
    m_sourceChannels(NULL),
    m_currentValueChanged(false),
    m_currentValue(m_channel->GetNaValue())
{
    m_name = new QLineEdit(channel->GetName(), this);
    m_units = new QLineEdit(channel->GetUnits(), this);

    if (m_channel->GetType() == ChannelBase::Type_Sample)
    {
        m_name->setVisible(false);
        _InitializeTimeFeatures();
    }
    else
    {
        if (m_channel->GetType() == ChannelBase::Type_Hw)
            _InitializeValueLine(channel);
        else //ghost
            _InitializeGhostLines();

        m_formLayout->addRow(new QLabel(tr("Title"), this), m_name);
        m_formLayout->addRow(new QLabel(tr("Units"), this), m_units);
    }

    AddColorButtonRow(m_channel->m_color);
    _InitializeAxisCombo(m_channel == _GetPropertiesChannel());
    _InitializeShapeCombo(m_channel);
    _InitializePenStyle(m_channel->GetPenStyle());
}

ChannelBase * ChannelSettings::_GetPropertiesChannel()
{
    if (
        m_sourceChannels == NULL ||
        m_sourceChannels->count() == 0 ||
        qvariant_cast<qlonglong>(m_sourceChannels->currentData()) == 0
       )
        return m_channel;

    return (ChannelBase *)qvariant_cast<qlonglong>(m_sourceChannels->currentData());
}

void ChannelSettings::_InitializeGhostLines()
{
    m_sourceMeasurements = new bases::ComboBox(this);
    foreach(Measurement *m, m_context.m_measurements)
    {
        if (m != m_context.m_mainWindow.GetCurrnetMeasurement())
        {
            m_sourceMeasurements->addItem(m->GetName(), (qlonglong)m);
            if (m == ((GhostChannel*)m_channel)->GetSourceChannel()->GetMeasurement())
                m_sourceMeasurements->setCurrentIndex(m_sourceMeasurements->count()-1);
        }
    }

    m_formLayout->addRow(new QLabel(tr("Source Measurement"), this), m_sourceMeasurements);

    m_sourceChannels = new bases::ComboBox(this);
    m_formLayout->addRow(new QLabel(tr("Source Channel"), this), m_sourceChannels);

    if (m_sourceMeasurements->count() == 0)
        m_sourceMeasurements->setDisabled(true);
    else
        fillSourceChannels(m_sourceMeasurements->currentIndex());

    connect(m_sourceMeasurements, SIGNAL(currentIndexChanged(int)), this, SLOT(fillSourceChannels(int)));
    connect(m_sourceChannels, SIGNAL(currentIndexChanged(int)), this, SLOT(updateChannelProperties(int)));

}

void ChannelSettings::fillSourceChannels(int index)
{
    m_sourceChannels->clear();
    Measurement *m = (Measurement *)qvariant_cast<qlonglong>(m_sourceMeasurements->itemData(index));
    foreach (ChannelBase *channel, m->GetChannels())
    {
       if (channel->GetType() == ChannelBase::Type_Hw)
           m_sourceChannels->addItem(channel->GetName(), (qlonglong)channel);

       if (channel == ((GhostChannel *)m_channel)->GetSourceChannel())
           m_sourceChannels->setCurrentIndex(m_sourceChannels->count()-1);
    }
}

void ChannelSettings::updateChannelProperties(int index)
{
    if (index < 0)
        return;

    ChannelBase *source_channel = (ChannelBase *)qvariant_cast<qlonglong>(m_sourceChannels->itemData(index));
    m_name->setText(((GhostChannel*)m_channel)->GenerateName(source_channel));
    m_units->setText(source_channel->GetUnits());
    if (NULL != m_shapeComboBox)
        m_shapeComboBox->setCurrentIndex(source_channel->m_shapeIndex);
    if (NULL != m_colorButtonWidget)
        SetColorButtonColor(source_channel->GetColor());
}

void ChannelSettings::_InitializeValueLine(ChannelBase *channel)
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

    int currentPos = _GetCurrentPos();
    if (currentPos < (int)channel->GetValueCount())
    {
        QString currentValueStr = (channel->IsValueNA(currentPos)) ?
            channel->GetNAValueString() : QString::number(channel->GetValue(currentPos));
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
    double currentValue = ((HwChannel*)m_channel)->GetOriginalValue(_GetCurrentPos());
    m_currentValueControl->setText(
        (currentValue == HwChannel::GetNaValue()) ?
            HwChannel::GetNAValueString() :
            QString::number(currentValue)
    );

    //was changed in currentValueChanged
    m_currentValue = currentValue;
}

void ChannelSettings::setNaValue(bool)
{
    m_currentValueControl->setText(ChannelBase::GetNAValueString());
    m_currentValue = ChannelBase::GetNaValue();
}

void ChannelSettings::currentValueChanged(QString const &content)
{
    m_currentValue = content.toDouble();
    m_currentValueChanged = true;
}
int ChannelSettings::_GetCurrentPos()
{
    return m_channel->GetMeasurement()->GetSliderPos();
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
    m_style->addItem(channel->GetStyleText(SampleChannel::Samples), false);
    m_style->addItem(channel->GetStyleText(SampleChannel::TimeOffset), false);
    m_style->addItem(channel->GetStyleText(SampleChannel::RealTime), true); //RealTime state as data
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
    m_timeUnits->setEnabled(channel->m_style == SampleChannel::TimeOffset);
    m_formLayout->addRow(new QLabel(tr("Units"), this), m_timeUnits);

    m_format = new bases::ComboBox(this);
    m_format->addItem(tr("day.month.year"));
    m_format->addItem(tr("day.month.hour:minute"));
    m_format->addItem(tr("hour:minute:second"));
    m_format->addItem(tr("minute:second.milisecond"));
    m_format->setCurrentIndex(channel->m_realTimeFormat);
    m_format->setEnabled(channel->m_style == SampleChannel::RealTime);
    m_formLayout->addRow(new QLabel(tr("Format"), this), m_format);
}

void ChannelSettings::styleChanged(int index)
{
    m_timeUnits->setEnabled((SampleChannel::Style)index == SampleChannel::TimeOffset);
    m_format->setEnabled((SampleChannel::Style)index == SampleChannel::RealTime);
    _RefillAxisCombo(); //on axis with RealTime channel must not be another channel
}

bool ChannelSettings::_AxisCheckForRealTimeMode()
{
    if (m_style != NULL && m_style->currentData().toBool()) //channel with realtime style
    {
        Axis *axis = ((Axis *)m_axisComboBox->currentData().toLongLong());
        if (!axis->IsHorizontal() && !axis->IsEmptyExcept(m_channel))
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
    Axis *axis = (Axis *)m_axisComboBox->currentData().toLongLong();
    if (m_channel->m_axis != axis)
    {
        if (axis->IsHorizontal())
        {
            if (!_MoveLastHorizontalToVertical())
                return false; //no axis has been selected

            changedHorizontal = true;
            m_channel->_ShowOrHideGraphAndPoin(false);
            m_channel->GetMeasurement()->SetHorizontalChannel(m_channel);
        }

        Axis *lastAxis = m_channel->m_axis;
        m_channel->AssignToAxis(axis);
        lastAxis->UpdateGraphAxisName();
        lastAxis->UpdateGraphAxisStyle();
        lastAxis->UpdateVisiblility();
        m_channel->GetMeasurement()->GetPlot()->RescaleAxis(lastAxis->GetGraphAxis());

        changed = true;
    }

    if (m_channel->GetType() == ChannelBase::Type_Hw && m_currentValueChanged)
    {
        char *endptr;
        strtof(m_currentValueControl->text().toStdString().c_str(),&endptr);
        if (endptr[0] != '\0')
        {
            MyMessageBox::information(this, tr("Current value is not a number."));
            return false;
        }
        changed = true;
        ((HwChannel *)m_channel)->ChangeValue(_GetCurrentPos(), m_currentValue);
    }
    if (m_channel->m_name != m_name->text() && m_channel->GetType() != ChannelBase::Type_Sample)
    {
        changed = true;
        m_channel->_SetName(m_name->text());
    }

    if (m_channel->m_color != m_color)
    {
        changed = true;
        m_channel->SetColor(m_color);
    }

    if (m_channel->m_shapeIndex != (unsigned)m_shapeComboBox->currentIndex())
    {
        changed = true;
        m_channel->_SetShapeIndex(m_shapeComboBox->currentIndex());
    }

    if (m_channel->GetType() == ChannelBase::Type_Sample)
    {
        SampleChannel *channelWithTime = (SampleChannel *)m_channel;
        if ((int)channelWithTime->m_timeUnits != m_timeUnits->currentIndex())
        {
            changed = true;
            channelWithTime->_SetTimeUnits((SampleChannel::TimeUnits)m_timeUnits->currentIndex());
        }

        if ((int)channelWithTime->m_realTimeFormat != m_format->currentIndex())
        {
            changed = true;
            channelWithTime->_SetFormat((SampleChannel::RealTimeFormat)m_format->currentIndex());
        }

        if ((int)channelWithTime->m_style != m_style->currentIndex())
        {
            changed = true;
            channelWithTime->_SetStyle((SampleChannel::Style)m_style->currentIndex());
        }
    }
    else
    {
        if (m_channel->GetType() == ChannelBase::Type_Ghost)
        {
            changed = true;
            GhostChannel *ghost = (GhostChannel *)m_channel;
            ghost->m_sourceChannel = _GetPropertiesChannel();
        }

        if (m_channel->m_units != m_units->text())
        {
            changed = true;
            m_channel->_SetUnits(m_units->text());
        }
    }

    if (m_penStyle->currentIndex() != (int)m_channel->GetPenStyle())
    {
        changed = true;
        m_channel->_SetPenStyle((Qt::PenStyle)m_penStyle->currentIndex());
    }

    if (changed)
    {
        m_context.m_mainWindow.SetSavedState(false);
        if (changedHorizontal)
        {
            m_channel->_UpdateTitle();
            m_channel->wasSetToHorizontal();
        }
        else
            m_channel->stateChanged();

        m_channel->GetMeasurement()->GetPlot()->ReplotIfNotDisabled();

    }

    return true;
}

bool ChannelSettings::_MoveLastHorizontalToVertical()
{
    foreach (ChannelBase *channel, m_channel->GetMeasurement()->GetChannels())
    {
        //find last horizontal axis
        if (channel->m_axis->IsHorizontal())
        {
            AxisChooseDialog dialog(this, m_context, channel, m_channel);
            return (QDialog::Rejected != dialog.exec());
        }
    }
    return false; //it should never reach this point
}

void ChannelSettings::_InitializeShapeCombo(ChannelBase *channel)
{
    m_shapeComboBox = new bases::ComboBox(this);
    m_shapeComboBox->addItem(tr("Cross"));
    m_shapeComboBox->addItem(tr("Plus"));
    m_shapeComboBox->addItem(tr("Circle"));
    m_shapeComboBox->addItem(tr("Disc"));
    m_shapeComboBox->addItem(tr("Square"));
    m_shapeComboBox->addItem(tr("Diamond"));
    m_shapeComboBox->addItem(tr("Star"));
    m_shapeComboBox->addItem(tr("Triangle"));
    m_shapeComboBox->addItem(tr("Inverted Triangle"));
    m_shapeComboBox->addItem(tr("Cross and Square"));
    m_shapeComboBox->addItem(tr("Plus and Square"));
    m_shapeComboBox->addItem(tr("Cross and Circle"));
    m_shapeComboBox->addItem(tr("Plus and Circle"));
    m_shapeComboBox->addItem(tr("Peace"));
    m_shapeComboBox->setCurrentIndex(channel->m_shapeIndex);
    m_shapeComboBox->setEnabled(!channel->IsOnHorizontalAxis());
    m_formLayout->addRow(new QLabel(tr("Shape"), this), m_shapeComboBox);
}

void ChannelSettings::_RefillAxisCombo()
{
    disconnect(m_axisComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(axisChanged(int)));
    m_axisComboBox->clear();
    m_axisComboBox->addItem(tr("New Axis..."));
    foreach (Axis *axis, m_channel->GetMeasurement()->GetAxes())
    {
        bool valid =
                m_channel->GetAxis() == axis || //I should be able to switch back to original axis
                axis->IsHorizontal(); //as same as to horizontal

        if (!valid)
        {
            if (m_channel->GetType() == ChannelBase::Type_Sample && m_style->currentData().toBool())
                valid = axis->IsEmptyExcept(NULL); //channel with real time style might be moved only on empty vertical axis because of differet graphic axis style
            else
                valid = !axis->ContainsChannelWithRealTimeStyle();//but on DateTime axis might be only one channel
        }

        if (valid)
            m_axisComboBox->addItem(axis->GetTitle(), (qlonglong)axis);
    }

    m_axisComboBox->setCurrentIndex(m_axisComboBox->findData((qlonglong)(m_channel->m_axis)));
    connect(m_axisComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(axisChanged(int)));
}
void ChannelSettings::_InitializeAxisCombo(bool enable)
{
    m_axisComboBox = new bases::ComboBox(this);
    _RefillAxisCombo();

    if (m_channel->IsOnHorizontalAxis())
        m_axisComboBox->setEnabled(false);
    m_formLayout->addRow(new QLabel(tr("Axis"), this), m_axisComboBox);

    m_axisComboBox->setEnabled(enable);
}

void ChannelSettings::axisChanged(int index)
{
    if (0 == index) //New Axis...
    {
        Axis*newAxis = m_channel->GetMeasurement()->CreateAxis(m_channel->GetColor());

        AxisSettings dialog(this, newAxis, m_context);
        if (QDialog::Accepted == dialog.exec())
        {
            m_axisComboBox->addItem(newAxis->GetTitle(), (qlonglong)newAxis);
            m_axisComboBox->setCurrentIndex(m_axisComboBox->findData((qlonglong)(newAxis)));
            m_shapeComboBox->setEnabled(true); //new axis might be just a horizontal one
        }
        else
            m_channel->GetMeasurement()->RemoveAxis(newAxis);
    }
    else
    {
        Axis *axis = (Axis *)m_axisComboBox->currentData().toLongLong();
        m_shapeComboBox->setEnabled(!axis->IsHorizontal());
    }
}
