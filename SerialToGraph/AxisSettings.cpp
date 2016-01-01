#include "AxisSettings.h"
#include <Axis.h>
#include <Context.h>
#include <Measurement.h>
#include <Plot.h>
#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPalette>
#include <QPushButton>
#include <QVector>
#include <QWidget>

AxisSettings::AxisSettings(Axis *axis, Context const & context) :
    FormDialogColor(NULL, tr("Edit Axis...")),
    m_axis(axis),
    m_name(new QLineEdit(axis->GetTitle(), this)),
    m_context(context),
    m_side(NULL),
    m_display(new QComboBox(this))
{
    m_formLayout->addRow(new QLabel(tr("Type"), this), new QLabel(axis->IsHorizontal() ? tr("Horizontal") : tr("Vertical"), this));
    m_formLayout->addRow(new QLabel(tr("Name"), this), m_name);

    AddColorButtonRow(axis->GetColor());
    if (!axis->m_isHorizontal)
    {
        m_side = new QComboBox(this);
        m_side->addItem(tr("Left"));
        m_side->addItem(tr("Right"));
        m_side->setCurrentIndex((int)m_axis->m_isOnRight);
        m_formLayout->addRow(new QLabel(tr("Side"), this), m_side);
    }

    m_display->addItem(tr("Channels and Units"));
    m_display->addItem(tr("Name"));
    m_display->setCurrentIndex((int)m_axis->m_isShownName);
    m_formLayout->addRow(new QLabel(tr("Show in Graph"), this), m_display);

}

bool AxisSettings::BeforeAccept()
{
    m_axis->m_title = m_name->text();

    bool newDisplayName = (bool)m_display->currentIndex();
    if (m_axis->m_isShownName != newDisplayName ||
        (newDisplayName && m_axis->m_title != m_name->text()))
    {
        m_axis->m_isShownName = newDisplayName;
            m_axis->UpdateGraphAxisName();
    }

    if (m_side != NULL && m_axis->m_isOnRight != (bool)m_side->currentIndex())
    {
        m_axis->m_isOnRight = (bool)m_side->currentIndex();
        m_axis->_AssignGraphAxis(m_axis->m_measurement->GetPlot()->AddYAxis(m_axis->m_isOnRight));
    }

    //also set color to plot axis
    m_axis->_SetColor(m_color);

    m_axis->m_measurement->GetPlot()->ReplotIfNotDisabled();
    return true;
}

