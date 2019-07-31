//Quest room control system//

/*algorytm:
Operator chooses game mode by pushing a select_mode_but, mode_leds indicate which mode is chosen
start_but starts a game, reset_but - switches game into a reset mode.

There is a few activities, which are working in a separate modes:
-Smoke machine
-Music
-Led projector
-RGB LED
-Syrene-lamp
-Disco-lamp
-Timer
-Score indicators

*/
#include <DFPlayer_Mini_Mp3.h>

#define rs485_direction_pin 2   //RS485 Direction control

//console buttons and leds
#define select_mode_but 52
#define start_but 50
#define reset_but 48
#define mode1_led 46
#define mode2_led 44
#define mode3_led 42

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

//12V N-channel MOSFET pins
#define M_1_rgb_led_1 53 //Catch_up mode - all time //Artefact mode - every 2 min for a 30 secs
#define M_2_rgb_led_2 51 //Catch_up mode - all time //Artefact mode - every 2 min for a 30 secs
#define M_3_syrene1 49 //artefact mode - 5,4,3,2,1 min before the end of the round for a 10 secs
#define M_4_syrene2 47 ////artefact mode - 5,4,3,2,1 min before the end of the round for a 10 secs
#define M_5 45
#define M_6 43
#define M_7 41
#define M_8 39

//220V time relay trigger pins
#define T_1_smoke_machine 8 //all modes - every 2 min for a 5-10 secs
#define T_2_disco_lamp 7 //all modes - every 2 min for a 10 secs
#define T_3_exit_door_led_projector 6 //all modes - at the end of the round
#define T_4 5

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

bool select_mode_but_flag = false; //button flag to realize trigger "push - on, release, push - off"
int mode = 1; // 1,2,3 - variable to store game mode parameter
bool start_state = false; //was pushed start button?

int counter_red = 0; //number of red cards founded
int counter_green = 0; //number of green cards founded


String string; //variable to store card uids

//Card uids
String red1 = "300311836#";
String red2 = "55922973#";
String red3 = "2978344986#";
String red4 = "2455429146#";
String red5 = "1917039386#";
String red6 = "2994181685#";
String red7 = "2203065883#";
String red8 = "294147356#";
String red9 = "324687645#";
String red10 = "2455523098#";

String green1 = "2455335450#";
String green2 = "2455522586#";
String green3 = "2434805530#";
String green4 = "325257757#";
String green5 = "2455109914#";
String green6 = "2989438517#";
String green7 = "1405263661#";
String green8 = "2734537269#";
String green9 = "2202833435#";
String green10 = "2202829595#";

String adm_key = "923640870#"; //admin key

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

unsigned long timer_game = 0;
unsigned long timer_smoke = 0;
unsigned long timer_disco = 0;
unsigned long timer_projector = 0;
unsigned long timer_reset = 0;

bool is_timer_active = false;
bool is_game_over = false;
bool is_smoke_started = false;
bool is_disco = false;
bool is_exit_projector = false;
bool is_T4 = false;
bool reset_state = false;

//initiate indicators functoins
void Num1_red_Write(int);
void Num2_green_Write(int);

void setup()
{
	Serial.begin(9600);
	Serial1.begin(9600); //dfplayer serial
	Serial2.begin(9600); //rs485 serial

  pinMode(rs485_direction_pin, OUTPUT); 
  digitalWrite(rs485_direction_pin, LOW); // переводим устройство в режим приёмника

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

  pinMode(M_1_rgb_led_1, OUTPUT);
  pinMode(M_2_rgb_led_2, OUTPUT);
  pinMode(M_3_syrene1, OUTPUT);
  pinMode(M_4_syrene2, OUTPUT);
  pinMode(M_5, OUTPUT);
  pinMode(M_6, OUTPUT);
  pinMode(M_7, OUTPUT);
  pinMode(M_8, OUTPUT);
  digitalWrite(M_1_rgb_led_1, LOW);
  digitalWrite(M_2_rgb_led_2, LOW);
  digitalWrite(M_3_syrene1, LOW);
  digitalWrite(M_4_syrene2, LOW);
  digitalWrite(M_5, LOW);
  digitalWrite(M_6, LOW);
  digitalWrite(M_7, LOW);
  digitalWrite(M_8, LOW);

  pinMode(T_1_smoke_machine, OUTPUT);
  pinMode(T_2_disco_lamp, OUTPUT);
  pinMode(T_3_exit_door_led_projector, OUTPUT);
  pinMode(T_4, OUTPUT);

  mp3_set_serial(Serial1);
  mp3_stop(); // stop any track playing after controller reset 
  delay(100);

  Serial.println("Helloword");
}

void loop()
{
  /*
  for(int i = 0; i < 10; i++)
  {
    Serial.println(i);
    Num1_red_Write(i);
    Num2_green_Write(i);
    delay(1000);
  }  */
 
	choose_mode(); //select game mode after powering on

	if(digitalRead(start_but) == LOW)
	{
    delay(50);
		start_state = true;

    digitalWrite(T_3_exit_door_led_projector, LOW);
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
      cossacks_mode();
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


  //////////////////reset/////////////
  if(digitalRead(reset_but) == LOW && reset_state == false)
  {
    delay(100);
    start_state = false;

    mp3_pause();
    mp3_stop();
    mp3_set_volume(0);

    is_timer_active = false;
    is_game_over = false;
    is_smoke_started = false;
    is_disco = false;
    is_exit_projector = false;
    is_T4 = false;

    digitalWrite(M_1_rgb_led_1, LOW); //power off
    digitalWrite(M_2_rgb_led_2, LOW); //power off
    digitalWrite(M_3_syrene1, LOW); //power off
    digitalWrite(M_4_syrene2, LOW); //power off
    digitalWrite(T_1_smoke_machine, LOW);
    digitalWrite(T_2_disco_lamp, LOW);
    digitalWrite(T_3_exit_door_led_projector, LOW);

    timer_reset = millis();

    digitalWrite(T_3_exit_door_led_projector, HIGH); //set a p4
    Serial.println("Exit_projector on");
    reset_state = true;
  }

  if((millis()-timer_reset >= 180000)) //3minutes has left
    {
      //send a start signal to an external timer
      digitalWrite(T_3_exit_door_led_projector, LOW); //set a p4

      Serial.println("Exit_projector off");
    }

  if(digitalRead(reset_but) == HIGH)
  {
    delay(100);
    reset_state = false;
  }
  
}

void choose_mode()
{
  if(digitalRead(select_mode_but) == LOW && select_mode_but_flag == false && mode == 1) //if select_mode_but was pushed
	{
    delay(100);
    Serial.println("mode2");
		mode = 2;
    delay(100);
		select_mode_but_flag = true;
	}

	if(digitalRead(select_mode_but) == LOW && select_mode_but_flag == false && mode == 2) //if select_mode_but was pushed 2nd time
	{
    delay(100);
    Serial.println("mode3");
		mode = 3;
    delay(100);
		select_mode_but_flag = true;
	}

	if(digitalRead(select_mode_but) == LOW && select_mode_but_flag == false && mode == 3) //if select_mode_but was pushed 3rd time
	{
    delay(100);
    Serial.println("mode1");
		mode = 1;
    delay(100);
		select_mode_but_flag = true;
	}

	if(digitalRead(select_mode_but) == HIGH && select_mode_but_flag == true) //if select_mode_but was released after pushing
	{
		select_mode_but_flag = false; //turn the flag to the false state back
	}

}









////////////////////////////////////////// catch_up_mode /////////////////////////////////////////////
void catch_up_mode()
{
  //set an external timer to a 15 minutes mode
  //send a start signal
  if (is_timer_active == false)
  {
    Serial.println("catch_up_mode");

    timer_game = millis();
    timer_smoke = millis();
    timer_disco = millis();
    timer_projector = millis();

    mp3_set_serial(Serial1);
    mp3_set_volume(20);
    mp3_play(1);

    digitalWrite(M_1_rgb_led_1, HIGH); //power on
    digitalWrite(M_2_rgb_led_2, HIGH); //power on

    is_timer_active = true;
  }

  if(millis()-timer_smoke >= 120000)  //2 minutes has left
  {
    //send a start signal to an external timer
    digitalWrite(T_1_smoke_machine, HIGH); //set a p.4
    Serial.println("smoooooooke");

    if(millis()-timer_smoke >= 125000)
    {
        digitalWrite(T_1_smoke_machine, LOW);
        timer_smoke = millis();
      }
  }

  if((millis()-timer_disco >= 180000)) //3minutes has left
  {
    //send a start signal to an external timer
    digitalWrite(T_2_disco_lamp, HIGH); //set a p4
    Serial.println("disco-disco");

    if(millis()-timer_disco >= 190000)
    {
      digitalWrite(T_2_disco_lamp, LOW);
      timer_disco = millis();
    }
  }

  if((millis()-timer_projector >= 900000)) //3minutes has left
  {
    //send a start signal to an external timer
    digitalWrite(T_3_exit_door_led_projector, HIGH); //set a p4
    Serial.println("game is over");

    digitalWrite(M_1_rgb_led_1, LOW); //power off
    digitalWrite(M_2_rgb_led_2, LOW); //power off
    digitalWrite(M_3_syrene1, LOW); //power off
    digitalWrite(M_4_syrene2, LOW); //power off
    digitalWrite(T_1_smoke_machine, LOW);
    digitalWrite(T_2_disco_lamp, LOW);
    digitalWrite(T_3_exit_door_led_projector, LOW);

    if(millis()-timer_projector >= 1080000)
    {
      digitalWrite(T_3_exit_door_led_projector, LOW);
    }
  }

}






////////////////////////////////////////// cossacks_mode /////////////////////////////////////////////
void cossacks_mode()
{
  //set an external timer to a 15 minutes mode
  //send a start signal
  if (is_timer_active == false)
  {
    Serial.println("catch_up_mode");

    timer_game = millis();
    timer_smoke = millis();
    timer_disco = millis();
    timer_projector = millis();

    mp3_set_serial(Serial1);
    mp3_set_volume(20);
    mp3_play(2);

    is_timer_active = true;
  }

  if(millis()-timer_smoke >= 120000)  //2 minutes has left
  {
    //send a start signal to an external timer
    digitalWrite(T_1_smoke_machine, HIGH); //set a p.4
    Serial.println("smoooooooke");

    if(millis()-timer_smoke >= 125000)
    {
        digitalWrite(T_1_smoke_machine, LOW);
        timer_smoke = millis();
      }
  }

  if((millis()-timer_disco >= 180000)) //3minutes has left
  {
    //send a start signal to an external timer
    digitalWrite(T_2_disco_lamp, HIGH); //set a p4
    Serial.println("disco-disco");

    if(millis()-timer_disco >= 190000)
    {
      digitalWrite(T_2_disco_lamp, LOW);
      timer_disco = millis();
    }
  }

  if((millis()-timer_projector >= 900000)) //15 minutes has left
  {
    //send a start signal to an external timer
    digitalWrite(T_3_exit_door_led_projector, HIGH); //set a p4

    Serial.println("game is over");

    digitalWrite(M_1_rgb_led_1, LOW); //power off
    digitalWrite(M_2_rgb_led_2, LOW); //power off
    digitalWrite(M_3_syrene1, LOW); //power off
    digitalWrite(M_4_syrene2, LOW); //power off
    digitalWrite(T_1_smoke_machine, LOW);
    digitalWrite(T_2_disco_lamp, LOW);
    digitalWrite(T_3_exit_door_led_projector, LOW);

    if(millis()-timer_projector >= 1080000) //3 minutes has left
    {
      digitalWrite(T_3_exit_door_led_projector, LOW);
    }
  }

}








////////////////////////////////////////// artefact_mode /////////////////////////////////////////////
void artefact_mode()
{
  //set an external timer to a 15 minutes mode
  //send a start signal
  if (is_timer_active == false)
  {
    Serial.println("catch_up_mode");
    timer_game = millis();
    is_timer_active = true;
  }

	digitalWrite(rs485_direction_pin, LOW); //set a rs485 port to a recieve mode
	if (Serial2.available()) 
	{
		string = "";
		delay(100);
		rs485_recieve();
	}

  Num1_red_Write(counter_red); // send to a Num1_red_Write function a counter_red value
  Num2_green_Write(counter_green);

  digitalWrite(T_1_smoke_machine, HIGH); //send a signal to trigger to start a loop //set a p3.1 with 5/120/---
  digitalWrite(T_2_disco_lamp, HIGH); //send a signal to trigger to start a loop //set a p3.1 with 10/180/---
  delay(500);
  digitalWrite(T_1_smoke_machine, LOW);
  digitalWrite(T_2_disco_lamp, LOW);


  if(millis()-timer_game == 1500000) //25 min(1500s) has left
  {
    digitalWrite(M_3_syrene1, HIGH);
    if(millis()-timer_game == 1510000) //+10sec
    {
      digitalWrite(M_3_syrene1, LOW);
    }
  }

  if(millis()-timer_game == 1560000) //26 min(1560s) has left
  {
    digitalWrite(M_3_syrene1, HIGH);
    if(millis()-timer_game == 1570000) //+10sec
    {
      digitalWrite(M_3_syrene1, LOW);
    }
  }

  if(millis()-timer_game == 1620000) //27 min(1620s) has left
  {
    digitalWrite(M_3_syrene1, HIGH);
    if(millis()-timer_game == 1630000) //+10sec
    {
      digitalWrite(M_3_syrene1, LOW);
    }
  }

  if(millis()-timer_game == 1680000) //28 min(1680s) has left
  {
    digitalWrite(M_3_syrene1, HIGH);
    if(millis()-timer_game == 1690000) //+10sec
    {
      digitalWrite(M_3_syrene1, LOW);
    }
  }

  if(millis()-timer_game == 1740000) //29 min(1740s) has left
  {
    digitalWrite(M_3_syrene1, HIGH);
    if(millis()-timer_game == 1750000) //+10sec
    {
      digitalWrite(M_3_syrene1, LOW);
    }
  }

  if(millis()-timer_game == 1800000) //30 min(1800s) has left
  {
    digitalWrite(T_1_smoke_machine, HIGH); //send a signal to trigger to end a loop //set a p3.1 with 5/120/---
    digitalWrite(T_2_disco_lamp, HIGH); //send a signal to trigger to end a loop //set a p3.1 with 10/180/---
    digitalWrite(T_3_exit_door_led_projector, HIGH); //send a signal to trigger to start a one loop //set a p3.1 with 0/180/1
    delay(500);
    digitalWrite(T_3_exit_door_led_projector, LOW);
    digitalWrite(T_1_smoke_machine, LOW);
    digitalWrite(T_2_disco_lamp, LOW);

    digitalWrite(M_3_syrene1, HIGH);
    if(millis()-timer_game == 1810000) //+10sec
    {
      digitalWrite(M_3_syrene1, LOW);
    }
  }

}

void rs485_recieve() 
{              //recieve something from rs485 inerface
  digitalWrite(rs485_direction_pin, LOW);
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