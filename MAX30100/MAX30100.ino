
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

#include <SPI.h>
#include <SD.h>

File myFile;


#define REPORTING_PERIOD_MS     600

PulseOximeter pox;

uint32_t tsLastReport = 0;

float filterweight = 0.5;
int average_beat = 0;
int average_SpO2 = 0;

void onBeatDetected()
{
  //    Serial.println("Beat!");
}

void setup()
{
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Initializing sdCard..");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("Initializing pulse oximeter..");

  if (!pox.begin()) {
    Serial.println("Initializing pulse oximeter failed");
    while (1);
  }
  myFile = SD.open("readings.txt", FILE_WRITE);
  if (myFile) {
  
    myFile.println("test");
    myFile.close();
  myFile = SD.open("readings.txt");
    Serial.print(myFile.read());
    myFile.close();
  } else {
    Serial.println("err");
  }
  // Register a callback for the beat detection
  pox.setOnBeatDetectedCallback(onBeatDetected);
}

void calculate_average(int beat, int SpO2)
{

  if (beat > 30 and beat<220 and SpO2>50) {
    average_beat = filterweight * (beat) + (1 - filterweight ) * average_beat;
    average_SpO2 = filterweight * (SpO2) + (1 - filterweight ) * average_SpO2;
  } else if (beat == 0 || SpO2 == 0) {
    average_beat = 0;
    average_SpO2 = 0;

  }

}

void loop()
{
  pox.update();

  if (millis() - tsLastReport > REPORTING_PERIOD_MS ) {
    calculate_average(pox.getHeartRate(), pox.getSpO2());

    //    Serial.print(average_beat);
    //    Serial.print(",");
    //
    //    Serial.print(average_SpO2);
    //    Serial.print(";");

    tsLastReport = millis();
  } else {
    calculate_average(pox.getHeartRate(), pox.getSpO2());
  }

}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char y = (char)Serial.read();
    Serial.print(String(y));
    if (String(y) == "read") {
      myFile = SD.open("readings.txt");
      if (myFile) {
        Serial.write(myFile.read());
        myFile.close();
      }
    }
  }
}
