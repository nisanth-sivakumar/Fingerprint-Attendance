#include "WiFi.h"
#include <HTTPClient.h>
#include "time.h"

const char* ssid = "MYHDSB";

const long gmtOffset_sec = 19800;
const int daylightOffset_sec = 0;
const char* ntpServer = "pool.ntp.org";

String GOOGLE_SCRIPT_ID = "AKfycbwuTEG92kzGfO-HbATaypmusI59pRJp6n4lV0DxQTpo3mWpneEyXK6DfztroUZZKhGNrw";

HTTPClient http;

/*-----------------------------------------*/

#include <Adafruit_Fingerprint.h>
#include <HardwareSerial.h>

#define mySerial Serial1
//HardwareSerial mySerial(2);
Adafruit_Fingerprint fingerprintSensor = Adafruit_Fingerprint(&Serial1);
//Adafruit_Fingerprint fingerprintSensor = Adafruit_Fingerprint(&Serial1,0x0);

int fingerprint_ID = 0;
uint8_t id;
String name;
int state = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  WiFi.begin(ssid);
  Serial.print("Beginning WiFi Connection");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  /*---------------------------------------------*/  
  fingerprintSensor.begin(57600);
  
  Serial.println("");
  Serial.print("Connecting to Fingerprint Sensor");
  while(!fingerprintSensor.verifyPassword()) {
    delay(500);
    Serial.println(".");
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.println(WiFi.status()); // Printing "3" means connection was successful
  //uploadToSheet();
  //delay(10000);

  switch(state) {
    case 0:
      scanFingerprintID();
      break;
    case 1:
      addFingerprint();
      break;
    default:
      scanFingerprintID();
  }
  delay(100);
}

void uploadToSheet() {
  // Add names based on ID
  if(fingerprint_ID == 1) {
    name = "Nisanth Sivakumar";
  }

  if (WiFi.status() == WL_CONNECTED) {
    static bool flag = false;
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      Serial.println("Failed to obtain time");
      return;
    }
    char timeStringBuff[50];
    strftime(timeStringBuff, sizeof(timeStringBuff), "%A, %B %d %Y %H:%M:%S", &timeinfo);
    String asString(timeStringBuff);
    asString.replace(" ", "-");
    Serial.print("Time:");
    Serial.println(asString);
    //name = "Nisanth Sivakumar";
    String urlFinal = "https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?"+"date=" + asString + "&sensor=" + name;
    Serial.print("POST data to spreadsheet:");
    Serial.println(urlFinal);
    HTTPClient http;
    http.begin(urlFinal.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET(); 
    Serial.print("HTTP Status Code: ");
    Serial.println(httpCode);
    //---------------------------------------------------------------------
    //getting response from google sheet
    String payload;
    if (httpCode > 0) {
        payload = http.getString();
        Serial.println("Payload: "+payload);    
    }
    //---------------------------------------------------------------------
    http.end();
  }
}

void scanFingerprintID() {
  uint8_t image = fingerprintSensor.getImage();
  // Return value/outcome of the scan in this conditional statement
  if(image == FINGERPRINT_OK) {
    Serial.println("Success");
  } 
  else if(image == FINGERPRINT_NOFINGER) {
    Serial.println("Not detected");
  }
  else if(image == FINGERPRINT_IMAGEFAIL) {
    Serial.println("Imaging error");
  }
  else if(image == FINGERPRINT_PACKETRECIEVEERR){
    Serial.println("Communication error");
  }
  else {
    Serial.println("Unknown error");
  }
  //return 0;
  image = fingerprintSensor.image2Tz();
  if(image == FINGERPRINT_OK) {
    Serial.println("Success");
  }
  else if(image == FINGERPRINT_IMAGEMESS) {
    Serial.println("Image too messy");
  }
  else if(image == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
  }
  else if(image == FINGERPRINT_FEATUREFAIL || image == FINGERPRINT_INVALIDIMAGE) {
    Serial.println("Failed to identify fingerprint features");
  }
  else {
    Serial.println("Unknown error");
  }

  image = fingerprintSensor.fingerFastSearch();
  if(image == FINGERPRINT_OK) {
    Serial.println("Success");
  }
  else if(image == FINGERPRINT_NOTFOUND) {
    Serial.println("No match made");
  }
  else if(image == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
  }
  else {
    Serial.println("Unknown error");
  }

  fingerprint_ID = fingerprintSensor.fingerID;
  uploadToSheet();
}

void addFingerprint() {
  int image = -1;

  while(image != FINGERPRINT_OK) {
    image = fingerprintSensor.getImage();

    if(image == FINGERPRINT_OK) {
    Serial.println("Success");
    } 
    else if(image == FINGERPRINT_NOFINGER) {
      Serial.println("Not detected");
    }
    else if(image == FINGERPRINT_IMAGEFAIL) {
      Serial.println("Imaging error");
    }
    else if(image == FINGERPRINT_PACKETRECIEVEERR){
      Serial.println("Communication error");
    }
    else {
      Serial.println("Unknown error");
    }
  }
  
  image = fingerprintSensor.image2Tz(1);

  if(image == FINGERPRINT_OK) {
    Serial.println("Success");
  }
  else if(image == FINGERPRINT_IMAGEMESS) {
    Serial.println("Image too messy");
  }
  else if(image == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
  }
  else if(image == FINGERPRINT_FEATUREFAIL || image == FINGERPRINT_INVALIDIMAGE) {
    Serial.println("Failed to identify fingerprint features");
  }
  else {
    Serial.println("Unknown error");
  }

  Serial.println("Remove finger and rescan");

  while(image != FINGERPRINT_OK) {
    image = fingerprintSensor.getImage();

    if(image == FINGERPRINT_OK) {
    Serial.println("Success");
    } 
    else if(image == FINGERPRINT_NOFINGER) {
      Serial.println("Not detected");
    }
    else if(image == FINGERPRINT_IMAGEFAIL) {
      Serial.println("Imaging error");
    }
    else if(image == FINGERPRINT_PACKETRECIEVEERR){
      Serial.println("Communication error");
    }
    else {
      Serial.println("Unknown error");
    }
  }
  
  image = fingerprintSensor.image2Tz(2);

  if(image == FINGERPRINT_OK) {
    Serial.println("Success");
  }
  else if(image == FINGERPRINT_IMAGEMESS) {
    Serial.println("Image too messy");
  }
  else if(image == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
  }
  else if(image == FINGERPRINT_FEATUREFAIL || image == FINGERPRINT_INVALIDIMAGE) {
    Serial.println("Failed to identify fingerprint features");
  }
  else {
    Serial.println("Unknown error");
  }

  image = fingerprintSensor.createModel();
  if(image == FINGERPRINT_OK) {
    Serial.println("Success");
  }
  else if(image == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
  }
  else if(image == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints not matching");
  }
  else {
    Serial.println("Unknown error");
  }

  image = fingerprintSensor.storeModel(fingerprint_ID);
  if(image == FINGERPRINT_OK) {
    Serial.println("Stored successfully");
  }
  else if(image == FINGERPRINT_BADLOCATION) {
    Serial.println("Location invalid");
  }
  else if(image == FINGERPRINT_FLASHERR) {
    Serial.println("Couldn't be written to flash memory");
  }
  else if(image == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
  }
  else {
    Serial.println("Unknown error");
  }
}