#include "ChannelSettings.h"
#include <Axis.h>
#include <AxisChooseDialog.h>
#include <AxisSettings.h>
#include <ChannelWidget.h>
#include <ChannelBase.h>
#include <SampleChannel.h>
#include <Context.h>
#include <Measurement.h>
#include <MainWindow.h>
#include <MyMessageBox.h>
#include <Plot.h>
#include <bases/ComboBox.h>
#include <QCheckBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSettings>
#include <QString>

ChannelSettings::ChannelSettings(ChannelBase *channel, const Context &context) :
    bases::FormDialogColor(channel->GetWidget(), tr("Channel settings"), context.m_settings),
    m_context(context),
    m_channel(channel),
    m_name(NULL),
    m_units(NULL),
    m_shapeComboBox(NULL),
    m_axisComboBox(NULL),
    m_style(NULL),
    m_timeUnits(NULL),
    m_format(NULL),
    m_penStyle(NULL)
{
    m_name = new QLineEdit(channel->GetName(), this);
    if (m_channel->GetType() == ChannelBase::Type_Hw)
    {
        m_formLayout->addRow(new QLabel(tr("Title"), this), m_name);
        m_units = new QLineEdit(channel->GetUnits(), this);
        m_formLayout->addRow(new QLabel(tr("Units"), this), m_units);
    }
    else
    {
        m_name->setVisible(false);
        _InitializeTimeFeatures();
    }

    AddColorButtonRow(m_channel->m_color);

    _InitializeAxisCombo();
    _InitializeShapeCombo();
    _InitializePenStyle();
}

void ChannelSettings::_InitializePenStyle()
{
    m_penStyle = new bases::ComboBox(this);
    m_penStyle->addItem(tr("No Line"));
    m_penStyle->addItem(tr("Solid Line"));
    m_penStyle->addItem(tr("Dash Line"));
    m_penStyle->addItem(tr("Dot Line"));
    m_penStyle->addItem(tr("Dash Dot Line"));
    m_penStyle->addItem(tr("Dash Dot Dot Line"));
    m_penStyle->setCurrentIndex((int)m_channel->GetPenStyle());

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
            m_channel->GetMeasurement()->GetPlot()->SetHorizontalChannel(m_channel);
        }

        Axis *lastAxis = m_channel->m_axis;
        m_channel->AssignToAxis(axis);
        lastAxis->UpdateGraphAxisName();
        lastAxis->UpdateGraphAxisStyle();
        lastAxis->UpdateVisiblility();
        m_channel->GetMeasurement()->GetPlot()->RescaleAxis(lastAxis->GetGraphAxis());

        changed = true;
    }

    if (m_channel->m_name != m_name->text() && m_channel->GetType() == ChannelBase::Type_Hw)
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

    if (m_channel->GetType() == ChannelBase::Type_Hw)
    {
        if (m_channel->m_units != m_units->text())
        {
            changed = true;
            m_channel->_SetUnits(m_units->text());
        }
    }
    else
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

void ChannelSettings::_InitializeShapeCombo()
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
    m_shapeComboBox->setCurrentIndex(m_channel->m_shapeIndex);
    m_shapeComboBox->setEnabled(!m_channel->IsOnHorizontalAxis());
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
void ChannelSettings::_InitializeAxisCombo()
{
    m_axisComboBox = new bases::ComboBox(this);
    _RefillAxisCombo();

    if (m_channel->IsOnHorizontalAxis())
        m_axisComboBox->setEnabled(false);
    m_formLayout->addRow(new QLabel(tr("Axis"), this), m_axisComboBox);
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
