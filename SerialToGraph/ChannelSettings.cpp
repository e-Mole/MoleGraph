#include "ChannelSettings.h"
#include <Axis.h>
#include <AxisChooseDialog.h>
#include <AxisSettings.h>
#include <Channel.h>
#include <ChannelWithTime.h>
#include <Context.h>
#include <Measurement.h>
#include <Plot.h>
#include <QComboBox>
#include <QCheckBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QSettings>
#include <QString>

ChannelSettings::ChannelSettings(Channel *channel, const Context &context) :
    FormDialogColor(channel, tr("Channel settings")),
    m_context(context),
    m_channel(channel),
    m_name(new QLineEdit(channel->GetName(), this)),
    m_units(NULL),
    m_shapeComboBox(new QComboBox(this)),
    m_axisComboBox(new QComboBox(this)),
    m_style(NULL),
    m_timeUnits(NULL),
    m_format(NULL)
{
    m_formLayout->addRow(new QLabel(tr("Title"), this),  m_name);

    if (m_channel->IsHwChannel())
    {
        m_units = new QLineEdit(channel->GetUnits(), this);
        m_formLayout->addRow(new QLabel(tr("Units"), this), m_units);
    }
    else
    {
        _InitializeTimeFeatures();
    }

    AddColorButtonRow(m_channel->m_color);

    _InitializeAxisCombo();
    _InitializeShapeCombo();
}

void ChannelSettings::_InitializeTimeFeatures()
{
    ChannelWithTime * channel = (ChannelWithTime*)m_channel;

    m_style = new QComboBox(this);
    m_style->addItem(tr("Samples"));
    m_style->addItem(tr("Time From Start"));
    m_style->addItem(tr("Real Time"));
    m_style->setCurrentIndex(channel->m_style);//unfortunately I cant use a template with a Qt class
    connect(m_style, SIGNAL(currentIndexChanged(int)), this, SLOT(styleChanged(int)));
    m_formLayout->addRow(new QLabel(tr("Style"), this), m_style);

    m_timeUnits = new QComboBox(this);
    m_timeUnits->addItem(tr("Microseconds"));
    m_timeUnits->addItem(tr("Miliseconds"));
    m_timeUnits->addItem(tr("Seconds"));
    m_timeUnits->addItem(tr("Minuts"));
    m_timeUnits->addItem(tr("Hours"));
    m_timeUnits->addItem(tr("Days"));
    m_timeUnits->setCurrentIndex(channel->m_timeUnits);
    m_timeUnits->setEnabled(channel->m_style == ChannelWithTime::TimeFromStart);
    m_formLayout->addRow(new QLabel(tr("Units"), this), m_timeUnits);

    m_format = new QComboBox(this);
    m_format->addItem(tr("day.month.year"));
    m_format->addItem(tr("day.month.hour:minute"));
    m_format->addItem(tr("hour:minute:second"));
    m_format->addItem(tr("minute:second.milisecond"));
    m_format->setCurrentIndex(channel->m_realTimeFormat);
    m_format->setEnabled(channel->m_style == ChannelWithTime::RealTime);
    m_formLayout->addRow(new QLabel(tr("Format"), this), m_format);
}

void ChannelSettings::styleChanged(int index)
{
    m_timeUnits->setEnabled((ChannelWithTime::Style)index == ChannelWithTime::TimeFromStart);
     m_format->setEnabled((ChannelWithTime::Style)index == ChannelWithTime::RealTime);
}

bool ChannelSettings::BeforeAccept()
{
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
        lastAxis->UpdateVisiblility();
        m_channel->UpdateGraphAxisStyle();
        changed = true;
    }

    if (m_channel->m_name != m_name->text())
    {
        changed = true;
        m_channel->m_name = m_name->text();
        m_channel->_UpdateTitle();
    }

    if (m_channel->m_color != m_color)
    {
        changed = true;
        m_channel->SetColor(m_color);
    }

    if (m_channel->m_shapeIndex != (unsigned)m_shapeComboBox->currentIndex())
    {
        changed = true;
        m_channel->m_shapeIndex = m_shapeComboBox->currentIndex();
        m_channel->GetMeasurement()->GetPlot()->SetShape(m_channel->m_graphPoint, m_channel->m_shapeIndex);
    }

    if (m_channel->IsHwChannel())
    {
        if (m_channel->m_units != m_units->text())
        {
            changed = true;
            m_channel->m_units = m_units->text();
            m_channel->_ShowLastValueWithUnits();
            m_channel->m_axis->UpdateGraphAxisName();
        }
    }
    else
    {
        ChannelWithTime *channelWithTime = (ChannelWithTime *)m_channel;
        if ((int)channelWithTime->m_timeUnits != m_timeUnits->currentIndex())
        {
            changed = true;
            channelWithTime->_SetTimeUnits((ChannelWithTime::TimeUnits)m_timeUnits->currentIndex());
        }

        if ((int)channelWithTime->m_realTimeFormat != m_format->currentIndex())
        {
            changed = true;
            channelWithTime->_SetFormat((ChannelWithTime::RealTimeFormat)m_format->currentIndex());
        }

        if ((int)channelWithTime->m_style != m_style->currentIndex())
        {
            changed = true;
            channelWithTime->_SetStyle((ChannelWithTime::Style)m_style->currentIndex());
        }


    }

    if (changed)
    {
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

/*void ChannelSettings::_AssignToNewAxis(ChannelWithTime *channel)
{
    QMessageBox::information(
        this,
        m_context.m_applicationName,
        QString(tr("There might be just one channel on a horizontal axis. Created an axis named '%1' for a real time styled channel '%2'.")).
            arg(channel->GetAxis()->GetTitle()).
            arg(channel->GetName())
    );

    channel->AssignToAxis(channel->GetMeasurement()->CreateAxis(channel->GetColor()));
    channel->UpdateGraphAxisStyle();
}*/

bool ChannelSettings::_MoveLastHorizontalToVertical()
{
    foreach (Channel *channel, m_channel->GetMeasurement()->GetChannels())
    {
        //find last horizontal axis
        if (channel->m_axis->IsHorizontal())
        {
            AxisChooseDialog dialog(m_context, channel, m_channel);
            return (QDialog::Rejected != dialog.exec());
        }
    }
}

void ChannelSettings::_InitializeShapeCombo()
{
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

void ChannelSettings::_InitializeAxisCombo()
{
    m_axisComboBox->addItem(tr("New Axis..."));
    foreach (Axis *axis, m_channel->GetMeasurement()->GetAxes())
    {
       m_axisComboBox->addItem(axis->GetTitle(), (qlonglong)axis);
    }

    m_axisComboBox->setCurrentIndex(m_axisComboBox->findData((qlonglong)(m_channel->m_axis)));
    if (m_channel->IsOnHorizontalAxis())
        m_axisComboBox->setEnabled(false);
    m_formLayout->addRow(new QLabel(tr("Axis"), this), m_axisComboBox);
    connect(m_axisComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(axisChanged(int)));
}

void ChannelSettings::axisChanged(int index)
{
    if (0 == index) //New Axis...
    {
        Axis*newAxis = m_channel->GetMeasurement()->CreateAxis(m_channel->GetColor());

        AxisSettings dialog(newAxis, m_context);
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
