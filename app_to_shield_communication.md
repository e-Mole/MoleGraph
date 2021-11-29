App To Shield Communication
===========================

In this file is described communication between two MoleGraph modules PC/mobile app (App) and HW shield (Shield) providing measured data. The description relates with the protocol version *ATG_5*.

Available Channels
======================

Communication can be established via
* Arduino USB connector (over USB to serial protocol)
* Included Bluetooth module (over RFCOMM protocol)

Communication Procedure
=======================

Originally the communication was based on the initiator/follower model, where the App initiate the communication. This approach had one disadvantage - it was necessary to perform measurement with the Shield and operate App at the same time. It was uncomfortable for some measurement types. Therefore the ability to initiate communication by Shield has been added. A measurement can be started or terminated remotely. It brings a possibility to provide Shield debug messages when necessary.

App/shield communication usually follow these steps:

1. A shield port is identified
2. Appropriate connection channel is opened
3. Measurement parameters are provided to from App to Shield
4. Measurement is started
5. Measured value set is provided
6. Measurement is terminated
7. Measurement can be repeated by performance steps 3. - 6.

Measurement can be performed in two modes (aka. measurement types)
* Periodical where are data provided in previously defined intervals
* On demand - when the user ask for values. The measurement can be triggered from the App or remotely from the Shield.

The periodical measurement can be defined as
* Time between samples (used for times >= 1 sec)
* Frequency (used for times <= 1 sec)

this approach is chosen to be possible to define measurement period as integer.

During the preparation phase
* Is asked the Shield to initialize
* Is set measurement mode
* In case of periodical measurement is defined frequency or time
* Are defined channels that should provide measured data
* Are defined quantities that should be measured if particular sensors are explicitly defined.

Once the measurement is started measurement module provides data on demand or spontaneously in defined intervals in base of pre-set mode.  Measurement is performed until the measurement is terminated from App or Shield.

Message Structure
=================

Common Structure
-----------------

Each message starts by a header and is terminated by a checksum byte.
Between the header and the checksum cam be placed a payload defined by data provided in header.

| 1 byte | X bytes | 1byt     |
|--------|---------|----------|
| header | payload | checksum |

Header is usually an instruction ID only but can contain a mixture of extra data as well.

Checksum is calculated from the whole message without the checksum byte itself. Checksum is calculated as a count of positive bites modulo 255 (bites added to unsigned byte).

Messages Initiated by App
-------------------------

| 1 byte                  | X bytes | 1 byte   |
|-------------------------|---------|----------|
| header (Instruction ID) | payload | checksum |

The app provides instructions only to the Shield.

Supported App to Shieled instructions:

* **INS_GET_VERSION** (0x01) - answer with a communication protocol version supported by Shield is expected. This instruction have no payload.

* **INS_SET_TIME** (0x02) - with sampling time of the following  measurement as the payload (big-endian uint16).

* **INS_SET_FREQUENCY** = (0x03) - with frequency of the following measurement as the payload (big-endian uint16).

* **INS_ENABLED_CHANNELS** = (0x04) - with set of channels activated for the following measurement (1 byte - each channel one bit, 1 means enabled, 0 means disabled, channel 1 is represented by the lowest bit channel 8 by highest bit)

* **INS_START** (0x05) - starting the measurement. This instruction has no payload.

* **INS_STOP** (0x06) - terminating the measurement. This instruction has no payload.

* **INS_SET_TYPE** (0x07) - this instruction determines whether samples will be provided periodically or on-demand (1 byte where 0 means Periodical and 1 means OnDeman)

* **INS_GET_SAMLPE** (0x08) - asks for one set of values during the on-demand measurement. This instruction has no payload.

* **INS_PAUSE** (0x09) - suspends running measurement. Samples are not provided in case of the periodical measurement. This instruction has no payload.

* **INS_CONTINUE** (0x0A) - resumes the suspended measurement. This instruction has no payload.

* **INS_INITIALIZE** (0x0B) - asks Shield to prepare for next measurement (to remove all cached data and/or initialize sensors). This instruction has no payload.

* **INS_SET_SENSOR** (0x0C) - asks shield to set a sensor to required "mode" to be provided expected data or expected data format. This instruction is followed by the payload in format:
  * **hwIndex** - defines index of assigned channel
  * **port** - defines index of Shield port (0-3)
  * **sensorId** - defines id of a sensor type
  * **quantityId** - defines id of a quantity
  * **quantityOrder** - defines order of the quantity defined for appropriate sensorId

Note: **sensorId**, **quantityId** and **quanitityOrder** are defined in file MoleGraph/*SerialToGraph/sensors/sensors.json*


Messages Initiated by Shield
----------------------------

| 1 byte           | X bytes | 1byt     |
|------------------|---------|----------|
| Header (mixture) | payload | checksum |

Where:

* Header is mixture of
  * **Missed sample** flag informing that reading speed of the app is not sufficient and one or more measured samples has/have been lost.
  * **Instruction** Shield initiates following instructions:
    * **INS_NONE** (0x00) - followed by measured data
    * **INS_START** (5x05) - starts measurement remotely. This instruction has no payload.
    * **INS_STOP** (0x06) - terminates measurement. This instruction has no payload.
    * **DEBUG** (0x7F) - followed by 1 byte message length and a debug message (debug message size is limited to 255 bytes).

| 1 highest bit | 7 lower bites  |
|---------------|----------------|
| missed_sample | Instruction Id |

Measured Data
-------------

Data message header consists of the *missed sample* flag and *INS_NONE*
Length of message is defined by the channel count.

Periodical:

|1 byte           | 1 float * channel count | 1byte    |
|-----------------|-------------------------|----------|
|Header (mixture) | channel values          | checksum |


On-demand:

|1 byte           | 1 float   | 1 float * channel count | 1byte    |
|-----------------|-----------|-------------------------|----------|
|Header (mixture) | timestamp | channel values          | checksum |


The **timestamp** is a time from the measurement start in seconds (4 bytes big endian float number)

* **Channel values** is set measured data for required channels (defined by the INS_ENABLED_CHANNELS instruction) in order *Channel 1* - *Channel 8*. No data for unsolicited channels are present in the payload.