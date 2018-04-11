#include "Axis.h"
#include <GlobalSettings.h>
#include <graphics/GraphicsContainer.h>
#include <graphics/SampleChannelProxy.h>
#include <Measurement.h>
#include <Plot.h>
#include <qcustomplot/qcustomplot.h>
#include <QString>

Axis::Axis(GraphicsContainer *graphicsContainer,
    QColor const & color,
    QCPAxis *graphAxis,
    QString title,
    bool isRemovable,
    bool isHorizontal,
    bool isOnRight,
    bool isShownName
) :
    QObject(NULL),
    m_graphicsContainer(graphicsContainer),
    m_title(title),
    m_isRemovable(isRemovable),
    m_color(color),
    m_isOnRight(isOnRight),
    m_isHorizontal(isHorizontal),
    m_graphAxis(NULL),
    m_isShownName(isShownName)
{
    _ReassignGraphAxis(graphAxis);
}

void Axis::_ReassignGraphAxis(QCPAxis *axis)
{
    if (NULL != m_graphAxis)
    {
        foreach (ChannelProxyBase *proxy, m_graphicsContainer->GetChannelProxies())
        {
            if (proxy->GetChannelGraph()->GetValuleAxis()->GetGraphAxis() == m_graphAxis)
                proxy->GetChannelGraph()-> AssignToGraphAxis(axis);
        }

        m_graphicsContainer->GetPlot()->RemoveAxis(m_graphAxis);
    }

    m_graphAxis = axis;
    if (NULL != axis)
    {
        _SetColor(m_color);
        UpdateGraphAxisName();
        UpdateVisiblility();
        m_graphicsContainer->GetPlot()->RescaleAxis(axis);

    }
}

const Axis &Axis::operator =(const Axis &axis)
{
    m_title = axis.m_title;
    m_isRemovable = axis.m_isRemovable;
    m_color = axis.m_color;
    m_isOnRight = axis.m_isOnRight;
    m_isHorizontal = axis.m_isHorizontal;
    m_graphAxis = axis.m_graphAxis;
    m_isShownName = axis.m_isShownName;
    return *this;
}

void Axis::_FillChannelProxies(std::vector<ChannelProxyBase *> &proxies, std::vector<GraphicsContainer *> &containers )
{
    foreach (ChannelProxyBase *proxy, m_graphicsContainer->GetChannelProxies())
    {
        if (proxy->GetChannelGraph()->GetValuleAxis() != this)
        {
            continue;
        }

        if (proxy->IsDrawable() || proxy->IsOnHorizontalAxis())
        {
            proxies.push_back(proxy);
            containers.push_back(m_graphicsContainer);
        }


        if (proxy->IsOnHorizontalAxis())
        {
            std::vector<ChannelProxyBase*> ghostProxies;
            std::vector<GraphicsContainer *> ghostContainers;
            foreach (ChannelProxyBase *proxy, m_graphicsContainer->GetChannelProxies())
            {
                if (!proxy->IsGhost() || !proxy->IsDrawable())
                {
                    continue;
                }

                Measurement *m = proxy->GetChannelMeasurement();
                GraphicsContainer *sourceContainer = m->GetWidget();
                ChannelProxyBase * horizontalChannelProxy = sourceContainer->GetHorizontalChannelProxy();
                if (std::find(proxies.begin(), proxies.end(), horizontalChannelProxy) == proxies.end())
                {
                    ghostProxies.push_back(horizontalChannelProxy);
                    ghostContainers.push_back(sourceContainer);
                }
            }

            if (!ghostProxies.empty() && !proxy->IsDrawable())
            {
                //It is not necessary to display name of horizintal channel because it is hidden and are here other to display
                proxies.clear();
                containers.clear();
            }
            proxies.insert(proxies.end(), ghostProxies.begin(), ghostProxies.end());
            containers.insert(containers.end(), ghostContainers.begin(), ghostContainers.end());
        }
    }
}

QString Axis::_GetChannelName(GraphicsContainer *container, ChannelProxyBase *proxy)
{

    if (container == m_graphicsContainer)
        return proxy->GetName();

    //horizontal channel of ghost
    return m_graphicsContainer->GetGhostName(container, proxy);
}

void Axis::UpdateGraphAxisName()
{
    if (m_isShownName)
    {
        m_graphAxis->setLabel(m_title);
        m_graphicsContainer->GetPlot()->ReplotIfNotDisabled();
        return;
    }

     QString channels;
    bool first =true;
    bool addMiddle = false;
    QString units;


    std::vector<ChannelProxyBase*> proxies;
    std::vector<GraphicsContainer*> containers;
    _FillChannelProxies(proxies, containers);
    unsigned size = proxies.size();
    for (unsigned i = 0; i < size; i++)
    {
        if (!first)
        {
            if (i+1 != size &&
                proxies[i+1]->IsDrawable() &&
                this == proxies[i+1]->GetChannelGraph()->GetValuleAxis() &&
                i != 0 &&
                proxies[i-1]->IsDrawable() &&
                this == proxies[i-1]->GetChannelGraph()->GetValuleAxis())
            {
                addMiddle = true;
                continue;
            }
            channels += ", ";
        }
        else
            first = false;

        if (addMiddle)
        {
            channels += ".. ,";
            addMiddle = false;
        }

        channels += _GetChannelName(containers[i], proxies[i]);

        if (0 == units.size())
            units = proxies[i]->GetUnits();
        else if (units != proxies[i]->GetUnits())
            units = "/n"; //escape sequence for no units
    }

    bool round = GlobalSettings::GetInstance().GetUnitBrackets() == "()";
    QString unitString =
        (0 == units.size() || "/n" == units) ?
            "" :
            (round ? " (" : " [") + units + (round ? ")" : "]");
    m_graphAxis->setLabel(channels + unitString);
    m_graphicsContainer->GetPlot()->ReplotIfNotDisabled();
}

void Axis::UpdateVisiblility()
{
    foreach (ChannelProxyBase *proxy, m_graphicsContainer->GetChannelProxies())
    {
        if (proxy->isVisible() && proxy->GetChannelGraph()->GetValuleAxis() == this)
        {
            m_graphAxis->setVisible(true);
            m_graphicsContainer->GetPlot()->ReplotIfNotDisabled();
            return;
        }
    }
    m_graphAxis->setVisible(IsHorizontal());
    m_graphicsContainer->GetPlot()->ReplotIfNotDisabled();

}

void Axis::_SetColor(QColor const & color)
{
    m_color = color;
    if (NULL == m_graphAxis)
        return;

    m_graphAxis->setTickLabelColor(color);
    m_graphAxis->setLabelColor(color);
    QPen pen = m_graphAxis->selectedBasePen();
    pen.setColor(Qt::black);
    m_graphAxis->setSelectedBasePen(pen);
    m_graphAxis->setSelectedTickLabelColor(color);
    m_graphAxis->setSelectedLabelColor(color);

    pen = m_graphAxis->selectedTickPen();
    pen.setColor(Qt::black);
    m_graphAxis->setSelectedTickPen(pen);

    pen = m_graphAxis->selectedSubTickPen();
    pen.setColor(Qt::black);
    m_graphAxis->setSelectedSubTickPen(pen);
}

GraphicsContainer * Axis::GetGraphicsContainer()
{
    return m_graphicsContainer;
}

bool Axis::IsEmptyExcept(ChannelProxyBase *except)
{
    foreach (ChannelProxyBase *proxy, m_graphicsContainer->GetChannelProxies())
    {
        if (proxy == except)
            continue;

        if (proxy->GetChannelGraph()->GetValuleAxis() == this)
            return false;
    }

    return true;
}

bool Axis::ContainsChannelWithRealTimeStyle()
{
    foreach (ChannelProxyBase *proxy, m_graphicsContainer->GetChannelProxies())
    {
        if (proxy->GetChannelGraph()->GetValuleAxis() != this)
            continue;

        SampleChannelProxy *sampleChannelProxy = dynamic_cast<SampleChannelProxy*>(proxy);
        if (sampleChannelProxy && sampleChannelProxy->GetStyle() == SampleChannelProxy::RealTime)
            return true;
    }
    return false;
}

void Axis::UpdateGraphAxisStyle()
{
    ChannelProxyBase *axisChannelProxy = NULL;
    foreach (ChannelProxyBase *proxy, m_graphicsContainer->GetChannelProxies())
    {
        if (proxy->GetChannelGraph()->GetValuleAxis() == this)
        {
            axisChannelProxy = proxy;
            break; //I know, that real time channel is on oun axis
        }
    }
    if (axisChannelProxy == NULL)
        return; //empty axis

    bool realTimeStyle = false;
    QString formatText = "";

    SampleChannelProxy *sampleChannelProxy = dynamic_cast<SampleChannelProxy *>(axisChannelProxy);
    if (sampleChannelProxy)
    {
        realTimeStyle = sampleChannelProxy->GetStyle() == SampleChannelProxy::RealTime;
        formatText = sampleChannelProxy->GetRealTimeFormatText();
    }

    m_graphicsContainer->GetPlot()->SetAxisStyle(m_graphAxis, realTimeStyle, formatText);
}

unsigned Axis::GetAssignedChannelCountWithoutGhosts()
{
    unsigned count = 0;
    foreach (ChannelProxyBase *proxy, m_graphicsContainer->GetChannelProxies())
    {
        if (!proxy->IsGhost() && proxy->GetChannelGraph()->GetValuleAxis() == this)
        {
            count++;
        }
    }
    return count;
}

void Axis::_SetIsOnRight(bool isOnRight)
{
    m_isOnRight = isOnRight;
    if (!IsHorizontal())
        _ReassignGraphAxis(m_graphicsContainer->GetPlot()->AddYAxis(isOnRight));
}

void Axis::_SetIsShownName(bool isShownName)
{
    m_isShownName = isShownName;
    UpdateGraphAxisName();
}

void Axis::_SetTitle(QString const& title)
{
    m_title = title;
    UpdateGraphAxisName();
}

void Axis::Rescale()
{
    m_graphicsContainer->GetPlot()->RescaleAxis(m_graphAxis);
}
