#include "ChannelSettings.h"
#include <Axis.h>
#include <AxisChooseDialog.h>
#include <AxisSettings.h>
#include <ChannelBase.h>
#include <ChannelWidget.h>
#include <GlobalSettings.h>
#include <graphics/GraphicsContainer.h>
#include <HwChannel.h>
#include <Measurement.h>
#include <MainWindow.h>
#include <MyMessageBox.h>
#include <Plot.h>
#include <bases/ComboBox.h>
#include <QCheckBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QLocale>
#include <QPushButton>
#include <QSettings>
#include <QString>
#include <SampleChannel.h>

ChannelSettings::ChannelSettings(std::vector<Measurement *> measurements, GraphicsContainer *graphicsContainer, ChannelWidget *channelWidget) :
    bases::FormDialogColor (channelWidget, tr("Channel settings"), GlobalSettings::GetInstance().GetAcceptChangesByDialogClosing()),
    m_measurements(measurements),
    m_graphicsContainer(graphicsContainer),
    m_channelWidget(channelWidget),
    m_channel(graphicsContainer->GetChannel(channelWidget)),
    m_currentValueControl(NULL),
    m_name(NULL),
    m_units(NULL),
    m_shapeComboBox(NULL),
    m_axisComboBox(NULL),
    m_style(NULL),
    m_timeUnits(NULL),
    m_format(NULL),
    m_penStyle(NULL),
    m_currentValueChanged(false),
    m_currentValue(ChannelBase::GetNaValue()),
    m_measurementCombo(NULL),
    m_channelCombo(NULL)
{
    if (m_channelWidget->isGhost())
    {
        _InitializeGhostCombos();
    }

    m_name = new QLineEdit(m_channelWidget->GetName(), this);
    m_units = new QLineEdit(m_channelWidget->GetUnits(), this);

    ChannelBase *channel = graphicsContainer->GetChannel(channelWidget);
    if (channel->GetType() == ChannelBase::Type_Sample)
    {
        m_name->setVisible(false);
        _InitializeTimeFeatures();
    }
    else
    {
        if (channel->GetType() == ChannelBase::Type_Hw)
            _InitializeValueLine(channelWidget);

        m_formLayout->addRow(new QLabel(tr("Title"), this), m_name);
        m_formLayout->addRow(new QLabel(tr("Units"), this), m_units);
    }

    AddColorButtonRow(channelWidget->GetForeColor());
    _InitializeAxisCombo();
    _InitializeShapeCombo(channelWidget);
    _InitializePenStyle(channelWidget->GetPenStyle());
}

void ChannelSettings::_InitializeGhostCombos()
{
    m_measurementCombo = new bases::ComboBox(this);
    m_formLayout->addRow(new QLabel(tr("Measurement"), this), m_measurementCombo);

    m_channelCombo = new bases::ComboBox(this);
    m_formLayout->addRow(new QLabel(tr("Current Value"), this), curValLayout);

    connect(m_measurementCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(fillChannelCombo()));
    _FillMeasurementCombo();
}

void ChannelSettings::_FillMeasurementCombo()
{
    ChannelBase *originalChannel = m_graphicsContainer->GetChannel(m_channelWidget);
    Measurement *originalMeasurement = originalChannel->GetMeasurement();

    unsigned index = 0;
    foreach (Measurement *item, m_measurements)
    {
        if (item->GetWidget() == m_graphicsContainer)
            continue; //it is not possible to select channels from current gc

        m_measurementCombo->addItem(item->GetWidget()->GetName(), index);
        if (item == originalMeasurement)
        {
            m_measurementCombo->setCurrentIndex(index);
        }
        index++;
    }
}

void ChannelSettings::fillChannelCombo()
{
    Measurement *currentMeasurement = m_measurements[m_measurementCombo->currentData()];
    GraphicsContainer *currentGC = currentMeasurement->GetWidget();
    ChannelBase *originalChannel = m_graphicsContainer->GetChannel(m_channelWidget);

    for (unsigned index = 0; index < currentMeasurement->GetChannelCount(); ++index)
    {
        ChannelBase * iteratingChannel = currentMeasurement->GetChannel(index);
        m_channelCombo->addItem(currentGC->GetChannelWidget(iteratingChannel)->GetName(), index);
        if (iteratingChannel == originalChannel)
        {
            m_channelCombo->setCurrentIndex(index);
        }
    }
}

void ChannelSettings::_InitializeValueLine(ChannelWidget *channelWidget)
{
    ChannelBase *channel = m_graphicsContainer->GetChannel(channelWidget);
    QHBoxLayout *curValLayout = new QHBoxLayout();
    m_currentValueControl = new QLineEdit(this);
    curValLayout->addWidget(m_currentValueControl);

    QPushButton *originlValue = new QPushButton(tr("Original"), this);
    connect(originlValue, SIGNAL(clicked(bool)), this, SLOT(setOriginalValue(bool)));
    curValLayout->addWidget(originlValue);

    QPushButton *naValue = new QPushButton(tr("n/a"), this);
    connect(naValue, SIGNAL(clicked(bool)), this, SLOT(setNaValue(bool)));
    curValLayout->addWidget(naValue);

    double currentHorizontalValue =
        m_graphicsContainer->GetHorizontalValueBySliderPos(m_graphicsContainer->GetCurrentIndex());
    unsigned currentIndex =
        m_graphicsContainer->GetHorizontalChannel(channel->GetMeasurement())->GetLastValueIndex(currentHorizontalValue);
    if (currentIndex < (int)channel->GetValueCount())
    {
        QLocale locale(QLocale::system());
        QString currentValueStr = (channel->IsValueNA(currentIndex)) ?
            channelWidget->GetNAValueString() : locale.toString(channel->GetValue(currentIndex));

        m_currentValueControl->setText(currentValueStr);
    }
    else
    {
        m_currentValueControl->setDisabled(true);
        originlValue->setDisabled(true);
        naValue->setDisabled(true);
    }
    //must be defined after setTest
    connect(m_currentValueControl, SIGNAL(textChanged(QString)), this, SLOT(currentValueChanged(QString)));


    m_formLayout->addRow(new QLabel(tr("Current Value"), this), curValLayout);
}

void ChannelSettings::setOriginalValue(bool checked)
{
    Q_UNUSED(checked);
    //this method is called just in a case the original value box is  enabled and
    //it is just in the case index is in range of this channel
    QLocale locale(QLocale::system());
    double currentValue = ((HwChannel*)m_channel)->GetOriginalValue(m_graphicsContainer->GetCurrentIndex());
    m_currentValueControl->setText(
        (currentValue == ChannelBase::GetNaValue()) ?
            ChannelWidget::GetNAValueString() :
            locale.toString(currentValue)
    );

    //was changed in currentValueChanged
    m_currentValue = currentValue;
}

void ChannelSettings::setNaValue(bool)
{
    m_currentValueControl->setText(ChannelWidget::GetNAValueString());
    m_currentValue = ChannelBase::GetNaValue();
}

void ChannelSettings::currentValueChanged(QString const &content)
{
    QLocale locale(QLocale::system());
    m_currentValue = locale.toDouble(m_currentValueControl->text());
    m_currentValueChanged = true;
}

void ChannelSettings::_InitializePenStyle(Qt::PenStyle selected)
{
    m_penStyle = new bases::ComboBox(this);
    m_penStyle->addItem(tr("No Line"));
    m_penStyle->addItem(tr("Solid Line"));
    m_penStyle->addItem(tr("Dash Line"));
    m_penStyle->addItem(tr("Dot Line"));
    m_penStyle->addItem(tr("Dash Dot Line"));
    m_penStyle->addItem(tr("Dash Dot Dot Line"));
    m_penStyle->setCurrentIndex((int)selected);

    m_formLayout->addRow(new QLabel(tr("Pen Style"), this), m_penStyle);
}

void ChannelSettings::_InitializeTimeFeatures()
{
    SampleChannel * channel = (SampleChannel*)m_channel;

    m_style = new bases::ComboBox(this);
    m_style->addItem(m_graphicsContainer->GetSampleChannelStyleText(SampleChannel::Samples), false);
    m_style->addItem(m_graphicsContainer->GetSampleChannelStyleText(SampleChannel::TimeOffset), false);
    m_style->addItem(m_graphicsContainer->GetSampleChannelStyleText(SampleChannel::RealTime), true); //RealTime state as data
    m_style->setCurrentIndex(channel->m_style);//unfortunately I cant use a template with a Qt class
    connect(m_style, SIGNAL(currentIndexChanged(int)), this, SLOT(styleChanged(int)));
    m_formLayout->addRow(new QLabel(tr("Style"), this), m_style);

    m_units->setVisible(false); //there will be diplayed timeUnits combo box instead units
    m_timeUnits = new bases::ComboBox(this);
    m_timeUnits->addItem(tr("Microseconds"));
    m_timeUnits->addItem(tr("Miliseconds"));
    m_timeUnits->addItem(tr("Seconds"));
    m_timeUnits->addItem(tr("Minuts"));
    m_timeUnits->addItem(tr("Hours"));
    m_timeUnits->addItem(tr("Days"));
    m_timeUnits->setCurrentIndex(channel->m_timeUnits);
    m_timeUnits->setEnabled(channel->m_style == SampleChannel::TimeOffset);
    m_formLayout->addRow(new QLabel(tr("Units"), this), m_timeUnits);

    m_format = new bases::ComboBox(this);
    m_format->addItem(tr("day.month.year"));
    m_format->addItem(tr("day.month.hour:minute"));
    m_format->addItem(tr("hour:minute:second"));
    m_format->addItem(tr("minute:second.milisecond"));
    m_format->setCurrentIndex(channel->m_realTimeFormat);
    m_format->setEnabled(channel->m_style == SampleChannel::RealTime);
    m_formLayout->addRow(new QLabel(tr("Format"), this), m_format);
}

void ChannelSettings::styleChanged(int index)
{
    m_timeUnits->setEnabled((SampleChannel::Style)index == SampleChannel::TimeOffset);
    m_format->setEnabled((SampleChannel::Style)index == SampleChannel::RealTime);
    _RefillAxisCombo(); //on axis with RealTime channel must not be another channel
}

bool ChannelSettings::_AxisCheckForRealTimeMode()
{
    if (m_style != NULL && m_style->currentData().toBool()) //channel with realtime style
    {
        Axis *axis = ((Axis *)m_axisComboBox->currentData().toLongLong());
        if (!axis->IsHorizontal() && !axis->IsEmptyExcept(m_channelWidget))
        {
            MyMessageBox::critical(
                this,
                tr("Time format channel must be placed on a separate axis. Please, choose one.")
            );
            return false;
        }
    }
    return true;
}
bool ChannelSettings::BeforeAccept()
{
    if (!_AxisCheckForRealTimeMode())
        return false;

    bool changed = false;
    bool changedHorizontal = false;
    Axis *axis = (Axis *)m_axisComboBox->currentData().toLongLong();
    if (m_channelWidget->GetChannelGraph()->GetValuleAxis() != axis)
    {
        if (axis->IsHorizontal())
        {
            if (!_MoveLastHorizontalToVertical())
                return false; //no axis has been selected

            changedHorizontal = true;
            m_channelWidget->ShowOrHideGraph(false);
            m_graphicsContainer->SetHorizontalChannel(m_channel->GetMeasurement(), m_channel);
        }

        Axis *lastAxis = m_channelWidget->GetChannelGraph()->GetValuleAxis();
        m_channelWidget->GetChannelGraph()->AssignToAxis(axis);
        lastAxis->UpdateGraphAxisName();
        lastAxis->UpdateGraphAxisStyle();
        lastAxis->UpdateVisiblility();
        m_graphicsContainer->GetPlot()->RescaleAxis(lastAxis->GetGraphAxis());

        changed = true;
    }

    if (m_channel->GetType() == ChannelBase::Type_Hw && m_currentValueChanged)
    {
        QLocale locale(QLocale::system());
        bool ok;
        locale.toDouble(m_currentValueControl->text(), &ok);
        if (m_currentValue != ChannelBase::GetNaValue() && !ok)
        {
            MyMessageBox::information(this, tr("Current value is not a number."));
            return false;
        }
        changed = true;
        ((HwChannel *)m_channel)->ChangeValue(
            m_channel->GetMeasurement()->GetCurrentIndex(),
            m_currentValue);
    }
    if (m_channelWidget->GetName() != m_name->text() && m_channel->GetType() != ChannelBase::Type_Sample)
    {
        changed = true;
        m_channelWidget->SetName(m_name->text());
    }

    if (m_channelWidget->GetForeColor() != m_color)
    {
        changed = true;
        m_channelWidget->SetForeColor(m_color);
    }

    if (m_channelWidget->GetChannelGraph()->GetShapeIndex() != (unsigned)m_shapeComboBox->currentIndex())
    {
        changed = true;
        m_channelWidget->SetShapeIndex(m_shapeComboBox->currentIndex());
    }

    if (m_channel->GetType() == ChannelBase::Type_Sample)
    {
        SampleChannel *channelWithTime = (SampleChannel *)m_channel;
        if ((int)channelWithTime->m_timeUnits != m_timeUnits->currentIndex())
        {
            changed = true;
            channelWithTime->_SetTimeUnits((SampleChannel::TimeUnits)m_timeUnits->currentIndex());
        }

        if ((int)channelWithTime->m_realTimeFormat != m_format->currentIndex())
        {
            changed = true;
            channelWithTime->_SetFormat((SampleChannel::RealTimeFormat)m_format->currentIndex());
        }

        if ((int)channelWithTime->m_style != m_style->currentIndex())
        {
            changed = true;
            channelWithTime->_SetStyle((SampleChannel::Style)m_style->currentIndex());
        }
    }
    else
    {
        if (m_channelWidget->GetUnits() != m_units->text())
        {
            changed = true;
            m_channelWidget->SetUnits(m_units->text());
        }
    }

    if (m_penStyle->currentIndex() != (int)m_channelWidget->GetPenStyle())
    {
        changed = true;
        m_channelWidget->SetPenStyle((Qt::PenStyle)m_penStyle->currentIndex());
    }

    if (changed)
    {
        GlobalSettings::GetInstance().SetSavedState(false);
        if (changedHorizontal)
        {
            m_channelWidget->UpdateTitle();
        }

        m_channelWidget->GetPlot()->ReplotIfNotDisabled();

    }

    return true;
}

bool ChannelSettings::_MoveLastHorizontalToVertical()
{
    foreach (ChannelWidget *channelWidget, m_graphicsContainer->GetChannelWidgets())
    {
        //find last horizontal axis
        if (channelWidget->GetChannelGraph()->GetValuleAxis()->IsHorizontal())
        {
            AxisChooseDialog dialog(this, m_graphicsContainer, channelWidget, m_channelWidget);
            return (QDialog::Rejected != dialog.exec());
        }
    }
    return false; //it should never reach this point
}

void ChannelSettings::_InitializeShapeCombo(ChannelWidget *channelWidget)
{
    m_shapeComboBox = new bases::ComboBox(this);
    m_shapeComboBox->addItem(tr("Cross"));
    m_shapeComboBox->addItem(tr("Plus"));
    m_shapeComboBox->addItem(tr("Circle"));
    m_shapeComboBox->addItem(tr("Disc"));
    m_shapeComboBox->addItem(tr("Square"));
    m_shapeComboBox->addItem(tr("Diamond"));
    m_shapeComboBox->addItem(tr("Star"));
    m_shapeComboBox->addItem(tr("Triangle"));
    m_shapeComboBox->addItem(tr("Inverted Triangle"));
    m_shapeComboBox->addItem(tr("Cross and Square"));
    m_shapeComboBox->addItem(tr("Plus and Square"));
    m_shapeComboBox->addItem(tr("Cross and Circle"));
    m_shapeComboBox->addItem(tr("Plus and Circle"));
    m_shapeComboBox->addItem(tr("Peace"));
    m_shapeComboBox->setCurrentIndex(channelWidget->GetChannelGraph()->GetShapeIndex());
    m_shapeComboBox->setEnabled(!channelWidget->IsOnHorizontalAxis());
    m_formLayout->addRow(new QLabel(tr("Shape"), this), m_shapeComboBox);
}

void ChannelSettings::_RefillAxisCombo()
{
    disconnect(m_axisComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(axisChanged(int)));
    m_axisComboBox->clear();
    m_axisComboBox->addItem(tr("New Axis..."));
    foreach (Axis *axis, m_graphicsContainer->GetAxes())
    {
        bool valid =
                m_channelWidget->GetChannelGraph()->GetValuleAxis() == axis || //I should be able to switch back to original axis
                axis->IsHorizontal(); //as same as to horizontal

        if (!valid)
        {
            if (m_channel->GetType() == ChannelBase::Type_Sample && m_style->currentData().toBool())
                valid = axis->IsEmptyExcept(NULL); //channel with real time style might be moved only on empty vertical axis because of differet graphic axis style
            else
                valid = !axis->ContainsChannelWithRealTimeStyle();//but on DateTime axis might be only one channel
        }

        if (valid)
            m_axisComboBox->addItem(axis->GetTitle(), (qlonglong)axis);
    }

    m_axisComboBox->setCurrentIndex(
        m_axisComboBox->findData((qlonglong)(m_channelWidget->GetChannelGraph()->GetValuleAxis())));
    connect(m_axisComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(axisChanged(int)));
}
void ChannelSettings::_InitializeAxisCombo()
{
    m_axisComboBox = new bases::ComboBox(this);
    _RefillAxisCombo();

    if (m_channelWidget->IsOnHorizontalAxis())
        m_axisComboBox->setEnabled(false);
    else
        m_axisComboBox->setEnabled(true);
    m_formLayout->addRow(new QLabel(tr("Axis"), this), m_axisComboBox);
}

void ChannelSettings::axisChanged(int index)
{
    if (0 == index) //New Axis...
    {
        Axis*newAxis = m_graphicsContainer->CreateYAxis(m_channelWidget->GetForeColor());

        AxisSettings dialog(this, newAxis, GlobalSettings::GetInstance().GetAcceptChangesByDialogClosing());
        if (QDialog::Accepted == dialog.exec())
        {
            GlobalSettings::GetInstance().SetSavedState(false);
            m_axisComboBox->addItem(newAxis->GetTitle(), (qlonglong)newAxis);
            m_axisComboBox->setCurrentIndex(m_axisComboBox->findData((qlonglong)(newAxis)));
            m_shapeComboBox->setEnabled(true); //new axis might be just a horizontal one
        }
        else
            m_graphicsContainer->RemoveAxis(newAxis);
    }
    else
    {
        Axis *axis = (Axis *)m_axisComboBox->currentData().toLongLong();
        m_shapeComboBox->setEnabled(!axis->IsHorizontal());
    }
}
