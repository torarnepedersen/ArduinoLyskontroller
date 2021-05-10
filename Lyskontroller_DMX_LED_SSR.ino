/*
 * LED, Rele og DMX-kontroller av Tor Arne Pedersen
 * Pin 7 Leds oppe (begge sider) (antall 120 på den lengste stripen)
 * Pin 11 Leds ved gulv og setet (antall 270 på den lengste stripen)
 * Pin 10 rele røykmaskin. Dette er et SSR Low trigger rele, så det settes til LOW for 
 * å skru røykmaskin på. NB: SSR (solid state relay) trenger last for å fungere.
 * Strobe kanal 001, value 000 blackout, 255 maks,
 * kanal 002 flash duration 0-255, kanal 003, value 6-255 0-30hz
 * Laser DMX-kanal 31, value 000-027 off, 168-195 sound1, 196-223 sound2, 224-251 sound3
 * Les bruksanvisningen til dine DMX-kontrollerbare enheter for å vite kanaler og verdier.
 * 
 * Siden det styres mange ting på en gang med samme Arduino her kan vi ikke bruke delay() i koden
 * (hvis feks røykmaskinen bruker delay() vil LED-stripene stå fryst inntil delay() er ferdig.
 * Derfor brukes her et ekstra bibliotek som timer.
 */
#include <Adafruit_NeoPixel.h>
#include <arduino-timer.h>
#include <Conceptinetics.h> //DMX-bibliotek
//#include <SoftwareSerial.h> //For seriell kommunikasjon over bt - ikke i bruk per se
//#include <ArduinoBlue.h> //For Bluetooth
#define DMX_MASTER_CHANNELS   64 //Bestemm hvor mange DMX-kanaler. Maks 512, men begrenset av minne på Arduino, så sett så lavt som mulig for å dekke dine lamper.
//Hvis du bruker ferdig DMX-shield brukes pin 2 til Rx.
#define RXEN_PIN 2
//Lag objekt 
DMX_Master dmx_master ( DMX_MASTER_CHANNELS, RXEN_PIN );

//Debug-info. Totalt antall piksler som kan adresseres dmx64: 115+270+82=466. Uten DMX:115+268+120=503
//Derfor ble leds til under setet (115) og ved gulv (270) koblet om til samme pin på Arduino.
int AntallPikslerGulv = 270; //Den lengste stripen nede.
int AntallPikslerOppe = 120; //Den lengste stripen oppe, begge stripene oppe kobles på samme pin og adresseres likt på begge sider
//byte Ledsundersetetpin = 12; Koblet fra pga minneforbruk på Arduino Uno.
byte Ledsvedgulvpin = 11;
byte Ledsoppepin = 7; //7
byte Rele = 10; //Røykmaskinen.

int  ForrigePikselOppe5=0; //dette er for å huske siste fem piksler. Kan løses på mange måter, denne er lett lesbar.
int  ForrigePikselOppe4=0;
int  ForrigePikselOppe3=0;
int  ForrigePikselOppe2=0;
int  ForrigePikselOppe1=0;
int  PikselOppe=0;
int PikselGulv=0;
boolean TilfeldigPikselOppe=0;
byte ModusOppe=5;
boolean RunOnceModusOppe=1;
boolean AnnenHver=1;
int Farge=0;
auto timer = timer_create_default();
byte OppeR=127;
byte OppeG=0;
byte OppeB=127;
byte GulvR=127;
byte GulvG=0;
byte GulvB=127;
byte StartOppeR=0;//for effekter som krever at piksel 0 huskes når vi kommer rundt
byte StartOppeG=127;
byte StartOppeB=255;

int Oppdateringsfrekvens=50;
Adafruit_NeoPixel Ledsvedgulv = Adafruit_NeoPixel(AntallPikslerGulv, Ledsvedgulvpin, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel Ledsoppe = Adafruit_NeoPixel(AntallPikslerOppe, Ledsoppepin, NEO_GRB + NEO_KHZ800);

void VelgModus(){
      Farge=random(0,6); //Jeg ønsker relativt rene farger, og ikke for høyt strømforbruk, derav tabell.
      switch (Farge) {
      case 0: GulvR=127; GulvG=0;   GulvB=0; break;     //rød
      case 1: GulvR=0;   GulvG=127; GulvB=0; break;     //grønn
      case 2:  GulvR=0;   GulvG=0;   GulvB=127; break;  //blå
      case 3: GulvR=127; GulvG=127; GulvB=0; break;     //gul
      case 4: GulvR=0;   GulvG=127; GulvB=127; break;   //turkis
      case 5:  GulvR=127; GulvG=0;   GulvB=127; break;  //lilla
      }
  
    for (int i=0; i < AntallPikslerGulv; i++){
    Ledsvedgulv.setPixelColor(i, Ledsvedgulv.Color(GulvR,GulvG,GulvB));
  }
  Ledsvedgulv.show();
    for (int i=0; i < AntallPikslerOppe; i++){
    Ledsoppe.setPixelColor(i, Ledsoppe.Color(0,0,0));
  }
  Ledsoppe.show();

  ModusOppe=random(0,7);
  RunOnceModusOppe=1;
}


//LEDs-effekter:

void NestePiksel(){
  ForrigePikselOppe5=ForrigePikselOppe4; //For å huske de siste fem pikslene.
  ForrigePikselOppe4=ForrigePikselOppe3; //Dette kan også løses vha array og loop.
  ForrigePikselOppe3=ForrigePikselOppe2;
  ForrigePikselOppe2=ForrigePikselOppe1;
  ForrigePikselOppe1=PikselOppe;

  if (TilfeldigPikselOppe==1) {
    PikselOppe=random(0,AntallPikslerOppe);
  }
  else {
    if (PikselOppe<AntallPikslerOppe) {
        PikselOppe++; //Hvis det er flere piksler skal vi gå til neste, hvis ikke start på 0.
      }
      else {
        PikselOppe=0;
      }
  }
  timer.in(Oppdateringsfrekvens, Oppdater); //Ventetid før vi går videre. Styrer hastighet på effekter
  }

void Oppdater() {
  switch (ModusOppe) {
    case 0:
    Oppdateringsfrekvens=50;
    TilfeldigPikselOppe=0;
    if (RunOnceModusOppe==1) {
      Farge=random(0,7);
      switch (Farge) {
      case 0: OppeR=255; OppeG=0;   OppeB=0; break;  
      case 1: OppeR=0;   OppeG=255; OppeB=0; break; 
      case 2:  OppeR=0;   OppeG=0;   OppeB=255; break;
      case 3: OppeR=127; OppeG=127; OppeB=0; break;
      case 4: OppeR=0;   OppeG=0; OppeB=0; break;
      case 5:  OppeR=127; OppeG=0;   OppeB=127; break;
      case 6:  OppeR=0;   OppeG=127;   OppeB=127; break;
      }
      RunOnceModusOppe=0;
    }
    if (PikselOppe==AntallPikslerOppe) {
      RunOnceModusOppe=1;
    }
    Ledsoppe.setPixelColor(PikselOppe, Ledsoppe.Color(OppeR, OppeG, OppeB));
    Ledsoppe.show();
    break;
    case 1:
    Oppdateringsfrekvens=30;
    TilfeldigPikselOppe=1;
    Farge=random(0,6);
      switch (Farge) {
      case 0: OppeR=255; OppeG=0;   OppeB=0; break;
      case 1: OppeR=0;   OppeG=255; OppeB=0; break;
      case 2:  OppeR=0;   OppeG=0;   OppeB=255; break;
      case 3: OppeR=127; OppeG=127; OppeB=0; break;
      case 4: OppeR=0;   OppeG=127; OppeB=127; break;
      case 5:  OppeR=127; OppeG=0;   OppeB=127; break;
      }
    Ledsoppe.setPixelColor(PikselOppe, Ledsoppe.Color(OppeR, OppeG, OppeB));
    Ledsoppe.setPixelColor(ForrigePikselOppe5, Ledsoppe.Color(0, 0, 0));
    Ledsoppe.show();
    break;
    case 2:
      TilfeldigPikselOppe=0;
      if (RunOnceModusOppe==1) {
        Oppdateringsfrekvens=(random(1,3)*25); //tre forskjellige hastigheter på effekten.
        Farge = random(0,6);
        RunOnceModusOppe=0;
      }
      switch (Farge) {
        case 0: OppeR=255; OppeG=0;   OppeB=0; break;
        case 1: OppeR=0;   OppeG=255; OppeB=0; break; 
        case 2:  OppeR=0;   OppeG=0;   OppeB=255; break;
        case 3: OppeR=127; OppeG=127; OppeB=0; break;
        case 4: OppeR=0;   OppeG=127; OppeB=127; break;
        case 5:  OppeR=127; OppeG=0;   OppeB=127; break;
      }
    Ledsoppe.setPixelColor(PikselOppe, Ledsoppe.Color(OppeR, OppeG, OppeB));
    Ledsoppe.setPixelColor(ForrigePikselOppe1, Ledsoppe.Color(OppeR/5*4, OppeG/5*4, OppeB/5*4));
    Ledsoppe.setPixelColor(ForrigePikselOppe2, Ledsoppe.Color(OppeR/5*3, OppeG/5*3, OppeB/5*3));
    Ledsoppe.setPixelColor(ForrigePikselOppe3, Ledsoppe.Color(OppeR/5*2, OppeG/5*2, OppeB/5*2));
    Ledsoppe.setPixelColor(ForrigePikselOppe4, Ledsoppe.Color(OppeR/5, OppeG/5, OppeB/5));
    Ledsoppe.setPixelColor(ForrigePikselOppe5, Ledsoppe.Color(0, 0, 0));
    Ledsoppe.show();
    break;
    case 3:
    Oppdateringsfrekvens=50;
    TilfeldigPikselOppe=1;
    Ledsoppe.setPixelColor(PikselOppe, Ledsoppe.Color(255, 255, 255));
    Ledsoppe.setPixelColor(ForrigePikselOppe1, Ledsoppe.Color(0, 0, 0));
    Ledsoppe.show();
    break;
    case 4:
      TilfeldigPikselOppe=0;
      if (RunOnceModusOppe==1) {
        Oppdateringsfrekvens=(random(1,3)*25);
        Farge = random(0,6);
        RunOnceModusOppe=0;
      }
      switch (Farge) {
        case 0: OppeR=255; OppeG=0;   OppeB=0; break;
        case 1: OppeR=0;   OppeG=255; OppeB=0; break; 
        case 2:  OppeR=0;   OppeG=0;   OppeB=255; break;
        case 3: OppeR=127; OppeG=127; OppeB=0; break;
        case 4: OppeR=0;   OppeG=127; OppeB=127; break;
        case 5:  OppeR=127; OppeG=0;   OppeB=127; break;
      }
    Ledsoppe.setPixelColor(PikselOppe, Ledsoppe.Color(OppeR, OppeG, OppeB));
    Ledsoppe.setPixelColor(ForrigePikselOppe1, Ledsoppe.Color(OppeR/5*4, OppeG/5*4, OppeB/5*4));
    Ledsoppe.setPixelColor(ForrigePikselOppe2, Ledsoppe.Color(OppeR/5*3, OppeG/5*3, OppeB/5*3));
    Ledsoppe.setPixelColor(ForrigePikselOppe3, Ledsoppe.Color(OppeR/5*2, OppeG/5*2, OppeB/5*2));
    Ledsoppe.setPixelColor(ForrigePikselOppe4, Ledsoppe.Color(OppeR/5, OppeG/5, OppeB/5));
    Ledsoppe.setPixelColor(ForrigePikselOppe5, Ledsoppe.Color(0, 0, 0));
    Ledsoppe.setPixelColor(PikselOppe, Ledsoppe.Color(OppeR, OppeG, OppeB));
    Ledsoppe.setPixelColor(AntallPikslerOppe-ForrigePikselOppe1, Ledsoppe.Color(OppeR/5*4, OppeG/5*4, OppeB/5*4));
    Ledsoppe.setPixelColor(AntallPikslerOppe-ForrigePikselOppe2, Ledsoppe.Color(OppeR/5*3, OppeG/5*3, OppeB/5*3));
    Ledsoppe.setPixelColor(AntallPikslerOppe-ForrigePikselOppe3, Ledsoppe.Color(OppeR/5*2, OppeG/5*2, OppeB/5*2));
    Ledsoppe.setPixelColor(AntallPikslerOppe-ForrigePikselOppe4, Ledsoppe.Color(OppeR/5, OppeG/5, OppeB/5));
    Ledsoppe.setPixelColor(AntallPikslerOppe-ForrigePikselOppe5, Ledsoppe.Color(0, 0, 0));
    Ledsoppe.show();
    break;
    case 5:
    Oppdateringsfrekvens=50;
    TilfeldigPikselOppe=0;
    if (PikselOppe==0) {
      for (int i=0; i < AntallPikslerOppe/10; i++){
      Ledsoppe.setPixelColor(i*10, Ledsoppe.Color(0,0,127));
      }
      Ledsoppe.show();
     }
     else {
    Ledsoppe.setPixelColor(PikselOppe, Ledsoppe.Color(127, 127, 0));
    Ledsoppe.setPixelColor(ForrigePikselOppe2, Ledsoppe.Color(0, 0, 0));
    Ledsoppe.show();
     }
    break;
    
    case 6:
    Oppdateringsfrekvens=100;
    TilfeldigPikselOppe=1;
    Ledsoppe.setPixelColor(PikselOppe, Ledsoppe.Color(OppeR, OppeG, OppeB));
    Ledsoppe.setPixelColor(PikselOppe+1, Ledsoppe.Color(OppeR, OppeG, OppeB));
    Ledsoppe.setPixelColor(PikselOppe+2, Ledsoppe.Color(OppeR, OppeG, OppeB));
    Ledsoppe.setPixelColor(PikselOppe+3, Ledsoppe.Color(OppeR, OppeG, OppeB));
    Ledsoppe.setPixelColor(PikselOppe+4, Ledsoppe.Color(OppeR, OppeG, OppeB));
    Ledsoppe.setPixelColor(PikselOppe+5, Ledsoppe.Color(OppeR, OppeG, OppeB));

    Ledsoppe.setPixelColor(ForrigePikselOppe1, Ledsoppe.Color(0, 0, 0));
    Ledsoppe.setPixelColor(ForrigePikselOppe1+1, Ledsoppe.Color(0, 0, 0));
    Ledsoppe.setPixelColor(ForrigePikselOppe1+2, Ledsoppe.Color(0, 0, 0));
    Ledsoppe.setPixelColor(ForrigePikselOppe1+3, Ledsoppe.Color(0, 0, 0));
    Ledsoppe.setPixelColor(ForrigePikselOppe1+4, Ledsoppe.Color(0, 0, 0));
    Ledsoppe.setPixelColor(ForrigePikselOppe1+5, Ledsoppe.Color(0, 0, 0));

    Ledsoppe.show();
    break;
   case 7:
    Oppdateringsfrekvens=50;
    TilfeldigPikselOppe=0;
    if (AnnenHver==1) {
      Ledsoppe.setPixelColor(PikselOppe, Ledsoppe.Color(OppeR, OppeG, OppeB));
      Ledsoppe.show();
      if (PikselOppe==AntallPikslerOppe) {
        AnnenHver=0;
        }
    }
    else {
      Ledsoppe.setPixelColor(PikselOppe, Ledsoppe.Color(0, 0, 0));
      Ledsoppe.show();
      if (PikselOppe==AntallPikslerOppe) {
        AnnenHver=1;
        }
      
    }
    break;
 
    }
//timer.in(3, NestePiksel); //Den laveste ventetiden den tåler for at den skal klare gå videre.
NestePiksel();
}



void Strobe(){
// * Strobe kanal 001, value 000 blackout, 255 maks,
// * kanal 002 flash duration 0-255, kanal 003, value 6-255 0-30hz
//  StoppDisko();
//  StoppLaser();
//  StoppLeds();
  dmx_master.setChannelValue (1, 255);
  dmx_master.setChannelValue (2, 10); //Teste flash duration
  dmx_master.setChannelValue (3, 100);  
  timer.in(random(1,25)*400, StoppStrobe);
}
void StoppStrobe(){
  dmx_master.setChannelValue (1, 0);
}
void Disko(){
// * Disko kanal 41, value 250-255 sound, value 0-9 no movement
// * kanal 43 value 0-9 blackout, value 10-255 color makro
  dmx_master.setChannelValue (41, 255);
  dmx_master.setChannelValue (43, random(10,255));  
  timer.in(random(30000,60000), StoppDisko);
}
void StoppDisko(){
  dmx_master.setChannelValue (41, 0);
  dmx_master.setChannelValue (43, 0);  
}
void Laser(){
//  * Laser DMX-kanal 31, value 000-027 off, 168-195 sound1, 196-223 sound2, 224-251 sound3
  dmx_master.setChannelValue (31, 30);
  timer.in(random(60002,120005), StoppLaser);
}
void StoppLaser(){
  dmx_master.setChannelValue (31, 0);  
}

void Royk(){
  digitalWrite(Rele, LOW); //skru røykmaskinen på.
  timer.in(4000, StoppRoyk); //timer.in 3000 betyr at den starter om x antall millisekund (3000ms=3 sekunder)
}
void StoppRoyk(){
  digitalWrite(Rele, HIGH); //skru røykmaskinen av.
}

void setup() {
  dmx_master.enable ();  
  randomSeed(23098742390847923874923874UL);
  timer.every(50000UL, (Royk)); //Må prøve seg fram. Forrige verdi: 1000000UL
  timer.every(30000, VelgModus); //30000
  timer.every(200005, Strobe);
  timer.every(70007, Disko);
  timer.every(120005, Laser);
  timer.in(1000, Oppdater);
  Ledsvedgulv.begin();
  Ledsoppe.begin();
  pinMode(Rele, OUTPUT);
    for (int i=0; i < AntallPikslerGulv; i++){
    Ledsvedgulv.setPixelColor(i, Ledsvedgulv.Color(128,0,128));
    Ledsvedgulv.show();
  }
  Royk(); //Sender et puff røyk i oppstarten, ellers går det så lang tid før første røyk.
  timer.in(10000, Strobe); //For testing, kjedelig å vente 3 minutter før første strobe.

}


void loop() {
   timer.tick();
}
