#include "ChannelSettings.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QString>
#include <QPushButton>
#include <QCheckBox>
#include <QGridLayout>

ChannelSettings::ChannelSettings(const QString &title, bool selected, bool xAxis, QWidget * parent, Qt::WindowFlags f) :
    QDialog(parent, f),
    m_name(NULL)
{
    setWindowTitle(tr("Channel settings"));
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
	QGridLayout *gridLaout = new QGridLayout(this);
	mainLayout->addLayout(gridLaout);


	QLabel *nameLabel = new QLabel(tr("Title"), this);
	gridLaout->addWidget(nameLabel, 0,0);
    m_name = new QLineEdit(title, this);
	gridLaout->addWidget(m_name, 0, 1);


	QLabel *selectedLabel = new QLabel(tr("Selected"), this);
	gridLaout->addWidget(selectedLabel, 1, 0);
	m_selected = new QCheckBox(this);
	m_selected->setChecked(selected);
	gridLaout->addWidget(m_selected, 1, 1);

	/*QLabel *xAxisLabel = new QLabel(tr("X-axes"), this);
	gridLaout->addWidget(xAxisLabel, 2, 0);
	m_xAxis = new QCheckBox(this);
	m_xAxis->setChecked(xAxis);
	gridLaout->addWidget(m_xAxis, 2, 1);
*/
    QHBoxLayout *buttonLayout = new QHBoxLayout(this);
    mainLayout->addLayout(buttonLayout);

    QPushButton *store = new QPushButton(tr("store"), this);
    buttonLayout->addWidget(store);
    connect(store, SIGNAL(clicked(bool)), this, SLOT(accept()));

    QPushButton *cancel = new QPushButton(tr("cancel"), this);
    buttonLayout->addWidget(cancel);
    connect(cancel, SIGNAL(clicked(bool)), this, SLOT(reject()));
}

QString ChannelSettings::GetName()
{
    return m_name->text();
}

bool ChannelSettings::GetSelected()
{
	return m_selected-> isChecked();
}
