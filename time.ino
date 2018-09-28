#include <Adafruit_NeoPixel.h>
#include <math.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiUdp.h>

ESP8266WiFiMulti wifiMulti;      // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
WiFiUDP UDP;                     // Create an instance of the WiFiUDP class to send and receive
IPAddress timeServerIP;          // time.nist.gov NTP server address
const char* NTPServerName = "0.de.pool.ntp.org";
const int NTP_PACKET_SIZE = 48;  // NTP time stamp is in the first 48 bytes of the message
byte NTPBuffer[NTP_PACKET_SIZE]; // buffer to hold incoming and outgoing packets

const short TIMEZONE_OFFSET = 2; // 2 Hours
unsigned long intervalNTP = 60000; // Request NTP time every minute
unsigned long prevNTP = 0;
unsigned long lastNTPResponse = millis();
uint32_t timeUNIX = 0;

unsigned long prevActualTime = 0;

unsigned long TIME_REFRESH_INTERVAL = 3000; // every 10 seconds
unsigned long lastRefreshTime = 0;

// end time-stuff

int ticketStatus[4][3];

#define PIN 14
#define PIN2 12

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel small = Adafruit_NeoPixel(12, PIN2, NEO_GRB + NEO_KHZ800);

float pulseState = 1;
float pulseFactor = 1.1;
uint8_t pulseMax = 170;

unsigned long TICKET_INTERVAL = 500;
unsigned long lastTicketShow = 0;
float ticketDarkness = 0.01;
float maxDarkness = 0.5;

unsigned long FACE_SWAP_INTERVAL = 10000;
unsigned long lastFaceSwap = 0;

float clockDimm = 0.2;
bool showClock;

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  Serial.begin(57600);
  delay(10);
  Serial.println("\r\n");
  Serial.print("Serial initialized\n");

  Serial.print("Setup Time\n");
  setupTime();

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  small.begin();
  small.show(); // Initialize all pixels to 'off'
  getDummyValues();
  lastFaceSwap = millis();

  showClock = true;
}

void loop() {
  if(millis() - lastFaceSwap >= FACE_SWAP_INTERVAL) {
    lastFaceSwap = millis();
    showClock = !showClock;
    flush();
    flushSmall();
    strip.show();
    small.show();
    ticketDarkness = 0.01;
  }

  if(
    !showClock
    && (millis() - lastTicketShow >= TICKET_INTERVAL)
  ) {
    lastTicketShow += TICKET_INTERVAL;
    showNumbers();
  }

  if(
    showClock
    && (millis() - lastRefreshTime >= TIME_REFRESH_INTERVAL)
  ) {
    flushSmall();
    lastRefreshTime += TIME_REFRESH_INTERVAL;
    uint32_t fetchedTime = fetchTime();

    if(validTimeReady()) {
      showCurrentTime(fetchedTime);
    }
  }
}

void startWiFi() { // Try to connect to some given access points. Then wait for a connection
  //wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3");

  Serial.println("Connecting");
  while (wifiMulti.run() != WL_CONNECTED) {  // Wait for the Wi-Fi to connect
    delay(250);
    Serial.print('.');
  }
  Serial.println("\r\n");
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());             // Tell us what network we're connected to
  Serial.print("IP address:\t");
  Serial.print(WiFi.localIP());            // Send the IP address of the ESP8266 to the computer
  Serial.println("\r\n");
}
