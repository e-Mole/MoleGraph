#include "ChannelGraph.h"
#include "Axis.h"
#include "GlobalSettings.h"
#include "Plot.h"

#define MARKER_WIDTH 2 //1.6
#define SELECTED_PEN_WIDTH 2.5
#define MARKER_SIZE 8
ChannelGraph::ChannelGraph(QCPAxis *keyAxis,
    Axis *valueAxis,
    QColor const &color,
    unsigned shapeIndex,
    bool showAllMarks,
    Qt::PenStyle penStyle)
  :
    QCPGraph(keyAxis, valueAxis->GetGraphAxis()),
    m_showAllMarks(showAllMarks),
    m_selectedHorizontalValue(-1),
    m_valueAxis(valueAxis)
{
    double normalWidth = GlobalSettings::GetInstance().GetChannelGraphPenWidth();
    setPen(QPen(color, normalWidth, penStyle));
    setSelectedPen(QPen(color, normalWidth * SELECTED_PEN_WIDTH, penStyle));

    mScatterStyle.setPen(QPen(color, normalWidth));
    mScatterStyle.setSize(sqrt(normalWidth) * MARKER_SIZE);
    m_selectedMarkStyle.setPen(QPen(color, normalWidth * MARKER_WIDTH));
    m_selectedMarkStyle.setSize(sqrt(normalWidth) * MARKER_SIZE);

    SetMarkShape(shapeIndex);
}

void ChannelGraph::SetPenWidth(double width)
{
    //FIXMEL two similar definitions (constructor)
    QPen p = pen();
    p.setWidthF(width);
    setPen(p);

    p = selectedPen();
    p.setWidthF(width * SELECTED_PEN_WIDTH);
    setSelectedPen(p);

    p = mScatterStyle.pen();
    p.setWidthF(width);
    mScatterStyle.setPen(p);
    mScatterStyle.setSize(sqrt(width) * MARKER_SIZE);

    p = m_selectedMarkStyle.pen();
    p.setWidthF(width * MARKER_WIDTH);
    m_selectedMarkStyle.setPen(p);
    m_selectedMarkStyle.setSize(sqrt(width) * MARKER_SIZE);
}

void ChannelGraph::SetColor(const QColor& color)
{
    QPen p = pen();
    p.setColor(color);
    setPen(p);

    p = selectedPen();
    p.setColor(color);
    setSelectedPen(p);

    p = mScatterStyle.pen();
    p.setColor(color);
    mScatterStyle.setPen(p);

    p = m_selectedMarkStyle.pen();
    p.setColor(color);
    m_selectedMarkStyle.setPen(p);
}

void ChannelGraph::SetPenStyle(Qt::PenStyle penStyle)
{
    QPen p = pen();
    p.setStyle(penStyle);
    setPen(p);

    p = selectedPen();
    p.setStyle(penStyle);
    setSelectedPen(p);
}

void ChannelGraph::SetMarkShape(unsigned shapeIndex)
{
    QCPScatterStyle::ScatterShape shape = (QCPScatterStyle::ScatterShape)(shapeIndex);
    m_selectedMarkStyle.setShape(shape);

    //there must be something else them ssNone to be drawScatterPlot called
    mScatterStyle.setShape(shape);

}

void ChannelGraph::drawScatterPlot(QCPPainter *painter, QVector<QCPData> *scatterData) const
{
  QCPAxis *keyAxis = mKeyAxis.data();
  QCPAxis *valueAxis = mValueAxis.data();
  if (!keyAxis || !valueAxis) { qDebug() << Q_FUNC_INFO << "invalid key or value axis"; return; }

  // draw error bars:
  if (mErrorType != etNone)
  {
    applyErrorBarsAntialiasingHint(painter);
    painter->setPen(mErrorPen);
    if (keyAxis->orientation() == Qt::Vertical)
    {
      for (int i=0; i<scatterData->size(); ++i)
        drawError(painter, valueAxis->coordToPixel(scatterData->at(i).value), keyAxis->coordToPixel(scatterData->at(i).key), scatterData->at(i));
    } else
    {
      for (int i=0; i<scatterData->size(); ++i)
        drawError(painter, keyAxis->coordToPixel(scatterData->at(i).key), valueAxis->coordToPixel(scatterData->at(i).value), scatterData->at(i));
    }
  }

  // draw scatter point symbols:
  applyScattersAntialiasingHint(painter);
  for (int i=0; i<scatterData->size(); ++i)
  {
      if (qIsNaN(scatterData->at(i).value))
        continue;

      if (!m_showAllMarks && scatterData->at(i).key != m_selectedHorizontalValue)
          continue;

      QCPScatterStyle current;
      if (scatterData->at(i).key == m_selectedHorizontalValue)
      {
          m_selectedMarkStyle.applyTo(painter, mPen);
          current = m_selectedMarkStyle;
      }

      else
      {
          mScatterStyle.applyTo(painter, mPen);
          current = mScatterStyle;
      }

      if (keyAxis->orientation() == Qt::Vertical)
        current.drawShape(painter, valueAxis->coordToPixel(scatterData->at(i).value), keyAxis->coordToPixel(scatterData->at(i).key));
      else
        current.drawShape(painter, keyAxis->coordToPixel(scatterData->at(i).key), valueAxis->coordToPixel(scatterData->at(i).value));
  }
}
void ChannelGraph::ChangeSelectedHorizontalValue(double horizontalValue)
{
    //It will be redrawn in drawScatterPlot
    m_selectedHorizontalValue = horizontalValue;
}

void ChannelGraph::ShowAllMarks(bool showAllMarks)
{
    m_showAllMarks = showAllMarks;
}

int ChannelGraph::GetShapeIndex() const
{
    return ((unsigned)m_selectedMarkStyle.shape());
}

void ChannelGraph::AssignToAxis(Axis *axis)
{
    if (m_valueAxis->IsHorizontal() || axis->IsHorizontal())
        setKeyAxis(m_valueAxis->GetGraphAxis()); //to would be possible to switch axis later

    m_valueAxis = axis;
    AssignToGraphAxis(axis->GetGraphAxis());
    m_valueAxis->UpdateGraphAxisName();
    m_valueAxis->UpdateGraphAxisStyle();
    m_valueAxis->UpdateVisiblility();
}

void ChannelGraph::AssignToGraphAxis(QCPAxis *graphAxis)
{
    if (m_valueAxis->IsHorizontal())
        return;

    setValueAxis(graphAxis);
    setVisible(m_active);
    m_valueAxis->Rescale();
}

void ChannelGraph::SetActive(bool active)
{
    m_active = active;
    setVisible(m_valueAxis->IsHorizontal() ? false : active);
    m_valueAxis->UpdateGraphAxisName();
    m_valueAxis->UpdateVisiblility();
}
