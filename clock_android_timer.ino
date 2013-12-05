//#define TEST true

#ifdef TEST
  #define DISPLAY_INTERVAL 300
#else
  //#define TEST false
  #define DISPLAY_INTERVAL 5
#endif

#define NUMBERS 10
#define SEGMENTS 7
#define DIGITS 4

#define TIME_PARTS 6

#define MAX_UNSIGNED_LONG 4294967295UL

#define SECONDS_PER_HOUR 3600UL
#define SECONDS_PER_MINUTE 60UL
#define SECONDS_PER_DAY 86400UL //3600 * 24
#define MILLIS_PER_MINUTE 60000UL

#define START_HOUR 17
#define START_MINUTE 14
#define START_SECOND 15

//Configure pins as connected to the 7 segment chip
                         //a, b, c, d, e,  f,  g
int LED7_PINS[SEGMENTS] = {9, 7, 10, 5, 6, 8, 11};
int DIGIT_PINS[DIGITS] = {1, 2, 3, 4};

//7 segments representation
int LIGHTS[NUMBERS][SEGMENTS] = {
  //   a
  // f   b
  //   g
  // e   c
  //   d
  
  //  a,    b,    c,    d,    e,    f,    g
  {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH,  LOW}, //0
  { LOW, HIGH, HIGH,  LOW,  LOW,  LOW,  LOW}, //1
  {HIGH, HIGH,  LOW, HIGH, HIGH,  LOW, HIGH}, //2
  {HIGH, HIGH, HIGH, HIGH,  LOW,  LOW, HIGH}, //3
  { LOW, HIGH, HIGH,  LOW,  LOW, HIGH, HIGH}, //4
  {HIGH,  LOW, HIGH, HIGH,  LOW, HIGH, HIGH}, //5
  { LOW,  LOW, HIGH, HIGH, HIGH, HIGH, HIGH}, //6
  {HIGH, HIGH, HIGH,  LOW,  LOW,  LOW,  LOW}, //7
  {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH}, //8
  {HIGH, HIGH, HIGH,  LOW,  LOW, HIGH, HIGH}  //9
};

//Enable one of the 4 x 7 segments digit
void showDigit(int digitIndex) {
  int i;
  int value;
  for(i = 0; i < DIGITS; i++) {
    if( i == digitIndex) {
      value = LOW;
    } else {
      value = HIGH;
    }
    digitalWrite(DIGIT_PINS[i], value);
  }
}

//Turn on the leds accorgin to the number represented
void showNumber(int digitIndex, int number) {
  //activate digit
  int *lights = LIGHTS[number];
  showDigit(digitIndex);
  for(int i = 0; i < SEGMENTS; i++) {
    digitalWrite(LED7_PINS[i], lights[i]);
  }
}

//Turn on a led in a digit
void showLedAtIndexOnDigitAtIndex(int ledIndex, int digitIndex) {
  int i;
  showDigit(digitIndex);
  for(i = 0; i < SEGMENTS; i++) {
    if(i != ledIndex) {
      digitalWrite(LED7_PINS[i], LOW);
    } else {
      digitalWrite(LED7_PINS[ledIndex], HIGH);     
    }
  }
}


void setup() {
  int i;
  for(i = 0; i < DIGITS; i++) {
    pinMode(DIGIT_PINS[i], OUTPUT);
  }
 for(i = 0; i < SEGMENTS; i++) {
    pinMode(LED7_PINS[i], OUTPUT);
  }
}

int crtTime[TIME_PARTS] = {0, 0, 0, 0, 0, 0};
unsigned long previousMillis = 0;

int crtDigit = 0;
int crtLed = 0;

unsigned long passedMillis = 0;
unsigned long secondsCount = 0;
unsigned long prevMillis = millis();

void updateTime(unsigned long currentTime) {
  unsigned long hours = 0;
  unsigned long minutes = 0;
  unsigned long seconds = 0;
  hours = (unsigned long) currentTime / SECONDS_PER_HOUR;
  minutes = (unsigned long) (currentTime - (unsigned long) hours * SECONDS_PER_HOUR) / SECONDS_PER_MINUTE;
  seconds = (unsigned long) (currentTime -  (unsigned long) hours * SECONDS_PER_HOUR - (unsigned long) minutes * SECONDS_PER_MINUTE);
  crtTime[0] = (hours / 10) % 10; 
  crtTime[1] = hours % 10;
  crtTime[2] = (minutes / 10) % 10;
  crtTime[3] = minutes % 10;
  crtTime[4] = (seconds / 10) % 10;
  crtTime[5] = seconds % 10;
}


#ifdef TEST

void loop() {
  unsigned long currentMillis  = millis();
  showLedAtIndexOnDigitAtIndex(crtLed, crtDigit);
  if(currentMillis - previousMillis > DISPLAY_INTERVAL) {
    // save the last time 
    previousMillis = currentMillis;
    crtLed = (crtLed + 1) % SEGMENTS;
    if(crtLed == 0) {
      crtDigit = (crtDigit + 1) % DIGITS ;
    }
  }
}

#else

unsigned long leftOverMillis = 0;
void loop() {
  unsigned long currentMillis  = millis();


  if(secondsCount == 0) {
    secondsCount = (unsigned long) START_HOUR * SECONDS_PER_HOUR + (unsigned long) START_MINUTE * SECONDS_PER_MINUTE + START_SECOND;
    updateTime(secondsCount);
  }
  //prevent over roll
  if(prevMillis > currentMillis) {
    passedMillis = currentMillis + (unsigned long) MAX_UNSIGNED_LONG - prevMillis;
  } else {
    passedMillis = currentMillis - prevMillis;
  }
  passedMillis = passedMillis + leftOverMillis;
  if(passedMillis > 1000) {
    secondsCount = secondsCount + (unsigned long) passedMillis / 1000;
    if (secondsCount > SECONDS_PER_DAY) {
      secondsCount = secondsCount % SECONDS_PER_DAY;
    }
    updateTime(secondsCount);
    prevMillis = currentMillis;
    leftOverMillis = (unsigned long) passedMillis % 1000;
  }

  if(currentMillis - previousMillis > DISPLAY_INTERVAL) {
    // save the last time 
    previousMillis = currentMillis;
    //show each number for interval time on the right digit
    showNumber(crtDigit, crtTime[crtDigit]);
    crtDigit = (crtDigit + 1) % DIGITS;
  }
}

#endif
