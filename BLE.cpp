#include <Arduino.h>
   #include <BLEDevice.h>
   #include <BLEServer.h>
   #include <BLEUtils.h>
   #include <BLE2902.h>
   #include <ArduinoJson.h>
   #include <WiFi.h>
   #include <HTTPClient.h>
   #define ssid "AndroidAP"
   #define password "gxbu3177"
const char* ID_WiFi = "AndroidAP"; // ID WiFi const char* Parola = "gxbu3177"; // Parola WiFi #define bleServerName "daren"
bool deviceConnected = false;
#define SERVICE_UUID "3693a246-d913-4623-9023-f9d76de6f983"
     
BLECharacteristic indexCharacteristic( "ca73b3ba-39f6-4ab3-91ae-186dc9577d99", BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY
);
BLEDescriptor *indexDescriptor = new BLEDescriptor(BLEUUID((uint16_t)0x2901));
BLECharacteristic detailsCharacteristic( "183f3323-b11f-4065-ab6e-6a13fd7b104d", // <-- TODO -- Change Me BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY
);
BLEDescriptor *detailsDescriptor = new BLEDescriptor(BLEUUID((uint16_t)0x2902));
class MyServerCallbacks:
public BLEServerCallbacks {
void onConnect(BLEServer* pServer) { deviceConnected = true; Serial.println("Device connected"); };
void onDisconnect(BLEServer* pServer) { deviceConnected = false; Serial.println("Device disconnected"); }
};
class CharacteristicsCallbacks:
public BLECharacteristicCallbacks {
void onWrite(BLECharacteristic *characteristic) {
std::string data = characteristic->getValue(); if(data.length()<=23){
WiFiClient client;
HTTPClient http;
http.useHTTP10(true);
http.begin(client, "http://proiectia.bogdanflorea.ro/api/the-meal- db/meals");

http.GET(); DynamicJsonDocument doc(6144);
DeserializationError error = deserializeJson(doc, client); if (error)
{
Serial.print("deserializeJson() failed prima deserializare: "); Serial.println(error.c_str());
return;
}
for (auto item : doc.as<JsonArray>())
{
std::string idMeal = item["idMeal"];
std::string strMeal = item["strMeal"];
std::string strCategory = item["strCategory"];
std::string strArea = item["strArea"];
std::string strMealThumb = item["strMealThumb"];
std::string strTags = item["strTags"];
std::string nume="{\"id\": \"" + idMeal + "\", \"name\": \"" + strMeal + "\", \"image\": \"" + strMealThumb+ "\"}"; characteristic->setValue(nume);
characteristic->notify();
}
delay (50);
}
else{
StaticJsonDocument<96>document;
DeserializationError error = deserializeJson(document, data);
if (error)
{
Serial.print("deserializeJson() failed a doua deserializare: "); Serial.println(error.c_str());
}

String action = document["action"]; String id = document["id"];
WiFiClient client;
HTTPClient http;
http.useHTTP10(true);
http.begin(client, "http://proiectia.bogdanflorea.ro/api/the-meal- db/meal?i="+id);
http.GET();
StaticJsonDocument<384> documentquery;
error = deserializeJson(documentquery, client); if (error)
{
Serial.print("deserializeJson() failed: a treia deserializare"); Serial.println(error.c_str());
return;
}
std::string idMeal = documentquery["idMeal"]; // "52774"
std::string strMeal = documentquery["strMeal"]; // "Pad See Ew" std::string strCategory = documentquery["strCategory"]; // "Chicken" std::string strArea = documentquery["strArea"]; // "Thai" std::string strMealThumb = documentquery["strMealThumb"];//"link" std::string strTags = documentquery["strTags"]; // "Pasta" std::string descriere="Preparatul "+strMeal +" face parte din "+strCategory+", din regiunea "+strArea+ " si are tagurile: "+strTags+".";
std::string afisare="{\"id\": \"" + idMeal + "\", \"name\": \"" + strMeal + "\", \"image\": \"" + strMealThumb + "\", \"description\": \"" + descriere + "\"}";
characteristic->setValue(afisare);
characteristic->notify();
delay (50);
}
}

};
void setup() {
Serial.begin(115200);
Serial.println("Acum poti sa te conectezi Bluetooth"); Serial.printf("Conectare la %s ", ID_WiFi); WiFi.begin(ID_WiFi, Parola);
while (WiFi.status() != WL_CONNECTED)
{
delay(500);
Serial.print(".");
}
Serial.println(" Conectat");
BLEDevice::init(bleServerName);
BLEServer *pServer = BLEDevice::createServer(); pServer->setCallbacks(new MyServerCallbacks());
BLEService *bmeService = pServer->createService(SERVICE_UUID);
bmeService->addCharacteristic(&indexCharacteristic); indexDescriptor->setValue("Get data list"); indexCharacteristic.addDescriptor(indexDescriptor); indexCharacteristic.setValue("Get data List"); indexCharacteristic.setCallbacks(new CharacteristicsCallbacks());
bmeService->addCharacteristic(&detailsCharacteristic); detailsDescriptor->setValue("Get data details"); detailsCharacteristic.addDescriptor(detailsDescriptor); detailsCharacteristic.setValue("Get data details");
detailsCharacteristic.setCallbacks(new CharacteristicsCallbacks());

bmeService->start();
BLEAdvertising *pAdvertising = BLEDevice::getAdvertising(); pAdvertising->addServiceUUID(SERVICE_UUID); pServer->getAdvertising()->start();
Serial.println("Waiting a client connection to notify...");
}
void loop() {
}