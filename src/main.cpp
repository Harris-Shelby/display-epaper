#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include "../lib/ArduinoJson/ArduinoJson.h"

#define JSON_BUFFER_SIZE 12 * 1024
#define NUM_BUFFER_SIZE 2 * 1024
#define NUM_IMAGES_LIMIT 200 // assuming there won't be more than 200 images

DynamicJsonDocument idJsonDocument(JSON_BUFFER_SIZE);
DynamicJsonDocument imageJsonDocument(NUM_BUFFER_SIZE);
const int IMAGE_SIZE = ((EPD_2IN9_WIDTH % 8 == 0) ? (EPD_2IN9_WIDTH / 8) : (EPD_2IN9_WIDTH / 8 + 1)) * EPD_2IN9_HEIGHT;
unsigned char buffer[IMAGE_SIZE];
const char *imageIds[NUM_IMAGES_LIMIT];
int totalImages;
const char *ids_url = "http://45.88.179.159:4000/api/v1/EpaperImg?fields='id'"; 
const char *img_url = "http://45.88.179.159:4000/api/v1/EpaperImg/"; 

unsigned char blackImage[IMAGE_SIZE];
const char *status = NULL; 

const char *ssid = "GL-MT1300-08c"; //"your ssid";
const char *password = "goodlife";   //"your password";

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
    connectToWiFi(ssid, password);
    EPD_2IN9_Init(EPD_2IN9_FULL);
    EPD_2IN9_Clear();
    DEV_Delay_ms(1000);
    RetrieveAllIds();
    // DEV_Delay_ms(1000);
 
#if 1   //show image for array  
    Paint_NewImage(blackImage, EPD_2IN9_WIDTH, EPD_2IN9_HEIGHT, 270, WHITE);  
    printf("show image for array\r\n");
    Paint_SelectImage(blackImage);
#endif

#if 1   // Drawing on the image
    printf("Drawing\r\n");
    //1.Select Image
    Paint_SelectImage(blackImage);
    Paint_Clear(WHITE);

    // 2.Drawing on the image
    printf("Drawing:blackImage\r\n");
    Paint_DrawPoint(10, 80, BLACK, DOT_PIXEL_1X1, DOT_STYLE_DFT);
    Paint_DrawPoint(10, 90, BLACK, DOT_PIXEL_2X2, DOT_STYLE_DFT);
    Paint_DrawPoint(10, 100, BLACK, DOT_PIXEL_3X3, DOT_STYLE_DFT);

    Paint_DrawLine(20, 70, 70, 120, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(70, 70, 20, 120, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);

    Paint_DrawRectangle(20, 70, 70, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawRectangle(80, 70, 130, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);

    Paint_DrawCircle(45, 95, 20, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawCircle(105, 95, 20, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);

    Paint_DrawLine(85, 95, 125, 95, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawLine(105, 75, 105, 115, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);

    Paint_DrawString_EN(10, 0, "waveshare", &Font16, BLACK, WHITE);
    Paint_DrawString_EN(10, 20, "hello world", &Font12, WHITE, BLACK);

    Paint_DrawNum(10, 33, 123456789, &Font12, BLACK, WHITE);
    Paint_DrawNum(10, 50, 987654321, &Font16, WHITE, BLACK);

    Paint_DrawString_CN(130, 0,"你好abc", &Font12CN, BLACK, WHITE);
    Paint_DrawString_CN(130, 20, "微雪电子", &Font24CN, WHITE, BLACK);

    EPD_2IN9_Display(blackImage);
    DEV_Delay_ms(2000);
#endif

#if 1   //Partial refresh, example shows time        
    printf("Partial refresh\r\n");
    EPD_2IN9_Init(EPD_2IN9_PART);
    Paint_SelectImage(blackImage);
    PAINT_TIME sPaint_time;
    sPaint_time.Hour = 12;
    sPaint_time.Min = 34;
    sPaint_time.Sec = 56;
    UBYTE num = 20;
    for (;;) {
        sPaint_time.Sec = sPaint_time.Sec + 1;
        if (sPaint_time.Sec == 60) {
            sPaint_time.Min = sPaint_time.Min + 1;
            sPaint_time.Sec = 0;
            if (sPaint_time.Min == 60) {
                sPaint_time.Hour =  sPaint_time.Hour + 1;
                sPaint_time.Min = 0;
                if (sPaint_time.Hour == 24) {
                    sPaint_time.Hour = 0;
                    sPaint_time.Min = 0;
                    sPaint_time.Sec = 0;
                }
            }
        }
        Paint_ClearWindows(150, 80, 150 + Font20.Width * 7, 80 + Font20.Height, WHITE);
        Paint_DrawTime(150, 80, &sPaint_time, &Font20, WHITE, BLACK);

        num = num - 1;
        if(num == 0) {
            break;
        }
        EPD_2IN9_Display(blackImage);
        DEV_Delay_ms(500);//Analog clock 1s
    }

#endif
//     printf("Clear...\r\n");
//     EPD_2IN9_Init(EPD_2IN9_FULL);
//     EPD_2IN9_Clear();

//     printf("Goto Sleep...\r\n");
//     EPD_2IN9_Sleep();
//     free(blackImage);
    // blackImage = NULL;
}

/* The main loop -------------------------------------------------------------*/
void loop()
{
    // DEV_Delay_ms(6000);
    // EPD_2IN9_Init(EPD_2IN9_PART);
    // getEpaperImgData();
    // Paint_SelectImage(blackImage);
    // // Paint_Clear(WHITE);
    // Paint_DrawBitMap(buffer);
    // EPD_2IN9_Display(blackImage);    

    for (int i = 0; i < totalImages; i++)
    {
        /* code */
        if(i % 5 == 0) {
                EPD_2IN9_Init(EPD_2IN9_FULL);
        } else {
                EPD_2IN9_Init(EPD_2IN9_PART);
        }
        getEpaperImgData(imageIds[i]);
        Paint_Clear(WHITE);

        Paint_DrawBitMap(buffer);

        EPD_2IN9_Display(blackImage);
        DEV_Delay_ms(2000);
    }
//   while(1);
    RetrieveAllIds();
    delay(1000);
}
