/*
Get Reddit Post using ESP32
by R.Pelayo for Teach Me Microcontrollers

Full tutorial: teachmemicro.com/esp32-get-reddit-post-using-api

*/

#include <WiFi.h>
#include <HTTPClient.h>
#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// other possible filters: month, year, week, hour
#define FILTER "day"
// the subreddit where we want to get data
#define SUBREDDIT "showerthoughts"

LiquidCrystal_I2C lcd(0x27, 20, 4);
const char* ssid = "<Your WiFi SSID>";
const char* password = "<Your WiFi Password>";

DynamicJsonDocument doc(16384); // the value here is according to https://arduinojson.org/v6/assistant/

void setup() {
  lcd.init();
  lcd.backlight();

  Serial.begin(9600);

  if(connectToWiFi()){
    lcd.clear();
    Serial.println(WiFi.localIP());
    String redditPost = getRedditFirstPost(SUBREDDIT);
    Serial.println(redditPost);
    printToLCD(redditPost);
  }
}

// Routine for connecting the ESP32 to WiFi with internet
bool connectToWiFi(){
  WiFi.mode(WIFI_STA);
  
  Serial.println("Starting WiFi Station...");

  WiFi.begin(ssid, password);

  int retrycon = 50;

  while (WiFi.status() != WL_CONNECTED)
  {  
    Serial.println("Connecting...");
    delay(500);
    if (--retrycon == 0)
    {
      Serial.println("RESTART");
      ESP.restart();
    }
    Serial.print(".");
  }

  Serial.print("WiFi connected with IP: ");
  return true;
}

// Routine for printing reddit post to 20x4 LCD
// required becayse 20x4 LCD does not automatically wrap excess characters to next line
void printToLCD(String redditPost){
  lcd.setCursor(0,0);
  int limit = redditPost.length() > 80 ? 80 : redditPost.length();
  for(int i=0;i<limit;i++){
    if(i==21){
        lcd.setCursor(0,1);
    }else if(i==41){
        lcd.setCursor(0,2);
    }else if(i==61){
        lcd.setCursor(0,3);
    }
    lcd.print(redditPost.charAt(i));
  }
}

// Routine for getting the first post from a subreddit
String getRedditFirstPost(String subReddit){
  HTTPClient http_reddit;
  String reddit_json = "";
  String reddit_server = "https://www.reddit.com/r/";
  reddit_server+=subReddit;
  reddit_server+="/top.json?limit=1&t=";
  reddit_server+=FILTER;

  http_reddit.begin(reddit_server);
  int resp = http_reddit.GET();

  if(resp > 0){
     reddit_json = http_reddit.getString();
  }else{
    Serial.println("Cannot get reddit post");
    return "";
  }
  //Serial.println(reddit_json);
  http_reddit.end();

  deserializeJson(doc, reddit_json);
  JsonObject data = doc["data"];
  JsonObject data_children_0_data = data["children"][0]["data"];
  const char* data_children_0_data_title = data_children_0_data["title"];
  const char* data_children_0_data_selftext = data_children_0_data["selftext"];
  String reddit_post = data_children_0_data_title;
  reddit_post += data_children_0_data_selftext;
  Serial.print("Post: ");
  return String(reddit_post);

}

void loop() {
  // insert time-based data fetching here
}
