#include "PlotContextMenu.h"
#include <QAction>
#include <QActionGroup>
#include <ChannelBase.h>
#include <QMenu>
#include <QWidget>
#include <SampleChannel.h>

PlotContextMenu::PlotContextMenu(QWidget *parent,  Measurement *measurement):
    QMenu(parent),
    m_measurement(measurement),
    m_sampleValue(NULL),
    m_rangeAutoBorder(NULL),
    m_rangeLeftBorder(NULL),
    m_rangeRightBorder(NULL),
    m_maxValue(NULL),
    m_minValue(NULL),
    m_averageValue(NULL),
    m_medianValue(NULL),
    m_varianceValue(NULL),
    m_standardDeviation(NULL),
    m_sumValue(NULL)
{
}

void PlotContextMenu::contextMenuRequestRelativePos(QPoint pos)
{
    contextMenuRequestGlobalPos(m_measurement->GetPlot()->mapToGlobal(pos));
}
void PlotContextMenu::contextMenuRequestGlobalPos(QPoint pos)
{
    clickPosition = pos;
    QMenu *menu = new QMenu(m_measurement->GetPlot());
    menu->setAttribute(Qt::WA_DeleteOnClose);
    menu->addAction(tr("Zoom in"), this, SLOT(zoomInSelected()));
    menu->addAction(tr("Zoom out"), this, SLOT(zoomOutSelected()));
    menu->addAction(tr("Zoom to fit"), this, SLOT(zoomToFitSelected()))->
        setEnabled(m_measurement->GetSampleChannel()->GetValueCount() > 0);
    menu->addSeparator();

    m_sampleValue = InitMarkerTypeSelection(
        menu, tr("Sample"), Plot::MTSSample);
    m_rangeAutoBorder = InitMarkerTypeSelection(
        menu, tr("Auto Selection Border"), Plot::MTSRangeAutoBorder);
    m_rangeLeftBorder = InitMarkerTypeSelection(
        menu, tr("Left Selection Border"), Plot::MTSRangeLeftBorder);
    m_rangeRightBorder = InitMarkerTypeSelection(
        menu, tr("Right Selection Border"), Plot::MTSRangeRightBorder);

    menu->addSeparator();

    m_deltaValue = InitMarkerRangeValue(menu, tr("Delta"), ChannelBase::DVDelta);
    m_maxValue = InitMarkerRangeValue(menu, tr("Maximum"), ChannelBase::DVMax);
    m_minValue = InitMarkerRangeValue(menu, tr("Minimum"), ChannelBase::DVMin);
    m_averageValue = InitMarkerRangeValue(menu, tr("Mean"), ChannelBase::DVAverage);
    m_medianValue = InitMarkerRangeValue(menu, tr("Median"), ChannelBase::DVMedian);
    m_varianceValue = InitMarkerRangeValue(menu, tr("Variance"), ChannelBase::DVVariance);
    m_standardDeviation = InitMarkerRangeValue(menu, tr("Standard Deviation"), ChannelBase::DVStandDeviation);
    m_sumValue = InitMarkerRangeValue(menu, tr("Sum"), ChannelBase::DVSum);
    menu->popup(clickPosition);
}

QAction * PlotContextMenu::InitMarkerTypeSelection(
    QMenu *menu, QString const &label, Plot::MarkerTypeSelection markerTypeSelection)
{
    QAction *action = menu->addAction(label, this, SLOT(markerTypeSelected()));
    action->setCheckable(true);
    action->setChecked(
        m_measurement->GetPlot()->m_markerTypeSelection == markerTypeSelection);
    action->setEnabled(m_measurement->GetSampleChannel()->GetValueCount() > 0);
    return action;
}

QAction * PlotContextMenu::InitMarkerRangeValue(
    QMenu *menu, QString const &label, ChannelBase::DisplayValue markerRangeValue)
{
    Plot *plot = m_measurement->GetPlot();
    QAction *action = menu->addAction(label, this, SLOT(valueSelectionSended()));
    action->setCheckable(true);
    action->setChecked(plot->m_markerRangeValue == markerRangeValue);
    action->setEnabled(plot->m_markerTypeSelection != Plot::MTSSample);
    return action;
}

void PlotContextMenu::zoomInSelected()
{
    m_measurement->GetPlot()->Zoom(clickPosition, 100);
}

void PlotContextMenu::zoomOutSelected()
{
    m_measurement->GetPlot()->Zoom(clickPosition, -100);
}

void PlotContextMenu::zoomToFitSelected()
{
    m_measurement->GetPlot()->ZoomToFit(clickPosition);
}

void PlotContextMenu::markerTypeSelected()
{
    Plot *plot = m_measurement->GetPlot();

    Plot::MarkerTypeSelection lastSelection = plot->m_markerTypeSelection;

    if ((QAction*)sender() == m_sampleValue)
        plot->m_markerTypeSelection = Plot::MTSSample;
    else if ((QAction*)sender() == m_rangeAutoBorder)
        plot->m_markerTypeSelection = Plot::MTSRangeAutoBorder;
    else if ((QAction*)sender() == m_rangeLeftBorder)
        plot->m_markerTypeSelection = Plot::MTSRangeLeftBorder;
    else if ((QAction*)sender() == m_rangeRightBorder)
        plot->m_markerTypeSelection = Plot::MTSRangeRightBorder;

    if (
        plot->m_markerTypeSelection != lastSelection &&
        (plot->m_markerTypeSelection == Plot::MTSSample || lastSelection == Plot::MTSSample)
    )
    {
        //marker line count changed. have to be redrawn
        plot->SetMarkerLine(m_measurement->GetSliderPos());
        plot->ReplotIfNotDisabled();
    }
}

void PlotContextMenu::valueSelectionSended()
{
    Plot *plot = m_measurement->GetPlot();
    QAction *action = (QAction*)sender();
    if (action == m_deltaValue)
        plot->m_markerRangeValue = ChannelBase::DVDelta;
    else if (action == m_maxValue)
        plot->m_markerRangeValue = ChannelBase::DVMax;
    else if (action == m_minValue)
        plot->m_markerRangeValue = ChannelBase::DVMin;
    else if (action == m_averageValue)
        plot->m_markerRangeValue = ChannelBase::DVAverage;
    else if (action == m_medianValue)
        plot->m_markerRangeValue = ChannelBase::DVMedian;
    else if (action == m_varianceValue)
        plot->m_markerRangeValue = ChannelBase::DVVariance;
    else if (action == m_standardDeviation)
        plot->m_markerRangeValue = ChannelBase::DVStandDeviation;
    else if (action == m_sumValue)
        plot->m_markerRangeValue = ChannelBase::DVSum;

    m_measurement->RedrawChannelValues();
}
