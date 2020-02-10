#include "SensorManager.h"
#include <hw/Sensor.h>
#include <hw/SensorComponent.h>
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


    QString SensorManager::_GetSensorNameTranslation(QString const &sensorName)
    {
        //TODO: TFs: for sensor name translation only
        if (sensorName == "")
            return "";
        if (sensorName == "Generic (AD 10bit)")
            return tr("Generic (AD 10bit)");
        if (sensorName == "Thermometer (DS18B20)")
            return tr("Thermometer (DS18B20)");
        if (sensorName == "Pressure + Thermometer + Hygrometer (BME280)")
            return tr("Pressure + Thermometer + Hygrometer (BME280)");
        if (sensorName == "Thermometer (K-Type, MAX6675)")
            return tr("Thermometer (K-Type, MAX6675)");
        if (sensorName == "Accelerometer + Magnetometer (LSM303DLHC)")
            return tr("Accelerometer + Magnetometer (LSM303DLHC)");
        if (sensorName == "IR Thermometer (MLX90614)")
            return tr("IR Thermometer (MLX90614)");
        if (sensorName == "Pressure sensor (MPX5700DP)")
            return tr("Pressure sensor (MPX5700DP)");
        if (sensorName == "Force sensor (HX711)")
            return tr("Force sensor (HX711)");
        if (sensorName == "EKG sensor (AD8232)")
            return tr("EKG sensor (AD8232)");
        if (sensorName == "Color sensor (TCS3200)")
            return tr("Color sensor (TCS3200)");
        if (sensorName == "UV sensor (GUVA-S12SD)")
            return tr("UV sensor (GUVA-S12SD)");
        if (sensorName == "UV sensor (VEML6070)")
            return tr("UV sensor (VEML6070)");
        if (sensorName == "pH sensor")
            return tr("pH sensor");
        if (sensorName == "Conductivity sensor")
            return tr("Conductivity sensor");
        if (sensorName == "DC Voltage sensor 0-25 V (voltage divider)")
            return tr("DC Voltage sensor 0-25 V (voltage divider)");
        if (sensorName == "AC/DC Current sensor 0-5 A (ACS712)")
            return tr("AC/DC Current sensor 0-5 A (ACS712)");
        if (sensorName == "AC/DC Current sensor 0-30 A (ACS712)")
            return tr("AC/DC Current sensor 0-30 A (ACS712)");
        if (sensorName == "Voltage + Current sensor (HX711)")
            return tr("Voltage + Current sensor (HX711)");
        if (sensorName == "Light sensor (photoresistor)")
            return tr("Light sensor (photoresistor)");
        if (sensorName == "Light sensor (BH1750 luxmeter)")
            return tr("Light sensor (BH1750 luxmeter)");
        if (sensorName == "CO2 gas sensor (MH-Z16)")
            return tr("CO2 gas sensor (MH-Z16)");
        if (sensorName == "O2 gas sensor (ME2-O2)")
            return tr("O2 gas sensor (ME2-O2)");
        if (sensorName == "Ultrasonic motion sensor (SR-HC04)")
            return tr("Ultrasonic motion sensor (SR-HC04)");
        if (sensorName == "Heart rate sensor (simple)")
            return tr("Heart rate sensor (simple)");
        if (sensorName == "Magnetic field sensor")
            return tr("Magnetic field sensor");
        if (sensorName == "Thermometer + Hygrometer (DHT11)")
            return tr("Thermometer + Hygrometer (DHT11)");
        if (sensorName == "Alcohol gas sensor (MQ-3)")
            return tr("Alcohol gas sensor (MQ-3)");
        if (sensorName == "Particle/Dust sensor (DSM501)")
            return tr("Particle/Dust sensor (DSM501)");
        if (sensorName == "Sound sensor (microphone)")
            return tr("Sound sensor (microphone)");
        if (sensorName == "Laser ranging sensor (VL53L0X)")
            return tr("Laser ranging sensor (VL53L0X)");
        if (sensorName == "Timer GPIO 02")
            return tr("Timer GPIO 02");
        qWarning() << QString("unsupported sensor name %1").arg(sensorName);
        return sensorName;
    }

    void SensorManager::_PrepareMinimalSensorSet()
    {
        m_quantities.push_back(new SensorQuantity(this, 0, ""));
        m_sensors.push_back((new Sensor(this, "", 0))->AddComponent(new SensorComponent(this, m_quantities.at(0), "", 0)));
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
        //unsigned jsonVersion = object["version"].toInt();
        //Q_UNUSED(jsonVersion)

        QJsonArray jsonQuantities = object["quantities"].toArray();
        foreach (QJsonValue const &quantityValue, jsonQuantities)
        {
            QJsonObject quantityObject = quantityValue.toObject();

            unsigned quantityId = quantityObject["id"].toInt();
            QString quantityName = quantityObject["name"].toString();

            m_quantities.push_back(new SensorQuantity(this, quantityId, quantityName));
        }

        QJsonArray jsonSensors = object["sensors"].toArray();
        foreach (QJsonValue const &sensorValue, jsonSensors)
        {
            QJsonObject sensorObject = sensorValue.toObject();

            unsigned sensorId = unsigned(sensorObject["id"].toInt());
            QString sensorName = _GetSensorNameTranslation(sensorObject["name"].toString());
            Sensor *sensor = new Sensor(this, sensorName, sensorId);
            QJsonArray jsonSensorComponents = sensorObject["components"].toArray();
            int order = -1;
            foreach (QJsonValue const &sensorValue, jsonSensorComponents)
            {
                order++;
                QJsonObject componentObject = sensorValue.toObject();
                QString correction = componentObject["correction"].toString();
                QString unit = componentObject["unit"].toString();
                QString quantity_name = componentObject["quantity"].toString();
                SensorQuantity * quantity = GetSensorQuantity(quantity_name);
                if (quantity == nullptr)
                {
                    MyMessageBox::warning(nullptr, QString("unexpected quantity '%1' in sensors.json file").arg(quantity_name));
                    continue;
                }
                sensor->AddComponent(new SensorComponent(this, quantity, unit, unsigned(order)));
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

    SensorComponent *SensorManager::GetSensorComponent(Sensor *sensor, unsigned quantityId)
    {
        foreach (SensorComponent *component, sensor->GetComponents())
        {
            if (component->GetQuantity()->GetId() == quantityId)
                return component;
        }

        qWarning() << "Sensor quantity has not been found by id " << quantityId;
        return nullptr;
    }

    SensorComponent *SensorManager::GetSensorComponent(Sensor *sensor, QString const &quantityName)
    {
        foreach (SensorComponent *component, sensor->GetComponents())
        {
            if (component->GetQuantity()->GetName() == quantityName)
                return component;
        }

        qWarning() << "Sensor Component has not been found by name " << quantityName;
        return nullptr;
    }

    SensorQuantity *SensorManager::GetSensorQuantity(QString const &quantityName)
    {
        foreach (SensorQuantity *quantity, m_quantities)
        {
            if (quantity->GetName() == quantityName)
                return quantity;
        }

        qWarning() << "Sensor Quantity has not been found by name " << quantityName;
        return nullptr;
    }
}
