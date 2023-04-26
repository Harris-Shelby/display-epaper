#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include <WiFi.h>
#include <EEPROM.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include "../lib/ArduinoJson/ArduinoJson.h"
#include "imagedata.h"

#define JSON_BUFFER_SIZE 12 * 1024
#define NUM_BUFFER_SIZE 2 * 1024
#define NUM_IMAGES_LIMIT 200 // assuming there won't be more than 200 images
#define NUM_NETWORKS_LIMIT 7

DynamicJsonDocument idJsonDocument(JSON_BUFFER_SIZE);
DynamicJsonDocument imageJsonDocument(NUM_BUFFER_SIZE);
const int IMAGE_SIZE = ((EPD_2IN9_WIDTH % 8 == 0) ? (EPD_2IN9_WIDTH / 8) : (EPD_2IN9_WIDTH / 8 + 1)) * EPD_2IN9_HEIGHT;
unsigned char buffer[IMAGE_SIZE];
const char *imageIds[NUM_IMAGES_LIMIT];
int totalImages;
const char *ids_url = "http://140.99.43.159:4400/api/v1/EpaperImg?fields='id'";
const char *img_url = "http://140.99.43.159:4400/api/v1/EpaperImg/"; 

unsigned char blackImage[IMAGE_SIZE];
const char *status = NULL; 

const int EEPROM_SIZE = 512;
const char* AP_SSID = "ESP32-AP";
const char* AP_PASSWORD = "12291229";
int wifiCount;

WebServer server(80);

void connectToWiFi(const char *ssid, const char *password) {
    WiFi.begin(ssid, password);
    Serial.println("Connecting to " + (String)ssid);
    while (WiFi.status() != WL_CONNECTED) {
        delay(300);
        Serial.print(".");
    }
    Serial.println("WiFi connected");
    Serial.println("IP address: " + WiFi.localIP().toString());
}

void handleRoot() {
    String html = "<html><head><title>ESP32 Wi-Fi Configuration</title></head><body>";
    html += "<h1>ESP32 Wi-Fi Configuration</h1>";
    html += "<form method=\"post\" action=\"/submit\">";
    html += "<label for=\"ssid\">Wi-Fi SSID:</label><br/>";
    html += "<input type=\"text\" id=\"ssid\" name=\"ssid\"><br/>";
    html += "<label for=\"password\">Wi-Fi Password:</label><br/>";
    html += "<input type=\"password\" id=\"password\" name=\"password\"><br/>";
    html += "<input type=\"submit\" value=\"Submit\">";
    html += "</form>";
    html += "</body></html>";

    server.send(200, "text/html", html);
}

void handleWifiConfig() {
    String ssid = server.arg("ssid");
    String password = server.arg("password");
    if (ssid != "" && password != "") {
        Serial.printf("Wi-Fi SSID: %s\n", ssid.c_str());
        Serial.printf("Wi-Fi password: %s\n", password.c_str());
        int addr = wifiCount * 64;  
        EEPROM.put(addr, ssid);
        EEPROM.put(addr + 32, password);
        wifiCount++;
        EEPROM.put(0, wifiCount);
        EEPROM.commit();
        EEPROM.end();

        String message = "Wi-Fi credentials saved. Restarting...";
        server.send(200, "text/plain", message);
        delay(2000);
        ESP.restart();
    }
}

void startAP() {
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    byte ip[] = {192, 168, 4, 1};
    byte gateway[] = {192, 168, 4, 1}; 
    byte subnet[] = {255, 255, 255, 0};
    WiFi.softAPConfig(ip, gateway, subnet);
    Serial.println("Access point started.");

    server.on("/", handleRoot);
    server.on("/submit", handleWifiConfig);
    server.begin();
}

void scanWiFi() {
    Paint_NewImage(blackImage, EPD_2IN9_WIDTH, EPD_2IN9_HEIGHT, 90, WHITE);  
    Paint_SelectImage(blackImage);
    Paint_DrawBitMap(gImage_2in9);
    EPD_2IN9_Display(blackImage);

    EPD_2IN9_Init(EPD_2IN9_PART);
    Paint_SelectImage(blackImage);

    int n = WiFi.scanNetworks();

    if (n == 0) {
        Paint_DrawString_EN(195, 70 , "No networks found", &Font12, WHITE, BLACK);
    } else {
        n = min(n, 7);
        for (int i = 0; i < n; i++) {
            String ssid = String(i + 1) + ": " + WiFi.SSID(i);
            ssid = ssid.substring(0, 15);
            Paint_DrawString_EN(190, 35 + i * 12, ssid.c_str(), &Font12, WHITE, BLACK);
        }
    }

    EPD_2IN9_Display(blackImage);
    DEV_Delay_ms(2000);

    for (int z = 0; z < n; z++) {
        for (int i = 0; i < wifiCount; i++) {
            int addr = i * 64;
            String ssid; 
            for (int j = 0; j < 32; j++) ssid += char(EEPROM.read(addr++)); 
            String password;
            for (int j = 0; j < 32; j++) password += char(EEPROM.read(addr++));
            if (strcmp(ssid.c_str(), WiFi.SSID(z).c_str()) == 0) {
                connectToWiFi(ssid.c_str(), password.c_str());
                return;  
            } 
        }
    }
    
    if (WiFi.status() != WL_CONNECTED) {
        startAP();
        EPD_2IN9_Init(EPD_2IN9_FULL);
        Paint_DrawBitMap(logo);
        EPD_2IN9_Display(blackImage);
        EPD_2IN9_Sleep();
    }
}


void RetrieveAllIds()
{
	HTTPClient http;
	http.begin(ids_url);
	int httpCode = http.GET();

    if (httpCode != 201)
    {
        Serial.printf("HTTP Get Failed, code: %d\r\n", httpCode);
        return;
    }

    Serial.printf("HTTP Get Code: %d\r\n", httpCode);
    DeserializationError error = deserializeJson(imageJsonDocument, http.getStream());

    if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
    }

    totalImages = imageJsonDocument["results"]; 
    JsonArray nums = imageJsonDocument["data"]["data"];

    for (int i = 0; i < totalImages; i++)
    {
        imageIds[i] = nums[i]["_id"];   
    }

	http.end();
}

void getEpaperImgData(const char* id)
{
    String address = img_url;
    address += id;
    HTTPClient http;
	http.begin(address.c_str());
	int httpCode = http.GET();

    if (httpCode != 200)
    {
        Serial.printf("HTTP Get Failed, code: %d\r\n", httpCode);
        return;
    }

    Serial.printf("HTTP Get Code: %d\r\n", httpCode);
    DeserializationError error = deserializeJson(idJsonDocument, http.getStream());

    if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
    }
    const char *hexData = idJsonDocument["data"]["data"]["EpaperImgData"];

    int binaryLen = strlen(hexData)/2; 

    for(int i = 0;  i < binaryLen; i++, hexData += 2)
    {
        sscanf(hexData, "%02hhX", &buffer[i]);
    }
	http.end();
}

/* Entry point ----------------------------------------------------------------*/
void setup()
{
    DEV_Module_Init();
  
    EPD_2IN9_Init(EPD_2IN9_FULL);
    EPD_2IN9_Clear();
    EEPROM.begin(EEPROM_SIZE);
    wifiCount = EEPROM.read(0);
    scanWiFi();
    return;
    RetrieveAllIds();
}

/* The main loop -------------------------------------------------------------*/
void loop()
{
    server.handleClient();
    if (WiFi.status() == WL_CONNECTED) {  
        for (int i = 0; i < totalImages; i++){
            if(i % 1 == 0) {
                    EPD_2IN9_Init(EPD_2IN9_FULL);
            } else {
                    EPD_2IN9_Init(EPD_2IN9_PART);
            }
            getEpaperImgData(imageIds[i]);
            Paint_Clear(WHITE);

            Paint_DrawBitMap(buffer);

            EPD_2IN9_Display(blackImage);
            EPD_2IN9_Sleep();
            DEV_Delay_ms(60000);
        }
        RetrieveAllIds();     
    } 
}


