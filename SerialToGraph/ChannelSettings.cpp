#include "ChannelSettings.h"
#include <Axis.h>
#include <AxisSettings.h>
#include <Channel.h>
#include <Context.h>
#include <Measurement.h>
#include <Plot.h>
#include <QComboBox>
#include <QCheckBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QSettings>
#include <QString>
#include <QVBoxLayout>

ChannelSettings::ChannelSettings(Channel *channel, const Context &context) :
    FormDialogColor(channel, tr("Channel settings")),
    m_context(context),
    m_channel(channel),
    m_name(NULL),
    m_shapeComboBox(NULL),
    m_axisComboBox(NULL)
{
    m_name = new QLineEdit(channel->GetName(), this);
    m_formLayout->addRow(new QLabel(tr("Title"), this),  m_name);

    m_units = new QLineEdit(channel->GetUnits(), this);
    m_formLayout->addRow(new QLabel(tr("Units"), this), m_units);

    AddColorButtonRow(m_channel->m_color);

    _InitializeShapeCombo();
    _InitializeAxisCombo();
}

bool ChannelSettings::BeforeAccept()
{
    bool changed = false;
    if (m_channel->m_name != m_name->text())
    {
        changed = true;
        m_channel->m_name = m_name->text();
        m_channel->_UpdateTitle();
    }


    if (m_channel->m_units != m_units->text())
    {
        changed = true;
        m_channel->m_units = m_units->text();
        m_channel->_ShowLastValueWithUnits();
        m_channel->m_axis->UpdateGraphAxisName();
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

    bool changedHorizontal = false;
    Axis *axis = (Axis *)m_axisComboBox->currentData().toLongLong();
    if (m_channel->m_axis != axis)
    {
        changed = true;
        if (axis->IsHorizontal())
        {
            _MoveLastHorizontalToVertical();
            changedHorizontal = true;
            m_channel->_ShowOrHideGraphAndPoin(false);
            m_channel->GetMeasurement()->GetPlot()->SetHorizontalChannel(m_channel);
        }

        Axis *lastAxis = m_channel->m_axis;
        m_channel->AssignToAxis(axis);
        lastAxis->UpdateVisiblility();
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

void ChannelSettings::_MoveLastHorizontalToVertical()
{
    foreach (Channel *channel, m_context.m_channels)
    {
        if (channel->m_axis->IsHorizontal())
        {
            foreach (Axis *axis, channel->GetMeasurement()->GetAxes())
            {
                if (!axis->IsHorizontal())
                {
                    channel->m_axis = axis;
                    if (!m_context.m_settings.value("horizontalSwitchMessageHidden", false).toBool() &&
                        1 == QMessageBox::warning(
                            this,
                            m_context.m_applicationName,
                            QString(tr("Axis '%1' has been assigned to a channel '%2'.")).
                                arg(axis->GetTitle()).
                                arg(channel->GetName()),
                            tr("OK"),
                            tr("Don't show it again"),
                            "",
                            0,
                            0
                        )
                    )
                    {
                        m_context.m_settings.setValue("horizontalSwitchMessageHidden", true);
                    }

                    channel->_UpdateTitle();
                    channel->_ShowOrHideGraphAndPoin(!channel->isHidden());

                    return;
                }
            }
        }
    }
}

void ChannelSettings::_InitializeShapeCombo()
{
    m_shapeComboBox = new QComboBox(this);

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

    m_formLayout->addRow(new QLabel(tr("Shape"), this), m_shapeComboBox);

}

void ChannelSettings::_InitializeAxisCombo()
{
    m_axisComboBox = new QComboBox(this);
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
        }
        else
            m_channel->GetMeasurement()->RemoveAxis(newAxis);
    }
}
