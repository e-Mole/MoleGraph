#include "Plot.h"
#include "qcustomplot/qcustomplot.h"
#include <QTimer>
#include <QThread>
#include <math.h>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <SerialPort.h>
#include <QtCore/QDebug>
#include <QByteArray>
#include <QFileDialog>
#include <QCheckBox>
#include <QColor>
#include <QSlider>
#include <QFile>
#include <QBoxLayout>
#include <QComboBox>
#include <QMessageBox>
#include <QShortcut>
#include <QKeySequence>
#include <Channel.h>

Plot::Plot(QWidget *parent, SerialPort &serialPort) :
	QWidget(parent),
	m_customPlot(NULL),
	m_serialPort(serialPort),
	m_minY(0),
	m_maxY(0),
	m_period(NULL),
    m_counter(0),
    m_slider(NULL),
    m_startButton(NULL),
	m_stopButton(NULL),
    m_periodType(NULL),
	m_periodUnits(NULL),
	m_connectivityLabel(NULL),
	m_connectButton(NULL),
	m_sampleChannel(NULL)
{
	setMinimumHeight(600);
	setMinimumWidth(800);
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	_InitializeButtonLine(mainLayout);
	QHBoxLayout *documentLayout = new QHBoxLayout(this);
	mainLayout->addLayout(documentLayout);
    QVBoxLayout *graphLayout = new QVBoxLayout(this);
	documentLayout->addLayout(graphLayout);
    QVBoxLayout *channelLayout = new QVBoxLayout(this);
	documentLayout->addLayout(channelLayout);


	_InitializePolt(graphLayout);
	_InitializeSlider(graphLayout);
    _InitializeChannelSideBar(channelLayout);

    m_drawTimer = new QTimer(this);
	connect(m_drawTimer, SIGNAL(timeout()), this, SLOT(draw()));
}

Plot::~Plot()
{

}

void Plot::_InitializePolt(QBoxLayout *graphLayout)
{
    m_customPlot = new QCustomPlot(this);
    graphLayout->addWidget(m_customPlot);

    for (int i = 0; i < 16; i++)
    {
        m_customPlot->addGraph();
    }

    _InitializeGraphs(0, Qt::red);
    _InitializeGraphs(1, Qt::blue);
    _InitializeGraphs(2, Qt::darkGreen);
    _InitializeGraphs(3, Qt::black);
    _InitializeGraphs(4, Qt::magenta);
    _InitializeGraphs(5, Qt::cyan);
    _InitializeGraphs(6, Qt::green);
    _InitializeGraphs(7, Qt::darkRed);

    m_customPlot->xAxis->setLabel(tr("sample"));
    m_customPlot->yAxis->setLabel(tr("amplitude"));
    m_customPlot->xAxis->setRange(0, 1);
    m_customPlot->yAxis->setRange(0, 1);

	m_customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
}

void Plot::_InitializeSlider(QBoxLayout *graphLayout)
{
    m_slider = new QSlider(Qt::Horizontal, this);
    m_slider->setRange(0,0);
    graphLayout->addWidget(m_slider);
    connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(redrawMarks(int)));
}

void Plot::_InitializeChannelSideBar(QBoxLayout *channelLayout)
{
	m_sampleChannel = new Channel(this, m_customPlot->xAxis->label(), Qt::black, true);
	channelLayout->addWidget(m_sampleChannel);
	m_channels.resize(8);
	for (int i = 0; i < m_channels.size(); i++)
	{
		m_channels[i] = new Channel(this, QString(tr("channel %1")).arg(i), m_customPlot->graph(i)->pen().color(), false);
		channelLayout->addWidget(m_channels[i]);
		connect(m_channels[i], SIGNAL(enableChanged()), this, SLOT(channelStateChanged()));
	}

	channelLayout->insertStretch(m_channels.size() + 1, 1); //+sample Nr.
}

void Plot::_InitializeButtonLine(QBoxLayout *graphLayout)
{
    QHBoxLayout *buttonLayout = new QHBoxLayout(this);
    graphLayout->addLayout(buttonLayout);

    m_periodType = new QComboBox(this);
    m_periodType->addItem(tr("Frequency"));
    m_periodType->addItem(tr("Time"));
    buttonLayout->addWidget(m_periodType);
    connect(m_periodType, SIGNAL(currentIndexChanged(int)), this, SLOT(periodTypeChanged(int)));

    m_period = new  QLineEdit(this);
    m_period->setValidator( new QDoubleValidator(1, 1500, 3, this));
    m_period->setMaxLength(6);
	m_period->setText("1");
    m_period->setFixedWidth(50);
    m_period->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    buttonLayout->addWidget(m_period);

    m_periodUnits = new QLabel(tr("Hz"),this);
    m_periodUnits->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    buttonLayout->addWidget(m_periodUnits);

    m_startButton = new QPushButton(tr("Start"), this);
    m_startButton->setDisabled(true);
    buttonLayout->addWidget(m_startButton);
    connect(m_startButton, SIGNAL(clicked()), this, SLOT(start()));

	QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_Space), this);
	connect(shortcut, SIGNAL(activated()), m_startButton, SLOT(animateClick()));

    m_stopButton = new QPushButton(tr("Stop"), this);
    m_stopButton->setDisabled(true);
    buttonLayout->addWidget(m_stopButton);
    connect(m_stopButton, SIGNAL(clicked()), this, SLOT(stop()));

	connect(shortcut, SIGNAL(activated()), m_stopButton, SLOT(animateClick()));

    QPushButton *exportButtonPng = new QPushButton(tr("Export to PNG"), this);
    buttonLayout->addWidget(exportButtonPng);
    connect(exportButtonPng, SIGNAL(clicked()), this, SLOT(exportPngSlot()));

    QPushButton *exportButtonCsv = new QPushButton(tr("Export to CSV"), this);
    buttonLayout->addWidget(exportButtonCsv);
    connect(exportButtonCsv, SIGNAL(clicked()), this, SLOT(exportCsvSlot()));


	m_connectivityLabel = new QLabel("", this);
	m_connectivityLabel->setMargin(5);
	buttonLayout->addWidget(m_connectivityLabel);

	//m_connectButton = new QPushButton(tr("Connect"), this);
	//buttonLayout->addWidget(m_connectButton);
	//connect(m_connectButton, SIGNAL(clicked()), this, SLOT(connectToDevice()));

	connectivityStateChange(m_serialPort.IsDeviceConnected());
	connect(&m_serialPort, SIGNAL(PortConnectivityChanged(bool)), this, SLOT(connectivityStateChange(bool)));
	buttonLayout->insertStretch(5, 1);

}

void Plot::periodTypeChanged(int index)
{
        m_periodUnits->setText((0 == index) ?  tr("Hz") : tr("s"));
}

void Plot::_InitializeGraphs(unsigned index, QColor color)
{
    m_customPlot->graph(index)->setPen(QPen(color));

    m_customPlot->graph(index + 8)->setPen(QPen(QBrush(color), 1.6));
    m_customPlot->graph(index + 8)->setLineStyle(QCPGraph::lsNone);
    m_customPlot->graph(index + 8)->setScatterStyle(QCPScatterStyle::ssPlus);
}

bool Plot::_FillGraphItem(GraphItem &item)
{
	if (m_queue.size() < 5)
		return false;

	item.channel = m_queue.dequeue();
	char value[4];
	value[0] = m_queue.dequeue();
	value[1] = m_queue.dequeue();
	value[2] = m_queue.dequeue();
	value[3] = m_queue.dequeue();

	item.value = *((float*)value);

	return true;
}

void Plot::redrawMarks(int pos)
{
    for (int i = 0; i < m_channels.size(); i++)
    {
        m_customPlot->graph(i + 8)->clearData();
		if (m_channels[i]->m_values.size() > pos)
        {
			m_customPlot->graph(i + 8)->addData(pos, m_channels[i]->m_values[pos]);
			m_channels[i]->m_selectedValue->setText(QString("%1").arg(m_channels[i]->m_values[pos]));
        }
    }

	m_sampleChannel->m_selectedValue->setText(QString("%1").arg(pos));
    m_customPlot->replot(QCustomPlot::rpImmediate);
}

void Plot::draw()
{
    unsigned lastPos = (m_x.size() > 0) ? m_x.last() : 0;
	GraphItem item;

    QByteArray array;
	m_serialPort.ReadAll(array);
    if (array.size() == 0)
    {
        return; //nothing to draw
    }

    for (int i = 0; i< array.size(); i++)
         m_queue.enqueue(array[i]);

    unsigned lastChannel = 100; //just huge number
    unsigned count = 0;
    while (_FillGraphItem(item))
	{
        count ++;
        if (item.channel <= lastChannel)
        {
			m_x.push_back(m_x.size());
        }
        lastChannel = item.channel;

		m_channels[item.channel]->m_values.push_back(item.value);

		if (item.value > m_maxY)
			m_maxY = item.value;
		if (item.value < m_minY)
			m_minY = item.value;
	}

    m_customPlot->xAxis->setRange(0, (m_x.size()-1) /*+ ((double)m_x.size() / (double)100)*/ );
	m_customPlot->yAxis->setRange(m_minY - abs(m_minY /10), m_maxY + abs(m_maxY / 10));

    for (int i = 0; i < m_channels.size(); i++)
		m_customPlot->graph(i)->setData(m_x, m_channels[i]->m_values);


    m_slider->setRange(0, m_x.last());

    if (m_slider->value() == lastPos)
    {
        m_slider->setValue(m_x.last());
        if (0 == m_x.last()) //slider value was not changed but I have first (initial) values and want to display them
            redrawMarks(0);
    }
    else
    {
        //if slider value is changed graph is redrown by slider. I have to redraw plot there because of moving by the slider
        //else i have to redraw it here
        m_customPlot->replot(QCustomPlot::rpImmediate);

    }
}

void Plot::start()
{
	if (!m_serialPort.IsDeviceConnected())
	{
		QMessageBox::critical(this,
			tr("Device not connected"),
			tr("Arduino device was not found on any serial port. Please, check the connectivity or unpug and plug it aagain."),
			tr("Close"));
		return;
	}
    m_startButton->setDisabled(true);
    m_stopButton->setEnabled(true);
    m_period->setDisabled(true);
    for(int i = 0; i < m_channels.size(); i++)
		m_channels[i]->m_enabled->setDisabled(true);

	m_customPlot->xAxis->setLabel(m_sampleChannel->title());
	m_counter = 0;
	m_x.clear();

    for (int i = 0; i < m_channels.size(); i++)
    {
		m_channels[i]->m_values.clear();
		m_channels[i]->m_selectedValue->setText("0");
    }

	m_queue.clear();
	m_serialPort.Clear(); //throw buffered data avay. I want to start to listen now

    if (0 == (unsigned)(m_period->text().toInt()))
    {
            m_period->setText("1");
            qDebug() << "empty period field replaced by 1";
    }

    if (0 == m_periodType->currentIndex())
    {
        unsigned frequency = (unsigned)(m_period->text().toInt());
        m_serialPort.SetFrequency(frequency);
        qDebug() << "frequency set to:" << frequency << " Hz";
    }
    else
    {
        unsigned time = (unsigned)(m_period->text().toInt());
        m_serialPort.SetTime(time);
        qDebug() << "time set to:" << time << " s";
    }

    unsigned enabledChannels = 0;
    for (unsigned i = 0; i < m_channels.size(); i++)
		enabledChannels |= ((m_channels[i]->m_enabled->isChecked()) << i);

    qDebug() << "enabled channels:" << enabledChannels;

    m_serialPort.SetEnabledChannels(enabledChannels);

    m_slider->setRange(0, 0);

	//m_serialPort.Clear(); //FIXME: workaround something is in buffer
    m_drawTimer->start(100);
    m_serialPort.Start();
}

void Plot::stop()
{
    m_stopButton->setDisabled(true);
    m_startButton->setEnabled(true);
    m_period->setEnabled(true);
    for(int i = 0; i < m_channels.size(); i++)
		m_channels[i]->m_enabled->setEnabled(true);

	m_serialPort.Stop();
	m_drawTimer->stop();
	draw(); //may be something is still in the buffer

}

void Plot::exportPngSlot()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Export graph to"), "./", "*.png");
    if (!fileName.contains(".png", Qt::CaseInsensitive))
            fileName += ".png";

    if (0 != fileName.size())
		m_customPlot->savePng(fileName);
}

void Plot::exportCsvSlot()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export graph to"), "./", "*.csv");
    if (!fileName.contains(".csv", Qt::CaseInsensitive))
            fileName += ".csv";

    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    unsigned lineNr = 0;

	file.write(m_customPlot->xAxis->label().toStdString().c_str());
	file.write(";");
    for (unsigned i = 0; i < m_channels.size(); i++)
    {
		file.write(m_channels[i]->title().toStdString().c_str());
        if (i == m_channels.size() - 1)
             file.write("\n");
        else
             file.write(";");
    }

	unsigned sampleNr = 0;
    while (true)
    {
        bool haveData = false;
		std::string lineContent = QString("%1;").arg(sampleNr++).toStdString();
        for (unsigned i = 0; i < m_channels.size(); i++)
        {
			if (m_channels[i]->m_values.size() > lineNr)
            {
				lineContent.append(QString("%1").arg(m_channels[i]->m_values[lineNr]).toStdString());
                haveData = true;
            }

            if (i == m_channels.size() - 1)
                lineContent.append("\n");
            else
                lineContent.append(";");
        }

        lineNr++;

        if (haveData)
            file.write(lineContent.c_str(), lineContent.size());
        else
            break;
    }
    file.close();
}

void Plot::channelStateChanged()
{
    bool anyChecked = false;
	foreach (Channel *channel, m_channels)
		anyChecked |= channel->m_enabled->isChecked();

     m_startButton->setEnabled(anyChecked);
}

void Plot::connectivityStateChange(bool connected)
{
	if (connected)
	{
		m_connectivityLabel->setStyleSheet("QLabel { background-color : green; color : white; }");
		m_connectivityLabel->setText(tr("Connected"));
		//m_connectButton->setDisabled(true);
	}
	else
	{
		m_connectivityLabel->setStyleSheet("QLabel { background-color : red; color : yellow; }");
		m_connectivityLabel->setText(tr("Disconnected"));
		QMessageBox::warning(
			this,
			QFileInfo(QCoreApplication::applicationFilePath()).fileName(),
			tr("You are working in offline mode. To estabilish a connection, please, reconnect an Arduino device and restart the application.")
		);
		//m_connectButton->setEnabled(true);
	}
	m_connectivityLabel->repaint();
}

void Plot::connectToDevice()
{
	//I connect device in a timer to be able react to disconnetion or start the application without the connection
	//I don't care that the timer is runnig all the time. It is triggered once per second so it is not any performance issue
	if (!m_serialPort.IsDeviceConnected())
	{
		qDebug() << "device will be connected";
        m_serialPort.FindAndOpenMySerialPort();
	}
}

