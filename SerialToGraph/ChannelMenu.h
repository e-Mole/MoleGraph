#ifndef CHANNELMENU_H
#define CHANNELMENU_H

#include <bases/MenuDialogBase.h>
#include <QMap>
class ChannelBase;
class Context;
class Measurement;
class QCheckBox;
class QLabel;
class QPushButton;
class ButtonLine;

namespace bases { class ClickableLabel; }

class ChannelMenu : public bases::MenuDialogBase
{
    Q_OBJECT

    void _AddChannel(ChannelBase *channel, unsigned row);
    void _ChangeAllChannels(bool checked);
    QLabel* _GetShortcutLabel(const QString &shortcut);
    void FillGrid();

    Measurement &m_measurement;
    ButtonLine *m_buttonLine;
    QCheckBox *m_graphCheckBox;
    QMap<ChannelBase*, QCheckBox*> m_channelCheckBoxes;
    QMap<QCheckBox*, ChannelBase*> m_checkBoxChannels;
    QMap<bases::ClickableLabel*, ChannelBase*> m_labelChannels;
    QMap<ChannelBase*, bases::ClickableLabel*> m_channelLabels;
    QMap<QPushButton*, ChannelBase*> m_editChannels;
    void _SetGraph(bool checked);

public:
    explicit ChannelMenu(QWidget *parent, Measurement &measurement, ButtonLine *buttonLine);
    void ActivateChannel(ChannelBase *channel, bool checked);
    void UpdateLabels();
signals:

public slots:
    void channelActivated();
    void graphActivated();
    void noChannelsActivated();
    void allChannelsActivated();
    void edit();

};

#endif // CHANNELMENU_H
