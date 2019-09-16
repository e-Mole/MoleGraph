#include "PlotContextMenu.h"
#include <QAction>
#include <QActionGroup>
#include <bases/MenuBase.h>

#if defined(Q_OS_ANDROID)
#   include <MenuAndroid.h>
#else
#   include <MenuDesktop.h>
#endif

PlotContextMenu::PlotContextMenu(QWidget *parent,  GraphicsContainer *graphicsContainer):
    QObject (parent),
    m_graphicsContainer(graphicsContainer),
    m_sampleValue(nullptr),
    m_rangeAutoBorder(nullptr),
    m_rangeLeftBorder(nullptr),
    m_rangeRightBorder(nullptr),
    m_maxValue(nullptr),
    m_minValue(nullptr),
    m_averageValue(nullptr),
    m_medianValue(nullptr),
    m_varianceValue(nullptr),
    m_standardDeviation(nullptr),
    m_sumValue(nullptr)
{
}

void PlotContextMenu::contextMenuRequestRelativePos(QPoint pos)
{
    contextMenuRequestGlobalPos(m_graphicsContainer->GetPlot()->mapToGlobal(pos));
}
void PlotContextMenu::contextMenuRequestGlobalPos(QPoint pos)
{
    clickPosition = pos;

#if defined(Q_OS_ANDROID)
    MenuAndroid *menu = new MenuAndroid(m_graphicsContainer->GetPlot(), "");
#else
    MenuDesktop *menu = new MenuDesktop(m_graphicsContainer->GetPlot(), "");
#endif

    //TODO
    //menu->setAttribute(Qt::WA_DeleteOnClose);
    menu->addItem(tr("Zoom in"), this, SLOT(zoomInSelected()));
    menu->addItem(tr("Zoom out"), this, SLOT(zoomOutSelected()));
    menu->addItem(tr("Zoom to fit"), this, SLOT(zoomToFitSelected()), false, false, m_graphicsContainer->ContainsAnyData());
    menu->addSeparator();
    menu->addItem(tr("Follow Mode"), this, SLOT(FollowMode()));
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

    m_deltaValue = InitMarkerRangeValue(menu, tr("Delta"), ChannelProxyBase::DVDelta);
    m_maxValue = InitMarkerRangeValue(menu, tr("Maximum"), ChannelProxyBase::DVMax);
    m_minValue = InitMarkerRangeValue(menu, tr("Minimum"), ChannelProxyBase::DVMin);
    m_averageValue = InitMarkerRangeValue(menu, tr("Mean"), ChannelProxyBase::DVAverage);
    m_medianValue = InitMarkerRangeValue(menu, tr("Median"), ChannelProxyBase::DVMedian);
    m_varianceValue = InitMarkerRangeValue(menu, tr("Variance"), ChannelProxyBase::DVVariance);
    m_standardDeviation = InitMarkerRangeValue(menu, tr("Standard Deviation"), ChannelProxyBase::DVStandDeviation);
    m_sumValue = InitMarkerRangeValue(menu, tr("Sum"), ChannelProxyBase::DVSum);
    menu->popup(clickPosition);
}

QObject * PlotContextMenu::InitMarkerTypeSelection(
    bases::MenuBase *menu, QString const &label, Plot::MarkerTypeSelection markerTypeSelection)
{
    QObject *item = menu->addItem(
        label,
        this,
        SLOT(markerTypeSelected()),
        true,
        m_graphicsContainer->GetPlot()->m_markerTypeSelection == markerTypeSelection,
        m_graphicsContainer->GetSampleChannelProxy()->GetValueCount() > 0
    );

    return item;
}

QObject * PlotContextMenu::InitMarkerRangeValue(
    bases::MenuBase *menu, QString const &label, ChannelProxyBase::DisplayValue markerRangeValue)
{
    Plot *plot = m_graphicsContainer->GetPlot();
    QObject *item = menu->addItem(
        label,
        this,
        SLOT(valueSelectionSended()),
        true,
        plot->m_markerRangeValue == markerRangeValue,
        plot->m_markerTypeSelection != Plot::MTSSample
    );

    return item;
}

void PlotContextMenu::zoomInSelected()
{
    m_graphicsContainer->GetPlot()->Zoom(clickPosition, 100);
}

void PlotContextMenu::zoomOutSelected()
{
    m_graphicsContainer->GetPlot()->Zoom(clickPosition, -100);
}

void PlotContextMenu::zoomToFitSelected()
{
    m_graphicsContainer->GetPlot()->ZoomToFit(clickPosition);
}

void PlotContextMenu::FollowMode()
{
    _SetMarkerType(m_sampleValue);
    m_graphicsContainer->SetFollowMode(true);
}

void PlotContextMenu::_SetMarkerType(QObject * action)
{
    Plot *plot = m_graphicsContainer->GetPlot();
    Plot::MarkerTypeSelection lastSelection = plot->m_markerTypeSelection;

    if (action == m_sampleValue)
        plot->m_markerTypeSelection = Plot::MTSSample;
    else if (action == m_rangeAutoBorder)
        plot->m_markerTypeSelection = Plot::MTSRangeAutoBorder;
    else if (action == m_rangeLeftBorder)
        plot->m_markerTypeSelection = Plot::MTSRangeLeftBorder;
    else if (action == m_rangeRightBorder)
        plot->m_markerTypeSelection = Plot::MTSRangeRightBorder;

    if (
        plot->m_markerTypeSelection != lastSelection &&
        (plot->m_markerTypeSelection == Plot::MTSSample || lastSelection == Plot::MTSSample)
    )
    {
        //marker line count changed. have to be redrawn
        plot->SetMarkerLine(m_graphicsContainer->GetCurrentIndex());
        plot->ReplotIfNotDisabled();
    }
}
void PlotContextMenu::markerTypeSelected()
{
    _SetMarkerType((QAction*)sender());
}

void PlotContextMenu::valueSelectionSended()
{
    Plot *plot = m_graphicsContainer->GetPlot();
    QAction *action = (QAction*)sender();
    if (action == m_deltaValue)
        plot->m_markerRangeValue = ChannelProxyBase::DVDelta;
    else if (action == m_maxValue)
        plot->m_markerRangeValue = ChannelProxyBase::DVMax;
    else if (action == m_minValue)
        plot->m_markerRangeValue = ChannelProxyBase::DVMin;
    else if (action == m_averageValue)
        plot->m_markerRangeValue = ChannelProxyBase::DVAverage;
    else if (action == m_medianValue)
        plot->m_markerRangeValue = ChannelProxyBase::DVMedian;
    else if (action == m_varianceValue)
        plot->m_markerRangeValue = ChannelProxyBase::DVVariance;
    else if (action == m_standardDeviation)
        plot->m_markerRangeValue = ChannelProxyBase::DVStandDeviation;
    else if (action == m_sumValue)
        plot->m_markerRangeValue = ChannelProxyBase::DVSum;

    m_graphicsContainer->RedrawChannelValues();
}
