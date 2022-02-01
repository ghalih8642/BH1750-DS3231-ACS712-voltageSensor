#include <Wire.h> // librarry untuk tiap sensor
#include <BH1750.h>                
#include <DS3231.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h> 

SoftwareSerial uno(7,8);  // komunukasi RX, TX

LiquidCrystal_I2C lcd (0x27, 20, 4); // alamat i2c LCD yg digunakan dan lcd yang digunakan 20kolom 4 baris
BH1750 lightMeter(0x23); // alamat i2c unutk sensor waktu
DS3231  rtc(SDA, SCL);

//deklarasi sensor tegangan
const int Vin = A0;             //pin input sensor tegangan
const int offset_Vin = -400;      //nilai kalibrasi tegangan
const int offsetvoltage = 2500; 
double adcvoltage = 0.0;
double voltage_in = 0.0;
int V_in = 0;

//dekralasi sensor arus
const int Iin = A1;             //pin input sensor arus
const float kalibrasi = 0.5;  //nilai kalibrasi arus
const int sensitivity = 100;    //sensitivitas sensor arus (berdasarkan datasheet)
int adcvalue = 0;
double Current = 0.0;
double Current_in = 0.0;

//deklarasi daya
float dayaPV = 0.0;

//deklarasi sensor lux
float lux = 0.0;

//komunikasi 1 arah uart
String kirim;
float data;
float data1;
float data2;
float data3;
float kosong=0;

void setup() { 
  uno.begin(9600); //baudrate utk komunikasi dengan nodemcu
  Wire.begin();
  Serial.begin(9600);//baudrate yg digunakan ke sensor
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE_2));
  
  
  lcd.init();
  lcd.init();
  lcd.backlight();
  pinMode(Iin, INPUT); // inisialisasi sensor arus sebagai input 
  pinMode(Vin, INPUT);// inisialisasi sensor tegangan sebagai input

  
  rtc.begin();
//  rtc.setDOW(THURSDAY);
//  rtc.setTime(14, 28, 00);
//  rtc.setDate(27, 04, 2021);
 
  lcd.setCursor(0,0);                 // tampilan pembuka saat alat dinyalakan selama setengah detik lalu clear
  lcd.print("RANCANG BANGUN");
  lcd.setCursor(0,1);
  lcd.print("SISTEM MONITORING");
  lcd.setCursor(0,2);
  lcd.print("SOLAR CELL");
  lcd.setCursor(0,3);
  lcd.print("MENGGUNAKAN BLYNK");
  
  delay(500);
  lcd.clear();
}

void SETUPBLYNK() { //looping unutk kounikasi satu arah degan nodeMCU data yg dikirim sebanyak 4 data 
  kirim = String(kosong) + "#" + String(data) + "#" + String(data1) + "#" + String(data2) + "#" + String(data3);
uno.println(kirim); 
}

void loop() {
  tegangan();
  arus();
  daya();
  luxmeter();
  tampilan();
  SETUPBLYNK();
  data= lux;
  data1 = voltage_in;
  data2 = Current_in;
  data3 = dayaPV;
}
void tegangan(){
  //pembacaan sensor tegangan
  V_in = analogRead(Vin);   //membaca input
  voltage_in = map(V_in,0,1023, 0, 2500) + offset_Vin; //rumus pembacaan sensor tegangan
  voltage_in /=100;
  if(voltage_in<=0.5){ voltage_in=0.0;} //kalibrasi agar tidak muncul nilai minus (-)
}
void arus(){
  //pembacaan sensor arus
  adcvalue = analogRead(Iin);  //membaca input
  adcvoltage = (adcvalue/1024.0)*5000;   //rumus pembacaan sensor arus
  Current=(adcvoltage-offsetvoltage) /sensitivity;
  if (Current<=0.06){Current=0.0;}   //agar tidak muncul nilai minus (-)
  Current_in=(Current*kalibrasi);
}
void daya(){
  //rumuspembacaan daya
  dayaPV = (voltage_in*Current_in);
}
void luxmeter(){
  //pembacaan sensor lux
  lux = lightMeter.readLightLevel();  //membaca input
}

void tampilan() {                         //untuk mengeksekusi dan menjalankan program yang sudah dibuat, akan dijalankan berulang kali oleh Arduino secara berkala
  //lcd.clear();
  lcd.setCursor(0,2);                   //Untuk sensor intensitas cahaya
  lcd.print("L:");
  lcd.print(lux,1);
  lcd.print("Lx ");

  lcd.setCursor(0,0);                   //Untuk Hari dan Tanggal dari rtc ds3231
  lcd.print(rtc.getDOWStr());
  lcd.print(" ");
  lcd.print(rtc.getDateStr());

  lcd.setCursor(0,1);                   //Untuk Waktu dari rtc ds3231
  lcd.print("T:");
  lcd.print(rtc.getTimeStr());

//  lcd.setCursor(0,3);                   //Untuk Temperatur dari rtc ds3231
//  lcd.print("T:");
//  lcd.print(rtc.getTemp());
//  lcd.print("C ");

  lcd.setCursor(11,3);                  //Untuk Daya
  lcd.print("P:");
  lcd.print(dayaPV,1);
  lcd.print("W ");

  lcd.setCursor(11,1);                  //Untuk sensor acs712
  lcd.print("I:");
  lcd.print(Current_in,1);
  lcd.print("A ");

  lcd.setCursor(11,2);                  //Untuk Voltage sensor
  lcd.print("V:");
  lcd.print(voltage_in,1);
  lcd.print("V ");

  Serial.print("L:");
  Serial.print(lux);
  Serial.print("\t V:");
  Serial.print(voltage_in);
  Serial.print("\t I:");
  Serial.print(Current_in);
  Serial.print("\t P:");
  Serial.print(dayaPV);
  Serial.println();
  delay(1000);
}
