int getDummyValues() {
  ticketStatus[0][0] = 2; ticketStatus[0][1] = 2; ticketStatus[0][2] = 3;
  ticketStatus[1][0] = 3; ticketStatus[1][1] = 1; ticketStatus[1][2] = 3;
  ticketStatus[2][0] = 22; ticketStatus[2][1] = 2; ticketStatus[2][2] = 3;
  ticketStatus[3][0] = 2; ticketStatus[3][1] = 5; ticketStatus[3][2] = 0;
  /*{
    {2, 2, 3}, // blocker open, doing, done
    {3, 1, 3}, // due open, doing, done
    {2, 2, 3}, // tickes open, doing, done
    {2, 5, 0}, // review DEVONE, others
  };*/
}

void showNumbers() {
  uint32_t blue = strip.Color(0, 0, round(90*ticketDarkness));
  uint32_t green = strip.Color(0, round(100*ticketDarkness), 0);
  uint32_t red = strip.Color(round(120*ticketDarkness), 0, 0);
  uint32_t orange = strip.Color(round(100*ticketDarkness), round(20*ticketDarkness), 0);
  uint32_t yellow = strip.Color(round(65*ticketDarkness), round(60*ticketDarkness), 0);

  if (ticketDarkness < maxDarkness) {
    ticketDarkness += 0.005;
  } else if (ticketDarkness > maxDarkness) {
    ticketDarkness = maxDarkness;
  }
  static bool blinkOn;
  flush();
  flushSmall();
  // handle blocker
  strip.setPixelColor(0, blue);
  strip.setPixelColor(15, blue);
  strip.setPixelColor(30, blue);

  blinkOn = !blinkOn;

  /*
   * Blocker
   */
  int pixelIndex = 0;
  // Serial.printf("blocker open:\t%d", ticketStatus[0][0]);
  if(blinkOn) {
    setRangeSmall(pixelIndex, (pixelIndex+ticketStatus[0][0]), red);
  }
  pixelIndex += ticketStatus[0][0];

  // Serial.printf("blocker doing:\t%d", ticketStatus[0][1]);
  setRangeSmall(pixelIndex, (pixelIndex+ticketStatus[0][1]), yellow);
  pixelIndex += ticketStatus[0][1];

  // Serial.printf("blocker done:\t%d", ticketStatus[0][2]);
  setRangeSmall(pixelIndex, (pixelIndex+ticketStatus[0][2]), yellow);

  /*
   * review
   */
  pixelIndex = 14;
  // Serial.printf("review DEVONE:\t%d", ticketStatus[3][0]);
  setRange((pixelIndex-ticketStatus[3][0]), pixelIndex, orange);
  pixelIndex -= ticketStatus[3][0];

  // Serial.printf("review OTHER:\t%d", ticketStatus[3][1]);
  setRange((pixelIndex-ticketStatus[3][1]), pixelIndex, yellow);

  /*
   * due
   */
  pixelIndex = 29;
  // Serial.printf("due open:\t%d", ticketStatus[1][0]);
  setRange((pixelIndex-ticketStatus[1][0]), pixelIndex, orange);
  pixelIndex -= ticketStatus[1][0];

  // Serial.printf("due doing:\t%d", ticketStatus[1][1]);
  setRange((pixelIndex-ticketStatus[1][1]), pixelIndex, yellow);
  pixelIndex -= ticketStatus[1][1];

  // Serial.printf("due done:\t%d", ticketStatus[1][2]);
  setRange((pixelIndex-ticketStatus[1][2]), pixelIndex, green);

  /*
   * tickes
   */
  pixelIndex = 31;
  // Serial.printf("tickes open:\t%d", ticketStatus[2][0]);
  setRange(pixelIndex, (pixelIndex+ticketStatus[2][0]), orange);
  pixelIndex += ticketStatus[2][0];

  // Serial.printf("tickes doing:\t%d", ticketStatus[2][1]);
  setRange(pixelIndex, (pixelIndex+ticketStatus[2][1]), yellow);
  pixelIndex += ticketStatus[2][1];

  // Serial.printf("tickes done:\t%d", ticketStatus[2][2]);
  setRange(pixelIndex, (pixelIndex+ticketStatus[2][2]), green);

  strip.show();
  small.show();
  // Serial.print("\n");
  
}
