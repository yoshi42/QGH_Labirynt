//Quest room control system//

#include <DFPlayer_Mini_Mp3.h>
#include <SoftwareSerial.h>


#define SSerialRX        6  //RS485 SoftwareSerial Receive pin
#define SSerialTX        7  //RS485 SoftwareSerial Transmit pin
#define SSerialTxControl 8   //RS485 Direction control

#define SSRx_music 9   //music player SoftwareSerial Receive pin
#define SSTx_music 10   //music player SoftwareSerial Transmit pin

//console buttons and leds
#define select_mode_but 11
#define start_but 12
#define reset_but 13
#define mode1_led 14
#define mode2_led 15
#define mode3_led 16

bool select_mode_but_flag = false; //button flag to realize trigger push - on, release, push - off
int mode = 1; // 1,2,3 - variable to choose game mode
bool start_state = false; //is pushed start button?

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
String string; //variable to write and cheeck card uids

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

SoftwareSerial musicSerial(SSRx_music, SSTx_music); // RX, TX плеер музыKа
SoftwareSerial RS485Serial(SSerialRX, SSerialTX); // RX, TX

void setup()
{
	Serial.begin(9600);
	RS485Serial.begin(9600);
	musicSerial.begin(9600);

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
}

void loop()
{
	choose_mode();
	if(digitalRead(start_but) == LOW)
	{
		start_state = true;
	}
	

	if(mode == 1)
	{
		digitalWrite(mode1_led == HIGH); //indicates game mode
		digitalWrite(mode2_led == LOW);
		digitalWrite(mode3_led == LOW);
		if(start_state == true)
		{
			catch_up_mode();
		}
	}

	if(mode == 2)
	{
		digitalWrite(mode1_led == LOW);
		digitalWrite(mode2_led == HIGH);
		digitalWrite(mode3_led == LOW);
		if(start_state == true)
		{
			artefact_mode();
		}
	}

	if(mode == 3)
	{
		digitalWrite(mode1_led == LOW);
		digitalWrite(mode2_led == LOW);
		digitalWrite(mode3_led == HIGH);
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

	if(digitalRead(select_mode_but == HIGH) && select_mode_but_flag == true) //if select_mode_but was realized after pushing
	{
		select_mode_but_flag = false;
	}

}



void catch_up_mode()
{
	
}

void artefact_mode()
{
	digitalWrite(SSerialTxControl, LOW);

	if (RS485Serial.available()) 
	{
		string = "";
		delay(100);
		rs485_recieve();
	}

}

void cossacks_mode()
{
	
}

void rs485_recieve() {              //recieve something from rs485 inerface
  digitalWrite(SSerialTxControl, LOW);
  while (RS485Serial.available())
  {
    char inChar = RS485Serial.read();
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