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
    m_axisOriginal(axis),
    m_axisCopy(*axis),
    m_name(NULL),
    m_context(context)
{
    m_formLayout->addRow(new QLabel(tr("Type"), this), new QLabel(axis->IsHorizontal() ? tr("Horizontal") : tr("Vertical"), this));

    m_name = new QLineEdit(axis->GetTitle(), this);
    m_formLayout->addRow(new QLabel(tr("Name"), this), m_name);
    connect(m_name, SIGNAL(textChanged(QString)), this, SLOT(nameChanged(QString)));

    AddColorButtonRow(axis->GetColor());
    if (!axis->m_isHorizontal)
    {
        QComboBox *side = new QComboBox(this);
        side->addItem(tr("Left"));
        side->addItem(tr("Right"));
        side->setCurrentIndex((int)m_axisCopy.m_isOnRight);
        connect(side, SIGNAL(currentIndexChanged(int)), this, SLOT(sideChanged(int)));
        m_formLayout->addRow(new QLabel(tr("Side"), this), side);
    }

    QComboBox *display = new QComboBox(this);
    display->addItem(tr("Channels and Units"));
    display->addItem(tr("Name"));
    display->setCurrentIndex((int)m_axisCopy.m_displayName);
    connect(display, SIGNAL(currentIndexChanged(int)), this, SLOT(displayChanged(int)));
    m_formLayout->addRow(new QLabel(tr("Display in Graph"), this), display);

}

void AxisSettings::nameChanged(QString const &text)
{
    m_axisCopy.m_title = text;
}

void AxisSettings::sideChanged(int index)
{
   m_axisCopy.m_isOnRight = (bool)index;
}

void AxisSettings::displayChanged(int index)
{
    m_axisCopy.m_displayName = (bool)index;
}

bool AxisSettings::BeforeAccept()
{

    bool sideChanged = m_axisOriginal->IsOnRight() != m_axisCopy.IsOnRight();
    bool displayNameChanged =
            m_axisOriginal->m_displayName != m_axisCopy.m_displayName ||
            (m_axisCopy.m_displayName && m_axisOriginal->m_title != m_axisCopy.m_title);

    *m_axisOriginal = m_axisCopy;

    //also set color to plot axis
    m_axisOriginal->_SetColor(m_axisCopy.m_color);
    
    if (!m_context.m_axes.contains(m_axisOriginal))
    {
        m_context.m_axes.push_back(m_axisOriginal);
        m_axisOriginal->_AssignGraphAxis(m_axisOriginal->m_measurement->GetPlot()->AddYAxis(m_axisOriginal->IsOnRight()));
    }
    else
    {
        if (sideChanged)
            m_axisOriginal->_AssignGraphAxis(m_axisOriginal->m_measurement->GetPlot()->AddYAxis(m_axisOriginal->IsOnRight()));
        if (displayNameChanged)
            m_axisOriginal->UpdateGraphAxisName();
    }

    m_axisOriginal->m_measurement->GetPlot()->ReplotIfNotDisabled();
    return true;
}

void AxisSettings::ColorChanged(QColor &color)
{
    m_axisCopy.m_color = color;
}

