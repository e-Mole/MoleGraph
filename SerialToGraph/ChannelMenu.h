#ifndef CHANNELMENU_H
#define CHANNELMENU_H

#include <bases/MenuDialogBase.h>
#include <QMap>
class ButtonLine;
class ChannelBase;
class ChannelWidget;
class ColorCheckBox;
class GraphicsContainer;
class QLabel;
class QPushButton;
class KeyShortcut;

namespace bases { class ClickableLabel; }
struct Context;
class ChannelMenu : public bases::MenuDialogBase
{
    Q_OBJECT

    void _AddChannel(ChannelWidget *channelWidget, bool removable);
    void _ChangeAllChannels(bool checked);
    QLabel* _GetShortcutLabel(const QString &shortcut);
    void _AddShortcut(unsigned row, QString const &shortcut);
    void FillGrid();
    QString _GetChannelShortcutText(ChannelWidget *channelWidget);

    GraphicsContainer *m_graphicsContainer;
    ButtonLine *m_buttonLine;
    ColorCheckBox *m_graphCheckBox;
    QMap<ChannelWidget*, ColorCheckBox*> m_channelCheckBoxes;
    QMap<ColorCheckBox*, ChannelWidget*> m_checkBoxChannels;
    QMap<QPushButton*, ChannelWidget*> m_editChannels;
    QMap<QPushButton*, ChannelWidget *> m_removeButtonToChannel;
    void _SetGraph(bool checked);
    KeyShortcut *m_plotShortcut;
    QMap<KeyShortcut*, ChannelWidget*> m_shortcutChannels;
    KeyShortcut *m_allChannelsShortcut;
    KeyShortcut *m_noChannelsShortcut;

public:
    ChannelMenu(GraphicsContainer *graphicsContainer);
    ~ChannelMenu();
    void ActivateChannel(ChannelWidget *channelWidget, bool checked);
    void UpdateCheckBoxes();
    void CreatePanelShortcuts();

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
