#include "AxisChooseDialog.h"
#include <Axis.h>
#include <AxisSettings.h>
#include <GlobalSettings.h>
#include <graphics/ChannelProxyBase.h>
#include <graphics/GraphicsContainer.h>
#include <graphics/SampleChannelProxy.h>
#include <ChannelGraph.h>
#include <MainWindow.h>
#include <Measurement.h>
#include <bases/Label.h>
#include <bases/RadioButton.h>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>


AxisChooseDialog::AxisChooseDialog(QWidget *parent, GraphicsContainer *graphicsContainer, ChannelProxyBase *originalHorizontalChannelProxy, ChannelProxyBase *newHorizontalChannelProxy) :
    QDialog(parent),
    m_graphicsContainer(graphicsContainer),
    m_originalHorizontalChannelProxy(originalHorizontalChannelProxy),
    m_newHorizontalChannelProxy(newHorizontalChannelProxy),
    m_isOriginalChannelRealTime(false),
    m_newAxis(NULL)
{
    SampleChannelProxy *sampleChannelProxy = dynamic_cast<SampleChannelProxy*>(originalHorizontalChannelProxy);
    m_isOriginalChannelRealTime = sampleChannelProxy && (sampleChannelProxy->GetStyle() == SampleChannelProperties::RealTime);
    QVBoxLayout *layout = new QVBoxLayout(this);

    QString text;
    if (m_isOriginalChannelRealTime)
    {
        text =
            QString(tr("There might be just one channel on a horizontal axis. Values of a hannel '%1' are shown in a time format and therefore must be assigned to an empty axis. Please, choose one.")).
                arg(originalHorizontalChannelProxy->GetName());
    }
    else
    {
        text =
            QString(tr("There might be just one channel on a horizontal axis. Please, chose a different one for a channel '%1'.")).
                arg(originalHorizontalChannelProxy->GetName());
    }

    Label *label = new Label(text, this);
    label->setWordWrap(true);
    layout->addWidget(label);

    m_newAxis = new RadioButton(tr("New Axis..."), this);
    connect(m_newAxis, SIGNAL(clicked()), this, SLOT(newAxisSelected()));
    layout->addWidget(m_newAxis);

    foreach (Axis *axis, m_graphicsContainer->GetAxes())
    {
        if (axis == originalHorizontalChannelProxy->GetChannelGraph()->GetValuleAxis())
            continue;

        if (m_isOriginalChannelRealTime && !axis->IsEmptyExcept(m_newHorizontalChannelProxy))
            continue;

        RadioButton *rb = new RadioButton(axis->GetTitle(), this);

        m_axes.insert(rb, axis);
        connect(rb, SIGNAL(clicked()), this, SLOT(axisSelected()));
        layout->addWidget(rb);
    }
}

void AxisChooseDialog::newAxisSelected()
{
    Axis*newAxis = m_graphicsContainer->CreateYAxis(m_originalHorizontalChannelProxy->GetForeColor());

    AxisSettings dialog(this, newAxis, GlobalSettings::GetInstance().GetAcceptChangesByDialogClosing());
    if (QDialog::Accepted == dialog.exec())
    {
        GlobalSettings::GetInstance().SetSavedState(false);
        m_originalHorizontalChannelProxy->AssignToAxis(newAxis);
        accept();
    }
    else
        m_graphicsContainer->RemoveAxis(newAxis);
}

void AxisChooseDialog::axisSelected()
{
    for (auto it = m_axes.begin(); it != m_axes.end(); ++it)
    {
        if (it.key() == (RadioButton *)sender())
        {
            m_originalHorizontalChannelProxy->AssignToAxis(it.value());
            break;
        }
    }

    accept();
}


