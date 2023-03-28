
#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>

#include <HTTPClient.h>

#include <WiFiClientSecure.h>

#include "https.h"

// 网页证书
const char *rootCACertificate =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIFZjCCBE6gAwIBAgIQPlGHD+eHfRcObcI2vQKYUjANBgkqhkiG9w0BAQsFADBG\n"
    "MQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExM\n"
    "QzETMBEGA1UEAxMKR1RTIENBIDFQNTAeFw0yMzAxMjYxMjMzNTFaFw0yMzA0MjYx\n"
    "MjMzNTBaMBYxFDASBgNVBAMMCyoucnR4dXgueHl6MIIBIjANBgkqhkiG9w0BAQEF\n"
    "AAOCAQ8AMIIBCgKCAQEAmcBlVtHqnejgsiV2c0bBq+hSOrTbMA6kPLwf0fR9c8Im\n"
    "d1XXLw7sNYeljx+E11xgdp/dFMfyxWMzjNbOlz2UL2Q3oXXRX+c3KOYuqV1jTs/0\n"
    "xznbZyDDBCdOvMm/UdByJh9QnkAJj34SE0FcHb2y/on3a2vPcaQpotyXfYC+k/nt\n"
    "ZKr3eaa8yNfVgR0WM8971qh5XziMeMcmZgJHDBt+q+8rFmamfjVprfvseGSyJ4N2\n"
    "RUEGbMcKMmZ6t/weV5momJlAJamRP6lTDlNCQMaMqBfR76gRgSWI0+Zgj+mvwleE\n"
    "ovjLGYGC0ni8dSr+u9yxOk13h47YDFHXmY1nnyu7kwIDAQABo4ICfjCCAnowDgYD\n"
    "VR0PAQH/BAQDAgWgMBMGA1UdJQQMMAoGCCsGAQUFBwMBMAwGA1UdEwEB/wQCMAAw\n"
    "HQYDVR0OBBYEFCBONqstq1NcJWEshqyINC2g0+/mMB8GA1UdIwQYMBaAFNX8ng3f\n"
    "HsrdCJeXbivFX8Ur9ey4MHgGCCsGAQUFBwEBBGwwajA1BggrBgEFBQcwAYYpaHR0\n"
    "cDovL29jc3AucGtpLmdvb2cvcy9ndHMxcDUvaXF1UjJoVDFjcDAwMQYIKwYBBQUH\n"
    "MAKGJWh0dHA6Ly9wa2kuZ29vZy9yZXBvL2NlcnRzL2d0czFwNS5kZXIwIQYDVR0R\n"
    "BBowGIILKi5ydHh1eC54eXqCCXJ0eHV4Lnh5ejAhBgNVHSAEGjAYMAgGBmeBDAEC\n"
    "ATAMBgorBgEEAdZ5AgUDMDwGA1UdHwQ1MDMwMaAvoC2GK2h0dHA6Ly9jcmxzLnBr\n"
    "aS5nb29nL2d0czFwNS9obThRb0Zfb3hKcy5jcmwwggEFBgorBgEEAdZ5AgQCBIH2\n"
    "BIHzAPEAdwB6MoxU2LcttiDqOOBSHumEFnAyE4VNO9IrwTpXo1LrUgAAAYXuSOO3\n"
    "AAAEAwBIMEYCIQCy9HR6+5SJzYlfuNQluLo9jdlMI+5GYknvtA0up34UywIhAOTw\n"
    "PYFfJxY2DPqkPZQvDMhyJOMLL0XpwzzO+iso7rdwAHYArfe++nz/EMiLnT2cHj4Y\n"
    "arRnKV3PsQwkyoWGNOvcgooAAAGF7kjjvwAABAMARzBFAiEA9RCwn2HBPYVcTKAo\n"
    "M3oE7RuAM9KvfsGMDH2DvAxdydMCIBgRNZ/wRERoKWQa86Fe0TehJSLWgrpl7Xis\n"
    "r4st/DJQMA0GCSqGSIb3DQEBCwUAA4IBAQBDuYXOTcVFr+FyQDPuHNBw4PMe2iWi\n"
    "myT+8pfgpA2CBqj2GJpJUfegqJTMdUK8XBdEwPoPvUCVuwMVYDCs/BEbtBx0wLRj\n"
    "wJ5GKEdgWrVDLQNzBVHG3fvBJGIzbH7IclvbMze3czAtYAD9Debk40PMC2s3qFeQ\n"
    "MUdU/9hrQGGGJg//jZoXcD/B8ujgrQxZRV4f3ZpTHO/7+RAK1DqOgSJ2zjmfQ9mx\n"
    "In1zTGW7ncGFAabtJUBEWTKmbfcv9tZbVOFE36Hy7EqtIHkar3jzTpI5yDlDl4Vh\n"
    "pqOTDvHIbSOHdMMXLF53w5W3RRJFaze41SAzxaaTBP3yEmRx9AJzCdi+\n"
    "-----END CERTIFICATE-----\n";

WiFiClientSecure *client;
WiFiMulti WiFiMulti;

// Setting clock
void setClock()
{
  configTime(0, 0, "time.ustc.edu.cn");

  Serial.print(F("Waiting for NTP time sync: "));
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2)
  {
    delay(500);
    Serial.print(F("."));
    yield();
    nowSecs = time(nullptr);
  }

  Serial.println();
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);
  Serial.print(F("Current time: "));
  Serial.print(asctime(&timeinfo));
}

void WIFIandHTTPS_setup()
{

  Serial.begin(115200);

  Serial.println();
  Serial.println();
  Serial.println();

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("LUG", "@makeluggreatagain");

  // wait for WiFi connection
  Serial.print("Waiting for WiFi to connect...");
  while ((WiFiMulti.run() != WL_CONNECTED))
  {
    Serial.print(".");
  }
  Serial.println(" connected");

  setClock();

  client = new WiFiClientSecure;
  client->setInsecure();
  while (1)
  {
    if (client)
    {
      client->setCACert(rootCACertificate);
      return;
    }
    else
    {
      delay(1000);
      client = new WiFiClientSecure;
    }
  }
}

void WIFIandHTTPS_delete()
{
    delete client;
}




void sendActive(String para)
{
  // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is
  HTTPClient https;
  
  Serial.print("[HTTPS] begin...\n");
  if (https.begin(*client, "https://206.rtxux.xyz/api/"+para+"/3VhEwjXFwWgArthU3XCe"))
  {
    // HTTPS
    Serial.print("[HTTPS] GET...\n");
    int httpCode = https.POST("");

    if (httpCode > 0)
    {
      Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
      {
        String payload = https.getString();
        Serial.println(payload);
      }
    }
    else
    {
      Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
       for (int i = 0; i < 1000000; i++)
      {
        digitalWrite(pin_beep, HIGH);//23ok
        delay(0.1);
        digitalWrite(pin_beep, LOW);//23ok
        delay(0.1);
      }


    }

    https.end();
  }
  else
  {
    Serial.printf("[HTTPS] Unable to connect\n");
  }
}
