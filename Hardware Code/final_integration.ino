//#include <SoftwareSerial.h>
//SoftwareSerial Serial1(2, 3); // RX, TX

#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

#include <Wire.h>
#include "MutichannelGasSensor.h"

int getFingerprintIDez();

String apiKey = "08VEON2RURIDHCDK";     // replace with your channel's thingspeak WRITE API key

String ssid="geetha";    // Wifi network SSID
String password ="9677260468";  // Wifi network password

boolean DEBUG=true;

//======================================================================== showRSerial3onse

// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
SoftwareSerial mySerial(52, 53);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// On Leonardo/Micro or others with hardware serial, use those! #0 is green wire, #1 is white
//Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial2);
int updates;
int failedUpdates;
int pos;
int stringplace = 0;

String timeUp;
String nmea[15];
String labels[12] {"Time: ", "Status: ", "Latitude: ", "Hemisphere: ", "Longitude: ", "Hemisphere: ", "Speed: ", "Track Angle: ", "Date: "};

//======================================================================== showRSerial3onse
void showRSerial3onse(int waitTime){
    long t=millis();
    char c;
    while (t+waitTime>millis()){
      if (Serial3.available()){
        c=Serial3.read();
        if (DEBUG) Serial.print(c);
      }
    }
                   
}

//========================================================================
boolean thingSpeakWrite(float value1, float value2, float value3, float value4, float value5, float value6, float value7,float value8){
  String cmd = "AT+CIPSTART=\"TCP\",\"";                  // TCP connection
  cmd += "184.106.153.149";                               // api.thingspeak.com
  cmd += "\",80";
  Serial3.println(cmd);
  if (DEBUG) Serial.println(cmd);
  if(Serial3.find("Error")){
    if (DEBUG) Serial.println("AT+CIPSTART error");
    return false;
  }
  
  
  String getStr = "GET /update?api_key=";   // prepare GET string
  getStr += apiKey;
  
  getStr +="&field1=";
  getStr += String(value1,6);
  getStr +="&field2=";
  getStr += String(value2,6);
  getStr +="&field3=";
  getStr += String(value3,6);
  getStr +="&field4=";
  getStr += String(value4,6);
  getStr +="&field5=";
  getStr += String(value5,6);
  getStr +="&field6=";
  getStr += String(value6,6);
  getStr +="&field7=";
  getStr += String(value7,6);
  getStr +="&field8=";
  getStr += String(value8,6);
  
  // ...
  getStr += "\r\n\r\n";

  // send data length
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  Serial3.println(cmd);
  if (DEBUG)  Serial.println(cmd);
  
  delay(100);
  if(Serial3.find(">")){
    Serial3.print(getStr);
    if (DEBUG)  {
      Serial.print(getStr);
      showRSerial3onse(1000);
    }
  }
  else{
    Serial3.println("AT+CIPCLOSE");
    // alert user
    if (DEBUG)   Serial.println("AT+CIPCLOSE");
    return false;
  }
  return true;
}

void setup() {
  while (!Serial);  // For Yun/Leo/Micro/Zero/...


  
  Serial.begin(115200);  // start serial for output
  Serial2.begin(9600); 
  Serial1.begin(9600);
  Serial3.begin(115200);  // enable software serial
                          // Your Serial38266 module's speed is probably at 115200. 
                          // For this reason the first time set the speed to 115200 or to your Serial38266 configured speed 
                          // and upload. Then change to 9600 and upload again
  
  //Serial3.println("AT+CIOBAUD=9600");         // set Serial38266 serial speed to 9600 bps
  Serial3.println("AT+UART_CUR=9600,8,1,0,0");         // set Serial38266 serial speed to 9600 bps
  
  showRSerial3onse(1000);
  
  Serial3.println("AT+RST");         // reset Serial38266
  showRSerial3onse(1000);

  Serial3.println("AT+CWMODE=1");   // set Serial38266 as client
  showRSerial3onse(1000);

  Serial3.println("AT+CWJAP=\""+ssid+"\",\""+password+"\"");  // set your home router SSID and password
  showRSerial3onse(5000);

   if (DEBUG)  Serial.println("Setup completed");
  delay(100);
  Serial.println("Adafruit finger detect test");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1);
  }
  Serial.println("Waiting for valid finger...");
  //calculateSerial1();
 
    Serial.println("power on!");
    gas.begin(0x04);//the default I2C address of the slave is 0x04
    gas.powerOn();
    Serial.print("Firmware Version = ");
    Serial.println(gas.getVersion());
  

  
}

void loop() {

   getFingerprintIDez();
  delay(50);
}



String ConvertLat() {
  String posneg = "";
  if (nmea[3] == "S") {
    posneg = "-";
  }
  String latfirst;
  float latsecond;
  for (int i = 0; i < nmea[2].length(); i++) {
    if (nmea[2].substring(i, i + 1) == ".") {
      latfirst = nmea[2].substring(0, i - 2);
      latsecond = nmea[2].substring(i - 2).toFloat();
    }
  }
  latsecond = latsecond / 60;
  String CalcLat = "";

  char charVal[9];
  dtostrf(latsecond, 4, 6, charVal);
  for (int i = 0; i < sizeof(charVal); i++)
  {
    CalcLat += charVal[i];
  }
  latfirst += CalcLat.substring(1);
  latfirst = posneg += latfirst;
  return latfirst;
}

String ConvertLng() {
  String posneg = "";
  if (nmea[5] == "W") {
    posneg = "-";
  }

  String lngfirst;
  float lngsecond;
  for (int i = 0; i < nmea[4].length(); i++) {
    if (nmea[4].substring(i, i + 1) == ".") {
      lngfirst = nmea[4].substring(0, i - 2);
      //Serial.println(lngfirst);
      lngsecond = nmea[4].substring(i - 2).toFloat();
      //Serial.println(lngsecond);

    }
  }
  lngsecond = lngsecond / 60;
  String CalcLng = "";
  char charVal[9];
  dtostrf(lngsecond, 4, 6, charVal);
  for (int i = 0; i < sizeof(charVal); i++)
  {
    CalcLng += charVal[i];
  }
  lngfirst += CalcLng.substring(1);
  lngfirst = posneg += lngfirst;
  return lngfirst;
}
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  // found a match!
 Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence); 
 
}

void calculate()
{
   //calculateSerial1();
   airp();
}
// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {

  delay(3000);
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) 
  {
    Serial.print("Invalid ID");
    SendMessage();
    return -1;
  }
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  while(1)
  {
   calculate();
  }
  return finger.fingerID; 
}
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

 void SendMessage()
{
  Serial2.println("AT+CMGF=1");    //Sets the Serial2 Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  Serial2.println("AT+CMGS=\"+919003019365\"\r"); // Replace x with mobile number
  delay(1000);
  for (int i = 0; i < 9; i++) {
      Serial2.print(labels[i]);
      Serial2.print(nmea[i]);
      Serial2.println("");
    }
  Serial2.println("Unauthorised Access");// The SMS text you want to send
  delay(100);
   Serial2.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}


void airp()
{
  float c1,c2,c3,c4,c5,c6,c7,c8;

    c1 = gas.measure_NH3();
    Serial.print("The concentration of NH3 is ");
    if(c1>=0) Serial.print(c1);
    else Serial.print("invalid");
    Serial.println(" ppm");

    c2 = gas.measure_CO();
    Serial.print("The concentration of CO is ");
    if(c2>=0) Serial.print(c2);
    else Serial.print("invalid");
    Serial.println(" ppm");

    c3 = gas.measure_NO2();
    Serial.print("The concentration of NO2 is ");
    if(c3>=0) Serial.print(c3);
    else Serial.print("invalid");
    Serial.println(" ppm");

    c4 = gas.measure_C3H8();
    Serial.print("The concentration of C3H8 is ");
    if(c4>=0) Serial.print(c4);
    else Serial.print("invalid");
    Serial.println(" ppm");


    c5 = gas.measure_H2();
    Serial.print("The concentration of H2 is ");
    if(c5>=0) Serial.print(c5);
    else Serial.print("invalid");
    Serial.println(" ppm");

    c6 = gas.measure_C2H5OH();
    Serial.print("The concentration of C2H5OH is ");
    if(c6>=0) Serial.print(c6);
    else Serial.print("invalid");
    Serial.println(" ppm");

    Serial.flush();
    Serial1.flush();
  
  while (Serial1.available() > 0)
  {
    Serial1.read();

  }
  if (Serial1.find("$GPRMC,")) {
    String tempMsg = Serial1.readStringUntil('\n');
    for (int i = 0; i < tempMsg.length(); i++) {
      if (tempMsg.substring(i, i + 1) == ",") {
        nmea[pos] = tempMsg.substring(stringplace, i);
        stringplace = i + 1;
        pos++;
      }
      if (i == tempMsg.length() - 1) {
        nmea[pos] = tempMsg.substring(stringplace, i);
      }
    }
    updates++;
    nmea[2] = ConvertLat();
    nmea[4] = ConvertLng();
    for (int i = 0; i < 9; i++) {
      Serial.print(labels[i]);
      Serial.print(nmea[i]);
      Serial.println("");
    }
    
  }
  else {

    failedUpdates++;

  }
  stringplace = 0;
  pos = 0;
  
    
    thingSpeakWrite(c1,c2,c3,c4,c5,c6,nmea[2].toFloat(),nmea[4].toFloat());                                      // Write values to thingspeak    
    delay(20000);   
    Serial.println("...");
}
