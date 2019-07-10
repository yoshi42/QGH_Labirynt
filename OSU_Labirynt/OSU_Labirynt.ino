//Quest room control system//

#include <DFPlayer_Mini_Mp3.h>

#define SSerialTxControl 8   //RS485 Direction control

//console buttons and leds
#define select_mode_but 11
#define start_but 12
#define reset_but 13
#define mode1_led 14
#define mode2_led 15
#define mode3_led 16

//define 7-seg indicators - pins a0-a6, a8-a14
//indicator red_left
#define ind1_A A0
#define ind1_B A1
#define ind1_C A2
#define ind1_D A3
#define ind1_E A4
#define ind1_F A5
#define ind1_G A6

//indicator green_right
#define ind2_A A8
#define ind2_B A9
#define ind2_C A10
#define ind2_D A11
#define ind2_E A12
#define ind2_F A13
#define ind2_G A14

// array to store indicator nuber combinations
int num_array[10][7] = {  { 1,1,1,1,1,1,0 },    // 0
                          { 0,1,1,0,0,0,0 },    // 1
                          { 1,1,0,1,1,0,1 },    // 2
                          { 1,1,1,1,0,0,1 },    // 3
                          { 0,1,1,0,0,1,1 },    // 4
                          { 1,0,1,1,0,1,1 },    // 5
                          { 1,0,1,1,1,1,1 },    // 6
                          { 1,1,1,0,0,0,0 },    // 7
                          { 1,1,1,1,1,1,1 },    // 8
                          { 1,1,1,0,0,1,1 }};   // 9
                                       
//initiate indicators functoins
void Num1_red_Write(int);
void Num2_green_Write(int);

bool select_mode_but_flag = false; //button flag to realize trigger "push - on, release, push - off"
int mode = 1; // 1,2,3 - variable to store game mode parameter
bool start_state = false; //was pushed start button?

int counter_red = 0; //number of red cards founded
int counter_green = 0; //number of green cards founded

//Card uids
String red1 = "12345#";
String red2 = "12345#";
String red3 = "12345#";
String red4 = "12345#";
String red5 = "12345#";
String red6 = "12345#";
String red7 = "12345#";
String red8 = "12345#";
String red9 = "12345#";
String red10 = "12345#";

String green1 = "12345#";
String green2 = "12345#";
String green3 = "12345#";
String green4 = "12345#";
String green5 = "12345#";
String green6 = "12345#";
String green7 = "12345#";
String green8 = "12345#";
String green9 = "12345#";
String green10 = "12345#";
String string; //variable to store card uids

//flags to read card uid just once per game
bool red1_flag = false;
bool red2_flag = false;
bool red3_flag = false;
bool red4_flag = false;
bool red5_flag = false;
bool red6_flag = false;
bool red7_flag = false;
bool red8_flag = false;
bool red9_flag = false;
bool red10_flag = false;

bool green1_flag = false;
bool green2_flag = false;
bool green3_flag = false;
bool green4_flag = false;
bool green5_flag = false;
bool green6_flag = false;
bool green7_flag = false;
bool green8_flag = false;
bool green9_flag = false;
bool green10_flag = false;

void setup()
{
	Serial.begin(9600);
	Serial1.begin(9600); //dfplayer serial
	Serial2.begin(9600); //rs485 serial

  pinMode(SSerialTxControl, OUTPUT); 
  digitalWrite(SSerialTxControl, LOW); // переводим устройство в режим приёмника

  pinMode(select_mode_but, INPUT_PULLUP); 
  pinMode(start_but, INPUT_PULLUP); 
  pinMode(reset_but, INPUT_PULLUP); 

  pinMode(mode1_led, OUTPUT); 
  digitalWrite(mode1_led, LOW);
  pinMode(mode2_led, OUTPUT); 
  digitalWrite(mode2_led, LOW);
  pinMode(mode3_led, OUTPUT); 
  digitalWrite(mode3_led, LOW);

  pinMode(ind1_A, OUTPUT);
  pinMode(ind1_B, OUTPUT);
  pinMode(ind1_C, OUTPUT);
  pinMode(ind1_D, OUTPUT);
  pinMode(ind1_E, OUTPUT);
  pinMode(ind1_F, OUTPUT);
  pinMode(ind1_G, OUTPUT);

  pinMode(ind2_A, OUTPUT);
  pinMode(ind2_B, OUTPUT);
  pinMode(ind2_C, OUTPUT);
  pinMode(ind2_D, OUTPUT);
  pinMode(ind2_E, OUTPUT);
  pinMode(ind2_F, OUTPUT);
  pinMode(ind2_G, OUTPUT);

  mp3_stop(); // stop any track playing after controller reset
}

void loop()
{
	choose_mode(); //select game mode after powering on

	if(digitalRead(start_but) == LOW)
	{
    delay(50);
		start_state = true;
	}
	
	if(mode == 1)
	{
		digitalWrite(mode1_led, HIGH); //indicates game mode
		digitalWrite(mode2_led, LOW);
		digitalWrite(mode3_led, LOW);
		if(start_state == true)
		{
			catch_up_mode();
		}
	}

	if(mode == 2)
	{
		digitalWrite(mode1_led, LOW);
		digitalWrite(mode2_led, HIGH);
		digitalWrite(mode3_led, LOW);
		if(start_state == true)
		{
			artefact_mode();
		}
	}

	if(mode == 3)
	{
		digitalWrite(mode1_led, LOW);
		digitalWrite(mode2_led, LOW);
		digitalWrite(mode3_led, HIGH);
		if(start_state == true)
		{
			cossacks_mode();
		}
	}
}

void choose_mode()
{
	if(digitalRead(select_mode_but == LOW) && select_mode_but_flag == false && mode == 1) //if select_mode_but was pushed
	{
		mode = 2;
		select_mode_but_flag = true;
	}

	if(digitalRead(select_mode_but == LOW) && select_mode_but_flag == false && mode == 2) //if select_mode_but was pushed 2nd time
	{
		mode = 3;
		select_mode_but_flag = true;
	}

	if(digitalRead(select_mode_but == LOW) && select_mode_but_flag == false && mode == 3) //if select_mode_but was pushed 3rd time
	{
		mode = 1;
		select_mode_but_flag = true;
	}

	if(digitalRead(select_mode_but == HIGH) && select_mode_but_flag == true) //if select_mode_but was relized after pushing
	{
		select_mode_but_flag = false; //turn the flag to the false state back
	}
}

void catch_up_mode() //timer 15, smoke, audio, exit light 12v
{
	mp3_set_serial(Serial1);
  mp3_set_volume(20);
  mp3_play(1);

  //set timer to 15 minutes


}

void artefact_mode() //timer 15, smoke, audio, indicators+cards, strobe, blink-lamp
{
	digitalWrite(SSerialTxControl, LOW);

	if (Serial2.available()) 
	{
		string = "";
		delay(100);
		rs485_recieve();
	}

  Num1_red_Write(counter_red);
  Num2_green_Write(counter_green);

}

void cossacks_mode() //timer 15, smoke, audio, rgb led, exit light 12v
{
	
}

void rs485_recieve() 
{              //recieve something from rs485 inerface
  digitalWrite(SSerialTxControl, LOW);
  while (Serial2.available())
  {
    char inChar = Serial2.read();
    string += inChar;
    if (inChar == '#')
    {
      //red cards uid check
      if (red1_flag == false && string.equals(red1))
      {
      	red1_flag = true;
      	counter_red +=1;
      }

      if (red2_flag == false && string.equals(red2))
      {
      	red2_flag = true;
      	counter_red +=1;
      }

      if (red3_flag == false && string.equals(red3))
      {
      	red3_flag = true;
      	counter_red +=1;
      }

      if (red4_flag == false && string.equals(red4))
      {
      	red4_flag = true;
      	counter_red +=1;
      }

      if (red5_flag == false && string.equals(red5))
      {
      	red5_flag = true;
      	counter_red +=1;
      }

      if (red6_flag == false && string.equals(red6))
      {
      	red6_flag = true;
      	counter_red +=1;
      }

      if (red7_flag == false && string.equals(red7))
      {
      	red7_flag = true;
      	counter_red +=1;
      }

      if (red8_flag == false && string.equals(red8))
      {
      	red8_flag = true;
      	counter_red +=1;
      }

      if (red9_flag == false && string.equals(red9))
      {
      	red9_flag = true;
      	counter_red +=1;
      }

      if (red10_flag == false && string.equals(red10))
      {
      	red10_flag = true;
      	counter_red +=1;
      }

      //green cards uid check
      if (green1_flag == false && string.equals(green1))
      {
      	green1_flag = true;
      	counter_green +=1;
      }

      if (green2_flag == false && string.equals(green2))
      {
      	green2_flag = true;
      	counter_green +=1;
      }

      if (green3_flag == false && string.equals(green3))
      {
      	green3_flag = true;
      	counter_green +=1;
      }

      if (green4_flag == false && string.equals(green4))
      {
      	green4_flag = true;
      	counter_green +=1;
      }

      if (green5_flag == false && string.equals(green5))
      {
      	green5_flag = true;
      	counter_green +=1;
      }

      if (green6_flag == false && string.equals(green6))
      {
      	green6_flag = true;
      	counter_green +=1;
      }

      if (green7_flag == false && string.equals(green7))
      {
      	green7_flag = true;
      	counter_green +=1;
      }

      if (green8_flag == false && string.equals(green8))
      {
      	green8_flag = true;
      	counter_green +=1;
      }

      if (green9_flag == false && string.equals(green9))
      {
      	green9_flag = true;
      	counter_green +=1;
      }

      if (green10_flag == false && string.equals(green10))
      {
      	green10_flag = true;
      	counter_green +=1;
      }

    string = "";
    }
  }
}

void Num1_red_Write(int number) //light up red indicator number
{
  digitalWrite(ind1_A, num_array[number][0]);
  digitalWrite(ind1_B, num_array[number][1]);
  digitalWrite(ind1_C, num_array[number][2]);
  digitalWrite(ind1_D, num_array[number][3]);
  digitalWrite(ind1_E, num_array[number][4]);
  digitalWrite(ind1_F, num_array[number][5]);
  digitalWrite(ind1_G, num_array[number][6]);
}

void Num2_green_Write(int number) //light up red indicator number
{
  digitalWrite(ind2_A, num_array[number][0]);
  digitalWrite(ind2_B, num_array[number][1]);
  digitalWrite(ind2_C, num_array[number][2]);
  digitalWrite(ind2_D, num_array[number][3]);
  digitalWrite(ind2_E, num_array[number][4]);
  digitalWrite(ind2_F, num_array[number][5]);
  digitalWrite(ind2_G, num_array[number][6]);
}