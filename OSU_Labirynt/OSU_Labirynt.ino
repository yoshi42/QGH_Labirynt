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
#define timer_start 40
#define timer_set_30 38

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
int num_array[12][7] = {  { 1,1,1,1,1,1,0 },    // 0
                          { 0,1,1,0,0,0,0 },    // 1
                          { 1,1,0,1,1,0,1 },    // 2
                          { 1,1,1,1,0,0,1 },    // 3
                          { 0,1,1,0,0,1,1 },    // 4
                          { 1,0,1,1,0,1,1 },    // 5
                          { 1,0,1,1,1,1,1 },    // 6
                          { 1,1,1,0,0,0,0 },    // 7
                          { 1,1,1,1,1,1,1 },    // 8
                          { 1,1,1,0,0,1,1 },    // 9
                          { 0,1,1,1,1,1,0 },    // Victory
                          { 0,0,0,0,0,0,0 }};   // empty

bool select_mode_but_flag = false; //button flag to realize trigger "push - on, release, push - off"
int mode = 1; // 1,2,3,4 - variable to store game mode parameter
bool start_state = false; //was pushed start button?

int counter_red = 0; //number of red cards founded
int counter_green = 0; //number of green cards founded
int counter_red_prev = 0;
int counter_green_prev = 0;

int counter_red_gun = 0; //number of red cards founded
int counter_blue_gun = 0; //number of green cards founded
int counter_gun_divider = 1; //variable to divide points to fit in 1 digit score


String string; //variable to store information recieved form serial to compare card uids

//Artefact mode card uids for 
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


//it is needed to invert codes: red gun codes to green and green to red
//red team codes gives point to green and conversely

String red_gun1 = "525609#";
String red_gun2 = "465751#";
String red_gun3 = "03#";
String red_gun4 = "04#";
String red_gun5 = "05#";

String blue_gun1 = "529822#";
String blue_gun2 = "487540#";
String blue_gun3 = "3#";
String blue_gun4 = "4#";
String blue_gun5 = "5#";

String recieved_RF = "0";
String recieved_RF_prev = "1";

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

//gun flags
bool red1_gun_flag = false;
bool red2_gun_flag = false;
bool red3_gun_flag = false;
bool red4_gun_flag = false;
bool red5_gun_flag = false;

bool blue1_gun_flag = false;
bool blue2_gun_flag = false;
bool blue3_gun_flag = false;
bool blue4_gun_flag = false;
bool blue5_gun_flag = false;

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
  
  pinMode(timer_start, OUTPUT);
  digitalWrite(timer_start, HIGH);
  pinMode(timer_set_30, OUTPUT);
  digitalWrite(timer_set_30, HIGH);

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
  digitalWrite(rs485_direction_pin, HIGH); // переводим устройство в режим передатчика
  //RS485Serial.println("Waiting for card..."); // Выводим UID метки в консоль.
  Serial2.println("RS485 Helloword...");
  delay(50);
  digitalWrite(rs485_direction_pin, LOW); // переводим устройство в режим приёмника
}

void loop_test()
{
  if (Serial2.available()) 
  {
    string = "";
    delay(100);
    rs485_test();
  }
}

void loop()
{
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
      artefact_mode();
		}
  }

  if(mode == 4)
  {
    digitalWrite(mode1_led, LOW);
    digitalWrite(mode2_led, HIGH);
    digitalWrite(mode3_led, HIGH);
    if(start_state == true)
    {
      lasertag_mode();
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

    counter_red = 0; //number of red cards founded
    counter_green = 0; //number of green cards founded

    counter_red_prev = 0; //number of red cards founded
    counter_green_prev = 0; //number of green cards founded

    counter_red_gun = 0;
    counter_blue_gun = 0;

    digitalWrite(timer_start, HIGH);
    digitalWrite(timer_set_30, HIGH);

    digitalWrite(M_1_rgb_led_1, LOW); //power off
    digitalWrite(M_2_rgb_led_2, LOW); //power off
    digitalWrite(M_3_syrene1, LOW); //power off
    digitalWrite(M_4_syrene2, LOW); //power off
    digitalWrite(T_1_smoke_machine, LOW);
    digitalWrite(T_2_disco_lamp, LOW);

    Num1_red_Write(11); //write empty values to turn off the indicator
    Num2_green_Write(11);

    timer_reset = millis();

    digitalWrite(T_3_exit_door_led_projector, HIGH); //set a p4
    Serial.println("Exit_projector on");
    reset_state = true;

    //reset all flags for artefact cards
    red1_flag = false;
    red2_flag = false;
    red3_flag = false;
    red4_flag = false;
    red5_flag = false;
    red6_flag = false;
    red7_flag = false;
    red8_flag = false;
    red9_flag = false;
    red10_flag = false;

    green1_flag = false;
    green2_flag = false;
    green3_flag = false;
    green4_flag = false;
    green5_flag = false;
    green6_flag = false;
    green7_flag = false;
    green8_flag = false;
    green9_flag = false;
    green10_flag = false;

    red1_gun_flag = false;
    red2_gun_flag = false;
    red3_gun_flag = false;
    red4_gun_flag = false;
    red5_gun_flag = false;

    blue1_gun_flag = false;
    blue2_gun_flag = false;
    blue3_gun_flag = false;
    blue4_gun_flag = false;
    blue5_gun_flag = false;

  }

  if((millis()-timer_reset >= 180000)) //3minutes has left
    {
      //send a start signal to an external timer
      digitalWrite(T_3_exit_door_led_projector, LOW); //set a p4

      //Serial.println("Exit_projector off");
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
    Serial.println("mode4");
		mode = 4;
    delay(100);
    select_mode_but_flag = true;
  }

  if(digitalRead(select_mode_but) == LOW && select_mode_but_flag == false && mode == 4) //if select_mode_but was pushed 3rd time
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




////////////////////////////////////////// #1 catch_up_mode /////////////////////////////////////////////
void catch_up_mode()
{
  //set an external timer to a 15 minutes mode
  //send a start signal
  if (is_timer_active == false)
  {
    Serial.println("catch_up_mode 15min");
    digitalWrite(timer_start, LOW);

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

  if((millis()-timer_projector >= 900000)) //15 minutes has left
  {
    digitalWrite(T_3_exit_door_led_projector, HIGH); //set a p4
    Serial.println("game is over");
    digitalWrite(timer_start, HIGH);

    digitalWrite(M_1_rgb_led_1, LOW); //power off
    digitalWrite(M_2_rgb_led_2, LOW); //power off
    digitalWrite(M_3_syrene1, LOW); //power off
    digitalWrite(M_4_syrene2, LOW); //power off
    digitalWrite(T_1_smoke_machine, LOW);
    digitalWrite(T_2_disco_lamp, LOW);

    if(millis()-timer_projector >= 1080000) //18 minutes has left
    {
      digitalWrite(T_3_exit_door_led_projector, LOW);
    }
  }

}



////////////////////////////////////////// #2 cossacks_mode /////////////////////////////////////////////
void cossacks_mode()
{
  //set an external timer to a 15 minutes mode
  //send a start signal
  if (is_timer_active == false)
  {
    Serial.println("catch_up_mode");
    digitalWrite(timer_start, LOW);

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
    digitalWrite(T_3_exit_door_led_projector, HIGH); //set a p4

    Serial.println("game is over");
    digitalWrite(timer_start, HIGH);

    digitalWrite(M_1_rgb_led_1, LOW); //power off
    digitalWrite(M_2_rgb_led_2, LOW); //power off
    digitalWrite(M_3_syrene1, LOW); //power off
    digitalWrite(M_4_syrene2, LOW); //power off
    digitalWrite(T_1_smoke_machine, LOW);
    digitalWrite(T_2_disco_lamp, LOW);

    if(millis()-timer_projector >= 1080000) //18 minutes has left
    {
      digitalWrite(T_3_exit_door_led_projector, LOW);
    }
  }

}








////////////////////////////////////////// #3 artefact_mode /////////////////////////////////////////////
void artefact_mode()
{
  //set an external timer to a 30 minutes mode
  //send a start signal
  if (is_timer_active == false)
  {
    Serial.println("artefact mode");
    timer_game = millis();
    digitalWrite(timer_set_30, LOW);
    delay(100);
    digitalWrite(timer_start, LOW);

    mp3_set_serial(Serial1);
    mp3_set_volume(20);
    mp3_play(3);

    is_timer_active = true;
  }

  digitalWrite(rs485_direction_pin, LOW); //set a rs485 port to a recieve mode
  if (Serial2.available()) 
  {
    string = "";
    delay(100);
    rs485_recieve_artefact_mode();
  }

  Num1_red_Write(counter_red); // send to a Num1_red_Write function a counter_red value
  Num2_green_Write(counter_green);

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
    digitalWrite(timer_set_30, HIGH);
    digitalWrite(timer_start, HIGH);

    mp3_play(13);

    digitalWrite(T_3_exit_door_led_projector, HIGH);
    digitalWrite(T_1_smoke_machine, LOW);
    digitalWrite(T_2_disco_lamp, LOW);

    digitalWrite(M_3_syrene1, HIGH);
    if(millis()-timer_game == 1810000) //+10sec
    {
      digitalWrite(M_3_syrene1, LOW);
    }

    if(millis()-timer_game == 1980000) //33 min has left
    {
      digitalWrite(T_3_exit_door_led_projector, LOW);
    }
  }

  if(counter_red == 7 || counter_green == 7) //victory!!!
  {
    if(counter_red == 7)
    {
      counter_red = 10; //to show V-letter
    }

    if(counter_green == 7)
    {
      counter_green = 10; //to show V-letter
    }

    is_disco = false;
    is_exit_projector = false;
    is_T4 = false;

    if(is_game_over == false)
    {
      mp3_play(5);
      delay(100);
      digitalWrite(timer_start, HIGH);
      digitalWrite(timer_set_30, HIGH);
      is_game_over = true;
    }

    digitalWrite(M_1_rgb_led_1, LOW); //power off
    digitalWrite(M_2_rgb_led_2, LOW); //power off
    digitalWrite(M_3_syrene1, LOW); //power off
    digitalWrite(M_4_syrene2, LOW); //power off
    digitalWrite(T_1_smoke_machine, LOW);
    digitalWrite(T_2_disco_lamp, LOW);
  }

}







////////////////////////////////////////// #4 lasertag_mode ///////////////////////////////////////////////////
void lasertag_mode()
{
  //set an external timer to a 15 minutes mode
  //send a start signal
  if (is_timer_active == false)
  {
    Serial.println("lasertag mode");
    timer_game = millis();
    digitalWrite(timer_start, LOW);

    mp3_set_serial(Serial1);
    mp3_set_volume(20);
    mp3_play(3);

    is_timer_active = true;
  }

  digitalWrite(rs485_direction_pin, LOW); //set a rs485 port to a recieve mode
  if (Serial2.available()) 
  {
    string = "";
    delay(100);
    rs485_recieve_lasertag_mode();
    //rs485_test();
  }

  //change a score on a display every X poins! and play a bell sound every time
  counter_red = counter_red_gun/counter_gun_divider;
  counter_green = counter_blue_gun/counter_gun_divider;

  if(counter_red != counter_red_prev) //when counter red was changed - play a sound
  {
    mp3_play(11);
    counter_red_prev = counter_red;
  }

  if(counter_green != counter_green_prev) //when counter red was changed - play a sound
  {
    mp3_play(12);
    counter_green_prev = counter_green;
  }

  Num1_red_Write(counter_red); // send to a Num1_red_Write function a counter_red value - refresh display!
  Num2_green_Write(counter_green);


  if(millis()-timer_game == 900000) //15 min(900s) has left
  {
    digitalWrite(timer_set_30, HIGH);
    digitalWrite(timer_start, HIGH);

    mp3_play(13);

    digitalWrite(T_3_exit_door_led_projector, HIGH);
    digitalWrite(T_1_smoke_machine, LOW);
    digitalWrite(T_2_disco_lamp, LOW);

    digitalWrite(M_3_syrene1, HIGH);
    if(millis()-timer_game == 910000) //+10sec
    {
      digitalWrite(M_3_syrene1, LOW);
    }

    if(millis()-timer_game == 1080000) //18 min has left
    {
      digitalWrite(T_3_exit_door_led_projector, LOW);
    }
  }

  if(counter_red == 10 || counter_green == 10) //victory!!!
  {

    is_disco = false;
    is_exit_projector = false;
    is_T4 = false;

    if(is_game_over == false)
    {
      mp3_play(5);
      delay(100);
      digitalWrite(timer_start, HIGH);
      is_game_over = true;
    }

    digitalWrite(M_1_rgb_led_1, LOW); //power off
    digitalWrite(M_2_rgb_led_2, LOW); //power off
    digitalWrite(M_3_syrene1, LOW); //power off
    digitalWrite(M_4_syrene2, LOW); //power off
    digitalWrite(T_1_smoke_machine, LOW);
    digitalWrite(T_2_disco_lamp, LOW);
  }

}


void ind_test()
{
  for(int i = 0; i < 10; i++)
  {
    Serial.println(i);
    Num1_red_Write(i);
    Num2_green_Write(i);
    delay(500);
  }
}


void rs485_test() 
{              //recieve something from rs485 inerface
  digitalWrite(rs485_direction_pin, LOW);
  delay(50);
  while (Serial2.available())
  {
    char inChar = Serial2.read();
    string += inChar;
    if (inChar == '#')
    {
      Serial.println(string);
      /*if(string.equals(red_gun1))
      {
        Serial.println("equals 01#");
      }*/
      string = "";
    }
  }
}

void rs485_recieve_artefact_mode() 
{              //recieve something from rs485 inerface
  digitalWrite(rs485_direction_pin, LOW);
  while (Serial2.available())
  {
    char inChar = Serial2.read();
    string += inChar;
    if (inChar == '#')
    {
      //red cards uid check
      Serial.println(string);
      if (red1_flag == false && string.equals(red1))
      {
        red1_flag = true;
        counter_red +=1;
        mp3_play(11);
      }

      if (red2_flag == false && string.equals(red2))
      {
        red2_flag = true;
        counter_red +=1;
        mp3_play(11);
      }

      if (red3_flag == false && string.equals(red3))
      {
        red3_flag = true;
        counter_red +=1;
        mp3_play(11);
      }

      if (red4_flag == false && string.equals(red4))
      {
        red4_flag = true;
        counter_red +=1;
        mp3_play(11);
      }

      if (red5_flag == false && string.equals(red5))
      {
        red5_flag = true;
        counter_red +=1;
        mp3_play(11);
      }

      if (red6_flag == false && string.equals(red6))
      {
        red6_flag = true;
        counter_red +=1;
        mp3_play(11);
      }

      if (red7_flag == false && string.equals(red7))
      {
        red7_flag = true;
        counter_red +=1;
        mp3_play(11);
      }

      if (red8_flag == false && string.equals(red8))
      {
        red8_flag = true;
        counter_red +=1;
        mp3_play(11);
      }

      if (red9_flag == false && string.equals(red9))
      {
        red9_flag = true;
        counter_red +=1;
        mp3_play(11);
      }

      if (red10_flag == false && string.equals(red10))
      {
        red10_flag = true;
        counter_red +=1;
        mp3_play(11);
      }

      //green cards uid check
      if (green1_flag == false && string.equals(green1))
      {
        green1_flag = true;
        counter_green +=1;
        mp3_play(12);
      }

      if (green2_flag == false && string.equals(green2))
      {
        green2_flag = true;
        counter_green +=1;
        mp3_play(12);
      }

      if (green3_flag == false && string.equals(green3))
      {
        green3_flag = true;
        counter_green +=1;
        mp3_play(12);
      }

      if (green4_flag == false && string.equals(green4))
      {
        green4_flag = true;
        counter_green +=1;
        mp3_play(12);
      }

      if (green5_flag == false && string.equals(green5))
      {
        green5_flag = true;
        counter_green +=1;
        mp3_play(12);
      }

      if (green6_flag == false && string.equals(green6))
      {
        green6_flag = true;
        counter_green +=1;
        mp3_play(12);
      }

      if (green7_flag == false && string.equals(green7))
      {
        green7_flag = true;
        counter_green +=1;
        mp3_play(12);
      }

      if (green8_flag == false && string.equals(green8))
      {
        green8_flag = true;
        counter_green +=1;
        mp3_play(12);
      }

      if (green9_flag == false && string.equals(green9))
      {
        green9_flag = true;
        counter_green +=1;
        mp3_play(12);
      }

      if (green10_flag == false && string.equals(green10))
      {
        green10_flag = true;
        counter_green +=1;
        mp3_play(12);
      }

    string = "";
    }
  }
}

void rs485_recieve_lasertag_mode() 
{ 
//recieve something from rs485 inerface
  digitalWrite(rs485_direction_pin, LOW);
  while (Serial2.available())
  {
    char inChar = Serial2.read();
    string += inChar;
    if (inChar == '#')
    {
      recieved_RF = string;

      //Serial.println(recieved_RF.equals(recieved_RF_prev));

      if(!(recieved_RF.equals(recieved_RF_prev))) // condition to pass the only one batch of code!
      {
        Serial.println(string);
        //red gun's codes
        if (string.equals(red_gun1))
        {
          counter_red_gun +=1;
        }

        if (string.equals(red_gun2))
        {
          counter_red_gun +=1;
        }

        if (string.equals(red_gun3))
        {
          counter_red_gun +=1;
        }

        if (string.equals(red_gun4))
        {
          counter_red_gun +=1;
        }

        if (string.equals(red_gun5))
        {
          counter_red_gun +=1;
        }

        //blue gun's codes
        if (string.equals(blue_gun1))
        {
          counter_blue_gun +=1;
        }

        if (string.equals(blue_gun2))
        {
          counter_blue_gun +=1;
        }

        if (string.equals(blue_gun3))
        {
          counter_blue_gun +=1;
        }

        if (string.equals(blue_gun4))
        {
          counter_blue_gun +=1;
        }

        if (string.equals(blue_gun5))
        {
          counter_blue_gun +=1;
        }
        recieved_RF_prev = recieved_RF;
        string = "";
      }
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