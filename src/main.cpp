#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUDP.h>
#include <WakeOnLan.h>
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"

const char *ssid = "SSID";
const char *password = "KEY";
AsyncWebServer server(80);
WiFiUDP UDP;
WakeOnLan WOL(UDP);

void logWebServer();

void setup()
{
  // put your setup code here, to run once:

  //
  // シリアル通信の有効化
  //
  Serial.begin(115200);
  delay(5000);
  Serial.println();

  //
  // SPIFFS（ファイルシステム）の有効化
  //
  SPIFFS.begin();

  //
  // Wi-Fiに接続
  //

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  //
  // WebServerの実装
  //

  server.on("/api/wol", HTTP_ANY, [](AsyncWebServerRequest *server) {
    logWebServer();
    String target = "";

    if (server->method() == HTTP_GET)
    {
      // レスポンスを返す
      server->send(200, "application/json", "{\"status\":\"ready\"}");
    }

    if (server->method() == HTTP_POST)
    {
      Serial.print("[WOL]");

      // MACアドレスを取得
      target=server->arg("mac");
      Serial.print(target);

      // WOLを実行
      WOL.sendMagicPacket(target.c_str());
      Serial.println("...done");
      // レスポンスを返す
      server->send(200, "application/json", "{\"status\":\"ok\",\"target\":\"" + target + "\"}");
    }
  });

  server.on("/wol", HTTP_GET, [](AsyncWebServerRequest *server) {
    logWebServer();
    File file = SPIFFS.open("/wol.html", "r");
    server->send(200, "text/html", file.readString());
    file.close();
  });

  server.onNotFound([](AsyncWebServerRequest *server) {
    server->send(404, "text/html", "<h1>Not Found</h1>");
  });

  // CORSを有効化する
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "*");

  // WebServerを開始する
  server.begin();
  Serial.println("Server started.");

  //
  // セットアップ完了！
  //
  Serial.println("Ready!");
}

void loop()
{
  // put your main code here, to run repeatedly:
}

// Webサーバーへのアクセスをコンソールに出力する関数
void logWebServer()
{
  Serial.print("[HTTP][");
  // switch (server->method())
  // {
  // case HTTP_GET:
  //   Serial.print("GET");
  //   break;
  // case HTTP_POST:
  //   Serial.print("POST");
  // default:
  //   break;
  // }
  Serial.print("]");
  // Serial.println(server.uri());
}