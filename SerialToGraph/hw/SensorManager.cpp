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
