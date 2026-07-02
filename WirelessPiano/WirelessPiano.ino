#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>

char* ssid = "WIFI-Name";
char* password = "WIFI-Password";

WebServer server(80);

const int NOK = 12; // number of keys
const int KPins[12] ={4, 5, 14, 18, 19, 21, 22, 23, 25, 26, 27, 33};
bool KState[NOK]; // State of each key

const int notes[NOK] =
{
  262, // C4
  277, // C#4
  294, // D4
  311, // D#4
  330, // E4
  349, // F4
  370, // F#4
  392, // G4
  415, // G#4
  440, // A4
  466, // A#4
  494  // B4
};

bool wifiMode;

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

void setupWifi()
{
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
}

void standalonePiano()
{
  bool notePlay = false;

  for(int i = 0; i < NOK; i++)
  {
    if(digitalRead(KPins[i]) == LOW)
    {
      tone(13, notes[i]);

      notePlay = true;
    }
  }

  if(!notePlay)
  {
    noTone(13);
  }
}

void setup()
{
  Serial.begin(115200);

  for(int i = 0; i < NOK; i++)
  {
    pinMode(KPins[i], INPUT_PULLUP);
  }

  pinMode(32, INPUT);

  wifiMode = digitalRead(32);

  if(wifiMode)
  {
    setupWifi();
    Serial.println("Wifi Mode");
  }
  else
  {
    Serial.println("Standalone Mode");
  }

  Serial.println("Server Ready");
}

void loop()
{
  bool newMode = digitalRead(32);

  if(newMode != wifiMode)
  {
    wifiMode = newMode;

    if(wifiMode)
    {
      setupWifi();
    }
    else
    {
      server.stop();
      WiFi.disconnect(true);
      WiFi.mode(WIFI_OFF);
    }
  }

  if(wifiMode)
  {
    server.handleClient();
    Serial.println("Wifi Mode");
  }
  else
  {
    standalonePiano();
    Serial.println("Standalone Mode");
  }
}
