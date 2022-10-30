#pragma once

#include "JsonHandler.h"
#include <map>
#include <forward_list>
#include <string>
// #include <stdexcept>  
// #include <TimeLib.h> 

char fullPath[200] = "";
char valueBuffer[50] = "";

/*
 * This structure is used within the WeatherEpaperDataHandler implementation.
 * It stores the values from the JSON we care about. Everything else 
 * is discarded.
 */
struct EpaperData
{
    char    ImgData[4736];
};

// Example: LIST - Store a list of all EpaperDatas
std::forward_list<EpaperData> myEpaperDatas;
std::forward_list<EpaperData>::iterator myEpaperDatas_it;

// Example: MAP  - Store a map of stuff we only care about that the parser sees.
std::map<std::string, float>mymap = {
    { "list[0].EpaperImgData",             0},
};
std::map<std::string, float>::iterator it;


// Virtual Handler class implementation
class WeatherEpaperDataHandler: public JsonHandler
{
  
    /* Open weather maps returns something like:
     *  
      cod': "200"
      message': 0.003200
      cnt': 36
      list[0].dt': 1487246336
      list[0].main.temp': 286.670013
      list[0].main.temp_min': 281.556000
      list[0].main.temp_max': 286.670013
      list[0].main.pressure': 972.729980
      list[0].main.sea_level': 1046.459961
      list[0].main.grnd_level': 972.729980
      list[0].main.humidity': 75
      list[0].main.temp_kf': 5.110000
      list[0].weather[0].id': 800
      list[0].weather[0].main': "Clear"
      start Object
      Current key is: dt
      start Object
      Current key is: temp
      Current key is: temp_min
      Current key is: temp_max
      Current key is: pressure
      Current key is: sea_level
      Current key is: grnd_level
      Current key is: humidity
      Current key is: temp_kf
      end Object
      start Object
      Current key is: id
      Current key is: main
      Current key is: description
      Current key is: icon
      end Object
      
      
     */
     
      private:
        EpaperData EpaperData   = { 0 };
       
      public:

      /* Process the value provided at a specific JSON path / element 
       * This forms the core operation of the Handler.
       */
      void value(ElementPath path, ElementValue value) 
      {
          // For debug only.
          memset(fullPath, 0 , sizeof(fullPath));          
          path.toString(fullPath);
          
          /*
          // Uncomment this to see the paths.
          
          Serial.print(fullPath); Serial.print("': ");
          Serial.println(value.toString(valueBuffer));
          */
                
          // Map Example 1: Based on path string
          it = mymap.find((std::string) fullPath);
          if (it != mymap.end())
          {
            Serial.print("-> Found a key we care about: ");
            Serial.print(fullPath);
            Serial.print(" = ");
            Serial.println(value.toString(valueBuffer));

            // Store this in the map
            it->second = value.getFloat();
          }
          

              
          // Tradition State-Machine based parser
          std::string currentKey = std::string(path.getKey());
                   
          if ( !currentKey.compare("EpaperImgData") )           
            EpaperData.ImgData = value.getString();

      } // end value


      // Functions we don't care about in this example
      void startDocument() { }      
      void startObject(ElementPath path) {} // Start of object '{'            
      void endObject(ElementPath path) {}  // End of a EpaperData object '}'           
      void startArray(ElementPath path) {}
      void endArray(ElementPath path) {}
      void whitespace(char c) {}
      
      void endDocument() { 
        myEpaperDatas.reverse(); // Reverse the list so it's most recent to most far away EpaperData
      }          

};