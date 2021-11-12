#include <Arduino.h>
#include <map>
#include <stdarg.h>
#include <string>

#define LED_1 18
#define LED_2 19

#define SERIAL_PRINTF_MAX_BUFF 256
#define EXECUTION_DELAY 800

// Instructions
#define EXT 0x01
#define SLP 0x02 // 1 arg: time in seconds
#define WRT 0x03 // 2 args: PIN id, output state

byte memory[256] = {0};
byte counter = 0;
std::map<byte, std::string> inst_names;

// source:
// https://medium.com/@kslooi/print-formatted-data-in-arduino-serial-aaea9ca840e3
void serialPrintf(const char *fmt, ...) {
  /* Buffer for storing the formatted data */
  char buff[SERIAL_PRINTF_MAX_BUFF];
  /* pointer to the variable arguments list */
  va_list pargs;
  /* Initialise pargs to point to the first optional argument */
  va_start(pargs, fmt);
  /* create the formatted data and store in buff */
  vsnprintf(buff, SERIAL_PRINTF_MAX_BUFF, fmt, pargs);
  va_end(pargs);
  Serial.print(buff);
}

void runInstruction() {
  byte instruction = memory[counter];
  serialPrintf("Read instruction %#x (%s) at [%d]\n", instruction,
               inst_names[instruction].c_str(), counter);

  switch (instruction) {
  case SLP: {
    byte seconds = memory[counter + 1];
    counter += 2;
    serialPrintf("Wait for %d seconds\n", seconds);
    delay(seconds * 1000);
    return;
  }
  case WRT: {
    byte pin = memory[counter + 1];
    byte state = memory[counter + 2];
    counter += 3;
    serialPrintf("Write value %d to pin %d\n", state, pin);
    digitalWrite(pin, state);
    return;
  }
  case EXT:
  default: {
    counter = 0;
    serialPrintf("Exit flag\n");
    return;
  }
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  Serial.begin(9600);

  inst_names.insert(std::make_pair(EXT, "Exit"));
  inst_names.insert(std::make_pair(SLP, "Sleep"));
  inst_names.insert(std::make_pair(WRT, "Write"));

  // example code
  byte c = 0;
  memory[c++] = WRT;
  memory[c++] = LED_1;
  memory[c++] = HIGH;
  memory[c++] = WRT;
  memory[c++] = LED_2;
  memory[c++] = HIGH;
  memory[c++] = SLP;
  memory[c++] = 0x02;
  memory[c++] = WRT;
  memory[c++] = LED_1;
  memory[c++] = LOW;
  memory[c++] = WRT;
  memory[c++] = LED_2;
  memory[c++] = LOW;
  memory[c++] = SLP;
  memory[c++] = 0x02;
  memory[c++] = EXT;

  counter = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  runInstruction();

  if (EXECUTION_DELAY)
    delay(EXECUTION_DELAY);
}
