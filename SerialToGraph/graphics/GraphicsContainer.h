#ifndef GRAPHICSCONTAINER_H
#define GRAPHICSCONTAINER_H

#include <ChannelBase.h>
#include <SampleChannel.h>
#include <QMap>
#include <QString>
#include <QVector>
#include <QWidget>
#include <set>
#include <map>

class Axis;
class Color;
class ChannelGraph;
class ChannelWidget;
class HwChannel;
class KeyShortcut;
class Measurement;
class Plot;
class QColor;
class QCPAxis;
class QHBoxLayout;
class QGridLayout;
class QScrollBar;
class QVBoxLayout;

class GraphicsContainer : public QWidget
{
    Q_OBJECT

    QHBoxLayout *m_mainLayout;
    QVBoxLayout *m_plotAndSliderLayout;
    QVBoxLayout *m_displaysAndSliderLayout;
    QGridLayout *m_displayLayout;
    bool m_grid;
    QString m_name;
    Plot *m_plot;
    QScrollBar *m_scrollBar;
    unsigned m_currentIndex;
    bool m_followMode;
    std::vector<ChannelWidget *> m_channelWidgets;
    std::map<ChannelWidget *, ChannelBase *> m_widgetToChannelMapping;
    std::map<ChannelBase *, ChannelWidget *> m_channelToWidgetMapping;
    std::map<ChannelBase *, ChannelGraph *> m_channelToGraphMapping;
    std::set<double> m_horizontalValueSet;
    ChannelBase *m_horizontalChannel;
    QVector<Axis*> m_axes;

    bool m_marksShown;
    ChannelWidget *m_sampleChannelWidget;
    SampleChannel *m_sampleChannel;

    virtual void resizeEvent(QResizeEvent *){ resized(); }
    virtual QSize sizeHint() const { return QSize(800,700); }

    void _InitializeLayouts();
    void _FollowLastMeasuredValue();
    QCPAxis * _GetGraphAxis(unsigned index);
    Axis * _CreateAxis(QColor const & color, QCPAxis *graphAxis);
    ChannelWidget *_CreateChannelWidget(ChannelBase *channel,
        ChannelGraph *graph,
        unsigned shortcutOrder,
        QString const name,
        QColor const &color,
        bool visible,
        QString const & units,
        bool isSampleChannel
    );
    QString _GetRealTimeText(SampleChannel *channel, double secSinceEpoch);
    void _CreateKeyShortcuts();
    void _RemoveKeyShortcuts();

    KeyShortcut *m_plotKeyShortcut;
    QMap<KeyShortcut*, ChannelWidget*> m_channelWidgetKeyShortcuts;
    KeyShortcut *m_allChannelsShortcut;
    KeyShortcut *m_noChannelsShortcut;

public:
    GraphicsContainer(QWidget *parent, QString const &name, bool markShown);
    ~GraphicsContainer();
    void SetGrid(bool grid);
    void ReplaceDisplays();
    void _AddChannelToMappings(ChannelBase *channel, ChannelWidget *widget, bool isSampleChannel);
    //void RemoveChannel(ChannelBase *channel, bool replaceDisplays);
    QString &GetName() {return m_name; }
    void SetName(QString const &name) {m_name = name; } //TODO:signal?
    Plot *GetPlot() const;
    void SetFollowMode(bool set);
    void RedrawChannelValues();
    int GetCurrentIndex() { return m_currentIndex; }
    void AddHorizontalValue(double value);
    void ClearHorizontalValueSet();
    void CalculateScrollbarRange();
    void ReadingValuesPostProcess(double lastHorizontalValue);
    void ShowGraph(bool show);
    bool IsPlotVisible() const;
    bool IsPlotInRangeMode();
    int GetLastClosestHorizontalValueIndex(double xValue) const;
    unsigned GetPositionByHorizontalValue(double value) const;
    double GetHorizontalValueBySliderPos(unsigned position) const;
    unsigned GetCurrentHorizontalChannelIndex() const;
    unsigned GetHorizontalValueLastInex(double value) const;
    int GetSliderPos();
    void SetHorizontalChannel(ChannelBase *channel);
    Axis * CreateNewAxis(unsigned index);
    Axis * CreateYAxis(QColor const & color);
    void RemoveAxis(Axis * axis);
    QVector<Axis *> const & GetAxes() const;
    Axis *GetAxis(int index);
    unsigned GetAxisCount();
    int GetAxisIndex(Axis *axis);
    Axis * GetFirstVerticalAxis();
    void InitializeAxes(QVector<Axis *> const &sourceAxes);
    bool GetMarksShown();
    void SetMarksShown(bool marksShown);
    ChannelGraph *AddChannelGraph(Axis *valueAxis);
    Axis *InitializeHorizontalAxis();
    Axis *InitializeVerticalAxis();
    void UpdateAxisNames();
    void UpdateAxis(Axis *axis);
    void UpdateAxes();
    ChannelGraph * AddBlackChannelGraph(Axis *valueAxis);
    ChannelGraph * AddChannelGraph(Axis *valueAxis, QColor const &color, unsigned shapeIndex, Qt::PenStyle penStyle);
    void SetAxisStyle(Axis *axis, bool dateTime, QString const &format);
    bool RemoveGraph(ChannelWidget *channelWidget);
    void RescaleAxes(ChannelWidget *channelWidget);
    ChannelBase *GetHorizontalChannel() const;
    std::vector<ChannelWidget *> &GetChannelWidgets();
    unsigned GetChannelWidgetCount();
    ChannelWidget *GetChannelWidget(unsigned index);
    ChannelWidget *GetChannelWidget(ChannelBase *channel);
    ChannelBase * GetChannel(ChannelWidget * widget);
    SampleChannel *GetSampleChannel();
    bool IsHorizontalValueSetEmpty();
    void RecalculateSliderMaximum();
    ChannelGraph* CloneChannelGraph(GraphicsContainer *sourceContainer, ChannelWidget *sourceChannelWidget);
    QColor GetColorByOrder(unsigned order);
    ChannelWidget *CreateSampleChannelWidget(SampleChannel *channel, Axis *valueAxis);
    ChannelWidget *CloneSampleChannelWidget(SampleChannel *channel, GraphicsContainer *sourceGraphicsContainer, ChannelWidget *sourceChannelWidget);

    ChannelWidget *_CreateHwChannelWidget(HwChannel *channel,
        Axis *valueAxis,
        unsigned shortcutOrder,
        QString const name,
        QColor const &color,
        bool visible,
        QString const & units);

    ChannelWidget *CloneHwChannelWidget(HwChannel *channel, GraphicsContainer *sourceGraphicsContainer, ChannelWidget *sourceChannelWidget, unsigned shortcutOrder);
    void UpdateGraphs();
    QString GetRealTimeFormatText(SampleChannel::RealTimeFormat realTimeFormat);
    QString GetSampleChannelStyleText(SampleChannel::Style style);
    QString GetValueTimestamp(SampleChannel *channel, unsigned index);
    void Activate();
    void Deactivate();
    QKeySequence GetPlotKeySequence();
    QKeySequence GetChannelWidgetKeySequence(ChannelWidget *channelWidget);
    QKeySequence GetAllChannelsSequence();
    QKeySequence GetNoChannelsSequence();
    void ActivateChannel(ChannelWidget *channelWidget, bool checked);
signals:
    void resized();

public slots:
    void sliderValueChanged(int value);
    void editChannel(ChannelWidget *channelWidget);
    void editChannel();
    void addNewValueSet();
    void sampleChannelPropertyChanged();
    void channelKeyShortcut();
    void plotKeyShortcut();
    void noChannelsKeyShortcut();
    void allChannelsKeyShortcut();
    void replaceDisplays();
private slots:
    void markerLinePositionChanged(int position);
    void hwValueChanged(unsigned index);
};

#endif // GRAPHICSCONTAINER_H
