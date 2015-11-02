#include "ChannelSettings.h"
#include <QComboBox>
#include <QCheckBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <Channel.h>

ChannelSettings::ChannelSettings(Channel *channel, QWidget *parent) :
    QDialog(parent),
    m_channel(channel),
    m_name(NULL),
    m_shape(NULL)
{
    setWindowTitle(tr("Channel settings"));
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
	QGridLayout *gridLaout = new QGridLayout(this);
	mainLayout->addLayout(gridLaout);

    unsigned row = 0;

    QLabel *nameLabel = new QLabel(tr("Title"), this);
	gridLaout->addWidget(nameLabel, row,0);
    m_name = new QLineEdit(channel->GetName(), this);
	gridLaout->addWidget(m_name, row++, 1);

    if (!channel->IsSampleChannel())
    {
        QLabel *unitsLabel = new QLabel(tr("Units"), this);
        gridLaout->addWidget(unitsLabel, row,0);
        m_units = new QLineEdit(channel->GetUnits(), this);
        gridLaout->addWidget(m_units, row++, 1);

        QLabel *toRigtSideLabel = new QLabel(tr("To right side"), this);
        gridLaout->addWidget(toRigtSideLabel, row, 0);
        m_toRightSide = new QCheckBox(this);
        m_toRightSide->setChecked(channel->ToRightSide());
        gridLaout->addWidget(m_toRightSide, row++, 1);

        _InitializeShapeCombo(gridLaout, row++, channel->GetShapeIndex());
    }

    QHBoxLayout *buttonLayout = new QHBoxLayout(this);
    mainLayout->addLayout(buttonLayout);

    QPushButton *store = new QPushButton(tr("Store"), this);
    buttonLayout->addWidget(store);
    connect(store, SIGNAL(clicked(bool)), this, SLOT(storeAndAccept()));

    QPushButton *cancel = new QPushButton(tr("Cancel"), this);
    buttonLayout->addWidget(cancel);
    connect(cancel, SIGNAL(clicked(bool)), this, SLOT(reject()));
}

void ChannelSettings::storeAndAccept()
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
        if (m_channel->m_shapeIndex != m_shape->currentIndex())
        {
            changed = true;
            m_channel->m_shapeIndex = m_shape->currentIndex();
        }
    }

    if (changed)
        m_channel->stateChanged();

    accept();
}
void ChannelSettings::_InitializeShapeCombo(QGridLayout *gridLaout, unsigned row, unsigned shapeIndex)
{
    QLabel *shapeLabel = new QLabel(tr("Shape"), this);
    gridLaout->addWidget(shapeLabel, row,0);

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

    gridLaout->addWidget(m_shape, row, 1);

}
