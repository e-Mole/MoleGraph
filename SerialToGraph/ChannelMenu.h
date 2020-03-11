#ifndef CHANNELMENU_H
#define CHANNELMENU_H

#include <bases/MenuDialogBase.h>
#include <QMap>
class ButtonLine;
class ChannelProxyBase;
class ColorCheckBox;
class GraphicsContainer;
class Label;
class PushButton;
class KeyShortcut;

struct Context;
class ChannelMenu : public bases::MenuDialogBase
{
    Q_OBJECT

    void _AddChannel(ChannelProxyBase *channelProxy);
    void _ChangeAllChannels(bool checked);
    Label* _GetShortcutLabel(const QString &shortcut);
    void _AddShortcut(int row, QString const &shortcut);
    void FillGrid();
    QString _GetChannelShortcutText(ChannelProxyBase *channelProxy);

    GraphicsContainer *m_graphicsContainer;
    ButtonLine *m_buttonLine;
    ColorCheckBox *m_graphCheckBox;
    QMap<ChannelProxyBase*, ColorCheckBox*> m_channelCheckBoxes;
    QMap<ColorCheckBox*, ChannelProxyBase*> m_checkBoxChannels;
    QMap<PushButton*, ChannelProxyBase*> m_editChannels;
    QMap<PushButton*, ChannelProxyBase *> m_removeButtonToChannel;
    void _SetGraph(bool checked);
    KeyShortcut *m_plotShortcut;
    QMap<KeyShortcut*, ChannelProxyBase*> m_shortcutChannels;
    KeyShortcut *m_allChannelsShortcut;
    KeyShortcut *m_noChannelsShortcut;
    bool m_isGhostAddable;

public:
    ChannelMenu(GraphicsContainer *graphicsContainer, bool isGhostAddable);
    ~ChannelMenu();
    void ActivateChannel(ChannelProxyBase *channelProxy, bool checked);
    void UpdateCheckBoxes();
    void CreatePanelShortcuts();
signals:
    void addGhostChannelActivated();
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
