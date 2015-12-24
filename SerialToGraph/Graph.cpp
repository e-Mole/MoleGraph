#include "Graph.h"

#include <Axis.h>
#include <cmath>
#include <Channel.h>
#include <Context.h>
#include <limits.h>
#include <math.h>
#include <Measurement.h>
#include <Plot.h>
#include <QBoxLayout>
#include <QColor>
#include <QCoreApplication>
#include <QHBoxLayout>
#include <QFile>
#include <QMessageBox>
#include <QScrollBar>
#include <QtCore/QDebug>
#include <QThread>
#include <QVBoxLayout>
#include <QWidget>
#include <SerialPort.h>


Graph::Graph(QWidget *parent, Context &context, SerialPort &serialPort, QScrollBar * scrollBar) :
    QWidget(parent),
    m_context(context),
    m_plot(NULL),
    m_serialPort(serialPort),
    m_scrollBar(scrollBar),
    m_connectButton(NULL)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(1);
    QHBoxLayout *documentLayout = new QHBoxLayout(this);
    mainLayout->addLayout(documentLayout);
    QVBoxLayout *graphLayout = new QVBoxLayout(this);
    documentLayout->addLayout(graphLayout);

    _InitializePolt(graphLayout);
}

void Graph::_InitializePolt(QBoxLayout *graphLayout)
{
    m_plot = new Plot(this, m_context);
    graphLayout->addWidget(m_plot);
}

Plot *Graph::GetPlot()
{
    return m_plot;
}

