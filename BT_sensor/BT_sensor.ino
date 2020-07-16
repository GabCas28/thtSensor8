#include <ArduinoBLE.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <string>

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme;

BLEService sensorService("b4a31aa5-7163-4dcb-8639-92b898970df2"); // BLE Sensor Service

// BLE Service Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEIntCharacteristic tempCharacteristic("2A3C", BLERead | BLENotify);
BLEIntCharacteristic pressCharacteristic("2A6D", BLERead | BLENotify);
BLEIntCharacteristic altCharacteristic("2AB3", BLERead | BLENotify);
BLEIntCharacteristic humidCharacteristic("2A6F", BLERead | BLENotify);

unsigned long delayTime = 1000;
unsigned long oldTime;

void setup() {
  Serial.begin(9600);
  Serial.println(F("BME280 test"));
  //

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    while (1);
  }

  //begin initalization
  if (!bme.begin()) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("BEST SENSOR EVERRRRR");
  BLE.setAdvertisedService(sensorService);

  // set Characteristics to the service
  sensorService.addCharacteristic(tempCharacteristic);
  sensorService.addCharacteristic(pressCharacteristic);
  sensorService.addCharacteristic(altCharacteristic);
  sensorService.addCharacteristic(humidCharacteristic);

  BLE.addService(sensorService);

  BLE.advertise();

  Serial.println("BLE Sensor Peripheral");
  
}

void loop() {
  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    //To measure time we need to store time from start
    oldTime = millis();
    
    // while the central is still connected to peripheral:
    while (central.connected()) {
      //This if will happen only when 1s has passed
      if (millis() - oldTime == delayTime) {
        //Creting variables for the sensor values and multiplying by 100 to create more precise results. Also Serial printOUT
        float lampo = bme.readTemperature() * 100;
        float presure = bme.readPressure() * 100;
        float humid = bme.readHumidity() * 100;
        float alti = bme.readAltitude(SEALEVELPRESSURE_HPA) * 100;
        String tempData= "Sending data: \n" + String(lampo) + " *C\n" + String(presure) + "hPa \n" + String(humid) + "% \n" + String(alti) + " m";
        Serial.println(tempData);

        //Writing values into the Characteristics
        tempCharacteristic.writeValue(lampo);
        pressCharacteristic.writeValue(presure);
        humidCharacteristic.writeValue(humid);
        altCharacteristic.writeValue(alti);
        //Storing time
        oldTime = millis();
      }
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}
