#ifndef CHANNELMENU_H
#define CHANNELMENU_H

#include <bases/MenuDialogBase.h>
#include <QMap>
class ButtonLine;
class ChannelBase;
class Context;
class ColorCheckBox;
class Measurement;
class QLabel;
class QPushButton;
class KeyShortcut;

namespace bases { class ClickableLabel; }
struct Context;
class ChannelMenu : public bases::MenuDialogBase
{
    Q_OBJECT

    void _AddChannel(ChannelBase *channel, bool removable);
    void _ChangeAllChannels(bool checked);
    QLabel* _GetShortcutLabel(const QString &shortcut);
    void _AddShortcut(unsigned row, QString const &shortcut);
    void FillGrid();
    ChannelBase * _GetFirstGhostableChannel();
    QString _GetChannelShortcutText(ChannelBase *channel);

    Measurement &m_measurement;
    ButtonLine *m_buttonLine;
    ColorCheckBox *m_graphCheckBox;
    QMap<ChannelBase*, ColorCheckBox*> m_channelCheckBoxes;
    QMap<ColorCheckBox*, ChannelBase*> m_checkBoxChannels;
    QMap<QPushButton*, ChannelBase*> m_editChannels;
    QMap<QPushButton*, ChannelBase *> m_removeButtonToChannel;
    void _SetGraph(bool checked);
    Context const &m_context;
    KeyShortcut *m_graphShortcut;
    QMap<KeyShortcut*, ChannelBase*> m_shortcutChannels;
    KeyShortcut *m_allChannelsShortcut;
    KeyShortcut *m_noChannelsShortcut;

public:
    explicit ChannelMenu(QWidget *parent, Context const &context, Measurement &measurement);
    void ActivateChannel(ChannelBase *channel, bool checked);
    void UpdateCheckBoxes();
    void CreatePanelShortcuts();
    void ClearPanelShortcuts();

signals:
    void stateChanged();

public slots:
    void channelActivatedCheckBox();
    void channelActivatedShortcut();
    void graphActivated();
    void noChannelsActivated();
    void allChannelsActivated();
    void edit();
    void remove();

};

#endif // CHANNELMENU_H
