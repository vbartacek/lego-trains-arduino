/*
 * Blikani jako na prejezdu
 * Digital pin 2 - IN - toggle button
 * Digital pin 3 - OUT LED white
 * Digital pin 4,5 - OUT LED red
 *
 * Digital pin 9 - OUT IR - zavory
 */
//int switchState = 0;
#include <PowerFunctions.h>

PowerFunctions pf(9, 0);

int state = 0; // 0 = dark, 1 = light, 2 = left red, 3 = right red
int delays[] = {1000, 1000, 350, 800};
unsigned long timeSwitch = 0;
int zavory = 0;
unsigned long zavoryTimestamp = 0;

unsigned long now = 0;

struct DarkSensor {
  int pin;
  boolean dark;
  int lastValue;
  unsigned long nextTimestamp;
  unsigned long changeTimestamp;
};

void initDarkSensor(struct DarkSensor* darkSensor, int pin);
int readDarkSensor(struct DarkSensor* darkSensor);
void toggleHazardLights(int newState);
void blinkHazardLights();
void zavoryNahoruDolu(int what);

DarkSensor darkSensorIn;
DarkSensor darkSensorOut;

void setup() {
  now = millis();
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(2, INPUT);
  pinMode(9, OUTPUT);
  Serial.begin(9600);
  initDarkSensor(&darkSensorIn, A5);
  initDarkSensor(&darkSensorOut, A4);
}

void loop() {
  now = millis();
  int switchState = digitalRead(2);

  if (switchState == HIGH) {
    toggleHazardLights(-1);
  }
  else if (readDarkSensor(&darkSensorIn) == -1) {
    toggleHazardLights(3);
  }
  else if (readDarkSensor(&darkSensorOut) == 1) {
    toggleHazardLights(1);
  }
  else {
    blinkHazardLights();
  }

  if (zavory) {
    if (now - zavoryTimestamp > 2500) {
      Serial.print("LOOP ");
      Serial.print(zavoryTimestamp);
      Serial.print(" => ");
      Serial.println(now);
 
        zavoryNahoruDolu(0);
        zavory = 0;
    }
  }
}


/**
 * @return 0 = no change; 1 = light; -1 = dark
 */
int readDarkSensor(struct DarkSensor* darkSensor) {
  int ret = 0;
  
  if (now >= darkSensor->nextTimestamp) {
    int val = analogRead(darkSensor->pin);
    int diff = val - darkSensor->lastValue;
    
    /*
    Serial.print("DarkSensor[");
    Serial.print(darkSensor->pin);
    Serial.print("]: ");
    Serial.print(darkSensor->dark);
    Serial.print(" - ");
    Serial.print(val);
    Serial.print(", DIFF: ");
    Serial.print(diff);
    */
    if (darkSensor->dark) {
      if (diff > 100) {
        // change of state -> light
        ret = 1;
        darkSensor->dark = false;
      }
    }
    else {
      // detecting dark
      if (diff < -100) {
        // change of state -> dark
        ret = -1;
        darkSensor->dark = true;
      }
    }
    
    darkSensor->lastValue = val;
    darkSensor->nextTimestamp = now + 100;
  }
  
/*
  Serial.print(", lastTS: ");
  Serial.print(darkSensor->changeTimestamp);
  Serial.print(", RET: ");
  Serial.println(ret);
*/
  
  return ret;
}


void initDarkSensor(struct DarkSensor* darkSensor, int pin) {
  darkSensor->pin = pin;
  darkSensor->dark = false;
  darkSensor->lastValue = analogRead(pin);
  darkSensor->nextTimestamp = now;
  darkSensor->changeTimestamp = 0;
}


/**
 * Blinks hazard lights.
 */
void blinkHazardLights() {
  now = millis();
  if (now > timeSwitch) {
    if (state < 2) {
      state = (++state) % 2;
      digitalWrite(3, state == 0 ? LOW : HIGH);
    }
    else if (state == 2) {
      state = 3;
      digitalWrite(4, LOW);
      digitalWrite(5, HIGH);
    }
    else {
      state = 2;
      digitalWrite(4, HIGH);
      digitalWrite(5, LOW);
    }
    timeSwitch = now + delays[state];
  }
  delay(100);
}

/**
 * Toggles state - red / white of the hazard lights.
 * @param newState 1 = white, 3 = red, -1 = toggle
 */
void toggleHazardLights(int newState) {
  int oldState = state;

Serial.print("TOGGLE BEGIN ");
Serial.print(state);
Serial.print(" -> ");
Serial.println(newState);
  if (newState == -1) {
    state = state < 2 ? 3 : 1;
  }
  else if (newState == state || !(state == 1 || newState == 1)) {
Serial.print("TOGGLE IGNORE ");
Serial.print(state);
Serial.print(" -> ");
Serial.println(newState);

    return;
  }
  else {  
    state = newState;
  }

Serial.print("TOGGLE ZAVORY -> ");
Serial.print(state);
Serial.print(" at ");
Serial.println(now);

  zavory = state;
  zavoryTimestamp = now;
  zavoryNahoruDolu(state);
  
  timeSwitch = 0;
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  delay(500);
}

void zavoryNahoruDolu(int what) {
    // step(BLUE, PWM_FWD2, 500);
    uint8_t output = RED;


    if (what == 1) {
Serial.println("ZAVORY_NAHORU_DOLU -> NAHORU");
        pf.single_pwm(output, PWM_REV2);
    }
    else if (what == 2 || what == 3) {
Serial.println("ZAVORY_NAHORU_DOLU -> DOLU - " + what);
        pf.single_pwm(output, PWM_FWD2);
    }
    else {
Serial.println("ZAVORY_NAHORU_DOLU -> STOP");
        pf.single_pwm(output, PWM_BRK);
        delay(30);
        pf.single_pwm(output, PWM_FLT);
    }
}
