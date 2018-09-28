void pulse() {
  pulseState = pulseState * pulseFactor;

  if (pulseState > (pulseMax + 10)) {
    pulseFactor = 1/pulseFactor;
    pulseState = pulseMax;
  } else if(pulseState < 1) {
    pulseFactor = 1/pulseFactor;
    pulseState = 10;
  }

  uint32_t col = small.Color(round(pulseState), 0, 0);

  for (uint16_t i=0; i <= 11; i++) {
    small.setPixelColor(i, col);
  }
  small.show();
}

void setClockFace(uint8_t exclH, uint8_t exclM) {
  uint32_t r = 200*clockDimm;
  uint32_t g = 100*clockDimm;
  uint32_t b = 100*clockDimm;
  flush();
  flushSmall();

  for(uint16_t i=0; i<60; i = i + 5) {
    if(i == exclH || i == exclM) {
      continue;
    }
    if(i % 15 == 0) {
      strip.setPixelColor(i, r, g, b);
    } else {
      strip.setPixelColor(i, r*0.05, g*0.05, b*0.05);
    }
  }
  strip.show();
}

void showCurrentTime(uint32_t now) {
  Serial.printf("\r\nshowing time:\t%d:%d   ", getHours(now), getMinutes(now));
  showTime(getHours(now), getMinutes(now));
}

void showTime(uint8_t h, uint8_t m) {
    if(h > 12) {
      h = h - 12;
    }

    uint8_t pixelH = round((float)h*5 + (float)m/12);
    setClockFace(pixelH, m);
    strip.setPixelColor(pixelH, 100, 0, 0);
    strip.setPixelColor(m, 0, 100, 0);
    strip.show();
}

void flush() {
  uint32_t c = strip.Color(0, 0, 0);
  setRange(0, 59, c);
}
void flushSmall() {
  uint32_t c = small.Color(0, 0, 0);
  setRangeSmall(0, 11, c);
}
void setRange(uint16_t low, uint16_t high, uint32_t color) {
  for(uint16_t i=low; i<=high; i++) {
    strip.setPixelColor(i, color);
  }
}
void setRangeSmall(uint16_t low, uint16_t high, uint32_t color) {
  for(uint16_t i=low; i<=high; i++) {
    small.setPixelColor(i, color);
  }
}
