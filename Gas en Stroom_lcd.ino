#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "Arduino.h"
#include "FS.h"

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

// WiFi Parameters

int vorigeminuut=0;
String vorigeuur="";
const char* ssid = "Het Wifimodem";
const char* password = "modemwachtwoord";

void setup() {
  Serial.begin(115200);
  //lcd.init();                      // initialize the lcd 
  lcd.init();
  lcd.backlight();
  delay(500);

  Serial.println("Inizializing FS...");
    if (SPIFFS.begin()){
        Serial.println("done.");
    }else{
        Serial.println("fail.");
    }
 
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
}

void loop() {
  // Check WiFi Status
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;  //Object of class HTTPClient
    http.begin("http://192.168.1.67/api/v1/sm/actual");
    int httpCode = http.GET();
    //Check the returning code                                                                  
    if (httpCode > 0) {
      // Parsing
      const size_t capacity = JSON_ARRAY_SIZE(12) + JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + 11*JSON_OBJECT_SIZE(3) + 510;
      DynamicJsonDocument root(capacity);
      deserializeJson(root, http.getString());

//JsonArray devtime = root["devtime"];
JsonArray actual = root["actual"];
      // Parameters

const char* actual_0_name = actual[0]["name"]; // "timestamp"
const char* actual_0_value = actual[0]["value"]; // "200611230911S"

JsonObject actual_1 = actual[1];
const char* actual_1_name = actual_1["name"]; // "geleverd tarief1"
float actual_1_value = actual_1["value"]; // 0.069
const char* actual_1_unit = actual_1["unit"]; // "kWh"

JsonObject actual_2 = actual[2];
const char* actual_2_name = actual_2["name"]; // "geleverd tarief2"
float actual_2_value = actual_2["value"]; // 0.069
const char* actual_2_unit = actual_2["unit"]; // "kWh"

JsonObject actual_3 = actual[3];
const char* actual_3_name = actual_3["name"]; // "teruggeleverd tarief1"
float actual_3_value = actual_3["value"]; // 0.069
const char* actual_3_unit = actual_3["unit"]; // "kWh"

JsonObject actual_4 = actual[4];
const char* actual_4_name = actual_4["name"]; // "teruggeleverd tarief2"
float actual_4_value = actual_4["value"]; // 0.069
const char* actual_4_unit = actual_4["unit"]; // "kWh"

// meterstanden op 2 dec 2019: 145,142,122,299   gas: 51
// meterstanden op 2 dec 2020: 561,503,370,867   gas: 307
lcd.setCursor(0,1);
lcd.print("Jr:      ");
lcd.setCursor(4,1);
lcd.print(actual_1_value+actual_2_value-actual_3_value-actual_4_value+173,0);  // +173 is de correctie voor het jaarverbruik, om de teller op de opnamedag van de jaarafrekening op 0 te zetten
									       // Dus 1 keer per jaar moet daarvoor de code opnieuw in de esp8266 worden gezet
	      
JsonObject actual_5 = actual[5];
const char* actual_5_name = actual_5["name"]; // "power_delivered"
float actual_5_value = actual_5["value"]; // 0.069
const char* actual_5_unit = actual_5["unit"]; // "kW"

JsonObject actual_6 = actual[6];
const char* actual_6_name = actual_6["name"]; // "power_returned"
float actual_6_value =actual_6["value"]; // 0
const char* actual_6_unit = actual_6["unit"]; // "kW"


int productie = 1000*actual_5_value-1000*actual_6_value;

lcd.setCursor(9,1);
lcd.print("Act:       ");
if (productie >= 0)
     {lcd.setCursor(16,1);
      lcd.print(productie);}
if (productie <0)
     {lcd.setCursor(15,1);
      lcd.print(productie);}


String tijd=String(actual_0_value);
String jaar ="20"+tijd.substring(0,2);
String maand =tijd.substring(2,4);
String dag =tijd.substring(4,6);
String uur = tijd.substring(6,8);

String minuut = tijd.substring(8,10);
String datumstempel=jaar+'-'+maand+'-'+dag;
String tijdstempel=uur+':'+minuut;
lcd.setCursor(0,3);
lcd.print(datumstempel);lcd.print("     ");lcd.print(tijdstempel);
int menuut= minuut.toInt();
//Serial.println(menuut);
if (uur!=vorigeuur)  
    {lcd.init();                            //een keer per uur wordt scherm schoongeveegd ivm evt troep erop
     lcd.backlight();};
vorigeuur=uur;     


if (menuut!=vorigeminuut)
{
  if(1==0)   // was: if((menuut%5)==0)  schrijft nu niet naar spiffs-file dus
    {
    File logFile = SPIFFS.open("/log.csv", "a");
    logFile.print(datumstempel);
    logFile.print(';');
    logFile.print(tijdstempel);
    logFile.print(';');
    logFile.println(productie);
    logFile.close();

    logFile = SPIFFS.open("/log.csv", "r");
    if (logFile){
        Serial.println("Read file content!");
        /**
         * File derivate from Stream so you can use all Stream method
         * readBytes, findUntil, parseInt, println etc
         */
        Serial.println(logFile.readString());
        logFile.close();
    }else{
        Serial.println("Problem on read file!");
    }

   }
vorigeminuut=menuut;
}
 
    }
    http.end();   //Close connection
  }

  
delay(9000);  





if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;  //Object of class HTTPClient
    http.begin("http://192.168.1.67/api/v1/hist/days");
    int httpCode = http.GET();
    //Check the returning code                                                                  
    if (httpCode > 0) {
      
const size_t capacity = JSON_ARRAY_SIZE(15) + JSON_OBJECT_SIZE(1) + 15*JSON_OBJECT_SIZE(8) + 210;
DynamicJsonDocument doc(capacity);


deserializeJson(doc, http.getString());

JsonArray days = doc["days"];
//JsonObject days_0 = days.createNestedObject();
JsonObject days_0 = days[0];
int days_0_recnr = days_0["recnr"]; // 0
const char* days_0_recid = days_0["recid"]; // "20092912"
int days_0_slot = days_0["slot"]; // 9
float days_0_edt1 = days_0["edt1"]; // 436.331
float days_0_edt2 = days_0["edt2"]; // 346.386                         vandaag
float days_0_ert1 = days_0["ert1"]; // 362.468
float days_0_ert2 = days_0["ert2"]; // 845.466
float days_0_gdt = days_0["gdt"]; // 290.596

//JsonObject days_1 = days.createNestedObject();
JsonObject days_1 = days[1];
int days_1_recnr = days_1["recnr"]; // 1
const char* days_1_recid = days_1["recid"]; // "20092823"
int days_1_slot = days_1["slot"]; // 8
float days_1_edt1 = days_1["edt1"]; // 436.117
float days_1_edt2 = days_1["edt2"]; // 345.961                        gisteren
float days_1_ert1 = days_1["ert1"]; // 362.468
float days_1_ert2 = days_1["ert2"]; // 845.451
float days_1_gdt = days_1["gdt"]; // 290.596

JsonObject days_2 = days[2];
int days_2_recnr = days_2["recnr"]; // 1
const char* days_2_recid = days_2["recid"]; // "20092823"
int days_2_slot = days_2["slot"]; // 8
float days_2_edt1 = days_2["edt1"]; // 436.117
float days_2_edt2 = days_2["edt2"]; // 345.961                         eergisteren
float days_2_ert1 = days_2["ert1"]; // 362.468
float days_2_ert2 = days_2["ert2"]; // 845.451
float days_2_gdt = days_2["gdt"]; // 290.596


JsonObject days_3 = days[3];
int days_3_recnr = days_3["recnr"]; // 1
const char* days_3_recid = days_3["recid"]; // "20092823"
int days_3_slot = days_3["slot"]; // 8
float days_3_edt1 = days_3["edt1"]; // 436.117
float days_3_edt2 = days_3["edt2"]; // 345.961                         eereergisteren
float days_3_ert1 = days_3["ert1"]; // 362.468
float days_3_ert2 = days_3["ert2"]; // 845.451
float days_3_gdt = days_3["gdt"]; // 290.596

lcd.setCursor(0,0); 
lcd.print("     ");
lcd.setCursor(0,0); 
lcd.print(days_2_edt1-days_3_edt1+days_2_edt2-days_3_edt2-days_2_ert1+days_3_ert1-days_2_ert2+days_3_ert2,2);  // stroomverbruik eergisteren
lcd.setCursor(0,2); 
lcd.print(days_2_gdt-days_3_gdt,1);   //   gasverbruik eergisteren


lcd.setCursor(7,0); 
lcd.print("     ");
lcd.setCursor(7,0); 
lcd.print(days_1_edt1-days_2_edt1+days_1_edt2-days_2_edt2-days_1_ert1+days_2_ert1-days_1_ert2+days_2_ert2,2);  // stroomverbruik gisteren
lcd.setCursor(7,2); 
lcd.print(days_1_gdt-days_2_gdt,1);   //   gasverbruik gisteren



lcd.setCursor(16,0); 
lcd.print("    ");
lcd.setCursor(16,0);
if (days_0_edt1-days_1_edt1+days_0_edt2-days_1_edt2-days_0_ert1+days_1_ert1-days_0_ert2+days_1_ert2<0)
   {lcd.setCursor(15,0);}  //  1 plaats naar links voor het minteken van dagproductie 
lcd.print(days_0_edt1-days_1_edt1+days_0_edt2-days_1_edt2-days_0_ert1+days_1_ert1-days_0_ert2+days_1_ert2,2);  // stroomverbruik vandaag
lcd.setCursor(16,2); 
lcd.print(days_0_gdt-days_1_gdt,2);   //   gasverbruik vandaag


    }
    http.end();   //Close connection
  }


   
  
  // Delay
  delay(9000);

  
}