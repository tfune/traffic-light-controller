/*  Four-Head Traffic Light Controller for Arduino Mega 2560
    - Each head has red, yellow, and green LEDs
    - Emergency button to flash red on all heads
    - Pedestrian button to stay red on all heads for 7 secs
    - Non-blocking timing using millis()
*/

// Define States
enum State { NS_GREEN, NS_YELLOW, ALL_RED, EW_GREEN, EW_YELLOW, PEDESTRIAN};
State currentState = NS_GREEN;
State nextStateAfterAllRed = EW_GREEN;

// Timing Constants
const unsigned long GREEN_TIME = 7000UL;
const unsigned long YELLOW_TIME = 3000UL;
const unsigned long ALL_RED_TIME = 1000UL;
const unsigned long PEDESTRIAN_TIME = 7000UL;
const unsigned long FLASH_INTERVAL = 500UL;
const unsigned long BUTTON_DEBOUNCE = 200UL;
const unsigned long EMER_DEBOUNCE = 300UL;

// Pin Mapping
const int NS1_RED = 22, NS1_YEL = 24, NS1_GRN = 26;
const int NS2_RED = 46, NS2_YEL = 48, NS2_GRN = 50;
const int EW1_RED = 30, EW1_YEL = 32, EW1_GRN = 34;
const int EW2_RED = 38, EW2_YEL = 40, EW2_GRN = 42;
const int PED_LED = 49;

const int EMER_PIN = 2;
const int PED_PIN = 53;

// Flags & timers
volatile bool emergencyTriggered = false;
bool emergencyMode = false;
unsigned long lastEmergencyToggle = 0;

bool pedRequest = false;
unsigned long lastPedDebounce = 0;

unsigned long stateMillis = 0;
unsigned long flashMillis = 0;
bool flashOn = false;

// Helper Functions
void setSignal(int r, int y, int g, bool rOn, bool yOn, bool gOn) {
  digitalWrite(r, rOn ? HIGH : LOW);
  digitalWrite(y, yOn ? HIGH : LOW);
  digitalWrite(g, gOn ? HIGH : LOW);
}

void allLightsOff() {
  setSignal(NS1_RED, NS1_YEL, NS1_GRN, false,false,false);
  setSignal(NS2_RED, NS2_YEL, NS2_GRN, false,false,false);
  setSignal(EW1_RED, EW1_YEL, EW1_GRN, false,false,false);
  setSignal(EW2_RED, EW2_YEL, EW2_GRN, false,false,false);
}

void setAllRed() {
  setSignal(NS1_RED, NS1_YEL, NS1_GRN, true,false,false);
  setSignal(NS2_RED, NS2_YEL, NS2_GRN, true,false,false);
  setSignal(EW1_RED, EW1_YEL, EW1_GRN, true,false,false);
  setSignal(EW2_RED, EW2_YEL, EW2_GRN, true,false,false);
}

// Emergency Interrupt
void emergencyISR() {
  emergencyTriggered = true;
}

// Pedestrian Button Polling
void pollPedButton(unsigned long now) {
  static bool lastPedState = HIGH;
  int reading = digitalRead(PED_PIN);
  if (reading != lastPedState) {
    lastPedState = reading;
    if (reading == LOW && (now - lastPedDebounce) > BUTTON_DEBOUNCE) {
      Serial.println("Pedestrian pressed!");
      pedRequest = true;
      lastPedDebounce = now;
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Outputs
  int outs[] = {NS1_RED, NS1_YEL, NS1_GRN, NS2_RED, NS2_YEL, NS2_GRN, PED_LED,
                EW1_RED, EW1_YEL, EW1_GRN, EW2_RED, EW2_YEL, EW2_GRN};

  for (int i = 0; i < 13; ++i) {
    pinMode(outs[i], OUTPUT);
    digitalWrite(outs[i], LOW);
  }

  // Inputs
  pinMode(EMER_PIN, INPUT_PULLUP); // Emergency Button
  pinMode(PED_PIN, INPUT_PULLUP); // Pedestrian Button

  attachInterrupt(digitalPinToInterrupt(EMER_PIN), emergencyISR, FALLING);

  stateMillis = millis();
}

void loop() {
  unsigned long now = millis();

  // Handle Emergency Button
  if (emergencyTriggered) {
    Serial.println("Emergency pressed!");
    if (now - lastEmergencyToggle > EMER_DEBOUNCE) {
      emergencyMode = !emergencyMode;
      lastEmergencyToggle = now;
    }
    emergencyTriggered = false;
  }

  // When in Emergency Mode
  if (emergencyMode) {
    if (now - flashMillis >= FLASH_INTERVAL) {
      flashMillis = now;
      flashOn = !flashOn;
      if (flashOn) setAllRed();
      else allLightsOff();
    }
    return;
  }

  // Check for Pedestrian Button Press
  pollPedButton(now);

  // FSM
  switch (currentState) {
    case NS_GREEN:
      setSignal(NS1_RED, NS1_YEL, NS1_GRN, false, false, true);
      setSignal(NS2_RED, NS2_YEL, NS2_GRN, false, false, true);
      setSignal(EW1_RED, EW1_YEL, EW1_GRN, true, false, false);
      setSignal(EW2_RED, EW2_YEL, EW2_GRN, true, false, false);
      if (now - stateMillis >= GREEN_TIME) {
        currentState = NS_YELLOW;
        stateMillis = now;
      }
      break;

    case NS_YELLOW:
      setSignal(NS1_RED, NS1_YEL, NS1_GRN, false, true, false);
      setSignal(NS2_RED, NS2_YEL, NS2_GRN, false, true, false);
      setSignal(EW1_RED, EW1_YEL, EW1_GRN, true, false, false);
      setSignal(EW2_RED, EW2_YEL, EW2_GRN, true, false, false);
      if (now - stateMillis >= YELLOW_TIME) {
        nextStateAfterAllRed = EW_GREEN;
        currentState = ALL_RED;
        stateMillis = now;
      }
      break;

    case EW_GREEN:
      setSignal(NS1_RED, NS1_YEL, NS1_GRN, true, false, false);
      setSignal(NS2_RED, NS2_YEL, NS2_GRN, true, false, false);
      setSignal(EW1_RED, EW1_YEL, EW1_GRN, false, false, true);
      setSignal(EW2_RED, EW2_YEL, EW2_GRN, false, false, true);
      if (now - stateMillis >= GREEN_TIME) {
        currentState = EW_YELLOW;
        stateMillis = now;
      }
      break;

    case EW_YELLOW:
      setSignal(NS1_RED, NS1_YEL, NS1_GRN, true, false, false);
      setSignal(NS2_RED, NS2_YEL, NS2_GRN, true, false, false);
      setSignal(EW1_RED, EW1_YEL, EW1_GRN, false, true, false);
      setSignal(EW2_RED, EW2_YEL, EW2_GRN, false, true, false);
      if (now - stateMillis >= YELLOW_TIME) {
        nextStateAfterAllRed = NS_GREEN;
        currentState = ALL_RED;
        stateMillis = now;
      }
      break;

    case ALL_RED:
      setAllRed();
      if (now - stateMillis >= ALL_RED_TIME) {
        if (pedRequest) {
          currentState = PEDESTRIAN;
          stateMillis = now;
          pedRequest = false;
        }
        else {
          currentState = nextStateAfterAllRed;
          stateMillis = now;
        }
      }
      break;

    case PEDESTRIAN:
      setAllRed();
      if (now - stateMillis < PEDESTRIAN_TIME - 3000) {
        digitalWrite(PED_LED, HIGH); // Turn on PED_LED
      } 
      else {
        if((now / 250) % 2 == 0) digitalWrite(PED_LED, HIGH);
        else digitalWrite(PED_LED, LOW);
      }
      if (now - stateMillis >= PEDESTRIAN_TIME) {
        digitalWrite(PED_LED, LOW);
        currentState = nextStateAfterAllRed;
        stateMillis = now;
      }
    break;
  }
}
