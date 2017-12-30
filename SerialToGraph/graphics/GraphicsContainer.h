#ifndef GRAPHICSCONTAINER_H
#define GRAPHICSCONTAINER_H

#include <QWidget>
#include <QString>
#include <QVector>
#include <set>
#include <map>

class Axis;
class ChannelBase;
class ChannelGraph;
class ChannelWidget;
class Plot;
class QColor;
class QCPAxis;
class QHBoxLayout;
class QGridLayout;
class QScrollBar;
class QVBoxLayout;
class SampleChannel;

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
    std::map<ChannelWidget *, ChannelBase *> m_channelMapping;
    std::set<double> m_horizontalValueSet;
    ChannelBase *m_horizontalChannel;
    QVector<Axis*> m_axes;

    bool m_marksShown;

    virtual void resizeEvent(QResizeEvent *){ resized(); }
    virtual QSize sizeHint() const { return QSize(800,700); }

    void _InitializeLayouts();
    void _FollowLastMeasuredValue();
    QCPAxis * _GetGraphAxis(unsigned index);
    Axis * _CreateAxis(QColor const & color, QCPAxis *graphAxis);
public:
    GraphicsContainer(QWidget *parent, QString const &name, bool markShown);
    ~GraphicsContainer();
    bool SetGrid(bool grid);
    void ReplaceDisplays();
    void AddChannel(ChannelBase *channel, bool replaceDisplays);
    void RemoveChannel(unsigned index, bool replaceDisplays);
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
    ChannelBase * GetChannel(ChannelWidget * widget);
    SampleChannel *GetSampleChannel();
    bool IsHorizontalValueSetEmpty();
    void RecalculateSliderMaximum();

signals:
    void resized();
public slots:
    void sliderValueChanged(int value);
    void editChannel(ChannelWidget *channelWidget);
private slots:
    void markerLinePositionChanged(int position);
};

#endif // GRAPHICSCONTAINER_H
