#ifndef CHANNELSETTINGS_H
#define CHANNELSETTINGS_H

#include <bases/FormDialogColor.h>
#include <QColor>
#include <QVector>
#include <memory>
#include <hw/Sensor.h>

namespace bases { class ComboBox; }
namespace hw { class SensorManager; class SensorQuantity; class ValueCorrection; }
class Axis;
class ChannelProxyBase;
class GraphicsContainer;
class HwChannelProxy;
class Measurement;
class PushButton;
class SampleChannel;
class QFormLayout;
class QFrame;
class LineEdit;
class QString;
class SampleChannelProxy;

class ChannelSettings : public bases::FormDialogColor
{
    Q_OBJECT

    void _InitializeShapeCombo(ChannelProxyBase *channelProxy);
    void _InitializeAxisCombo();
    bool _MoveLastHorizontalToVertical();
    void _InitializeTimeFeatures(SampleChannelProxy *channelProxy);
    void _RefillAxisCombo();
    bool _AxisCheckForRealTimeMode();
    void _InitializePenStyle(Qt::PenStyle selected);
    unsigned _GetCurrentValueIndex(ChannelProxyBase *channelProxy);
    void _InitializeValueLine(HwChannelProxy *channelProxy);
    void _FillValueLine(HwChannelProxy *channelProxy);
    void _FillTimeFeatures(SampleChannelProxy *channelProxy);
    void _InitializeGhostCombos();
    void _FillMeasurementCombo();
    void _InitializeSensorItems(HwChannelProxy *channelProxy);
    void _InitializeCorrectionPoint(QString const &label, LineEdit *origValueEdit, LineEdit *newValueEdit);
    void _FillCorrectionValues(unsigned id, bool addItem);
    void _InitializeCorrectionItems(HwChannelProxy *channelProxy);
    void _FillSensorItems(HwChannelProxy *channelProxy);
    void _InitializeSensorItem(bases::ComboBox *item, const QString &label, const char *slot);
    void _FillSensorQuanitityCB(HwChannelProxy *channelProxy);
    void _FillSensorNameCB(HwChannelProxy *channelProxy);
    void _FillSensorPortCB(HwChannelProxy *channelProxy);
    QString _GetQuantityNameTranslation(const QString &quantityName);
    QString _GetPortName(int port);
    void _HideAllOptional();
    void _SetHorizontalChannel(Measurement *m);
    void _ConnectCurrentValueChange();
    void _DisconnectCurrentValueChange();
    virtual bool BeforeAccept();
    virtual void BeforeReject();

    QVector<Measurement *> m_measurements;
    GraphicsContainer *m_graphicsContainer;
    ChannelProxyBase *m_channelProxy;
    ChannelProxyBase * m_originalProxy;
    LineEdit *m_currentValueControl;
    LineEdit * m_name;
	LineEdit * m_units;
    bases::ComboBox * m_sourceMeasurementCombo;
    bases::ComboBox * m_sourceChannelCombo;
    bases::ComboBox * m_shapeComboBox;
    bases::ComboBox * m_axisComboBox;
    bases::ComboBox * m_style;
    bases::ComboBox * m_timeUnits;
    bases::ComboBox * m_format;
    bases::ComboBox * m_penStyle;
    bases::ComboBox * m_sensorQuantityComboBox;
    bases::ComboBox * m_sensorNameComboBox;
    bases::ComboBox * m_sensorPortComboBox;
    bases::ComboBox * m_correctionComboBox;
    LineEdit * m_correctionPoint1Orig;
    LineEdit * m_correctionPoint1New;
    LineEdit * m_correctionPoint2Orig;
    LineEdit * m_correctionPoint2New;
    PushButton *m_originlValue;
    PushButton *m_naValue;

    bool m_currentValueChanged;
    double m_currentValue;
    hw::SensorManager *m_sensorManager;
    hw::ValueCorrection *m_valueCorrection;
public:
    ChannelSettings(QVector<Measurement *> measurements,
        GraphicsContainer *graphicsContainer,
        ChannelProxyBase *channelProxy,
        hw::SensorManager *sensorManager);
    GraphicsContainer *GetGraphicsContainer();
signals:

private slots:
    void axisChanged(int index);
    void styleChanged(int index);
    void currentValueChanged(QString const &content);
    void setOriginalValue(bool checked);
    void setNaValue(bool);
    void fillChannelCombos(int measurementComboIndex);
    void loadFromOriginalWidget(int channelComboIndex);
    void sensorQuantityChanged(int index);
    void sensorNameChanged(int index);
    void sensorPortChanged(int index);
    void correctionTypeChanged(int index);
    void correctionVariableChanged(QString newValue);
};

#endif // CHANNELSETTINGS_H
