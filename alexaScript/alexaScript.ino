#include <WiFi.h>
#include <esp_task_wdt.h>
#include <Espalexa.h>
#include <esp_wifi.h>

//define all constants that will be used in the code
#define PIN_PORTONE  5
#define PIN_CANCELLO  4
#define delayTime 500
#define WDT_TIMEOUT 3600

//wifi credentials to make esp32 connect to wifi
const char* ssid     = "****";
const char* password = "****";

boolean connectWifi();
void openDoor(uint8_t brightness);
void openGate(uint8_t brightness);
void openAll(uint8_t brightness);

String door = "Apri Portone";
String gate = "Apri Cancello";
String all = "Apri Tutto";

boolean wifiConnected = false;

Espalexa espAlexa;

int i;

void setup() {
  Serial.begin(115200);
  pinMode(PIN_PORTONE, OUTPUT);
  pinMode(PIN_CANCELLO, OUTPUT);
  
  digitalWrite(PIN_PORTONE, HIGH);
  digitalWrite(PIN_CANCELLO, HIGH);

  i=0;

  wifiConnected = connectWifi();
  
  if(wifiConnected){
    //code part to make the devices visible to alexa's scan
    espAlexa.addDevice(door, openDoor);
    espAlexa.addDevice(gate, openGate);
    espAlexa.addDevice(all, openAll);

    espAlexa.begin();
  } else {
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        i++;
        if(i==6){
          ESP.restart();
        }
    }
  }
  //the following three commands are used to make the device restart after an hour, this is to make the device more reliable(my unit has some problem with becoming unresponsive). 
  esp_wifi_set_ps(WIFI_PS_NONE);
  esp_task_wdt_init(WDT_TIMEOUT, true);
  esp_task_wdt_add(NULL);
}

void loop() {
  espAlexa.loop();
  delay(10);
}

//function that when called open the front door
void openDoor(uint8_t brightness){
  digitalWrite(5, LOW);
  delay(delayTime);
  digitalWrite(5, HIGH);
  //restart the esp is used to make the device more reliable(my unit has some problem with becoming unresponsive).
  ESP.restart();  
}

//function that when called open the gate
void openGate(uint8_t brightness){
  digitalWrite(4, LOW);
  delay(delayTime);
  digitalWrite(4, HIGH);
  //restart the esp is used to make the device more reliable(my unit has some problem with becoming unresponsive).
  ESP.restart();
}

//function that when called open both gate and door
void openAll(uint8_t brightness){
  digitalWrite(5, LOW);
  delay(delayTime);
  digitalWrite(5, HIGH);
  delay(delayTime);  
  digitalWrite(4, LOW);
  delay(delayTime);
  digitalWrite(4, HIGH);
  //restart the esp is used to make the device more reliable(my unit has some problem with becoming unresponsive).
  ESP.restart();
}

boolean connectWifi()
{
  boolean state = true;
  int i = 0;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 20) {
      state = false; 
      break;
    }
    i++;
  }
  Serial.println("");
  if (state) {
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("Connection failed.");
  }
  return state;
}
