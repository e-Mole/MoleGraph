#ifndef CHANNELMENU_H
#define CHANNELMENU_H

#include <bases/MenuDialogBase.h>
#include <QMap>
class ChannelBase;
class Context;
class ColorCheckBox;
class Measurement;
class QLabel;
class QPushButton;
class ButtonLine;

namespace bases { class ClickableLabel; }
struct Context;
class ChannelMenu : public bases::MenuDialogBase
{
    Q_OBJECT

    void _AddChannel(ChannelBase *channel, unsigned row);
    void _ChangeAllChannels(bool checked);
    QLabel* _GetShortcutLabel(const QString &shortcut);
    void _AddShortcut(unsigned row, QString const &shortcut);
    void FillGrid();

    Measurement &m_measurement;
    ButtonLine *m_buttonLine;
    ColorCheckBox *m_graphCheckBox;
    QMap<ChannelBase*, ColorCheckBox*> m_channelCheckBoxes;
    QMap<ColorCheckBox*, ChannelBase*> m_checkBoxChannels;
    QMap<QPushButton*, ChannelBase*> m_editChannels;
    void _SetGraph(bool checked);
    Context const &m_context;

public:
    explicit ChannelMenu(QWidget *parent, Context const &context, Measurement &measurement, ButtonLine *buttonLine);
    void ActivateChannel(ChannelBase *channel, bool checked);
    void UpdateCheckBoxes();
signals:

public slots:
    void channelActivated();
    void graphActivated();
    void noChannelsActivated();
    void allChannelsActivated();
    void edit();

};

#endif // CHANNELMENU_H
