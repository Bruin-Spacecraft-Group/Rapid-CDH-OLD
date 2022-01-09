
const int resetPin = 9;
const int inputPin = 3;

void setup() {
  Serial.begin(9600);
  pinMode(resetPin, OUTPUT);
  pinMode(inputPin, INPUT);
}

const int waitCycles = 50000;
const int shutDownWaitCycles = 0xFFFF;
const int startUpWaitCycles = 0xFFFF;

enum StateKind {
  WAITING_FOR_SWITCH_TO_HIGH,
  WAITING_FOR_SWITCH_TO_LOW,
  SHUTTING_DOWN,
  WAITING_FOR_RPI_TO_SHUT_DOWN,
  STARTING_UP,
  WAITING_FOR_RPI_TO_START_UP,
  EXPECT_RPI_READY
};

int state = WAITING_FOR_SWITCH_TO_HIGH;
int counter = 0;
void loop() {
  switch(state) {
    case WAITING_FOR_SWITCH_TO_HIGH: 
    if(digitalRead(inputPin) == HIGH) {
      state = WAITING_FOR_SWITCH_TO_LOW;
    } else if(counter > waitCycles) {
      state = SHUTTING_DOWN;
    } else {
      counter++;
    }
    break;
    case WAITING_FOR_SWITCH_TO_LOW:
    if(digitalRead(inputPin) == LOW) {
      state = WAITING_FOR_SWITCH_TO_HIGH;
    } else if(counter > waitCycles) {
      state = SHUTTING_DOWN;
    } else {
      counter++;
    }
    break;
    case SHUTTING_DOWN:
    counter = 0;
    digitalWrite(resetPin, HIGH);
    state = WAITING_FOR_RPI_TO_SHUT_DOWN;
    break;
    case WAITING_FOR_RPI_TO_SHUT_DOWN:
    if(counter > shutDownWaitCycles) {
      state = STARTING_UP;
    } else {
      counter++;
    }
    break;
    case STARTING_UP:
    counter = 0;
    digitalWrite(resetPin, LOW);
    state = WAITING_FOR_RPI_TO_START_UP;
    break;
    case WAITING_FOR_RPI_TO_START_UP:
    if(counter > startUpWaitCycles) {
      state = EXPECT_RPI_READY;
    } else {
      counter++;
    }
    break;
    case EXPECT_RPI_READY:
    counter = 0;
    state = WAITING_FOR_SWITCH_TO_HIGH;
    break;
  }
}
