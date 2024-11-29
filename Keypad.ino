//LIBRARY
#include <Keypad.h>
#include <ZMPT101B.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <PZEM004Tv30.h>

//PZEM
PZEM004Tv30 pzem(Serial2);

//LCD
LiquidCrystal_I2C lcd(0x27, 20, 4);

//ZMPT SENSOR
#define SENSITIVITY 339.0f
ZMPT101B voltageSensor(A0, 50.0);

//DF ROBOT CURRENT SENSOR
const int currentSensorPin = A1; //define sensor pin
const int mVperAmp = 100; // use 185 for 5A Module, and 66 for 30A Module
float Vref  = 0; //read your Vcc voltage,typical voltage should be 5000mV(5.0V)

//KEYPAD
const byte ROWS = 4;
const byte COLS = 4;
String valueKey = "";
float value = 0.00;
float value1 = 0.00;
float value2 = 0.00; //TESTING VALUE
String charKey = "";
int kondisi = 0;
int condition = 0;
int decimal = 0;
int decimal2 = 0;
int decimal3 = 0;
float dec1;
float dec2;
float dec3;
float total1;
float total2;
float total3 = 1.00;
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

//RELAY PINOUT
#define relay1 53
#define relay2 51
#define relay3 47
#define relay4 49
#define relay5 45
#define relay6 43

void setup() {
  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(2, 1);
  lcd.print("Relay");
  lcd.setCursor(2, 2);
  lcd.print("Differential");
  delay(3000);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);
  pinMode(relay5, OUTPUT);
  pinMode(relay6, OUTPUT);
  digitalWrite(relay4, HIGH);
  digitalWrite(relay3, HIGH);
  Vref = readVref(); //read the reference votage(default:VCC)
  voltageSensor.setSensitivity(SENSITIVITY);
}

void loop() {
  char customKey = customKeypad.getKey();
  float CurrentValue =  readACCurrent(currentSensorPin);
  float v = voltageSensor.getRmsVoltage();
  float voltage = pzem.voltage();
  float current = pzem.current();
  if (kondisi == 1) {
    Serial.println(condition);
    Serial.println(total1);
    if (condition == 0) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Insert Short Circuit");
      lcd.setCursor(0, 1);
      lcd.print("Ip :" + String(total1) + "A" + " Is :" + String(total2) + "A");
      lcd.setCursor(0, 3);
      lcd.print("Mode : Simulation");
      if (total1 == total2) {
        digitalWrite(relay3, HIGH);
        digitalWrite(relay4, HIGH);
        digitalWrite(relay1, LOW);
        digitalWrite(relay2, LOW);
        digitalWrite(relay5, LOW);
        digitalWrite(relay6, LOW);
      }
      else if (total1 > total2) {
        Serial.println("RELAY PRIMER NYALA");
        lcd.setCursor(0, 2);
        lcd.print("PRIMER");
        digitalWrite(relay4, LOW);
        digitalWrite(relay3, LOW);
        digitalWrite(relay1, HIGH);
        digitalWrite(relay5, HIGH);
        digitalWrite(relay2, LOW);
        digitalWrite(relay6, LOW);
      }
      else if (total1 < total2) {
        Serial.println("RELAY SEKUNDER NYALA");
        lcd.setCursor(0, 2);
        lcd.print("SEKUNDER");
        digitalWrite(relay3, LOW);
        digitalWrite(relay4, HIGH);
        digitalWrite(relay2, HIGH);
        digitalWrite(relay1, LOW);
        digitalWrite(relay5, LOW);
        digitalWrite(relay6, HIGH);
      }
      if (customKey == 'A') {
        kondisi = 0;
        digitalWrite(relay4, HIGH);
        digitalWrite(relay3, HIGH);
        digitalWrite(relay1, LOW);
        digitalWrite(relay2, LOW);
        digitalWrite(relay5, LOW);
        digitalWrite(relay6, LOW);
      }
      else if (customKey == '*') {
        condition = 1;
        Serial.println("setting primer");
      }
      else if (customKey == '#') {
        condition = 2;
      }
    }
    else if (condition == 1) {
      if (decimal == 0) {
        Serial.println("status : BIG");
        lcd.clear();
        lcd.setCursor(0, 1);
        total1 = value1 + (dec1 / 100);
        lcd.print("IP :" + String(total1) + " A");
        if (customKey >= '0' && customKey <= '9') {
          valueKey += customKey;
          value1 = valueKey.toFloat();
          Serial.print("Current Value: ");
          Serial.println(value1);
        }
        if (customKey == 'C') {
          value1 = 0;
          valueKey = "";
        }
        if (customKey == 'B') {
          condition = 0;
        }
        if (customKey == 'D') {
          decimal = 1;
        }
      }
      else if (decimal == 1) {
        Serial.println("status : Small");
        lcd.clear();
        lcd.setCursor(0, 1);
        total1 = value1 + (dec1 / 100);
        lcd.print("IP :" + String(total1) + " A");
        if (customKey >= '0' && customKey <= '9') {
          valueKey += customKey;
          dec1 = valueKey.toFloat();
          Serial.print("Current Value: ");
          Serial.println(dec1);
        }
        if (customKey == 'C') {
          dec1 = 0;
          valueKey = "";
        }
        if (customKey == 'B') {
          condition = 0;
        }
        if (customKey == 'D') {
          decimal = 0;
        }
      }

    }
    else if (condition == 2) {
      if (decimal2 == 0) {
        Serial.println("status : BIG");
        lcd.clear();
        lcd.setCursor(0, 1);
        total2 = value2 + (dec2 / 100);
        lcd.print("IS :" + String(total2) + " A");
        if (customKey >= '0' && customKey <= '9') {
          valueKey += customKey;
          value2 = valueKey.toFloat();
          Serial.print("Current Value: ");
          Serial.println(value2);
        }
        if (customKey == 'C') {
          value2 = 0;
          valueKey = "";
        }
        if (customKey == 'B') {
          condition = 0;
        }
        if (customKey == 'D') {
          decimal2 = 1;
        }
      }
      else if (decimal2 == 1) {
        Serial.println("status : Small");
        lcd.clear();
        lcd.setCursor(0, 1);
        total2 = value2 + (dec2 / 100);
        lcd.print("IS :" + String(total2) + " A");
        if (customKey >= '0' && customKey <= '9') {
          valueKey += customKey;
          dec2 = valueKey.toFloat();
          Serial.print("Current Value: ");
          Serial.println(dec2);
        }
        if (customKey == 'C') {
          dec2 = 0;
          valueKey = "";
        }
        if (customKey == 'B') {
          condition = 0;
        }
        if (customKey == 'D') {
          decimal2 = 0;
        }
      }
    }
  }
  else if (kondisi == 0) {
    if (condition == 0) {
      Serial.println("V1 :" + String(voltage) + "V" + "||" + " V2 :" + String(v) + "V");
      Serial.println("I1 :" + String(current) + "A" + "||" + " I2 :" + String(CurrentValue) + "A");
      Serial.println("Kondisi :" + String(kondisi) + "||" + "Kedua Relay Mati");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Vp :" + String(voltage) + " Vs :" + String(v));
      lcd.setCursor(0, 1);
      lcd.print("Ip :" + String(current) + " Is :" + String(CurrentValue));
      lcd.setCursor(0, 3);
      lcd.print("Treshold :" + String(total3) + " A");
      if (current <= total3 && CurrentValue <= total3) {
        digitalWrite(relay3, HIGH);
        digitalWrite(relay4, HIGH);
        digitalWrite(relay1, LOW);
        digitalWrite(relay2, LOW);
        digitalWrite(relay5, LOW);
        digitalWrite(relay6, LOW);
      }
      else if (current > total3) {
        condition = 1;
        Serial.println("RELAY PRIMER NYALA");
        digitalWrite(relay3, LOW);
        digitalWrite(relay4, LOW);
        digitalWrite(relay1, HIGH);
        digitalWrite(relay5, HIGH);
        digitalWrite(relay2, LOW);
        digitalWrite(relay6, LOW);
      }
      else if (CurrentValue > total3) {
        condition = 2;
        Serial.println("RELAY SEKUNDER NYALA");
        lcd.setCursor(0, 2);
        lcd.print("SEKUNDER");
        digitalWrite(relay3, LOW);
        digitalWrite(relay4, HIGH);
        digitalWrite(relay2, HIGH);
        digitalWrite(relay1, LOW);
        digitalWrite(relay5, LOW);
        digitalWrite(relay6, HIGH);
      }
      if (customKey == 'B') {
        kondisi = 1;
        Serial.println("Switched to Kondisi 1");
        digitalWrite(relay3, HIGH);
        digitalWrite(relay4, HIGH);
        digitalWrite(relay1, LOW);
        digitalWrite(relay2, LOW);
        digitalWrite(relay5, LOW);
        digitalWrite(relay6, LOW);
      }
      if (customKey == 'D') {
        kondisi = 2;
      }
    }
    else if (condition == 1) {
      lcd.setCursor(0, 0);
      lcd.print("Vp :" + String(voltage) + " Vs :" + String(v));
      lcd.setCursor(0, 1);
      lcd.print("Ip :" + String(current) + " Is :" + String(CurrentValue));
      lcd.setCursor(0, 2);
      lcd.print("PRIMER");
      lcd.setCursor(0, 3);
      lcd.print("Treshold :" + String(total3) + " A");
      if (customKey == 'B') {
        condition = 0;
        digitalWrite(relay3, HIGH);
        digitalWrite(relay4, HIGH);
        digitalWrite(relay1, LOW);
        digitalWrite(relay2, LOW);
        digitalWrite(relay5, LOW);
        digitalWrite(relay6, LOW);
      }
      else if (customKey == 'D') {
        kondisi = 2;
      }
    }
    else if (condition == 2) {
      lcd.setCursor(0, 0);
      lcd.print("Vp :" + String(voltage) + " Vs :" + String(v));
      lcd.setCursor(0, 1);
      lcd.print("Ip :" + String(current) + " Is :" + String(CurrentValue));
      lcd.setCursor(0, 2);
      lcd.print("SEKUNDER");
      lcd.setCursor(0, 3);
      lcd.print("Treshold :" + String(total3) + " A");
      if (customKey == 'B') {
        condition = 0;
        digitalWrite(relay3, HIGH);
        digitalWrite(relay4, HIGH);
        digitalWrite(relay1, LOW);
        digitalWrite(relay2, LOW);
        digitalWrite(relay5, LOW);
        digitalWrite(relay6, LOW);
      }
      else if (customKey == 'D') {
        kondisi = 2;
      }
    }
  }
  else if (kondisi == 2) {
    if (decimal3 == 0) {
      Serial.println("status : BIG");
      lcd.clear();
      lcd.setCursor(0, 1);
      total3 = value + (dec3 / 100);
      lcd.print("Treshold :" + String(total3) + " A");
      if (customKey >= '0' && customKey <= '9') {
        charKey += customKey;
        value = charKey.toFloat();
        Serial.print("Current Value: ");
        Serial.println(value);
      }
      if (customKey == 'C') {
        value = 0;
        charKey = "";
      }
      if (customKey == 'A') {
        kondisi = 0;
        digitalWrite(relay4, HIGH);
        digitalWrite(relay3, HIGH);
        digitalWrite(relay5, LOW);
        digitalWrite(relay6, LOW);
      }
      if (customKey == 'D') {
        decimal3 = 1;
      }
    }
    else if (decimal3 == 1) {
      Serial.println("status : Small");
      lcd.clear();
      lcd.setCursor(0, 1);
      total3 = value + (dec3 / 100);
      lcd.print("Treshold :" + String(total3) + " A");
      if (customKey >= '0' && customKey <= '9') {
        charKey += customKey;
        dec3 = charKey.toFloat();
        Serial.print("Current Value: ");
        Serial.println(dec3);
      }
      if (customKey == 'C') {
        dec3 = 0;
        charKey = "";
      }
      if (customKey == 'A') {
        kondisi = 0;
        digitalWrite(relay4, HIGH);
        digitalWrite(relay3, HIGH);
        digitalWrite(relay5, LOW);
        digitalWrite(relay6, LOW);
      }
      if (customKey == 'D') {
        decimal3 = 0;
      }
    }
  }
}

float readACCurrent(int Pin)
{
  int analogValue;             //analog value read from the sensor output pin
  int maxValue = 0;            // store max value
  int minValue = 1024;         // store min value
  unsigned long start_time = millis();
  while ((millis() - start_time) < 200) //sample for 0.2s
  {
    analogValue = analogRead(Pin);
    if (analogValue > maxValue)
    {
      maxValue = analogValue;
    }
    if (analogValue < minValue)
    {
      minValue = analogValue;
    }
  }
  float Vpp = (maxValue - minValue) * Vref / 1024.0;
  float Vrms = Vpp / 2.0 * 0.707 / mVperAmp; //Vpp -> Vrms
  return Vrms;
}

/*read reference voltage*/
long readVref()
{
  long result;
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328__) || defined (__AVR_ATmega328P__)
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#elif defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_AT90USB1286__)
  ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  ADCSRB &= ~_BV(MUX5);   // Without this the function always returns -1 on the ATmega2560 https://openenergymonitor.org/emon/node/2253#comment-11432
#elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
  ADMUX = _BV(MUX5) | _BV(MUX0);
#elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
  ADMUX = _BV(MUX3) | _BV(MUX2);
#endif
#if defined(__AVR__)
  delay(2);                                        // Wait for Vref to settle
  ADCSRA |= _BV(ADSC);                             // Convert
  while (bit_is_set(ADCSRA, ADSC));
  result = ADCL;
  result |= ADCH << 8;
  result = 1126400L / result;  //1100mV*1024 ADC steps https://openenergymonitor.org/emon/node/1186
  return result;
#elif defined(__arm__)
  return (3300);                                  //Arduino Due
#else
  return (3300);                                  //Guess that other un-supported architectures will be running a 3.3V!
#endif
}
