/* Includes ------------------------------------------------------------------*/
#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "../lib/ArduinoJson/ArduinoJson.h"

DynamicJsonDocument JSON_Buffer(12*1024);
DynamicJsonDocument Num_Buffer(1*1024);

//Create a new image cache
unsigned char buffer[12*1024];
const char* Numbuffer[1*1024];

int results;

UBYTE *BlackImage;

const char *status = NULL; 

const char *ssid = "GL-MT1300-08c"; //"your ssid";
const char *password = "goodlife";   //"your password";

const char *serverNum = "http://45.88.179.159:4000/api/v1/EpaperImg?fields='id'"; 

const char *server = "http://45.88.179.159:4000/api/v1/EpaperImg/"; 


void WiFi_Connect() {
   WiFi.begin(ssid, password);

   Serial.print("Connecting to ");
   Serial.println(ssid);

   while (WiFi.status() != WL_CONNECTED) {
       delay(300);
       Serial.print(".");
   }

   Serial.println("WiFi connected");
   Serial.print("IP address: ");
   Serial.println(WiFi.localIP());

} 

void getAllData()
{
	HTTPClient http;
	http.begin(serverNum);
	int httpCode = http.GET();

	if (httpCode > 0)
	{
		Serial.printf("HTTP Get Code: %d\r\n", httpCode);

		if (httpCode == 201) // 收到正确的内容
		{
            delay(300);
            DeserializationError error = deserializeJson(Num_Buffer, http.getStream());

            if (error) {
                Serial.print("deserializeJson() failed: ");
                Serial.println(error.c_str());
                return;
            }
            results = Num_Buffer["results"]; // 3
            JsonArray nums = Num_Buffer["data"]["data"];
            for (int i = 0; i < results; i++)
            {
                /* code */
                const char* newdata = nums[i]["_id"];                
                Numbuffer[i] = newdata;
            }
		}
	}
	else
	{
		Serial.printf("HTTP Get Error: %s\n", http.errorToString(httpCode).c_str());
	}

	http.end();
}

void getEpaperImgData(const char* id)
{
	char address[100];
    strcpy(address, server);
    strcat(address, id); 
    HTTPClient http;
	http.begin(address); //HTTP begin
	int httpCode = http.GET();

	if (httpCode > 0)
	{
		// httpCode will be negative on error
		Serial.printf("HTTP Get Code: %d\r\n", httpCode);

		if (httpCode == 200) // 收到正确的内容
		{
            delay(300);
            DeserializationError error = deserializeJson(JSON_Buffer, http.getStream());

            if (error) {
                Serial.print("deserializeJson() failed: ");
                Serial.println(error.c_str());
                return;
            }
            const char* data_data_0_EpaperImgData = JSON_Buffer["data"]["data"]["EpaperImgData"];

            int buffer_half_len;

            buffer_half_len = strlen(data_data_0_EpaperImgData)/2;  // avoiding division in the for loop;

            for(int i = 0;  i < buffer_half_len; i++, data_data_0_EpaperImgData += 2)
            {
                sscanf(data_data_0_EpaperImgData, "%02hhX", &buffer[i]);
            }
		}
	}
	else
	{
		Serial.printf("HTTP Get Error: %s\n", http.errorToString(httpCode).c_str());
	}

	http.end();
}

/* Entry point ----------------------------------------------------------------*/
void setup()
{
    DEV_Module_Init();
    WiFi_Connect();
    EPD_2IN9_Init(EPD_2IN9_FULL);
    EPD_2IN9_Clear();
    DEV_Delay_ms(1000);
    getAllData();
    DEV_Delay_ms(1000);
 
    /* you have to edit the startup_stm32fxxx.s file and set a big enough heap size */
    UWORD Imagesize = ((EPD_2IN9_WIDTH % 8 == 0)? (EPD_2IN9_WIDTH / 8 ): (EPD_2IN9_WIDTH / 8 + 1)) * EPD_2IN9_HEIGHT;
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        while(1);
    }
    // printf("Paint_NewImage\r\n");
    // Paint_NewImage(BlackImage, EPD_2IN9_WIDTH, EPD_2IN9_HEIGHT, 270, WHITE);


#if 1   //show image for array  
    Paint_NewImage(BlackImage, EPD_2IN9_WIDTH, EPD_2IN9_HEIGHT, 270, WHITE);  
    printf("show image for array\r\n");
    Paint_SelectImage(BlackImage);
    for (int i = 0; i < results; i++)
    {
        /* code */
        getEpaperImgData(Numbuffer[i]);
        Paint_Clear(WHITE);

        Paint_DrawBitMap(buffer);

        EPD_2IN9_Display(BlackImage);
        DEV_Delay_ms(2000);
    }
#endif

// #if 1   // Drawing on the image
//     printf("Drawing\r\n");
//     //1.Select Image
//     Paint_SelectImage(BlackImage);
//     Paint_Clear(WHITE);

//     // 2.Drawing on the image
//     printf("Drawing:BlackImage\r\n");
//     Paint_DrawPoint(10, 80, BLACK, DOT_PIXEL_1X1, DOT_STYLE_DFT);
//     Paint_DrawPoint(10, 90, BLACK, DOT_PIXEL_2X2, DOT_STYLE_DFT);
//     Paint_DrawPoint(10, 100, BLACK, DOT_PIXEL_3X3, DOT_STYLE_DFT);

//     Paint_DrawLine(20, 70, 70, 120, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
//     Paint_DrawLine(70, 70, 20, 120, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);

//     Paint_DrawRectangle(20, 70, 70, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
//     Paint_DrawRectangle(80, 70, 130, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);

//     Paint_DrawCircle(45, 95, 20, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
//     Paint_DrawCircle(105, 95, 20, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);

//     Paint_DrawLine(85, 95, 125, 95, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
//     Paint_DrawLine(105, 75, 105, 115, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);

//     Paint_DrawString_EN(10, 0, "waveshare", &Font16, BLACK, WHITE);
//     Paint_DrawString_EN(10, 20, "hello world", &Font12, WHITE, BLACK);

//     Paint_DrawNum(10, 33, 123456789, &Font12, BLACK, WHITE);
//     Paint_DrawNum(10, 50, 987654321, &Font16, WHITE, BLACK);

//     Paint_DrawString_CN(130, 0,"你好abc", &Font12CN, BLACK, WHITE);
//     Paint_DrawString_CN(130, 20, "微雪电子", &Font24CN, WHITE, BLACK);

//     EPD_2IN9_Display(BlackImage);
//     DEV_Delay_ms(2000);
// #endif

// #if 1   //Partial refresh, example shows time        
//     printf("Partial refresh\r\n");
//     EPD_2IN9_Init(EPD_2IN9_PART);
//     Paint_SelectImage(BlackImage);
//     PAINT_TIME sPaint_time;
//     sPaint_time.Hour = 12;
//     sPaint_time.Min = 34;
//     sPaint_time.Sec = 56;
//     UBYTE num = 20;
//     for (;;) {
//         sPaint_time.Sec = sPaint_time.Sec + 1;
//         if (sPaint_time.Sec == 60) {
//             sPaint_time.Min = sPaint_time.Min + 1;
//             sPaint_time.Sec = 0;
//             if (sPaint_time.Min == 60) {
//                 sPaint_time.Hour =  sPaint_time.Hour + 1;
//                 sPaint_time.Min = 0;
//                 if (sPaint_time.Hour == 24) {
//                     sPaint_time.Hour = 0;
//                     sPaint_time.Min = 0;
//                     sPaint_time.Sec = 0;
//                 }
//             }
//         }
//         Paint_ClearWindows(150, 80, 150 + Font20.Width * 7, 80 + Font20.Height, WHITE);
//         Paint_DrawTime(150, 80, &sPaint_time, &Font20, WHITE, BLACK);

//         num = num - 1;
//         if(num == 0) {
//             break;
//         }
//         EPD_2IN9_Display(BlackImage);
//         DEV_Delay_ms(500);//Analog clock 1s
//     }

// #endif
//     printf("Clear...\r\n");
//     EPD_2IN9_Init(EPD_2IN9_FULL);
//     EPD_2IN9_Clear();

    // printf("Goto Sleep...\r\n");
    // EPD_2IN9_Sleep();
    // free(BlackImage);
    // BlackImage = NULL;
}

/* The main loop -------------------------------------------------------------*/
void loop()
{
    // DEV_Delay_ms(6000);
    // EPD_2IN9_Init(EPD_2IN9_PART);
    // getEpaperImgData();
    // Paint_SelectImage(BlackImage);
    // // Paint_Clear(WHITE);
    // Paint_DrawBitMap(buffer);
    // EPD_2IN9_Display(BlackImage);

  while(1);
}
