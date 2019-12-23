#include "SensorManager.h"
#include <hw/Sensor.h>
#include <hw/SensorQuantity.h>
#include <MyMessageBox.h>
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#define JSON_FILE_NAME ":/sensors/sensors.json"
namespace hw
{
    SensorManager::SensorManager(QObject *parent):
        QObject(parent)
    {
        _InitializeSensors();
    }


    void SensorManager::_PrepareMinimalSensorSet()
    {
        m_quantities.push_back(new SensorQuantity(this, 0, ""));
        m_sensors.push_back((new Sensor(this, "", 0))->AddQuantity(GetSensorQuantity("")));
    }

    void SensorManager::_InitializeSensors()
    {
        QString jsonData = _GetSensorsFileContent();
        if (jsonData.isEmpty())
        {
            //TODO: this code is probably never reached because sensors are in resources now
            _PrepareMinimalSensorSet();
        }
        QJsonDocument document = QJsonDocument::fromJson(jsonData.toUtf8());
        QJsonObject object = document.object();
        unsigned jsonVersion = object["version"].toInt();
        Q_UNUSED(jsonVersion)

        QJsonArray jsonQuantities = object["quantities"].toArray();
        foreach (QJsonValue const &quantityValue, jsonQuantities)
        {
            QJsonObject quantityObject = quantityValue.toObject();

            unsigned quantityId = quantityObject["quantityId"].toInt();
            QString quantityName = quantityObject["quantityName"].toString();

            m_quantities.push_back(new SensorQuantity(this, quantityId, quantityName));
        }

        QJsonArray jsonSensors = object["sensors"].toArray();
        foreach (QJsonValue const &sensorValue, jsonSensors)
        {
            QJsonObject sensorObject = sensorValue.toObject();

            unsigned sensorId = sensorObject["sensorId"].toInt();
            QString sensorName = sensorObject["sensorName"].toString();


            //TODO: TFs: for sensor name translation only
            if (sensorName == "Generic (AD 10bit)")
                sensorName = tr("Generic (AD 10bit)");
            if (sensorName == "Thermometer (DS18B20)")
                sensorName = tr("Thermometer (DS18B20)");
            if (sensorName == "Pressure + Thermometer + Hygrometer (BME280)")
                sensorName = tr("Pressure + Thermometer + Hygrometer (BME280)");
            if (sensorName == "Accelerometer + Magnetometer (LSM303DLHC)")
                sensorName = tr("Accelerometer + Magnetometer (LSM303DLHC)");
            if (sensorName == "IR Thermometer (MLX90614)")
                sensorName = tr("IR Thermometer (MLX90614)");
            if (sensorName == "Pressure sensor (MPX5700DP)")
                sensorName = tr("Pressure sensor (MPX5700DP)");
            if (sensorName == "Force sensor (HX711)")
                sensorName = tr("Force sensor (HX711)");
            if (sensorName == "EKG sensor (AD8232)")
                sensorName = tr("EKG sensor (AD8232)");
            if (sensorName == "Color sensor (TCS3200)")
                sensorName = tr("Color sensor (TCS3200)");
            if (sensorName == "UV sensor (GUVA-S12SD)")
                sensorName = tr("UV sensor (GUVA-S12SD)");
            if (sensorName == "UV sensor (VEML6070)")
                sensorName = tr("UV sensor (VEML6070)");
            if (sensorName == "pH sensor")
                sensorName = tr("pH sensor");
            if (sensorName == "Conductivity sensor")
                sensorName = tr("Conductivity sensor");
            if (sensorName == "DC Voltage sensor 0-25 V (voltage divider)")
                sensorName = tr("DC Voltage sensor 0-25 V (voltage divider)");
            if (sensorName == "AC/DC Current sensor 0-5 A (ACS712)")
                sensorName = tr("AC/DC Current sensor 0-5 A (ACS712)");
            if (sensorName == "AC/DC Current sensor 0-30 A (ACS712)")
                sensorName = tr("AC/DC Current sensor 0-30 A (ACS712)");
            if (sensorName == "Voltage + Current sensor (HX711)")
                sensorName = tr("Voltage + Current sensor (HX711)");
            if (sensorName == "Light sensor (photoresistor)")
                sensorName = tr("Light sensor (photoresistor)");
            if (sensorName == "Light sensor (BH1750 luxmeter)")
                sensorName = tr("Light sensor (BH1750 luxmeter)");
            if (sensorName == "CO2 gas sensor (MH-Z16)")
                sensorName = tr("CO2 gas sensor (MH-Z16)");
            if (sensorName == "O2 gas sensor (ME2-O2)")
                sensorName = tr("O2 gas sensor (ME2-O2)");
            if (sensorName == "Ultrasonic motion sensor (SR-HC04)")
                sensorName = tr("Ultrasonic motion sensor (SR-HC04)");
            if (sensorName == "Heart rate sensor (simple)")
                sensorName = tr("Heart rate sensor (simple)");
            if (sensorName == "Magnetic field sensor")
                sensorName = tr("Magnetic field sensor");
            if (sensorName == "Thermometer + Hygrometer (DHT11)")
                sensorName = tr("Thermometer + Hygrometer (DHT11)");
            if (sensorName == "Alcohol gas sensor (MQ-3)")
                sensorName = tr("Alcohol gas sensor (MQ-3)");
            if (sensorName == "Particle/Dust sensor (DSM501)")
                sensorName = tr("Particle/Dust sensor (DSM501)");
            if (sensorName == "Sound sensor (microphone)")
                sensorName = tr("Sound sensor (microphone)");
            if (sensorName == "Laser ranging sensor‎ (VL53L0X)")
                sensorName = tr("Laser ranging sensor‎ (VL53L0X)");
            if (sensorName == "Timer")
                sensorName = tr("Timer");


            //sensorName = tr("Thermometer (DS18B20)");

            Sensor *sensor = new Sensor(this, sensorName, sensorId);
            QJsonArray jsonSensorQuantities = sensorObject["quantities"].toArray();
            foreach (QJsonValue const &sensorValue, jsonSensorQuantities)
            {
                sensor->AddQuantity((sensorValue.toInt(-1) != -1) ?
                    GetSensorQuantity(sensorValue.toInt()) : GetSensorQuantity(sensorValue.toString()));
            }

            m_sensors.push_back(sensor);
        }
    }

    QString SensorManager::_GetSensorsFileContent()
    {
        QString content;
        QFile jsonFile;
        jsonFile.setFileName(JSON_FILE_NAME);
        if (!jsonFile.exists())
        {
            qWarning() << "Sensor file has not been found";
            return "";
        }
        jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
        content = jsonFile.readAll();
        jsonFile.close();
        return content;
    }

    std::vector<Sensor *> const SensorManager::GetSensors() const
    {
        return m_sensors;
    }

    Sensor *SensorManager::GetNoneSensor()
    {
        return m_sensors.front();
    }

    Sensor *SensorManager::GetSensor(unsigned sensorId)
    {
        foreach (Sensor *sensor, m_sensors)
        {
            if (sensor->GetId() == sensorId)
            {
                return sensor;
            }
        }
        qWarning() << "SensorId has bot been found in the sensor collection";
        return NULL;
    }

    SensorQuantity *SensorManager::GetSensorQuantity(unsigned quantityId)
    {
        foreach (SensorQuantity *quantity, m_quantities)
        {
            if (quantity->GetId() == quantityId)
                return quantity;
        }

        qWarning() << "Sensor Quantity has not been found by id " << quantityId;
        return NULL;
    }

    SensorQuantity *SensorManager::GetSensorQuantity(QString const &quantityName)
    {
        foreach (SensorQuantity *quantity, m_quantities)
        {
            if (quantity->GetName() == quantityName)
                return quantity;
        }

        qWarning() << "Sensor Quantity has not been found by name " << quantityName;
        return NULL;
    }
}
