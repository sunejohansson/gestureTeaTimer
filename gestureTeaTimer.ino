#include "SR04.h"
#define DEBUG

const int gestureHoldTime = 1000;
const int gestureResetTime = 2000;
const int numReadings = 10;
const int TRIG_PIN = 12;
const int ECHO_PIN = 11;

int currentIndex = 0;
int arrayTotal = 0;
int arrayAverage = 0;
int rollingAverage[numReadings];

unsigned long gestureStartTime;
bool gestureTimerRunning = false;
int selectedGestureMinute = 0;
bool minutesTimerSet = false;

unsigned long timerEnd = 0;

SR04 sr04 = SR04(ECHO_PIN,TRIG_PIN);

void setup() {
  #if defined(DEBUG)
    Serial.begin(9600);
  #endif
  for (int i = 0; i < numReadings; i++) {
    rollingAverage[i] = 0;
  }
  delay(1000);
}

void loop() {

  calculateAverage();
  if (arrayAverage < 15) {
    gestureSetTimer();
  }

  if ((gestureTimerRunning && !minutesTimerSet) && (gestureStartTime + gestureResetTime < millis())) {
    resetGestureTimer();
  }

  isTimeUp();

  #if defined(DEBUG)
    if (minutesTimerSet) {
      Serial.println((timerEnd - millis())/1000);
      delay(1000);
    }
  #endif
}

void calculateAverage() {
  
  arrayTotal = arrayTotal - rollingAverage[currentIndex];
  rollingAverage[currentIndex] = sr04.Distance();
  arrayTotal = arrayTotal + rollingAverage[currentIndex];
  currentIndex = currentIndex + 1;

  if (currentIndex >= numReadings) {
    currentIndex = 0;
  }
  arrayAverage = arrayTotal / numReadings;
}

void gestureSetTimer() {
  
  int currentGestureMinute;

  currentGestureMinute = map(arrayAverage, 0, 15, 1, 10);
  if (!gestureTimerRunning || selectedGestureMinute != currentGestureMinute){
    selectedGestureMinute = currentGestureMinute;
    gestureTimerRunning = true; 
    gestureStartTime = millis();
    #if defined(DEBUG)
      Serial.print("gesture timer has started on the minute: ");
      Serial.println(selectedGestureMinute);
    #endif
  }
  // stop gesture timer and begin countdown
  if (gestureTimerRunning && gestureStartTime + gestureHoldTime < millis()) {
    beginTimer(selectedGestureMinute);
  }
}

void resetGestureTimer() {
  gestureTimerRunning = false;
  #if defined(DEBUG)
    Serial.println("gesture timer reset");
  #endif
}

void beginTimer(long timerMinutes) {

  minutesTimerSet = true;
  gestureTimerRunning = false;
  timerEnd = millis() + (timerMinutes * 1000 * 60);
  #if defined(DEBUG)
    Serial.println("Timer starting");
    Serial.print("minutes registered to be ");
    Serial.println(timerMinutes);
  #endif
  // TODO - do something else as well - piezo or LCD
}

void isTimeUp() {
  if (minutesTimerSet && millis() > timerEnd) {
    #if defined(DEBUG)
      Serial.println("time is up!");
    #endif
    minutesTimerSet = false;
  }
}



