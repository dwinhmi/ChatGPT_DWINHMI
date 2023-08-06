/*
 * -----------------------------------------------------------------------------------------
 * ChatGPT DWIN HMI Demo  | Author : Tejeet ( tejdwin@gmail.com )
 * -----------------------------------------------------------------------------------------
 * This is Simple ChatGPT Demo Using DWIN HMI and ESP32 Microcontroller Board
 * 
 * Example sketch/program showing how to search on ChatGPT DWIN UI 
 * Search response is shown on the Screen window of DWIN UI
 * 
 * DWIN HMI to ESP32 Boards Pinout Connection
 * -----------------------------------------------------------------------------------------
 * DWIN            ESP32                               
 * Pin             Pin           
 * -----------------------------------------------------------------------------------------
 * 5V              Vin          
 * GND             GND         
 * RX2             16         
 * TX2             17         
 *------------------------------------------------------------------------------------------
 *
 * For More information Please Mail : Tejeet@dwin.com.cn
 *
 */


#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <DWIN.h>

// Replace with your network credentials
const char* WIFI_SSID     = "ABCD";
const char* WIFI_PASSWORD = "qwerty12345@6";

// Replace with your OpenAI API key
const char* apiKey = "sk-2URpSXWeHxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

// Create HMI Object
#define DGUS_SERIAL Serial2
DWIN hmi(DGUS_SERIAL, 16, 17);

String question = "";

// Event Occura when response comes from HMI
void onHMIEvent(String address, int lastByte, String messege, String response){  
  Serial.println("OnEvent : [ A : " + address + " | D : "+ String(lastByte, HEX)+ " | M : "+messege+" | R : "+response+ " ]"); 

    if(messege != ""){
      question = messege;
    }
    hmi.setVP(0x3010, 00);
    if (address == "2000" && (messege != "")){
        Serial.println("Entered Keyboard ");
        hmi.setVP(0x3010, 02);
        askChatGPT(question);
    } else if (address == "3010" && (messege != "")) {
        Serial.println("ASK Button Clicked");
        hmi.setVP(0x3010, 02);
        askChatGPT(question);
    }
}

void initWiFi(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print('.');
      delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void setup() {

  Serial.begin(115200);
  
  hmi.echoEnabled(false);
  hmi.hmiCallBack(onHMIEvent);
  hmi.restartHMI();

  // Initialize WiFi
  initWiFi();

}

void loop() {
  // Listen HMI Events
  hmi.listen();
}


void askChatGPT(String question){
  // Send request to OpenAI API
  hmi.setText(0x2050, "                                                                                                                                                                                           ");
  String inputText = question;
  String apiUrl = "https://api.openai.com/v1/completions";
  String payload = "{\"prompt\":\"" + inputText + "\",\"max_tokens\":100, \"model\": \"text-davinci-003\"}";

  HTTPClient http;
  http.begin(apiUrl);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + String(apiKey));
  
  int httpResponseCode = http.POST(payload);
  if (httpResponseCode == 200) {
    String response = http.getString();
  
    // Parse JSON response
    DynamicJsonDocument jsonDoc(1024);
    deserializeJson(jsonDoc, response);
    String outputText = jsonDoc["choices"][0]["text"];
    Serial.println("ChatGPT Response -->" + outputText);
    hmi.setVP(0x3010, 00);
    hmi.setText(0x2050, outputText);
  } else {
    Serial.printf("Error %i \n", httpResponseCode);
  }
}