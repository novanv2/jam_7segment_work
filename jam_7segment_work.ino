/*
Author: Danny van den Brande, arduinosensors.nl. BlueCore Tech.
This code is written for the AT24C32/DS1307 RTC Module and
4 digit Seven Segment displays.

MODIFIED BY : NOVAN ARIF ARDIANSYAH
cukup hanya menggunakan library sevseg milik Dean Reading dan juga penambahan beberapa tombol 
source code diambil dari WWW.ROKHMAD.COM dan menghilangkan beberapa fungsi
*/

#include "Wire.h"
#include "SevSeg.h"
#include <DS3231.h>

#define DS3231_ADDRESS 0x68
DS3231  rtc(SDA, SCL);
SevSeg sevseg; //Instantiate a seven segment object


#define tombol_bOK A2             // Tombol OK sesuaikan dengan PIN di arduino anda mana  klo ini terhubung dengan PIN analog2
#define Jam_bDN A3           //tombol setting   sesuaikan dengan PIN di arduino anda mana  klo ini terhubung dengan PIN analog 3

Time  t;

int Hor;
int Min;
int Sec;

unsigned long timer;

byte ConvertToPCD(byte val)
{
  //Convert Decimal to binary
  return ( (val / 10 * 16) + (val % 10) );
}

byte ConvertToDecimal(byte val)
{
  //Convert binary to decimal
  return ( (val / 16 * 10) + (val % 16) );
}

void setup() {
  Serial.begin(115200); //set komunikasi baut serial monitor pada 115200
  rtc.begin();
  Wire.begin();
  //Comment this line after you have set the date @ void TimeSelection() below
  //TimeSelection(); //Uncomment this line when your setting the time. Set time @ (line 66)
 
  pinMode(tombol_bOK, INPUT_PULLUP);                                 // Mode Pin Sebagai Input dengan Pull Up Internal
  pinMode(Jam_bDN, INPUT_PULLUP);                                    // Mode Pin Sebagai Input dengan Pull Up Internal
 
  pinMode(9, OUTPUT);
  byte numDigits = 4;
  byte digitPins[] = {9, 10, 11, 12};
  byte segmentPins[] = {2, 3, 4, 5, 6, 7, 8};
  bool resistorsOnSegments = false; // 'false' means resistors are on digit pins
  byte hardwareConfig = COMMON_ANODE; //ganti COMMON_ANODE bila 7 segment anda bertipe common anoda
  

  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments);

  sevseg.setBrightness(90);
  timer = millis();
}

void loop()
{
  char tempString[10]; //Used for sprintf
  Wire.beginTransmission(DS3231_ADDRESS); //pembacaan komunikasi I2C
  Wire.write(0x00);
  Wire.endTransmission();
  Wire.requestFrom(DS3231_ADDRESS, 7);
  int seconds = ConvertToDecimal(Wire.read());
  int minutes = ConvertToDecimal(Wire.read());
  int hours = ConvertToDecimal(Wire.read() & 0b111111);

   if (digitalRead(Jam_bDN) == LOW) {                               // Tombol Setting menit
    delay(10); minutes++;
    rtc.setTime(hours, minutes, 0);
    if (minutes >= 60) {
      minutes = 0; // Batas Nilai menit Lebih dari 59 Kembali ke 0
      rtc.setTime(hours, minutes, 0);
    }
  }
  while (digitalRead(Jam_bDN) == LOW) {}                          // Kunci Tombol Settingmenit Sampai Lepas Tombol


  //////kodimgku set jam

  if (digitalRead(tombol_bOK) == LOW) {                               // Tombol Setting jam
    delay(10);
    hours++;
    rtc.setTime(hours, minutes, 0);
    if (hours >= 24) {

      hours = 0; // Batas Nilai jam Lebih dari 23 Kembali ke 0
      rtc.setTime(hours, minutes, 0);
    }
  }
  while (digitalRead(tombol_bOK) == LOW) {}                          // Kunci Tombol Setting jam Sampai Lepas Tombol
 
  sprintf(tempString, "%02d%02d", hours, minutes); //sprintf merupakan sebuah function milik bahasa C

  sevseg.setChars(tempString);
  sevseg.refreshDisplay();
  
  //mengatur nyala kedip led sesuai detik
  if (seconds == 1 || seconds == 3 || seconds == 5 || seconds == 7 || seconds == 9 || seconds == 11 || seconds == 13 || seconds == 15 || seconds == 17 || seconds == 19 || seconds == 21 ||
      seconds == 23 || seconds == 25 || seconds == 27 || seconds == 29 || seconds == 31 || seconds == 33 || seconds == 35 || seconds == 37 || seconds == 39
      || seconds == 41 || seconds == 43 || seconds == 45 || seconds == 47 || seconds == 49 ||
      seconds == 51 || seconds == 53 || seconds == 55 || seconds == 57 || seconds == 59 ) {
    digitalWrite(9, LOW);
  } else  {
    digitalWrite(9, HIGH);

  }
  
  Serial.println(tempString); //print waktu ke serial monitor

}

void TimeSelection()   //Select date and time here below!!(line 69,70,71)
//after setting time also comment time TimeSelection() @ void setup
{
  byte seconds = 30; //Valores de 0 a 59
  byte minutes = 27; //Valores de 0 a 59
  byte hours = 21; //Valores de 0 a 23
  Wire.beginTransmission(DS3231_ADDRESS);
  Wire.write(0x00); //Stop no CI para que o mesmo possa receber os dados

  //As linhas abaixo escrevem no CI os valores de
  //data e hora que foram colocados nas variaveis acima
  Wire.write(ConvertToPCD(seconds));
  Wire.write(ConvertToPCD(minutes));
  Wire.write(ConvertToPCD(hours));
  Wire.write(0x00);
  Wire.endTransmission();
}
