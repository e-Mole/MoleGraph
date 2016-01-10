#include "AxisChooseDialog.h"
#include <Axis.h>
#include <AxisSettings.h>
#include <Channel.h>
#include <ChannelWithTime.h>
#include <Context.h>
#include <Measurement.h>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QString>
#include <QLabel>
AxisChooseDialog::AxisChooseDialog(
    Context const &context, Channel *originalHChannel, Channel *newHChannel
) :
    QDialog(NULL),
    m_context(context),
    m_newAxis(new QRadioButton(tr("New Axis..."), this)),
    m_originalHChannel(originalHChannel),
    m_newHChannel(newHChannel),
    m_isOriginalChannelRealTime(
        originalHChannel->IsSampleChannel() &&
        ((ChannelWithTime *)originalHChannel)->IsInRealtimeStyle()
    )
{
    QVBoxLayout *layout = new QVBoxLayout(this);


    QString text;
    if (m_isOriginalChannelRealTime)
    {
        text =
            QString(tr("There might be just one channel on a horizontal axis. Channel '%1' have Real time style and must be assigned to an empty axis. Please, choose one.")).
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

        if (m_isOriginalChannelRealTime && !_IsEmptyAxis(axis))
            continue;

        QRadioButton *rb = new QRadioButton(axis->GetTitle(), this);

        m_axes.insert(rb, axis);
        connect(rb, SIGNAL(clicked()), this, SLOT(axisSelected()));
        layout->addWidget(rb);
    }
}

bool AxisChooseDialog::_IsEmptyAxis(Axis *axis)
{
    foreach (Channel *channel, m_originalHChannel->GetMeasurement()->GetChannels())
    {
        if (channel == m_newHChannel)
            continue; //will be moved so it will not be here

        if (channel->GetAxis() == axis)
            return false;
    }

    return true;
}

void AxisChooseDialog::newAxisSelected()
{
    Axis*newAxis = m_originalHChannel->GetMeasurement()->CreateAxis(m_originalHChannel->GetColor());

    AxisSettings dialog(newAxis, m_context);
    if (QDialog::Accepted == dialog.exec())
    {
        m_originalHChannel->AssignToAxis(newAxis);
        if (m_isOriginalChannelRealTime)
            m_originalHChannel->UpdateGraphAxisStyle();
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

    if (m_isOriginalChannelRealTime)
        m_originalHChannel->UpdateGraphAxisStyle();

    accept();
}


