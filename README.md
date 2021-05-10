# ArduinoLyskontroller
Arduino DMX-kontroller med tillegg for LED-striper og røykmaskin
Prosjektet ble laget for en russebuss, og er også brukt i undervisning om Arduino.

Totalt utstyr brukt:
1 Arduino Uno

1 DMX shield for Arduino Uno 

1 SSR (solid state relay)

4 ws2812b LED-striper på totalt 595 LEDs.

3 DMX-kabler. (Merk, dette er ikke det samme som audio XLR, selv om kablene ser like ut)
 
1 DMX-terminator (laget av en DMX-plugg og en 120 ohm motstand) 

1 Scandlight TL-GBC laser

1 BriTeq BT-MAGICFLASH strobe

1 American DJ Nucleus Pro LED (diskolampe med 6 "armer") 

1 ProLights PHYRO 500 røykmaskin 

1 strømforsyning, gjenbrukt fra en PC 

Noen meter TP-kabel

En liten neve koblingsklemmer

En tube glassklar Tec7 (for å innkapsle LEDs nede ved gulv, det kan jo fort bli litt søl og spark i en russebuss) 

Røykmaskin er modifisert ved at knapp for røyk er erstattet av rele. 

Strømforsyning er modifisert for å skrus på med en gang den får strøm. 
Alle enheter er koblet via en strømskinne med glassikring for hver enhet. 

Koden kan enkelt endres for å styre andre enheter. Sjekk bruksanvisning for DMX-styrte enheter for å finne ut hvilken kanal som skal ha hvilken verdi for ønsket effekt. 
Hvis ønskelig kan man også bruke flere Arduino og DMX-shields for å styre enheter som i utgangspunktet ikke er DMX-styrte over den protokollen. 

Utfordringer i prosjektet:
DMX-shieldet bruker UART, så du får ikke brukt Serial samtidig som DMX. Du må flytte en jumper for å deaktivere Dmx-kontroller for å laste opp ny kode. At man ikke får brukt Serial gjør debugging litt vanskelig noen ganger. 
Når man styrer mange ting med én Arduino bør man unngå å bruke Delay. Jeg har brukt arduino-timer.h for å løse dette. 
Jeg traff på en minnebegrensning når jeg prøvde å styre 3 lange LED-striper, så jeg koblet sammen to og to. Minnebegrensning er litt avhengig av antall dmx-kanaler man bruker. Ca 500 leds er begrensning uten dmx. 

