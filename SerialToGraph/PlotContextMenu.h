#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#include <Measurement.h>
#include <Plot.h>
#include <QMenu>
#include <QPoint>

class QAction;
class QWidget;
class PlotContextMenu : public QMenu
{
    Q_OBJECT

    QAction * InitMarkerTypeSelection(
        QMenu *menu, QString const &label, Plot::MarkerTypeSelection markerTypeSelection);
    QAction * InitMarkerRangeValue(
        QMenu *menu, QString const &label, ChannelBase::DisplayValue markerRangeValue);

    QPoint clickPosition;
    Measurement *m_measurement;

    QAction *m_sampleValue;
    QAction *m_rangeAutoBorder;
    QAction *m_rangeLeftBorder;
    QAction *m_rangeRightBorder;


    QAction *m_maxValue;
    QAction *m_minValue;
    QAction *m_averageValue;
    QAction *m_medianValue;
    QAction *m_varianceValue;
    QAction *m_standardDeviation;
    QAction *m_sumValue;

public:
    PlotContextMenu(QWidget *parent, Measurement *measurement);

public slots:
    void contextMenuRequestRelativePos(QPoint pos);
    void contextMenuRequestGlobalPos(QPoint pos);
private slots:
    void zoomInSelected();
    void zoomOutSelected();
    void zoomToFitSelected();
    void markerTypeSelected();
    void valueSelectionSended();
};


#endif // CONTEXTMENU_H
