#include "ChannelSettings.h"
#include <Axis.h>
#include <AxisChooseDialog.h>
#include <AxisSettings.h>
#include <ChannelBase.h>
#include <ChannelWidget.h>
#include <GlobalSettings.h>
#include <graphics/ChannelProxyBase.h>
#include <graphics/GraphicsContainer.h>
#include <graphics/HwChannelProxy.h>
#include <graphics/SampleChannelProperties.h>
#include <graphics/SampleChannelProxy.h>
#include <hw/Sensor.h>
#include <hw/SensorComponent.h>
#include <hw/ValueCorrection.h>
#include <hw/SensorManager.h>
#include <hw/SensorQuantity.h>
#include <HwChannel.h>
#include <Measurement.h>
#include <memory>
#include <MainWindow.h>
#include <MyMessageBox.h>
#include <Plot.h>
#include <bases/ComboBox.h>
#include <QDoubleValidator>
#include <QFormLayout>
#include <QHBoxLayout>
#include <bases/Label.h>
#include <bases/LineEdit.h>
#include <bases/CheckBox.h>
#include <QLocale>
#include <bases/PushButton.h>
#include <QSettings>
#include <QString>
#include <SampleChannel.h>

QString ChannelSettings::_GetValueCorrectionTranslation(QString const &correctionName)
{
    if (correctionName == "None")
        return tr("None");
    if (correctionName == "Offset")
        return tr("Offset");
    if (correctionName == "Linear")
        return tr("Linear");
    if (correctionName == "Quadratic")
        return tr("Quadratic");

    qWarning() << "unknown correction to translate: " << correctionName;
    return  correctionName;
}

QString ChannelSettings::_GetQuantityNameTranslation(QString const &quantityName)
{

    if (quantityName == "")
        return "";
    if (quantityName == "Temperature")
        return tr("Temperature");
    if (quantityName == "Voltage")
        return tr("Voltage");
    if (quantityName == "Current")
        return tr("Current");
    if (quantityName == "Pressure")
        return tr("Pressure");
    if (quantityName == "Humidity")
        return tr("Humidity");

    if (quantityName == "RAW")
        return tr("RAW");
    if (quantityName == "Acceleration")
        return tr("Acceleration");
    if (quantityName == "Magnetic Field Intensity")
        return tr("Magnetic Field Intensity");
    if (quantityName == "Force")
        return tr("Force");
    if (quantityName == "UV Index")
        return tr("UV Index");

    if (quantityName == "pH")
        return tr("pH");
    if (quantityName == "Conductivity")
        return tr("Conductivity");
    if (quantityName == "Illuminace")
        return tr("Illuminace");
    if (quantityName == "Concentration")
        return tr("Concentration");
    if (quantityName == "Distance")
        return tr("Distance");

    if (quantityName == "Velocity")
        return tr("Velocity");
    if (quantityName == "Trigger")
        return tr("Trigger");
    if (quantityName == "Sound intensity level")
        return tr("Sound intensity level");
    if (quantityName == "Altitude")
        return tr("Altitude");
    if (quantityName == "Dewpoint")
        return tr("Dewpoint");

    if (quantityName == "Concentration (ppm)")
        return tr("Concentration (ppm)");
    if (quantityName == "Concentration (%)")
        return tr("Concentration (%)");
    if (quantityName == "UV Intensity")
        return tr("UV Intensity");
    if (quantityName == "Accel X")
        return tr("Accel X");
    if (quantityName == "Accel Y")
        return tr("Accel Y");

    if (quantityName == "Accel Z")
        return tr("Accel Z");
    if (quantityName == "Mag X")
        return tr("Mag X");
    if (quantityName == "Mag Y")
        return tr("Mag Y");
    if (quantityName == "Mag Z")
        return tr("Mag Z");
    if (quantityName == "Heading")
        return tr("Heading");

    if (quantityName == "Roll")
        return tr("Roll");
    if (quantityName == "Pitch")
        return tr("Pitch");
    if (quantityName == "Pulse duration (On)")
        return tr("Pulse duration (On)");
    if (quantityName == "Pulse duration (Off)")
        return tr("Pulse duration (Off)");
    if (quantityName == "Period (Rising)")
        return tr("Period (Rising)");

    if (quantityName == "Period (Falling)")
        return tr("Period (Falling)");
    if (quantityName == "Frequency (Rising)")
        return tr("Frequency (Rising)");
    if (quantityName == "Frequency (Falling)")
        return tr("Frequency (Falling)");
    if (quantityName == "Frequency")
        return tr("Frequency");
    if (quantityName == "Length")
        return tr("Length");

    qWarning() << "unknown quantity to translate: " << quantityName;
    return quantityName; //TODO: TFs mod: Remove!!!
}

ChannelSettings::ChannelSettings(
    QVector<Measurement *> measurements,
    GraphicsContainer *graphicsContainer,
    ChannelProxyBase *channelProxy,
    hw::SensorManager *sensorManager
) :
    bases::FormDialogColor (channelProxy->GetWidget(), tr("Channel settings"), GlobalSettings::GetInstance().GetAcceptChangesByDialogClosing()),
    m_measurements(measurements),
    m_graphicsContainer(graphicsContainer),
    m_channelProxy(channelProxy),
    m_originalProxy(channelProxy),
    m_currentValueControl(new LineEdit(this)),
    m_name(new LineEdit(m_channelProxy->GetName(), this)),
    m_units(new LineEdit(m_channelProxy->GetUnits(), this)),
    m_sourceMeasurementCombo(new bases::ComboBox(this)),
    m_sourceChannelCombo(new bases::ComboBox(this)),
    m_shapeComboBox(new bases::ComboBox(this)),
    m_axisComboBox(new bases::ComboBox(this)),
    m_style(new bases::ComboBox(this)),
    m_timeUnits(new bases::ComboBox(this)),
    m_format(new bases::ComboBox(this)),
    m_penStyle(new bases::ComboBox(this)),
    m_sensorQuantityComboBox(new bases::ComboBox(this)),
    m_sensorNameComboBox(new bases::ComboBox(this)),
    m_sensorPortComboBox(new bases::ComboBox(this)),
    m_correctionComboBox(new bases::ComboBox(this)),
    m_correctionPoint1Orig(new LineEdit(this)),
    m_correctionPoint1New(new LineEdit(this)),
    m_correctionPoint2Orig(new LineEdit(this)),
    m_correctionPoint2New(new LineEdit(this)),
    m_correctionPoint3Orig(new LineEdit(this)),
    m_correctionPoint3New(new LineEdit(this)),
    m_originlValue(new PushButton(tr("Original"), this)),
    m_naValue(new PushButton(tr("n/a"), this)),
    m_currentValueChanged(false),
    m_currentValue(ChannelBase::GetNaValue()),
    m_sensorManager(sensorManager),
    m_valueCorrection(nullptr),
    m_sensorPropertiesInitialized(false),
    m_showAllMarks(new bases::CheckBox(this))
{
    _HideAllOptional();

    if (m_channelProxy->IsGhost())
    {
        _InitializeGhostCombos();
        connect(m_sourceMeasurementCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(fillChannelCombos(int)));
        connect(m_sourceChannelCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(loadFromOriginalWidget(int)));
    }

    if (dynamic_cast<SampleChannelProxy*>(channelProxy) || m_channelProxy->IsGhost())
    {
        _InitializeTimeFeatures(dynamic_cast<SampleChannelProxy*>(channelProxy));
    }

    if (dynamic_cast<HwChannelProxy*>(channelProxy) || m_channelProxy->IsGhost())
    {
        HwChannelProxy * hwProxy = dynamic_cast<HwChannelProxy*>(channelProxy);
        _InitializeValueLine(hwProxy);
        _InitializeSensorItems(hwProxy);
        AddSeparator();
        if (hwProxy != nullptr){
            _InitializeCorrectionItems(hwProxy->GetValueCorrection());
            AddSeparator();
        }m_name->setVisible(true);
        m_formLayout->addRow(new Label(tr("Title"), this), m_name);
        m_units->setVisible(true);
        m_units->setEnabled(hwProxy);
        m_formLayout->addRow(new Label(tr("Units"), this), m_units);

    }
    AddColorButtonRow(channelProxy->GetForeColor());
    _InitializeAxisCombo();
    _InitializeShapeCombo(channelProxy);
    _InitializeShowAllMarks(channelProxy->AreAllMarksShown());
    _InitializePenStyle(channelProxy->GetPenStyle());
}

void ChannelSettings::_HideAllOptional()
{
    m_name->setVisible(false);
    m_currentValueControl->setVisible(false);
    m_units->setVisible(false);
    m_sourceMeasurementCombo->setVisible(false);
    m_sourceChannelCombo->setVisible(false);
    m_style->setVisible(false);
    m_timeUnits->setVisible(false);
    m_format->setVisible(false);
    m_sensorQuantityComboBox->setVisible(false);
    m_sensorNameComboBox->setVisible(false);
    m_sensorPortComboBox->setVisible(false);
    m_originlValue->setVisible(false);
    m_naValue->setVisible(false);
    m_correctionComboBox->setVisible(false);
    m_correctionPoint1Orig->setVisible(false);
    m_correctionPoint1New->setVisible(false);
    m_correctionPoint2Orig->setVisible(false);
    m_correctionPoint2New->setVisible(false);
    m_correctionPoint3Orig->setVisible(false);
    m_correctionPoint3New->setVisible(false);
}
void ChannelSettings::_InitializeSensorItem(bases::ComboBox *item, QString const &label, const char* slot)
{
    item->setVisible(true);
    item->setEnabled(m_channelProxy->GetChannelMeasurement()->GetState() == Measurement::Ready);
    m_formLayout->addRow(new Label(label, this), item);
    connect(item, SIGNAL(currentIndexChanged(int)), this, slot);
}

void ChannelSettings::_FillSensorPortCB(HwChannelProxy *channelProxy)
{
    m_sensorPortComboBox->addItem(tr("Undefined"), hw::SensorManager::nonePortId);
    if (m_sensorManager->GetSensors().size() == 1)
        return; //sensors are not defined

    for (unsigned i = 1; i <= hw::SensorManager::sensorPortCount; ++i)
    {
        m_sensorPortComboBox->addItem(_GetPortName(i), i);
        if (i == channelProxy->GetSensorPort() && !channelProxy->GetChannelMeasurement()->IsLegacyFirmwareVersion()){
            m_sensorPortComboBox->setCurrentIndex(m_sensorPortComboBox->count() - 1);
        }
    }
    m_sensorPortComboBox->setEnabled(
        channelProxy->GetChannelMeasurement()->GetState() == Measurement::Ready &&
        !channelProxy->GetChannelMeasurement()->IsLegacyFirmwareVersion()
    );
}

void ChannelSettings::_FillSensorNameCB(HwChannelProxy *channelProxy)
{
    unsigned currentSensorId = channelProxy->GetSensor()->GetId();
    m_sensorNameComboBox->clear();
    foreach (hw::Sensor *sensor, m_sensorManager->GetSensors())
    {

        if (sensor->GetId() == hw::Sensor::noSensorId)
        {
            if (m_sensorPortComboBox->currentData().toInt() == hw::SensorManager::nonePortId)
            {
                m_sensorNameComboBox->addItem(sensor->GetName(), sensor->GetId());
                m_sensorNameComboBox->setEnabled(false);
                return;
            }
        }
        else
        {
            m_sensorNameComboBox->addItem(sensor->GetName(), sensor->GetId());
        }

        if (currentSensorId == sensor->GetId())
        {
            m_sensorNameComboBox->setCurrentIndex(m_sensorNameComboBox->count() - 1);
        }
    }
    m_sensorNameComboBox->setEnabled(channelProxy->GetChannelMeasurement()->GetState() == Measurement::Ready);
}

void ChannelSettings::_FillSensorQuanitityCB(HwChannelProxy *channelProxy)
{
    hw::SensorComponent *currentSensorComponent = channelProxy->GetSensorComponent();
    m_sensorQuantityComboBox->clear();
    unsigned currentSensorId = m_sensorNameComboBox->currentData().toInt();

    foreach (hw::Sensor *sensor, m_sensorManager->GetSensors())
    {
        if (sensor->GetId() == currentSensorId)
        {
            foreach (hw::SensorComponent *component, sensor->GetComponents())
            {
                hw::SensorQuantity * quantity = component->GetQuantity();
                m_sensorQuantityComboBox->addItem(_GetQuantityNameTranslation(quantity->GetName()), quantity->GetId());
                if (currentSensorComponent == component)
                {
                    m_sensorQuantityComboBox->setCurrentIndex(m_sensorQuantityComboBox->count() - 1);
                }
            }
            break;
        }
    }
    m_sensorQuantityComboBox->setEnabled(
        m_channelProxy->GetChannelMeasurement()->GetState() == Measurement::Ready &&
        m_sensorQuantityComboBox->count() > 1
    );
}

void ChannelSettings::sensorQuantityIndexChanged(int index)
{
    if (index == -1)
        return;

    unsigned currentSensorId = m_sensorNameComboBox->currentData().toInt();
    hw::SensorComponent *component = m_sensorManager->GetSensor(currentSensorId)->GetComponent(index);
    hw::ValueCorrection * correction = component->GetValueCorrection();
    m_correctionComboBox->setCurrentIndex(correction->GetId());
}
void ChannelSettings::_InitializeSensorItems(HwChannelProxy *channelProxy)
{
    _InitializeSensorItem(m_sensorPortComboBox, tr("Sensor Port"), SLOT(sensorPortChanged(int)));
    _InitializeSensorItem(m_sensorNameComboBox, tr("Sensor Name"), SLOT(sensorNameChanged(int)));
    _InitializeSensorItem(m_sensorQuantityComboBox, tr("Sensor Quantity"), SLOT(sensorQuantityChanged(int)));
    _FillSensorItems(channelProxy);
    m_sensorPropertiesInitialized = true;
}

void ChannelSettings::_InitializeCorrectionPoint(QString const &label, LineEdit *origValueEdit, LineEdit *newValueEdit)
{
    QHBoxLayout * layout = new QHBoxLayout(this);
    m_formLayout->addRow(label, layout);
    layout->addWidget(origValueEdit);
    layout->addWidget(newValueEdit);

    connect(origValueEdit, SIGNAL(textChanged(QString)), this, SLOT(correctionVariableChanged(QString)));
    connect(newValueEdit, SIGNAL(textChanged(QString)), this, SLOT(correctionVariableChanged(QString)));
}

void ChannelSettings::_FillCorrectionPoints()
{
    QLocale locale = QLocale(QLocale::system());

    unsigned pointCount = m_valueCorrection->GetPointCount();
    auto points = m_valueCorrection->GetPoints();
    if (pointCount > 2)
    {
        m_correctionPoint1Orig->setText(locale.toString(points[0].first));
        m_correctionPoint1New->setText(locale.toString(points[0].second));
        m_correctionPoint2Orig->setText(locale.toString(points[1].first));
        m_correctionPoint2New->setText(locale.toString(points[1].second));
        m_correctionPoint3Orig->setText(locale.toString(points[2].first));
        m_correctionPoint3New->setText(locale.toString(points[2].second));
    }
    else if (pointCount > 1)
    {
        m_correctionPoint1Orig->setText(locale.toString(points[0].first));
        m_correctionPoint1New->setText(locale.toString(points[0].second));
        m_correctionPoint2Orig->setText(locale.toString(points[1].first));
        m_correctionPoint2New->setText(locale.toString(points[1].second));
        m_correctionPoint3Orig->setText("");
        m_correctionPoint3New->setText("");
    }
    else if (pointCount > 0)
    {
        qDebug() << "points " << points[0].first << " " << points[0].second;
        m_correctionPoint1Orig->setText(locale.toString(points[0].first));
        m_correctionPoint1New->setText(locale.toString(points[0].second));
        m_correctionPoint2Orig->setText("");
        m_correctionPoint2New->setText("");
        m_correctionPoint3Orig->setText("");
        m_correctionPoint3New->setText("");
    }
    else{
        m_correctionPoint1Orig->setText("");
        m_correctionPoint1New->setText("");
        m_correctionPoint2Orig->setText("");
        m_correctionPoint2New->setText("");
        m_correctionPoint3Orig->setText("");
        m_correctionPoint3New->setText("");
    }

    m_correctionPoint1Orig->setEnabled(pointCount > 0);
    m_correctionPoint1New->setEnabled(pointCount > 0);
    m_correctionPoint2Orig->setEnabled(pointCount > 1);
    m_correctionPoint2New->setEnabled(pointCount > 1);
    m_correctionPoint3Orig->setEnabled(pointCount > 2);
    m_correctionPoint3New->setEnabled(pointCount > 2);
}
void ChannelSettings::_FillCorrectionValues(unsigned id, bool addItem)
{
    unsigned index = 0;
    foreach (hw::ValueCorrection *correction, m_sensorManager->GetSensorCorrections())
    {
        if (addItem)
        {
            m_correctionComboBox->addItem(
                _GetValueCorrectionTranslation(correction->GetName()) +  " (" + correction->GetDescription() +")", correction->GetId()
            );
        }
        if (id == correction->GetId())
        {
            if (!addItem){
                hw::ValueCorrection * temp = new hw::ValueCorrection(this, correction);
                temp->CopyPoints(m_valueCorrection);
                delete m_valueCorrection;
                m_valueCorrection = temp;
            }
            _FillCorrectionPoints();

            if (addItem){
                m_correctionComboBox->setCurrentIndex(index);
            }
            else{
                return;
            }
        }
        index++;
    }
}

void ChannelSettings::_InitializeCorrectionItems(hw::ValueCorrection * originalCorrection)
{ 
    m_valueCorrection = new hw::ValueCorrection(this, originalCorrection);
    m_correctionComboBox->setVisible(true);
     m_correctionComboBox->setEnabled(true);
    m_formLayout->addRow(new Label(tr("Correction Type"), this), m_correctionComboBox);

    m_correctionPoint1Orig->setVisible(true);
    m_correctionPoint1Orig->setValidator(new QDoubleValidator(this));
    m_correctionPoint1New->setVisible(true);
    m_correctionPoint1New->setValidator(new QDoubleValidator(this));
    m_correctionPoint2Orig->setVisible(true);
    m_correctionPoint2Orig->setValidator(new QDoubleValidator(this));
    m_correctionPoint2New->setVisible(true);
    m_correctionPoint2New->setValidator(new QDoubleValidator(this));
    m_correctionPoint3Orig->setVisible(true);
    m_correctionPoint3Orig->setValidator(new QDoubleValidator(this));
    m_correctionPoint3New->setVisible(true);
    m_correctionPoint3New->setValidator(new QDoubleValidator(this));

    QHBoxLayout * correctionHeaderLayout = new QHBoxLayout(this);
    m_formLayout->addRow("", correctionHeaderLayout);
    correctionHeaderLayout->addWidget(new Label(tr("Measured"), this));
    correctionHeaderLayout->addWidget(new Label(tr("Expected"), this));

    _InitializeCorrectionPoint(tr("Point 1"), m_correctionPoint1Orig, m_correctionPoint1New);
    _InitializeCorrectionPoint(tr("Point 2"), m_correctionPoint2Orig, m_correctionPoint2New);
    _InitializeCorrectionPoint(tr("Point 3"), m_correctionPoint3Orig, m_correctionPoint3New);

    _FillCorrectionValues(m_valueCorrection->GetId(), true);
    connect(m_correctionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(correctionTypeChanged(int)));
}

void ChannelSettings::correctionTypeChanged(int index)
{    
    if (index == -1)
        return;

    _FillCorrectionValues(m_correctionComboBox->itemData(index).toUInt(), false);
}

void ChannelSettings::correctionVariableChanged(QString newValue)
{
    if (newValue.isEmpty())
        return;

    double doubleValue = QLocale(QLocale::system()).toDouble(newValue);
    qDebug() << "entered value:" << doubleValue;
    if (sender() == m_correctionPoint1Orig)
        m_valueCorrection->SetPoint(0, true, doubleValue);
    else if (sender() == m_correctionPoint1New)
        m_valueCorrection->SetPoint(0, false, doubleValue);
    else if (sender() == m_correctionPoint2Orig)
        m_valueCorrection->SetPoint(1, true, doubleValue);
    else if (sender() == m_correctionPoint2New)
        m_valueCorrection->SetPoint(1, false, doubleValue);
    else if (sender() == m_correctionPoint3Orig)
        m_valueCorrection->SetPoint(2, true, doubleValue);
    else if (sender() == m_correctionPoint3New)
        m_valueCorrection->SetPoint(2, false, doubleValue);
}

void ChannelSettings::_FillSensorItems(HwChannelProxy *channelProxy)
{
    m_sensorPortComboBox->clear();
    m_sensorNameComboBox->clear();
    m_sensorQuantityComboBox->clear();

    m_sensorPortComboBox->setEnabled(channelProxy);
    m_sensorNameComboBox->setEnabled(channelProxy);
    m_sensorNameComboBox->setEnabled(channelProxy);

    if (channelProxy)
    {
        _FillSensorPortCB(channelProxy);
        _FillSensorNameCB(channelProxy);
        _FillSensorQuanitityCB(channelProxy);
        connect(m_sensorQuantityComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(sensorQuantityIndexChanged(int)));
    }
}

void ChannelSettings::sensorPortChanged(int index)
{
    Q_UNUSED(index)
    if (index != -1 &&
        (m_sensorPortComboBox->currentData().toInt() == hw::SensorManager::nonePortId) ==
        !(m_sensorNameComboBox->currentData().toInt() == hw::Sensor::noSensorId)
    )
    {
        _FillSensorNameCB(dynamic_cast<HwChannelProxy*>(m_channelProxy));
    }
}

void ChannelSettings::sensorNameChanged(int index)
{
    if (index != -1)
    {
        _FillSensorQuanitityCB(dynamic_cast<HwChannelProxy*>(m_channelProxy));
    }
}
void ChannelSettings::sensorQuantityChanged(int index)
{
    if (!m_sensorPropertiesInitialized)
        return; //changed by pre-filling

    if (index == -1)
    {
        m_name->setText("");
        m_units->setText("");
        return;
    }

    m_name->setText(m_sensorQuantityComboBox->itemText(index));

    unsigned currentSensorId = m_sensorNameComboBox->currentData().toInt();
    hw::Sensor *sensor = m_sensorManager->GetSensor(currentSensorId);
    hw::SensorComponent *component = sensor->GetComponent(index);
    m_units->setText(component->GetUnit());
}

void ChannelSettings::_InitializeGhostCombos()
{
    m_sourceMeasurementCombo->setVisible(true);
    m_sourceChannelCombo->setVisible(true);
    m_formLayout->addRow(new Label(tr("Source Measurement"), this), m_sourceMeasurementCombo);
    m_formLayout->addRow(new Label(tr("Source Channel"), this), m_sourceChannelCombo);
    AddSeparator();
    _FillMeasurementCombo();
}

void ChannelSettings::_FillMeasurementCombo()
{
    Measurement *originalMeasurement = m_channelProxy->GetChannelMeasurement();
    m_sourceMeasurementCombo->clear();
    unsigned comboIndex = 0;
    unsigned measurementIndex = ~0;
    foreach (Measurement *item, m_measurements)
    {
        measurementIndex++;
        if (item->GetGC() == m_graphicsContainer)
            continue; //it is not possible to select channels from current gc

        m_sourceMeasurementCombo->addItem(item->GetGC()->GetName(), measurementIndex);
        if (item == originalMeasurement)
        {
            m_sourceMeasurementCombo->setCurrentIndex(comboIndex);
            fillChannelCombos(comboIndex);
        }
        comboIndex++;
    }
}

void ChannelSettings::fillChannelCombos(int measurementComboIndex)
{
    Q_UNUSED(measurementComboIndex)
    m_sourceChannelCombo->clear();
    Measurement *sourceMeasurement = m_measurements[m_sourceMeasurementCombo->currentData().toInt()];
    GraphicsContainer *sourceGC = sourceMeasurement->GetGC();

    bool channelFound = false;
    for (unsigned index = 0; index < sourceMeasurement->GetChannelCount(); ++index)
    {
        ChannelBase * iteratingChannel = sourceMeasurement->GetChannel(index);
        QString channelName = sourceGC->GetChannelProxy(iteratingChannel)->GetName();
        m_sourceChannelCombo->addItem(channelName, index);

        if (iteratingChannel == m_channelProxy->GetChannel())
        {
            m_sourceChannelCombo->setCurrentIndex(index);
            channelFound = true;
        }
    }
    if (!channelFound)
    {
        m_sourceChannelCombo->setCurrentIndex(1); //skip samples
        loadFromOriginalWidget(m_sourceChannelCombo->currentIndex());
    }
}

void ChannelSettings::loadFromOriginalWidget(int channelComboIndex)
{
    Q_UNUSED(channelComboIndex)
    _DisconnectCurrentValueChange();

    Measurement *originalMeasurement = m_measurements[m_sourceMeasurementCombo->currentData().toInt()];
    GraphicsContainer *originalGC = originalMeasurement->GetGC();
    ChannelBase *originalChannel = originalMeasurement->GetChannel(m_sourceChannelCombo->currentData().toInt());
    ChannelProxyBase *originalChannelProxy = originalGC->GetChannelProxy(originalChannel);

    if (originalChannelProxy->GetChannel() != m_channelProxy->GetChannel())
    {
        m_channelProxy = originalChannelProxy->Clone(m_graphicsContainer, m_channelProxy->GetWidget());
    }
    _FillSensorItems(dynamic_cast<HwChannelProxy*>(m_channelProxy));

    if (dynamic_cast<HwChannelProxy*>(m_channelProxy) != nullptr)
    {
        HwChannelProxy *hwProxy = dynamic_cast<HwChannelProxy*>(m_channelProxy);
        m_valueCorrection = new hw::ValueCorrection(this, hwProxy->GetValueCorrection());
        m_correctionComboBox->setCurrentIndex(m_valueCorrection->GetId());
        _FillCorrectionPoints();
    }

    _FillValueLine(dynamic_cast<HwChannelProxy*>(m_channelProxy));
    _FillTimeFeatures(dynamic_cast<SampleChannelProxy*>(m_channelProxy));
    m_name->setText(m_graphicsContainer->GetGhostName(originalGC, originalChannelProxy));

    m_units->setEnabled(dynamic_cast<HwChannelProxy*>(m_channelProxy) != nullptr);
    m_units->setText(originalChannelProxy->GetUnits());

    if (m_colorButtonWidget) //it is created thgether with add color button
    {
        SetColorButtonColor(originalChannelProxy->GetForeColor());
    }

    m_shapeComboBox->setCurrentIndex(originalChannelProxy->GetChannelGraph()->GetShapeIndex());

    _ConnectCurrentValueChange();
}

unsigned ChannelSettings::_GetCurrentValueIndex(ChannelProxyBase *channelProxy)
{
    Measurement *m = channelProxy->GetChannelMeasurement();
    if (!m_graphicsContainer->IsHorizontalChannelProxyDefined(m))
        return ~0;

    double currentHorizontalValue =
        m_graphicsContainer->GetHorizontalValueBySliderPos(m_graphicsContainer->GetCurrentIndex());
    return m_graphicsContainer->GetHorizontalChannelProxy(m)->GetLastValueIndex(currentHorizontalValue);
}

void ChannelSettings::_InitializeValueLine(HwChannelProxy *channelProxy)
{
    m_currentValueControl->setVisible(true);
    m_currentValueControl->setValidator(new QDoubleValidator(this));
    m_originlValue->setVisible(true);
    m_naValue->setVisible(true);
    QHBoxLayout *curValLayout = new QHBoxLayout();
    curValLayout->addWidget(m_currentValueControl);
    connect(m_originlValue, SIGNAL(clicked(bool)), this, SLOT(setOriginalValue(bool)));
    curValLayout->addWidget(m_originlValue);
    connect(m_naValue, SIGNAL(clicked(bool)), this, SLOT(setNaValue(bool)));
    curValLayout->addWidget(m_naValue);
    m_formLayout->addRow(new Label(tr("Current Value"), this), curValLayout);
    AddSeparator();

    _FillValueLine(channelProxy);
    //must be defined after setText
    _ConnectCurrentValueChange();
}


void ChannelSettings::_ConnectCurrentValueChange()
{
    connect(m_currentValueControl, SIGNAL(textChanged(QString)), this, SLOT(currentValueChanged(QString)));
}

void ChannelSettings::_DisconnectCurrentValueChange()
{
    disconnect(m_currentValueControl, SIGNAL(textChanged(QString)), this, SLOT(currentValueChanged(QString)));
}

void ChannelSettings::_FillValueLine(HwChannelProxy *channelProxy)
{
    unsigned currentIndex = _GetCurrentValueIndex(m_channelProxy);
    if (~0 == currentIndex) //it can happend when horizontal proxy is not deffined yet (added/changed to ghost from a new measurement)
    {
        m_currentValueControl->setText(channelProxy->GetNAValueString());
        m_currentValueControl->setEnabled(false);
        return;
    }

    m_currentValueControl->setText("");
    m_currentValueControl->setEnabled(channelProxy);
    m_originlValue->setEnabled(channelProxy);
    m_naValue->setEnabled(channelProxy);

    if (channelProxy && currentIndex < (int)channelProxy->GetValueCount())
    {
        QLocale locale(QLocale::system());
        QString currentValueStr = (channelProxy->IsValueNA(currentIndex)) ?
            channelProxy->GetNAValueString() : locale.toString(channelProxy->GetValue(currentIndex));

        m_currentValueControl->setText(currentValueStr);
    }
}

void ChannelSettings::setOriginalValue(bool checked)
{
    Q_UNUSED(checked);
    //this method is called just in a case the original value box is  enabled and
    //it is just in the case index is in range of this channel
    QLocale locale(QLocale::system());
    int currentValueIndex = _GetCurrentValueIndex(m_channelProxy);
    if (currentValueIndex == -1)
    {
        qWarning("index for original value has not been found.");
        return;
    }

    HwChannelProxy *hwChannelProxy = dynamic_cast<HwChannelProxy*>(m_channelProxy);
    double originalValue = hwChannelProxy->GetOriginalValue(currentValueIndex);
    double currentValue =  hwChannelProxy->GetValueWithCorrection(currentValueIndex);

    m_currentValueControl->setText(
        (currentValue == ChannelBase::GetNaValue()) ?
            ChannelWidget::GetNAValueString() :
            locale.toString(originalValue)
    );

    //was changed in currentValueChanged
    m_currentValue = originalValue;
}

void ChannelSettings::setNaValue(bool)
{
    m_currentValueControl->setText(ChannelWidget::GetNAValueString());
    m_currentValue = ChannelBase::GetNaValue();
}

void ChannelSettings::currentValueChanged(QString const &content)
{
    Q_UNUSED(content)
    QLocale locale(QLocale::system());
    m_currentValue = locale.toDouble(m_currentValueControl->text());
    m_currentValueChanged = true;
}

void ChannelSettings::_InitializePenStyle(Qt::PenStyle selected)
{
    m_penStyle->addItem(tr("No Line"));
    m_penStyle->addItem(tr("Solid Line"));
    m_penStyle->addItem(tr("Dash Line"));
    m_penStyle->addItem(tr("Dot Line"));
    m_penStyle->addItem(tr("Dash Dot Line"));
    m_penStyle->addItem(tr("Dash Dot Dot Line"));
    m_penStyle->setCurrentIndex((int)selected);

    m_formLayout->addRow(new Label(tr("Pen Style"), this), m_penStyle);
}

void ChannelSettings::_InitializeShowAllMarks(bool show)
{
    m_showAllMarks->setChecked(show);
    m_formLayout->addRow(new Label(tr("All Marks"), this), m_showAllMarks);
}

void ChannelSettings::_FillTimeFeatures(SampleChannelProxy *channelProxy)
{
    m_style->clear();
    m_timeUnits->clear();
    m_format->clear();

    m_style->setEnabled(channelProxy);
    m_timeUnits->setEnabled(channelProxy);
    m_format->setEnabled(channelProxy);
    if (!channelProxy)
    {
        return;
    }

    m_style->addItem(SampleChannelProxy::GetSampleChannelStyleText(SampleChannelProperties::Samples), false);
    m_style->addItem(SampleChannelProxy::GetSampleChannelStyleText(SampleChannelProperties::TimeOffset), false);
    m_style->addItem(SampleChannelProxy::GetSampleChannelStyleText(SampleChannelProperties::RealTime), true); //RealTime state as data
    m_style->setCurrentIndex(channelProxy->GetStyle());//unfortunately I cant use a template with a Qt class

    m_timeUnits->addItem(tr("Microseconds"));
    m_timeUnits->addItem(tr("Miliseconds"));
    m_timeUnits->addItem(tr("Seconds"));
    m_timeUnits->addItem(tr("Minuts"));
    m_timeUnits->addItem(tr("Hours"));
    m_timeUnits->addItem(tr("Days"));
    m_timeUnits->setCurrentIndex(channelProxy->GetTimeUnits());
    m_timeUnits->setEnabled(channelProxy->GetStyle() == SampleChannelProperties::TimeOffset);

    m_format->addItem(tr("day.month.year"));
    m_format->addItem(tr("day.month.hour:minute"));
    m_format->addItem(tr("hour:minute:second"));
    m_format->addItem(tr("minute:second.milisecond"));
    m_format->setCurrentIndex(channelProxy->GetRealTimeFormat());
    m_format->setEnabled(channelProxy->GetStyle() == SampleChannelProperties::RealTime);
}

void ChannelSettings::_InitializeTimeFeatures(SampleChannelProxy *channelProxy)
{
    m_style->setVisible(true);
    m_timeUnits->setVisible(true);
    m_format->setVisible(true);
    m_formLayout->addRow(new Label(tr("Style"), this), m_style);
    m_formLayout->addRow(new Label(tr("Units"), this), m_timeUnits);
    m_formLayout->addRow(new Label(tr("Format"), this), m_format);
    AddSeparator();
    _FillTimeFeatures(channelProxy);
    connect(m_style, SIGNAL(currentIndexChanged(int)), this, SLOT(styleChanged(int)));
}

void ChannelSettings::styleChanged(int index)
{
    m_timeUnits->setEnabled((SampleChannelProperties::Style)index == SampleChannelProperties::TimeOffset);
    m_format->setEnabled((SampleChannelProperties::Style)index == SampleChannelProperties::RealTime);
    _RefillAxisCombo(); //on axis with RealTime channel must not be another channel
}

bool ChannelSettings::_AxisCheckForRealTimeMode()
{
    if (m_style != NULL && m_style->currentData().toBool()) //channel with realtime style
    {
        Axis *axis = ((Axis *)m_axisComboBox->currentData().toLongLong());
        if (!axis->IsHorizontal() && !axis->IsEmptyExcept(m_channelProxy))
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

bool ChannelSettings::_MoveLastHorizontalToVertical()
{
    foreach (ChannelProxyBase *proxy, m_graphicsContainer->GetChannelProxies())
    {
        //find last horizontal axis
        if (proxy->GetChannelGraph()->GetValuleAxis()->IsHorizontal())
        {
            AxisChooseDialog dialog(this, m_graphicsContainer, proxy, m_channelProxy);
            if (QDialog::Rejected != dialog.exec())
            {
                m_channelProxy->ShowGraph(false);
                m_graphicsContainer->SetHorizontalChannel(
                    m_channelProxy->GetChannelMeasurement(), m_channelProxy->GetChannel(), proxy);
                return true;
            }
            return false;
        }
    }
    qWarning() << "horizontal axes has not been found";

    return false; //it should never reach this point
}

void ChannelSettings::_InitializeShapeCombo(ChannelProxyBase *channelProxy)
{
    m_shapeComboBox->addItem(tr("None"), 0);
    m_shapeComboBox->addItem(tr("Cross"), 2);
    m_shapeComboBox->addItem(tr("Plus"), 3);
    m_shapeComboBox->addItem(tr("Circle"), 4);
    m_shapeComboBox->addItem(tr("Disc"), 5);
    m_shapeComboBox->addItem(tr("Square"), 6);
    m_shapeComboBox->addItem(tr("Diamond"), 7);
    m_shapeComboBox->addItem(tr("Star"), 8);
    m_shapeComboBox->addItem(tr("Triangle"), 9);
    m_shapeComboBox->addItem(tr("Inverted Triangle"), 10);
    m_shapeComboBox->addItem(tr("Cross and Square"), 11);
    m_shapeComboBox->addItem(tr("Plus and Square"), 12);
    m_shapeComboBox->addItem(tr("Cross and Circle"), 13);
    m_shapeComboBox->addItem(tr("Plus and Circle"), 14);
    m_shapeComboBox->addItem(tr("Peace"), 15);
    if (channelProxy->GetChannelGraph()->GetShapeIndex() == 0)
    {
        m_shapeComboBox->setCurrentIndex(0);
    }
    else
    {
        m_shapeComboBox->setCurrentIndex(channelProxy->GetChannelGraph()->GetShapeIndex() - 1); //skip dot
    }

    m_shapeComboBox->setEnabled(!channelProxy->IsOnHorizontalAxis());
    m_formLayout->addRow(new Label(tr("Shape"), this), m_shapeComboBox);
}

void ChannelSettings::_RefillAxisCombo()
{
    disconnect(m_axisComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(axisChanged(int)));
    m_axisComboBox->clear();
    m_axisComboBox->addItem(tr("New Axis..."));
    foreach (Axis *axis, m_graphicsContainer->GetAxes())
    {
        bool valid =
                m_channelProxy->GetChannelGraph()->GetValuleAxis() == axis || //I should be able to switch back to original axis
                axis->IsHorizontal(); //as same as to horizontal


        if (!valid)
        {
            if (dynamic_cast<SampleChannelProxy *>(m_channelProxy) && m_style->currentData().toBool())
                valid = axis->IsEmptyExcept(NULL); //channel with real time style might be moved only on empty vertical axis because of differet graphic axis style
            else
                valid = !axis->ContainsChannelWithRealTimeStyle();//but on DateTime axis might be only one channel
        }

        if (valid && axis->IsHorizontal() && m_channelProxy->IsGhost())
            valid = false;

        if (valid)
            m_axisComboBox->addItem(axis->GetTitle(), (qlonglong)axis);
    }

    m_axisComboBox->setCurrentIndex(
        m_axisComboBox->findData((qlonglong)(m_channelProxy->GetChannelGraph()->GetValuleAxis())));
    connect(m_axisComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(axisChanged(int)));
}
void ChannelSettings::_InitializeAxisCombo()
{
    _RefillAxisCombo();

    if (m_channelProxy->IsOnHorizontalAxis())
        m_axisComboBox->setEnabled(false);
    else
        m_axisComboBox->setEnabled(true);
    m_formLayout->addRow(new Label(tr("Axis"), this), m_axisComboBox);
}

void ChannelSettings::axisChanged(int index)
{
    if (0 == index) //New Axis...
    {
        Axis*newAxis = m_graphicsContainer->CreateYAxis(m_channelProxy->GetForeColor());

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

QString ChannelSettings::_GetPortName(int port)
{
    return tr("Port %1").arg(port);
}

GraphicsContainer *ChannelSettings::GetGraphicsContainer()
{
    return m_graphicsContainer;
}

void ChannelSettings::_SetHorizontalChannel(Measurement *m)
{
    m_graphicsContainer->SetHorizontalChannel(m, m->GetGC()->GetHorizontalChannelProxy(m)->GetChannel());
}

bool ChannelSettings::_CheckCorrectionPointsValidity()
{
    auto points = m_valueCorrection->GetPoints();
    QString errorMessage = tr("Entered different expected values for one meassured.");
    switch (static_cast<hw::ValueCorrection::CorrectionType>(m_valueCorrection->GetId())) {
    case hw::ValueCorrection::CorrectionType::None:
        return true;
    case hw::ValueCorrection::CorrectionType::Offset:
        return true;
    case hw::ValueCorrection::CorrectionType::Linear:
        if (ChannelBase::IsEqual(points[0].first, points[1].first) && !ChannelBase::IsEqual(points[0].second, points[1].second))
        {
            MyMessageBox::critical(this, errorMessage);
            return false;
        }
        return true;
    case hw::ValueCorrection::CorrectionType::Quadratic:
        if ((ChannelBase::IsEqual(points[0].first, points[1].first) && !ChannelBase::IsEqual(points[0].second, points[1].second)) ||
            (ChannelBase::IsEqual(points[1].first, points[2].first) && !ChannelBase::IsEqual(points[1].second, points[2].second)) ||
            (ChannelBase::IsEqual(points[0].first, points[2].first) && !ChannelBase::IsEqual(points[0].second, points[2].second))
        )
        {
            MyMessageBox::critical(this, errorMessage);
            return false;
        }
        return true;

    default:
        qDebug() << "usuported correction id " << m_valueCorrection->GetId();
        return false;
    }
}

bool ChannelSettings::BeforeAccept()
{
    m_channelProxy->GetPlot()->SetDisabled(true);
    if (!_AxisCheckForRealTimeMode())
        return false;

    bool changed = false;
    bool changedHorizontal = false;
    bool rescaleAxis = false;
    bool changeStyle = false;

    Axis *axis = (Axis *)m_axisComboBox->currentData().toLongLong();
    if (m_channelProxy->GetChannelGraph()->GetValuleAxis() != axis)
    {
        if (axis->IsHorizontal())
        {
            if (!_MoveLastHorizontalToVertical())
                return false; //no axis has been selected

            changedHorizontal = true;
        }

        Axis *lastAxis = m_channelProxy->GetAxis();
        m_channelProxy->AssignToAxis(axis);
        lastAxis->UpdateGraphAxisName();
        lastAxis->UpdateGraphAxisStyle();
        lastAxis->UpdateVisiblility();
        m_graphicsContainer->GetPlot()->RescaleAxis(lastAxis->GetGraphAxis());

        rescaleAxis = true;
        changed = true;
    }

    HwChannelProxy *hwChannelProxy = dynamic_cast<HwChannelProxy*>(m_channelProxy);
    if (hwChannelProxy && m_currentValueChanged)
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
        rescaleAxis = true;
        unsigned currentIndex = _GetCurrentValueIndex(m_channelProxy);
        hwChannelProxy->ChangeValue(currentIndex, m_currentValue);
    }
    if (m_channelProxy->GetName() != m_name->text() && (hwChannelProxy || m_channelProxy->IsGhost()))
    {
        changed = true;
        m_channelProxy->SetName(m_name->text());
    }

    if (m_channelProxy->GetForeColor() != m_color)
    {
        changed = true;
        m_channelProxy->SetForeColor(m_color);
    }

    if (m_channelProxy->GetChannelGraph()->GetShapeIndex() != (unsigned)m_shapeComboBox->currentData().toInt())
    {
        changed = true;
        m_channelProxy->SetShapeIndex(m_shapeComboBox->currentData().toInt());
        rescaleAxis = true;

    }

    if (dynamic_cast<SampleChannelProxy*>(m_channelProxy))
    {
        SampleChannelProxy *sampleChannelProxy = dynamic_cast<SampleChannelProxy*>(m_channelProxy);
        if ((int)sampleChannelProxy->GetTimeUnits() != m_timeUnits->currentIndex())
        {
            changed = true;
            changeStyle = true;
            sampleChannelProxy->SetTimeUnits((SampleChannelProperties::TimeUnits)m_timeUnits->currentIndex());
        }

        if ((int)sampleChannelProxy->GetRealTimeFormat() != m_format->currentIndex())
        {
            changed = true;
            changeStyle = true;
            sampleChannelProxy->SetRealTimeFormat((SampleChannelProperties::RealTimeFormat)m_format->currentIndex());
        }

        if ((int)sampleChannelProxy->GetStyle() != m_style->currentIndex())
        {
            changed = true;
            changeStyle = true;
            sampleChannelProxy->SetStyle((SampleChannelProperties::Style)m_style->currentIndex());
        }
        sampleChannelProxy->SetUnits(sampleChannelProxy->GetUnits());
    }
    else
    {
        if (m_channelProxy->GetUnits() != m_units->text())
        {
            changed = true;
            m_channelProxy->SetUnits(m_units->text());
        }
    }

    if (m_penStyle->currentIndex() != (int)m_channelProxy->GetPenStyle())
    {
        changed = true;
        m_channelProxy->SetPenStyle((Qt::PenStyle)m_penStyle->currentIndex());
        rescaleAxis = true;
    }

    if (m_showAllMarks->isChecked() != m_channelProxy->AreAllMarksShown())
    {
        m_channelProxy->SetAllMarksShown(m_showAllMarks->isChecked());
        changed = true;
    }

    if (hwChannelProxy)
    {
        hw::Sensor *sensor = m_sensorManager->GetSensor(m_sensorNameComboBox->currentData().toInt());
        if (NULL != sensor && sensor != hwChannelProxy->GetSensor())
        {
          hwChannelProxy->SetSensor(sensor);
          changed = true;
        }

        unsigned port = m_sensorPortComboBox->currentData().toInt();
        if (port != hwChannelProxy->GetSensorPort())
        {
            hwChannelProxy->SetSensorPort(port);
            changed = true;
        }

        hw::SensorComponent *component = m_sensorManager->GetSensorComponent(sensor, m_sensorQuantityComboBox->currentData().toInt());
        if (component != hwChannelProxy->GetSensorComponent())
        {
            hwChannelProxy->SetSensorComponent(component);
            changed = true;
        }

        if (*hwChannelProxy->GetValueCorrection() != *m_valueCorrection)
        {
            if (!_CheckCorrectionPointsValidity())
            {
                return false;
            }

            if (hwChannelProxy->IsGhost())
            {
                if (MyMessageBox::No == MyMessageBox::question(
                    this,
                    tr("Value correction changes performed on a virtual channel are applied to the original channel as well. Continue?"),
                    tr("Continue")
                ))
                {
                    return false;
                }

            }
            hwChannelProxy->SetValueCorrection(m_valueCorrection);
            changed = true;
            rescaleAxis = true;
            changeStyle = true; //to be re-displayed all channel values
        }
    }

    if (m_channelProxy->IsGhost())
    {
        Measurement *newOriginalMeasurement = m_measurements[m_sourceMeasurementCombo->currentData().toInt()];
        ChannelBase *newOriginalChannel = newOriginalMeasurement->GetChannel(m_sourceChannelCombo->currentData().toInt());

        if (m_originalProxy->GetChannel() != newOriginalChannel)
        {
            //Note: proxy is compleetly exchanged also in the case when is changed just ChannelBase. it is more generic
            //returns false when it is a new ghost - in this case it will be processed by a confirmation process
            if (m_graphicsContainer->ReplaceChannelProxy(m_originalProxy, m_channelProxy))
            {
                m_graphicsContainer->GhostManipupationPostProcess(m_channelProxy);
            }

            changed = true;
            //it was aready done in GhostAddingChangingPostProcess
            changedHorizontal = false;
            rescaleAxis = false;
        }
    }

    //NOTE: must be last!
    if (changed)
    {
        if (changeStyle)
        {
           m_channelProxy->GetAxis()->UpdateGraphAxisStyle();
           m_graphicsContainer->GetPlot()->RefillSingleGraph(m_channelProxy);
        }
        else if (rescaleAxis) //axis rescaling is part of RefillSingleGraph
        {
           m_graphicsContainer->GetPlot()->RescaleAxis(axis->GetGraphAxis());
        }

        GlobalSettings::GetInstance().SetSavedState(false);
        if (changedHorizontal)
        {
            m_graphicsContainer->RefillHorizontalChannelMapping();
            m_channelProxy->UpdateTitle();
            if (m_graphicsContainer->ContainsGhost())
            {
                m_graphicsContainer->GetPlot()->RefillGraphs();
                m_graphicsContainer->GetPlot()->SetMarkerLine(m_graphicsContainer->GetCurrentIndex());
            }
        }
        m_channelProxy->GetPlot()->SetDisabled(false);
        m_channelProxy->GetPlot()->ReplotIfNotDisabled();
    }
    m_channelProxy->GetPlot()->SetDisabled(false);

    return true;
}

void ChannelSettings::BeforeReject()
{
    if (m_channelProxy != m_originalProxy)
    {
        delete m_channelProxy;
        m_graphicsContainer->GhostManipupationPostProcess(m_originalProxy);
    }
}
