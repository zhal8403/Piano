#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>

char* ssid = "NETGEAR31";
char* password = "greensocks474";

WebServer server(80);

const int NOK = 4; // number of keys
const int KPins[NOK] = {14, 27,26, 25}; // pins keys are on
bool KState[NOK]; // State of each key

void sendFile(const char* path, const char* type)
{
  File file = LittleFS.open(path, "r");

  if (!file)
  {
    server.send(404, "text/plain", "File Not Found");
    return;
  }

  server.streamFile(file, type);

  file.close();
}

void handleKeys()
{
  String json = "{";

  for(int i = 0; i < NOK; i++)
  {
    bool pressed = digitalRead(KPins[i]) == LOW;

    //create json like: {"1":1,"2":0,"3":1}
    json += "\"";
    json += String(i+1);
    json += "\":";

    if(pressed)
    {
      json += "1";
    }
    else
    {
      json += "0";
    }

    if(i < NOK-1)
    {
      json += ",";
    }
  }
  json += "}";

  server.send(200, "application/json", json);
}

void setup()
{
  Serial.begin(115200);

  for(int i = 0; i < NOK; i++)
  {
    pinMode(KPins[i], INPUT_PULLUP);
  }

  if(!LittleFS.begin(true))
  {
    Serial.println("LittleFs Failed");

    while(true); //stops if failed
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("connecting");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Connected");

  Serial.print("Open browser to: http://");
  Serial.println(WiFi.localIP());

  //links html file
  server.on("/", []()
  {
    sendFile("/index.html", "text/html");
  });

  //links css file
  server.on("/style.css", []()
  {
    sendFile("/style.css", "text/css");
  });

  //links js file
  server.on("/script.js", []()
  {
    sendFile("/script.js", "application/javascript");
  });

  server.on("/keys", handleKeys);

  server.begin();
  Serial.println("Server Ready");
}

void loop()
{
  server.handleClient();
}