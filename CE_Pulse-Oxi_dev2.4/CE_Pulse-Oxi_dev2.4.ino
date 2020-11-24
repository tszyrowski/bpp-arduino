/* This program is free software: you can redistribute it and/or modify it under 
 *  the terms of the GNU General Public License as published by the Free Software 
 *  Foundation, either version 3 of the License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
 *  See the GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License along with this program.  
 *  If not, see <http://www.gnu.org/licenses/>.
 *  Copyright 2020,CEREBRUM srl
 *  
 *  More details by CEREBRUM Srl
 *  www.cerebrum.it
 *  ITALY
 *  
 *  Please consider that this application, sensors and functional device are NOT tested for 
 *  medical purpose and the single parts aren't calibrated and aren't certified. 
 *  Please use this simple device for preventive scope and to monitorate patients only under 
 *  emergency situation when no other medical deviced and Pulse-oximeters are available.
 *  
 *  CEREBRUM-oximeter dev. 2.4 - version 1.1   // April, 6 2020
 *  
 *  Arduino NANO - 3,3v | i2c A4 (SDA) , A5 (SCL)
 *  Arduino NANO Every - 3,3v | i2c A4 (SDA) , A5 (SCL)
 *  Arduino MICRO - 3,3v | i2c 2 (SDA), 3 (SCL)
 *  
 *  MAX30100 - PulseOximeter board (+3.3v | GND | SCA/SCL)
 *  OLED SSD1306 128x32 (+3.3v | GND | SCA/SCL)
*/

#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include "MAX30100.h"
#include <U8g2lib.h>
 
#define REPORTING_PERIOD_MS      1000
#define PULSE_WIDTH              MAX30100_SPC_PW_1600US_16BITS
#define IR_LED_CURRENT           MAX30100_LED_CURR_40MA  
#define LED_CURRENT              MAX30100_LED_CURR_20_8MA 
#define SAMPLING_RATE            MAX30100_SAMPRATE_100HZ

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);
PulseOximeter pox;
MAX30100 sensor;

uint32_t tsLastReport = 0;
uint32_t last_beat=0;
bool initialized=false;
int HRclean;
int SpO2;
 
void onBeatDetected()
{
  show_beat();
  last_beat=millis();
}

void show_beat() 
{
  u8g2.setFont(u8g2_font_cursor_tr);
  u8g2.setCursor(118,10);
  u8g2.print("_");
  u8g2.sendBuffer();
}

void initial_display() 
{
  if (not initialized) 
  {
    u8g2.clearBuffer();
    u8g2.setCursor(15,12);
    u8g2.setFont(u8g2_font_crox2hb_tr); 
    u8g2.print("CEREBRUM.it"); 
    u8g2.setFont(u8g2_font_crox2h_tr);
    u8g2.setCursor(30,29);
    u8g2.print("Initializing...");
    u8g2.sendBuffer(); 
    delay(4000);
   
    initialized=true;

      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_crox2hb_tr); 
        if (!pox.begin()) {
          u8g2.setCursor(40,12);
          u8g2.print("FAILED");
          u8g2.setCursor(15,29);
          u8g2.print("Check Sensor !");
          u8g2.sendBuffer(); 
          for(;;);
        } else {
          u8g2.setCursor(20,12);
          u8g2.print("INITIALIZED");
          u8g2.setCursor(0,29);
          u8g2.print("Wear the Sensor...");
          u8g2.sendBuffer(); 
        }
     delay(2000);
  }
}

void setup()
{
    u8g2.begin();
    initial_display();
    pox.begin();

    pox.setOnBeatDetectedCallback(onBeatDetected); 
    pox.setIRLedCurrent(LED_CURRENT);  
    sensor.setMode(MAX30100_MODE_SPO2_HR);
    sensor.setLedsPulseWidth(PULSE_WIDTH);
    sensor.setSamplingRate(SAMPLING_RATE);
}
 
void loop()
{
    pox.update();
    HRclean = pox.getHeartRate();
    SpO2 = pox.getSpO2();
    if ((millis() - tsLastReport > REPORTING_PERIOD_MS) and (HRclean>30 and HRclean<220 and SpO2>30 and SpO2<100)) {
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_crox2h_tr);
      u8g2.setCursor(0,12);  
      u8g2.print("HR");
      u8g2.setCursor(75,12);  
      u8g2.print("Bpm");
      u8g2.setCursor(0,30);
      u8g2.print("SpO2 ");
      u8g2.setCursor(75,30);
      u8g2.print("%"); 
      u8g2.setFont(u8g2_font_fub11_tf); 
      u8g2.setCursor(45,12);  
      u8g2.print(HRclean);
      u8g2.setCursor(45,30);  
      u8g2.print(SpO2);
      u8g2.setFont(u8g2_font_cursor_tr);
      u8g2.setCursor(118,10);
      u8g2.print("^");
      u8g2.sendBuffer();

     tsLastReport = millis();      
    }
}
