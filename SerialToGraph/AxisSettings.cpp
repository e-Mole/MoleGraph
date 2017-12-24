#include "AxisSettings.h"
#include <Axis.h>
#include <graphics/GraphicsContainer.h>
#include <MainWindow.h>
#include <Measurement.h>
#include <Plot.h>
#include <bases/ComboBox.h>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPalette>
#include <QPushButton>
#include <QVector>
#include <QWidget>

AxisSettings::AxisSettings(QWidget *parent, Axis *axis, bool acceptChangesByDialogClosing) :
    bases::FormDialogColor(parent, tr("Axis Setting"), acceptChangesByDialogClosing),
    m_axis(axis),
    m_name(NULL),
    m_side(NULL),
    m_display(NULL)
{
    m_formLayout->addRow(new QLabel(tr("Type"), this), new QLabel(axis->IsHorizontal() ? tr("Horizontal") : tr("Vertical"), this));

    m_name = new QLineEdit(axis->GetTitle(), this);
    m_formLayout->addRow(new QLabel(tr("Name"), this), m_name);

    AddColorButtonRow(axis->GetColor());
    if (!axis->m_isHorizontal)
    {
        m_side = new bases::ComboBox(this);
        m_side->addItem(tr("Left"));
        m_side->addItem(tr("Right"));
        m_side->setCurrentIndex((int)m_axis->m_isOnRight);
        m_formLayout->addRow(new QLabel(tr("Side"), this), m_side);
    }

    m_display = new bases::ComboBox(this);
    m_display->addItem(tr("Channels and Units"));
    m_display->addItem(tr("Axis Name"));
    m_display->setCurrentIndex((int)m_axis->m_isShownName);
    m_formLayout->addRow(new QLabel(tr("Show in Graph"), this), m_display);
}

bool AxisSettings::BeforeAccept()
{
    bool changed = false;

    if (m_name->text() != m_axis->m_title)
    {
        changed = true;
        m_axis->m_title = m_name->text();
    }

    bool newDisplayName = (bool)m_display->currentIndex();
    if (m_axis->m_isShownName != newDisplayName || (newDisplayName && m_axis->m_title != m_name->text()))
    {
        changed = true;
        m_axis->_SetIsShownName(newDisplayName);
    }

    if (m_side != NULL && m_axis->m_isOnRight != (bool)m_side->currentIndex())
    {
        changed = true;
        m_axis->_SetIsOnRight((bool)m_side->currentIndex());
    }
    //also set color to plot axis
    if (m_color != m_axis->GetColor())
    {
        changed = true;
        m_axis->_SetColor(m_color);
    }

    m_axis->m_graphicsContainer->GetPlot()->ReplotIfNotDisabled();

    m_changed = changed;
    return true;
}

