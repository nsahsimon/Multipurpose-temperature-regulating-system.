/*
  LiquidCrystal Library - Hello World

  Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
  library works with all LCD displays that are compatible with the
  Hitachi HD44780 driver. There are many of them out there, and you
  can usually tell them by the 16-pin interface.

  This sketch prints "Hello World!" to the LCD
  and shows the time.

  The circuit:
   LCD RS pin to digital pin 7
   LCD Enable pin to digital pin 6
   LCD D4 pin to digital pin 5
   LCD D5 pin to digital pin 4
   LCD D6 pin to digital pin 3
   LCD D7 pin to digital pin 2
   LCD R/W pin to ground
   LCD VSS pin to ground
   LCD VCC pin to 5V
   10K resistor:
   ends to +5V and ground
   wiper to LCD VO pin (pin 3)

  Library originally added 18 Apr 2008
  by David A. Mellis
  library modified 5 Jul 2009
  by Limor Fried (http://www.ladyada.net)
  example added 9 Jul 2009
  by Tom Igoe
  modified 22 Nov 2010
  by Tom Igoe */
  
// include the library code:
#include <LiquidCrystal.h>
#include <Keypad.h>
const byte ROWS = 4;
const byte COLS = 3;
int fanpin = A2,fanled = A4; //initialising and declaring the arduino pin in charge of the fan.
int heaterpin = A3,heaterled = A5; //initialing and declairing th arduino pin in charge of the heater.
#include <dht.h>
#define tempPin A1
dht DHT;

char hexakeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'},
};
byte rowPins[ROWS] = {11, 10, 9, 8};
byte colPins[COLS] = {12, 13, 14};

Keypad customkeypad = Keypad(makeKeymap(hexakeys), rowPins, colPins, ROWS, COLS);


// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup()
{
  // set up the LCD's number of columns and rows:
  pinMode(fanpin,OUTPUT);
  pinMode(heaterpin,OUTPUT);
  pinMode(fanled,OUTPUT);
  pinMode(heaterled,OUTPUT);
  //Serial.begin(9600);
  //analogReference(INTERNAL);//sets the reference voltage of pin A3 to 1.1 (since we are using the lM35 temp sensor);
  lcd.begin(16, 2);
  lcd.setCursor(4,0); //centralising welcome
  lcd.print("WELCOME");
  delay(1000);
  lcd.clear();
}
int MIN[2],MAX[2]; /*TEMP holds the digits for temperature inputed by the user,
                      TOL holds the digits for the toleracne inputed by the user*/

int max_temp = 28,min_temp = 26,mean_temp;   /*user_temp holds the user defined temperature
                                                              max_temp holds the user defiend maximum temperature
                                                              min_temp holds the user defined miminum temperature
                                                              mean_temp holds the mean value of max_temp and mean_temp
                                                              */
float box_temp;
int target_temp = 0,initial_tem,stop_temp,heating_range,start_temp; 
void loop()
{
  char customkey = customkeypad.getKey(); //this variable holds the character corresponding to the key the user has pressed.
  
  DHT.read22(tempPin);
  box_temp = DHT.temperature;
  
  
  if(customkey) //if a key is pressed, get it .
  {
    if(customkey == '#')  //takes user to the max temperature setting page if # key is press
    {
      get_max_temp();
      delay(1000);                  
    }

    if(customkey == '*') //takes the user the min  temperature setting page if the * key is pressed.
    {
      get_min_temp();         
      delay(1000);
    }
    if(box_temp > max_temp || box_temp < min_temp) target_temp = mean_temp; //checking if the box temperature is within range
    else target_temp = 0;
    delay(2500);
    lcd.clear();
  }
  
  while(min_temp > max_temp) //if the maximum temperature is less than the minimum temperature, the user should re-enter the temperatures.
  {
    lcd.print("INVALID INPUT!!!");
    delay(1000);
    get_max_temp();
    delay(1000);
    get_min_temp();
    delay(1000);
    lcd.clear();
    target_temp = 0;
    if(box_temp > max_temp || box_temp < min_temp) target_temp = mean_temp;
    else target_temp = 0;
    }
  hmscr();
  mean_temp = 0.5*(max_temp + min_temp);   //holds the average of the max and min temperatures.
  
  if(box_temp > max_temp) 
    target_temp = mean_temp; //checking if the box's temperature is out of range
  if(box_temp < min_temp){
    if (target_temp = 0) 
    {
      start_temp = box_temp;
      heating_range = mean_temp - start_temp;
      stop_temp1 = 0.5*heating_range;
      stop_temp2 = 0.75*heating_range;
      stop_temp3 = 0.875*heating_range; 
    }
    target_temp = mean_temp;
    start_temp = box_temp;
  }

  
  if(target_temp != 0 && box_temp > target_temp)//if the box temperature is above the range, turn fan on and turn the heater off.
  {
     digitalWrite(fanpin,HIGH); 
     digitalWrite(A4,HIGH);
     digitalWrite(heaterpin,LOW); 
     digitalWrite(A5,LOW);                         
  }

   if(target_temp != 0 && box_temp < target_temp)//if the box temperature is below the range, turn heater on and turn the fan off.
  {
     digitalWrite(fanpin,LOW);
     digitalWrite(A4,LOW);        
     digitalWrite(heaterpin,HIGH); 
     digitalWrite(A5,HIGH);     
  }

  else if(target_temp == box_temp || target_temp == 0)//once the target temperature is attained ,turn all the devices off.
  {
    target_temp = 0;
    digitalWrite(fanpin,LOW); 
    digitalWrite(A4,LOW);       
    digitalWrite(heaterpin,LOW);
    digitalWrite(A5,LOW); 
  }
}





void hmscr() //this is the home display. It gives a summary of the user's settings.
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("TEMP= "); //displays the current temperature in the box
  //Serial.println(box_temp);
  lcd.print(box_temp);
  lcd.print(" C");
  
  
  lcd.setCursor(0,1);
  
  if(max_temp != min_temp) //do this if the maximun temperature is not equal to the mininum temperature
  {
    lcd.print("RANGE:"); //displays the user defined parameters. i.e max and minimum temperatures.
    lcd.print(min_temp);
    lcd.print(" C");
    lcd.print("-");
    lcd.print(max_temp);
    lcd.print(" C"
    );
  }

  else //do this if the maximum temperature is equal to the minimum temperature.
  {
     lcd.print("SET TEMP= ");
     lcd.print(min_temp);
     lcd.print(" C");
  }
}





void get_min_temp()//this function retrieves the user defined maximum temperature
{ 
  
  char customkey;                       //this variable holds the different characters pressed by the user
                                                                  
  int counter = 0;                      //counter to loop through all the digits
  lcd.clear();
  lcd.print("MIN TEMPERATURE");         //prompts user to set the minimum temperature
  lcd.setCursor(0,1);                   //sets the cursor to the firt cell of the second row
  
  while(counter < 2)
  {
    customkey = customkeypad.getKey();
    if(customkey)
    {
      if(customkey == '#' || customkey == '*') {;} //rejection of any non-numeric entry 
      
      else //do this if a numeric key is entered.
      {
        MIN[counter] = customkey;
        MIN[counter] = MIN[counter] - 48; //generates the actual number pressed from its encoded form.
        lcd.print(MIN[counter]); //displays the digits entered by the user
        counter++;
      }
    }
  }
  min_temp = 10*MIN[0] + MIN[1]; //calculating the minimum temperature value from its digits.
}




void get_max_temp()//this function retrieves the user defined maximum temperature
{ 
  
  char customkey;                       //this variable holds the different characters
                                                                             
  int counter = 0;                      //counter to loop through all the digits
  lcd.clear();
  lcd.print("MAX TEMPERATURE:");         //prompts user to set the maximum temperature
  lcd.setCursor(0,1);                    //sets the cursor to the firt cell on the second row
  
  while(counter < 2)//gets exactly a 2 digit number from the user.
  {
    customkey = customkeypad.getKey();
    if(customkey)
    {
      if(customkey == '#' || customkey == '*') {;} //rejection of any non-numeric entry 
      
      else //do this if a numeric key is entered.
      {
        MAX[counter] = customkey;
        MAX[counter] = MAX[counter] - 48; //generates the actual number pressed from its encoded form.
        lcd.print(MAX[counter]); //shows the user the key he just pressed.
        counter++;
      }
    }
  }
  max_temp = 10*MAX[0] + MAX[1]; // calculates the value of the maximum temperature from its digits.
  
  if (max_temp >= 50) max_temp = 50;
}
