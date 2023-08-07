#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#define APSSID "Czytnik licznikow"
#define APPSK  "123456789"
const char *ssid = APSSID;
const char *password = APPSK;
float energia, przeplyw, godziny;
char *nazwy[] = { "Gj", "m3", "godz" };
char *k;
int blad;
int to;
ESP8266WebServer server(80);
String StronaGlowna() {
  String page = "<html lang=fr-FR><head><meta http-equiv='refresh' content='3'/>";
  page += "<title>Praca inzynierska Piotr Trzewik</title>";
  page += "<style> body { background-color: #fffff; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }</style>";
  page += "</head><body><h30>Odczyt Licznikow</h30>";
  page += "<ul><li>Energia : ";
  page += energia;
  page += " Gj</li>";
  page += "<li>Przeplyw : ";
  page += przeplyw;
  page += " m3</li>";
  page += "<li>Czas pracy : ";
  page += godziny;
  page += " godz</li></ul></h30>";
  page += "</body></html>";
  return page;
}
void handleRoot()
{
server.send(200, "text/html", StronaGlowna());
}
SoftwareSerial Glowica(13, 15);  

void setup () {
  WiFi.softAP(ssid, password);
  server.begin();
  server.on("/", handleRoot);
  Serial.begin(57600);
  pinMode(13, INPUT);
  pinMode(15, OUTPUT);
  delay(100);
}
bool evenspr(unsigned input) {
bool wejscie = input & 128;
int x = input & 127;
int parzystosc = 0;
  while (x != 0) {
    parzystosc ^= x;
    x >>= 1;
  }
  if ( parzystosc & 0x1 != wejscie )
    return (1);
  else
    return (0);
}

void loop ()
{
 byte wyslij[] = { 175, 163, 177 };        
 byte s  = 0;
 int i;
 char wiadomosc[255];
  
  Glowica.begin(300);
  for (int x = 0; x < 3; x++) {
    Glowica.write(wyslij[x]);
  }

  Glowica.flush();
  Glowica.begin(1200);

  to = 0;
  s = 0;
  i = 0;

  blad = 0;
 
 
  while (s != 0x0A)
  {
    if (Glowica.available())
    {
     s= Glowica.read();
      if (evenspr(s))
      {
        blad++;
      }
      s = s & 127; 
      wiadomosc[i++] = char(s);

    }
    else
    {
      to++;
      delay(25);
    }
    if (i >= 24)
    {
      if ( blad == 0 )
      {
        k = strtok(wiadomosc, " ");
        energia = atol(k) / 100.0;
        Serial.print(energia);
        Serial.println(nazwy[0]);
        k = strtok(NULL, " ");
        przeplyw = atol(k) / 100.0;
        Serial.print(przeplyw);
        Serial.println(nazwy[1]);
        k = strtok(NULL, " ");
        godziny = atol(k);
        Serial.print(godziny);
        Serial.print (" ");
        Serial.println(nazwy[2]);
        Serial.println(" ");
      }
      
      else
      {
        Serial.print("blad parzystosci " );
        wiadomosc[i] = 0;
        Serial.println(wiadomosc);
       }
      break;
    }
    if (to > 100)
    {
      wiadomosc[i] = 0;
      Serial.print("koniec czasu" );
      Serial.println(wiadomosc);
      energia =0;
      przeplyw =0;
      godziny =0;
      break;
    }
   server.handleClient();
  }
 delay(2000);

}
