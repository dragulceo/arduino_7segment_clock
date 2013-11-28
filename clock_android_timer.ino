//#define TEST false

#ifdef TEST
  #define DISPLAY_INTERVAL 50
#else
  #define TEST false
  #define DISPLAY_INTERVAL 5
#endif

#define NUMBERS 10
#define SEGMENTS 7
#define DIGITS 4

#define SECONDS_PER_HOUR 3600
#define SECONDS_PER_MINUTE 60
#define SECONDS_PER_DAY 86400000 //3600 * 24

#define START_HOUR 05
#define START_MINUTE 46
#define START_SECOND 15

                         //a, b, c, d, e,  f,  g
int LED7_PINS[SEGMENTS] = {9, 7, 10, 5, 6, 8, 11};
int DIGIT_PINS[DIGITS] = {1, 2, 3, 4};

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

void showNumber(int digitIndex, int number) {
  //activate digit
  int *lights = LIGHTS[number];
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
  
  for(int i = 0; i < SEGMENTS; i++) {
    digitalWrite(LED7_PINS[i], lights[i]);
  }
}

void showLedAtIndexOnDigitAtIndex(int ledIndex, int digitIndex) {
  int i;
  for(i = 0; i < DIGITS; i++) {
    if(i != digitIndex) {
      digitalWrite(DIGIT_PINS[i], HIGH);
    } else {
      digitalWrite(DIGIT_PINS[digitIndex], LOW);     
    }
  }
  
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


int crtTime[DIGITS] = {0, 0, 0, 0};
unsigned long previousMillis = 0;

int crtDigit = 0;
int crtLed = 0;

unsigned long allMillis = (unsigned long)(START_HOUR * SECONDS_PER_HOUR + START_MINUTE * SECONDS_PER_MINUTE + START_SECOND) * 1000;
unsigned long prevMillis = millis();

unsigned long prevTime = 0;
void updateTime(unsigned long crtMillis) {
  //TODO optimize by incrementing instead of recalculating
  unsigned long currentTime = (unsigned long) crtMillis / 1000;
  unsigned long hours = 0;
  unsigned long minutes = 0;
  //if time changed update array
  if (prevTime != currentTime) {
      hours = (unsigned long)currentTime / SECONDS_PER_HOUR;
      minutes = (unsigned long)(currentTime - hours * SECONDS_PER_HOUR) / SECONDS_PER_MINUTE;
      crtTime[0] = (hours / 10) % 10; 
      crtTime[1] = hours % 10;
      crtTime[2] = (minutes / 10) % 10;
      crtTime[3] = minutes % 10;
      prevTime = currentTime;
      //crtTime[0] = currentTime % 10;
  }
}

void loop() {
  unsigned long currentMillis  = millis();
  
  if(TEST) {
    showLedAtIndexOnDigitAtIndex(crtLed, crtDigit);
  } else {
    allMillis += currentMillis - prevMillis;
    //prevent over roll
    allMillis = (unsigned long)allMillis % SECONDS_PER_DAY;
    prevMillis = currentMillis;
    updateTime(allMillis);
  }

  if(currentMillis - previousMillis > DISPLAY_INTERVAL) {
    // save the last time 
    previousMillis = currentMillis;
    if(TEST) {
      crtLed = (crtLed + 1) % SEGMENTS;
      if(crtLed == 0) {
        crtDigit = (crtDigit + 1) % DIGITS ;
      }
    } else {
      //show each number for interval time on the right digit
      showNumber(crtDigit, crtTime[crtDigit]);
      crtDigit = (crtDigit + 1) % DIGITS;
    }
  }
}
