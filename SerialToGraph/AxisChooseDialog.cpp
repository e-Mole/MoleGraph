#include "AxisChooseDialog.h"
#include <Axis.h>
#include <AxisSettings.h>
#include <ChannelBase.h>
#include <ChannelWidget.h>
#include <GlobalSettings.h>
#include <graphics/GraphicsContainer.h>
#include <SampleChannel.h>
#include <ChannelGraph.h>
#include <MainWindow.h>
#include <Measurement.h>
#include <QLabel>
#include <QRadioButton>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>


AxisChooseDialog::AxisChooseDialog(QWidget *parent, GraphicsContainer *graphicsContainer, ChannelWidget *originalHorizontalChannelWidget, ChannelWidget *newHorizontalChannelWidget) :
    QDialog(parent),   
    m_graphicsContainer(graphicsContainer),
    m_originalHorizontalChannelWidget(originalHorizontalChannelWidget),
    m_newHorizontalChannelWidget(newHorizontalChannelWidget),
    m_isOriginalChannelRealTime(false),
    m_newAxis(NULL)
{
    ChannelProxyBase *proxy = m_graphicsContainer->GetChannelProxy(originalHorizontalChannelWidget);
    SampleChannelProxy *sampleChannelProxy = dynamic_cast<SampleChannelProxy*>(proxy);
    m_isOriginalChannelRealTime = sampleChannelProxy && (sampleChannelProxy->GetStyle() == SampleChannelProxy::RealTime);
    QVBoxLayout *layout = new QVBoxLayout(this);

    QString text;
    if (m_isOriginalChannelRealTime)
    {
        text =
            QString(tr("There might be just one channel on a horizontal axis. Values of a hannel '%1' are shown in a time format and therefore must be assigned to an empty axis. Please, choose one.")).
                arg(proxy->GetWidget()->GetName());
    }
    else
    {
        text =
            QString(tr("There might be just one channel on a horizontal axis. Please, chose a different one for a channel '%1'.")).
                arg(proxy->GetWidget()->GetName());
    }

    QLabel *label = new QLabel(text, this);
    label->setWordWrap(true);
    layout->addWidget(label);

    m_newAxis = new QRadioButton(tr("New Axis..."), this);
    connect(m_newAxis, SIGNAL(clicked()), this, SLOT(newAxisSelected()));
    layout->addWidget(m_newAxis);

    foreach (Axis *axis, m_graphicsContainer->GetAxes())
    {
        if (axis == proxy->GetWidget()->GetChannelGraph()->GetValuleAxis())
            continue;

        if (m_isOriginalChannelRealTime && !axis->IsEmptyExcept(m_newHorizontalChannelWidget))
            continue;

        QRadioButton *rb = new QRadioButton(axis->GetTitle(), this);

        m_axes.insert(rb, axis);
        connect(rb, SIGNAL(clicked()), this, SLOT(axisSelected()));
        layout->addWidget(rb);
    }
}

void AxisChooseDialog::newAxisSelected()
{
    Axis*newAxis = m_graphicsContainer->CreateYAxis(m_originalHorizontalChannelWidget->GetForeColor());

    AxisSettings dialog(this, newAxis, GlobalSettings::GetInstance().GetAcceptChangesByDialogClosing());
    if (QDialog::Accepted == dialog.exec())
    {
        GlobalSettings::GetInstance().SetSavedState(false);
        m_originalHorizontalChannelWidget->GetChannelGraph()->AssignToAxis(newAxis);
        accept();
    }
    else
        m_graphicsContainer->RemoveAxis(newAxis);
}

void AxisChooseDialog::axisSelected()
{
    for (auto it = m_axes.begin(); it != m_axes.end(); ++it)
    {
        if (it.key() == (QRadioButton *)sender())
        {
            ChannelGraph *channelGraph = m_originalHorizontalChannelWidget->GetChannelGraph();
            channelGraph->AssignToAxis(it.value());
            break;
        }
    }

    accept();
}


