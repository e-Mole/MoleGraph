#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H
#include <graphics/ChannelProxyBase.h>
#include <graphics/GraphicsContainer.h>
#include <Plot.h>
#include <QPoint>

class QAction;
class QWidget;
namespace bases { class MenuBase; }

class PlotContextMenu : public QObject
{
    Q_OBJECT
    QObject * InitMarkerTypeSelection(
        bases::MenuBase *menu, QString const &label, Plot::MarkerTypeSelection markerTypeSelection);
    QObject * InitMarkerRangeValue(
        bases::MenuBase *menu, QString const &label, ChannelProxyBase::DisplayValue markerRangeValue);

    void _SetMarkerType(QObject * action);

    QPoint clickPosition;
    GraphicsContainer *m_graphicsContainer;

    QObject *m_sampleValue;
    QObject *m_rangeAutoBorder;
    QObject *m_rangeLeftBorder;
    QObject *m_rangeRightBorder;

    QObject *m_deltaValue;
    QObject *m_maxValue;
    QObject *m_minValue;
    QObject *m_averageValue;
    QObject *m_medianValue;
    QObject *m_varianceValue;
    QObject *m_standardDeviation;
    QObject *m_sumValue;

public:
    PlotContextMenu(QWidget *parent, GraphicsContainer *graphicsContainer);

public slots:
    void contextMenuRequestRelativePos(QPoint pos);
    void contextMenuRequestGlobalPos(QPoint pos);
private slots:
    void zoomInSelected();
    void zoomOutSelected();
    void zoomToFitSelected();
    void FollowMode();
    void markerTypeSelected();
    void valueSelectionSended();
};


#endif // CONTEXTMENU_H
