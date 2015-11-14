#include "ChannelSettings.h"
#include <QComboBox>
#include <QCheckBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <Channel.h>

ChannelSettings::ChannelSettings(Channel *channel, QWidget *parent) :
    FormDialogBase(parent, tr("Channel settings")),
    m_channel(channel),
    m_name(NULL),
    m_shape(NULL)
{
    m_name = new QLineEdit(channel->GetName(), this);
    m_formLayout->addRow(new QLabel(tr("Title"), this),  m_name);

    if (!channel->IsSampleChannel())
    {
        m_units = new QLineEdit(channel->GetUnits(), this);
        m_formLayout->addRow(new QLabel(tr("Units"), this), m_units);

        m_toRightSide = new QCheckBox(this);
        m_toRightSide->setChecked(channel->ToRightSide());
        m_formLayout->addRow(new QLabel(tr("To right side"), this), m_toRightSide);

        _InitializeShapeCombo(m_formLayout, channel->GetShapeIndex());
    }
}

void ChannelSettings::BeforeAccept()
{
    bool changed = false;
    if (m_channel->m_title != m_name->text())
    {
        changed = true;
        m_channel->m_title = m_name->text();
    }

    if (!m_channel->IsSampleChannel())
    {
        if (m_channel->m_units != m_units->text())
        {
            changed = true;
            m_channel->m_units = m_units->text();
        }
        if (m_channel->m_toRightSide != m_toRightSide->isChecked())
        {
            changed = true;
            m_channel->m_toRightSide = m_toRightSide->isChecked();
        }
        if (m_channel->m_shapeIndex != (unsigned)m_shape->currentIndex())
        {
            changed = true;
            m_channel->m_shapeIndex = m_shape->currentIndex();
        }
    }

    if (changed)
        m_channel->stateChanged();

    accept();
}
void ChannelSettings::_InitializeShapeCombo(QFormLayout *formLaout, unsigned shapeIndex)
{
    m_shape = new QComboBox(this);

    m_shape->addItem(tr("Cross"));
    m_shape->addItem(tr("Plus"));
    m_shape->addItem(tr("Circle"));
    m_shape->addItem(tr("Disc"));
    m_shape->addItem(tr("Square"));
    m_shape->addItem(tr("Diamond"));
    m_shape->addItem(tr("Star"));
    m_shape->addItem(tr("Triangle"));
    m_shape->addItem(tr("Inverted Triangle"));
    m_shape->addItem(tr("Cross and Square"));
    m_shape->addItem(tr("Plus and Square"));
    m_shape->addItem(tr("Cross and Circle"));
    m_shape->addItem(tr("Plus and Circle"));
    m_shape->addItem(tr("Peace"));

    m_shape->setCurrentIndex(shapeIndex);

    formLaout->addRow(new QLabel(tr("Shape"), this), m_shape);

}
