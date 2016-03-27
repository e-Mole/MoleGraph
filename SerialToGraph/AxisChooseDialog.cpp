#include "AxisChooseDialog.h"
#include <Axis.h>
#include <AxisSettings.h>
#include <ChannelBase.h>
#include <SampleChannel.h>
#include <Context.h>
#include <Measurement.h>
#include <QLabel>
#include <QRadioButton>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>


AxisChooseDialog::AxisChooseDialog(
    QWidget *parent, Context const &context, ChannelBase *originalHChannel, ChannelBase *newHChannel
) :
    QDialog(parent),
    m_context(context),
    m_newAxis(new QRadioButton(tr("New Axis..."), this)),
    m_originalHChannel(originalHChannel),
    m_newHChannel(newHChannel),
    m_isOriginalChannelRealTime(
        originalHChannel->IsSampleChannel() &&
        ((SampleChannel *)originalHChannel)->IsInRealtimeStyle()
    )
{
    QVBoxLayout *layout = new QVBoxLayout(this);


    QString text;
    if (m_isOriginalChannelRealTime)
    {
        text =
            QString(tr("There might be just one channel on a horizontal axis. Values of a hannel '%1' are shown in a time format and therefore must be assigned to an empty axis. Please, choose one.")).
                arg(originalHChannel->GetName());
    }
    else
    {
        text =
            QString(tr("There might be just one channel on a horizontal axis. Please, chose a different one for a channel '%1'.")).
                arg(originalHChannel->GetName());
    }

    QLabel *label = new QLabel(text, this);
    label->setWordWrap(true);
    layout->addWidget(label);

    connect(m_newAxis, SIGNAL(clicked()), this, SLOT(newAxisSelected()));
    layout->addWidget(m_newAxis);

    foreach (Axis *axis, originalHChannel->GetMeasurement()->GetAxes())
    {
        if (axis == originalHChannel->GetAxis())
            continue;

        if (m_isOriginalChannelRealTime && !axis->IsEmptyExcept(m_newHChannel))
            continue;

        QRadioButton *rb = new QRadioButton(axis->GetTitle(), this);

        m_axes.insert(rb, axis);
        connect(rb, SIGNAL(clicked()), this, SLOT(axisSelected()));
        layout->addWidget(rb);
    }
}

void AxisChooseDialog::newAxisSelected()
{
    Axis*newAxis = m_originalHChannel->GetMeasurement()->CreateAxis(m_originalHChannel->GetColor());

    AxisSettings dialog(this, newAxis, m_context);
    if (QDialog::Accepted == dialog.exec())
    {
        m_originalHChannel->AssignToAxis(newAxis);
        accept();
    }
    else
        m_originalHChannel->GetMeasurement()->RemoveAxis(newAxis);
}

void AxisChooseDialog::axisSelected()
{
    for (auto it = m_axes.begin(); it != m_axes.end(); ++it)
    {
        if (it.key() == (QRadioButton *)sender())
        {
            m_originalHChannel->AssignToAxis(it.value());
            break;
        }
    }

    accept();
}


