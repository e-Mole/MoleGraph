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

    Axis *axis = (Axis *)m_axisComboBox->currentData().toLongLong();
    if (m_channel->m_axis != axis)
    {
        changed = true;
        m_channel->m_axis = axis;
    }

    if (changed)
        m_channel->stateChanged();

    accept();
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
    m_formLayout->addRow(new QLabel(tr("Axis"), this), m_axisComboBox);
    connect(m_axisComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(axisChanged(int)));
}

void ChannelSettings::axisChanged(int index)
{
    if (0 == index) //New Axis...
    {
        AxisCopy *newAxis = new AxisCopy(m_context);

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
