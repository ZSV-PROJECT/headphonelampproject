#include <CapacitiveSensor.h>
#include <Adafruit_NeoPixel.h>
#include "RTClib.h"
#include "SevSeg.h"

//Sevseg
SevSeg sevseg;

//RTClib
RTC_DS3231 rtc;
int theTime;

//CapacitiveSensor
CapacitiveSensor   cs_4_2 = CapacitiveSensor(24, 22); // 1M resistor between pins 4 & 2, pin 2 is sensor pin
long total1 =  cs_4_2.capacitiveSensor(30);

//Neopixel
int lights = 64;
int pin = 32;
boolean on, debounce;
Adafruit_NeoPixel strip (lights, pin);

//Create a stored colour
uint32_t red = strip.Color(255, 0, 0);
uint32_t green = strip.Color(0, 255, 0);
uint32_t blue = strip.Color(0, 0, 255);
uint32_t white = strip.Color(255, 255, 255);

////////////////////////////////////////////////////////////

void setup() {
  byte numDigits = 4;
  byte digitPins[] = {10, 11, 12, 13};
  byte segmentPins[] = {2, 3, 4, 5, 6, 7, 8, 9};
  bool resistorsOnSegments = false; // 'false' means resistors are on digit pins
  byte hardwareConfig = COMMON_CATHODE; // See README.md for options
  bool updateWithDelays = false; // Default 'false' is Recommended
  bool leadingZeros = false; // Use 'true' if you'd like to keep the leading zeros
  bool disableDecPoint = false; // Use 'true' if your decimal point doesn't exist or isn't connected

  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
               updateWithDelays, leadingZeros, disableDecPoint);
  sevseg.setBrightness(90);

  /////////////

  cs_4_2.set_CS_AutocaL_Millis(0xFFFFFFFF);
  strip.clear();
  strip.begin();
  strip.setBrightness(150);
  on = false;
  debounce = false;

  Serial.begin(57600);

#ifndef ESP8266
  while (!Serial);
#endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }


}

void loop() {
  static unsigned long timer = millis();
  static int deciSeconds = 0;
  total1 =  cs_4_2.capacitiveSensor(30);
  lightController();

  DateTime now = rtc.now();
  deciSeconds = (now.hour() * 100) + now.minute();

  if (millis() - timer >= 100) {
    timer += 100;
    //Serial.println(deciSeconds);
    sevseg.setNumber(deciSeconds, 2);
  }

  sevseg.refreshDisplay();


}

//////////////////////////////////////////



void lightController()
{
  Serial.println(total1);
  //Check if touched: on
  if (total1 > 1000 && !on && !debounce)
  {
    for (int x = 0; x <= strip.numPixels(); x++)
    {
      strip.setPixelColor(x, white);
    }
    strip.show();
    Serial.println("Lamp turned on");
    on = true;
    debounce = true;
  }

  //Debounce
  if (total1 < 1000)
    debounce = false;

  //Check if touched: off
  if (total1 > 1000 && on && !debounce)
  {
    for (int x = 0; x <= strip.numPixels(); x++)
    {
      strip.setPixelColor(x, 0);
    }
    strip.show();
    strip.clear();
    Serial.println("Lamp turned off");
    on = false;
    debounce = true;
  }
}

