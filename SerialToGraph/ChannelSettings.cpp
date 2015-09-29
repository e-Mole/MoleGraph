#include "ChannelSettings.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QString>
#include <QPushButton>
#include <QCheckBox>
#include <QGridLayout>

ChannelSettings::ChannelSettings
(
	const QString &title,
	const QString &units,
	bool selected,
	bool xAxis,
	bool toRightSide,
	QWidget * parent,
	Qt::WindowFlags f
) :
	QDialog(parent, f),
    m_name(NULL)
{
    setWindowTitle(tr("Channel settings"));
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
	QGridLayout *gridLaout = new QGridLayout(this);
	mainLayout->addLayout(gridLaout);

    unsigned row = 0;

    QLabel *selectedLabel = new QLabel(tr("Selected"), this);
    gridLaout->addWidget(selectedLabel, row, 0);
    m_selected = new QCheckBox(this);
    m_selected->setChecked(selected);
    connect(m_selected, SIGNAL(clicked(bool)), this , SLOT(selectedCheckboxClicked(bool)));
    gridLaout->addWidget(m_selected, row++, 1);

	QLabel *nameLabel = new QLabel(tr("Title"), this);
	gridLaout->addWidget(nameLabel, row,0);
    m_name = new QLineEdit(title, this);
	gridLaout->addWidget(m_name, row++, 1);

	QLabel *unitsLabel = new QLabel(tr("Units"), this);
	gridLaout->addWidget(unitsLabel, row,0);
	m_units = new QLineEdit(units, this);
	gridLaout->addWidget(m_units, row++, 1);

	QLabel *toRigtSideLabel = new QLabel(tr("To right side"), this);
	gridLaout->addWidget(toRigtSideLabel, row, 0);
	m_toRightSide = new QCheckBox(this);
	m_toRightSide->setChecked(toRightSide);
	gridLaout->addWidget(m_toRightSide, row++, 1);

    QHBoxLayout *buttonLayout = new QHBoxLayout(this);
    mainLayout->addLayout(buttonLayout);

    QPushButton *store = new QPushButton(tr("Store"), this);
    buttonLayout->addWidget(store);
    connect(store, SIGNAL(clicked(bool)), this, SLOT(accept()));

    QPushButton *cancel = new QPushButton(tr("Cancel"), this);
    buttonLayout->addWidget(cancel);
    connect(cancel, SIGNAL(clicked(bool)), this, SLOT(reject()));

    //set intitial state
    selectedCheckboxClicked(selected);
}

void ChannelSettings::selectedCheckboxClicked(bool checked)
{
    m_name->setEnabled(checked);
    m_units->setEnabled(checked);
    m_toRightSide->setEnabled(checked);
}

QString ChannelSettings::GetName()
{
    return m_name->text();
}

bool ChannelSettings::GetSelected()
{
	return m_selected-> isChecked();
}

QString ChannelSettings::GetUnits()
{
	return m_units->text();
}

bool ChannelSettings::IsSetToRightSide()
{
    return m_toRightSide->isChecked();
}
