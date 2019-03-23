
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

#define REPORTING_PERIOD_MS     60000

PulseOximeter pox;

uint32_t tsLastReport = 0;

float filterweight=0.5;
int average_beat=0;
int average_SpO2=0;

void onBeatDetected()
{
//    Serial.println("Beat!");
}

void setup()
{
    Serial.begin(9600);

    Serial.print("Initializing pulse oximeter..");

    if (!pox.begin()) {
        Serial.println("FAILED");
        for(;;);
    } else {
        Serial.println("SUCCESS");
    }

    // Register a callback for the beat detection
    pox.setOnBeatDetectedCallback(onBeatDetected);
}

void calculate_average(int beat, int SpO2)  
{

    if (beat>30 and beat<220 and SpO2>50) {
        average_beat = filterweight * (beat) + (1 - filterweight ) * average_beat;
        average_SpO2 = filterweight * (SpO2) + (1 - filterweight ) * average_SpO2;
    }
        
}

void loop()
{
      pox.update();

      if (millis() - tsLastReport > REPORTING_PERIOD_MS ) {
        calculate_average(pox.getHeartRate(),pox.getSpO2());

          Serial.print(average_beat);
          Serial.print(",");

          Serial.print(average_SpO2);
          Serial.print(";");
        
        tsLastReport = millis();
      } else {
        calculate_average(pox.getHeartRate(),pox.getSpO2()); 
      }      

}
