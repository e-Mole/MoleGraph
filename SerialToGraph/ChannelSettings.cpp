#include "ChannelSettings.h"
#include <Axis.h>
#include <AxisEditDialog.h>
#include <Channel.h>
#include <Context.h>
#include <QComboBox>
#include <QCheckBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>

ChannelSettings::ChannelSettings(Channel *channel, QWidget *parent, Context &context) :
    FormDialogBase(parent, tr("Channel settings")),
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

    _InitializeShapeCombo();
    _InitializeAxisCombo();
}

void ChannelSettings::BeforeAccept()
{
    bool changed = false;
    if (m_channel->m_title != m_name->text())
    {
        changed = true;
        m_channel->m_title = m_name->text();
    }


    if (m_channel->m_units != m_units->text())
    {
        changed = true;
        m_channel->m_units = m_units->text();
    }

    if (m_channel->m_shapeIndex != (unsigned)m_shapeComboBox->currentIndex())
    {
        changed = true;
        m_channel->m_shapeIndex = m_shapeComboBox->currentIndex();
    }

    Channel * lastHorizontal = NULL;
    bool changedhorizontal = false;
    Axis *axis = (Axis *)m_axisComboBox->currentData().toLongLong();
    if (m_channel->m_axis != axis)
    {
        changed = true;
        if (axis->IsHorizontal())
        {
            lastHorizontal = _MoveLastHorizontalToVertical();
            changedhorizontal = true;
        }
        m_channel->m_axis = axis;
    }

    if (changed)
    {
        if (changedhorizontal)
        {
            m_channel->stateChangedToHorizontal(); //to be redrown all graph axis
            lastHorizontal->stateChanged(); //to be redrown display
        }
        else
            m_channel->stateChanged();
    }
    accept();
}

Channel * ChannelSettings::_MoveLastHorizontalToVertical()
{
    foreach (Channel *channel, m_context.m_channels)
    {
        if (channel->m_axis->IsHorizontal())
        {
            foreach (Axis *axis, m_context.m_axes)
            {
                if (!axis->IsHorizontal())
                {
                    channel->m_axis = axis;
                    QMessageBox::warning(
                        this,
                        m_context.m_applicationName,
                        QString(tr("Only one horizontal channel is supported. Axis '%1' has been assigned to the channel '%2'")).
                            arg(axis->GetTitle()).
                            arg(channel->GetName())
                    );

                    return channel;
                }
            }
        }
    }
    return NULL; //it should not be passed
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
    foreach (Axis *axis, m_context.m_axes)
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
        Axis*newAxis = new Axis(m_context, m_channel->GetName(), m_channel->GetColor());

        AxisEditDialog dialog(newAxis);
        if (QDialog::Accepted == dialog.exec())
        {
            m_context.m_axes.push_back(newAxis);
             m_axisComboBox->addItem(newAxis->GetTitle(), (qlonglong)newAxis);
             m_axisComboBox->setCurrentIndex(m_axisComboBox->findData((qlonglong)(newAxis)));
        }
        else
            delete newAxis;
    }
}
