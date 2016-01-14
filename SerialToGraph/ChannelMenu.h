#ifndef CHANNELMENU_H
#define CHANNELMENU_H

#include <MenuDialogBase.h>
#include <QMap>
class Channel;
class ClickableLabel;
class Context;
class Measurement;
class QCheckBox;
class QLabel;
class QPushButton;
class ButtonLine;

class ChannelMenu : public MenuDialogBase
{
    Q_OBJECT

    void _AddChannel(Channel *channel, unsigned row);
    void _ChangeAllChannels(bool checked);
    QLabel* _GetShortcutLabel(const QString &shortcut);

    Measurement &m_measurement;
    ButtonLine *m_buttonLine;
    QCheckBox *m_graphCheckBox;
    QMap<Channel*, QCheckBox*> m_channelCheckBoxes;
    QMap<QCheckBox*, Channel*> m_checkBoxChannels;
    QMap<ClickableLabel*, Channel*> m_labelChannels;
    QMap<Channel*, ClickableLabel*> m_channelLabels;
    QMap<QPushButton*, Channel*> m_editChannels;
    void _SetGraph(bool checked);

public:
    explicit ChannelMenu(Measurement &measurement, ButtonLine *buttonLine);
    void ActivateChannel(Channel *channel, bool checked);
    void FillGrid();
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
