#include<Keypad.h>
const byte ROWS = 4;
const byte COLS = 3;

char hexakeys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'},
};
byte rowPins[ROWS] = {11,10,9,8};
byte colPins[COLS] = {12,13,14};

Keypad customkeypad = Keypad(makeKeymap(hexakeys),rowPins,colPins,ROWS,COLS);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
    // put your main code here, to run repeatedly:
  char customkey = customkeypad.getKey();

  if(customkey)Serial.println(customkey);
}
