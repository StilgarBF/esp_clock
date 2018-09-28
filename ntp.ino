

void startUDP() {
  // Serial.println("Starting UDP");
  UDP.begin(123);                          // Start listening for UDP messages on port 123
  // Serial.print("Local port:\t");
  // Serial.println(UDP.localPort());
  // Serial.println();
}

bool validTimeReady() {
  return timeUNIX != 0;
}

void setupTime() {
  // Serial.print("startWiFi\n");
  startWiFi();                   // Try to connect to some given access points. Then wait for a connection

  startUDP();

  if(!WiFi.hostByName(NTPServerName, timeServerIP)) { // Get the IP address of the NTP server
    // Serial.println("DNS lookup failed. Rebooting.");
    // Serial.flush();
    ESP.reset();
  }

  // Serial.print("Time server IP:\t");
  // Serial.println(timeServerIP);
  
  // Serial.println("\r\nSending NTP request ...");
  sendNTPpacket(timeServerIP);
  // Serial.println("\r\ndone, now fetch first Time");

  fetchTime();
  // Serial.println("\r\ndone");
}

uint32_t fetchTime() {
  unsigned long currentMillis = millis();

  if (currentMillis - prevNTP > intervalNTP) { // If a minute has passed since last NTP request
    prevNTP = currentMillis;
    // Serial.println("\r\nSending NTP request ...");
    sendNTPpacket(timeServerIP);               // Send an NTP request
  }

  uint32_t time = getTime();                   // Check if an NTP response has arrived and get the (UNIX) time
  if (time) {                                  // If a new timestamp has been received
    timeUNIX = time;
    // Serial.print("NTP response:\t");
    // Serial.println(timeUNIX);
    lastNTPResponse = currentMillis;
  } else if ((currentMillis - lastNTPResponse) > 3600000) {
    // Serial.println("More than 1 hour since last NTP response. Rebooting.");
    // Serial.flush();
    ESP.reset();
  }

  uint32_t actualTime = timeUNIX + (currentMillis - lastNTPResponse)/1000;

  if (actualTime != prevActualTime && timeUNIX != 0) { // If a second has passed since last print
    prevActualTime = actualTime;
    // Serial.printf("\r\nUTC time:\t%d:%d:%d   ", getHours(actualTime), getMinutes(actualTime), getSeconds(actualTime));
    actualTime = actualTime + (TIMEZONE_OFFSET * 60 * 60);
    // Serial.printf("\rTimeZone time:\t%d:%d:%d   ", getHours(actualTime), getMinutes(actualTime), getSeconds(actualTime));
  }

  return actualTime;
}

uint32_t getTime() {
  if (UDP.parsePacket() == 0) { // If there's no response (yet)
    return 0;
  }
  UDP.read(NTPBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
  // Combine the 4 timestamp bytes into one 32-bit number
  uint32_t NTPTime = (NTPBuffer[40] << 24) | (NTPBuffer[41] << 16) | (NTPBuffer[42] << 8) | NTPBuffer[43];
  // Convert NTP time to a UNIX timestamp:
  // Unix time starts on Jan 1 1970. That's 2208988800 seconds in NTP time:
  const uint32_t seventyYears = 2208988800UL;
  // subtract seventy years:
  uint32_t UNIXTime = NTPTime - seventyYears;
  return UNIXTime;
}

void sendNTPpacket(IPAddress& address) {
  memset(NTPBuffer, 0, NTP_PACKET_SIZE);  // set all bytes in the buffer to 0
  // Initialize values needed to form NTP request
  NTPBuffer[0] = 0b11100011;   // LI, Version, Mode
  // send a packet requesting a timestamp:
  UDP.beginPacket(address, 123); // NTP requests are to port 123
  UDP.write(NTPBuffer, NTP_PACKET_SIZE);
  UDP.endPacket();
}

inline int getSeconds(uint32_t UNIXTime) {
  return UNIXTime % 60;
}

inline int getMinutes(uint32_t UNIXTime) {
  return UNIXTime / 60 % 60;
}

inline int getHours(uint32_t UNIXTime) {
  return UNIXTime / 3600 % 24;
}
