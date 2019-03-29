
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

#include <SPI.h>
#include <SD.h>

File myFile;


#define REPORTING_PERIOD_MS     60000

uint32_t tsLastReport = 0;

float filterweight = 0.5;
int average_beat = 0;
int average_SpO2 = 0;

bool isDataSendingStarted = true;

PulseOximeter pox;

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

  Serial.println("sd_i");

  if (!SD.begin(4)) {
    Serial.println("sd_i_f");
    while (1);
  }
  Serial.println("p_o_i");

  if (!pox.begin()) {
    Serial.println("p_o_i_f");
    while (1);
  }

  // Register a callback for the beat detection
  pox.setOnBeatDetectedCallback(onBeatDetected);
}

void calculate_average(int beat, int SpO2)
{

  if (beat > 30 and beat<220 and SpO2>50) {
    average_beat = filterweight * (beat) + (1 - filterweight) * average_beat;
    average_SpO2 = filterweight * (SpO2) + (1 - filterweight) * average_SpO2;
  } else if (beat == 0 || SpO2 == 0) {
    average_beat = 0;
    average_SpO2 = 0;

  }

}

void loop() {

  pox.update();

  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    calculate_average(pox.getHeartRate(), pox.getSpO2());
    myFile = SD.open("readings.txt", FILE_WRITE);

    myFile.print(average_beat);
    myFile.print(",");
    myFile.print(average_SpO2);
    myFile.print(";");

    if (!isDataSendingStarted) {
      Serial.print(average_beat);
      Serial.print(",");

      Serial.print(average_SpO2);
      Serial.print(";");
    }

    myFile.close();

    tsLastReport = millis();
  } else {
    calculate_average(pox.getHeartRate(), pox.getSpO2());
  }

}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char y = Serial.read();
    if (y == '1') {
      myFile = SD.open("readings.txt");
      
      if (myFile) {
       
        isDataSendingStarted = true;
        
        while (myFile.available()) {
          Serial.write(myFile.read());
        }
        
        myFile.close();
        
      } else {
        
        myFile.close();
        
      }
      
    } else if (y == '2') {
      
      myFile = SD.open("readings.txt");

      Serial.println(y);
      
      if (myFile) {
        
        myFile.close();
        SD.remove("readings.txt");
        
      } else {
        
        myFile.close();
        
      }
    }
  }
}
