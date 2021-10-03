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
String red1 = "503398431#";//ok
String red2 = "2358333711#";//ok
String red3 = "2978344986#";//ok
String red4 = "2455429146#";//ok
String red5 = "1917039386#";//ok
String red6 = "2994181685#";//ok
String red7 = "2203065883#";//ok
String red8 = "294147356#";//ok
String red9 = "324687645#";//ok
String red10 = "2420007695#";//ok
String red11 = "1239830799#";//ok
String red12 = "538930959#";//ok
String red13 = "2513136143#";//ok
String red14 = "3724562959#";//ok
String red15 = "2868789023#";//ok
String red16 = "4087705359#";//ok
String red17 = "red17#";
String red18 = "red18#";
String red19 = "red19#";
String red20 = "red20#";
String red21 = "red21#";
String red22 = "red22#";
String red23 = "red23#";
String red24 = "red24#";

String green1 = "2455335450#";//ok
String green2 = "2455522586#"; //ok
String green3 = "2434805530#";//ok
String green4 = "325257757#";//ok
String green5 = "3951120911#";//ok
String green6 = "2989438517#";//ok
String green7 = "2976338447#";//ok
String green8 = "123625487#";//ok
String green9 = "2202833435#";//ok
String green10 = "3406979343#";//ok
String green11 = "2761773327#"; //ok
String green12 = "2519228431#"; //ok
String green13 = "2904576527#"; //ok
String green14 = "212152351#"; //ok
String green15 = "825978639#"; //ok
String green16 = "380850703#"; //ok
String green17 = "3440721951#";//ok
String green18 = "green18#";
String green19 = "green19#";
String green20 = "green20#";
String green21 = "green21#";
String green22 = "green22#";
String green23 = "green23#";
String green24 = "green24#";

String adm_key = "923640870#"; //admin key

String red_gun1 = "529903#";//1
String red_gun2 = "487540#";//2
String red_gun3 = "354052#";//3
String red_gun4 = "163945#";//4
String red_gun5 = "529822#";//5
String red_gun6 = "12345#";
 //blue pistols - plus 1score to red team
String blue_gun1 = "525618#"; //1
String blue_gun2 = "465751#";//2 
String blue_gun3 = "489808#";//3
String blue_gun4 = "352756#";//4b
String blue_gun5 = "412453#";//5
String blue_gun6 = "97606#";//new 6  "117289#"

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
bool red11_flag = false;
bool red12_flag = false;
bool red13_flag = false;
bool red14_flag = false;
bool red15_flag = false;
bool red16_flag = false;
bool red17_flag = false;
bool red18_flag = false;
bool red19_flag = false;
bool red20_flag = false;
bool red21_flag = false;
bool red22_flag = false;
bool red23_flag = false;
bool red24_flag = false;

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
bool green11_flag = false;
bool green12_flag = false;
bool green13_flag = false;
bool green14_flag = false;
bool green15_flag = false;
bool green16_flag = false;
bool green17_flag = false;
bool green18_flag = false;
bool green19_flag = false;
bool green20_flag = false;
bool green21_flag = false;
bool green22_flag = false;
bool green23_flag = false;
bool green24_flag = false;

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

int last_gun_num = 0;

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
    red11_flag = false;
    red12_flag = false;
    red13_flag = false;
    red14_flag = false;
    red15_flag = false;
    red16_flag = false;
    red17_flag = false;
    red18_flag = false;
    red19_flag = false;
    red20_flag = false;
    red21_flag = false;
    red22_flag = false;
    red23_flag = false;
    red24_flag = false;

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
    green11_flag = false;
    green12_flag = false;
    green13_flag = false;
    green14_flag = false;
    green15_flag = false;
    green16_flag = false;
    green17_flag = false;
    green18_flag = false;
    green19_flag = false;
    green20_flag = false;
    green21_flag = false;
    green22_flag = false;
    green23_flag = false;
    green24_flag = false;

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
    mp3_play(30);

    is_timer_active = true;
  }

  digitalWrite(rs485_direction_pin, LOW); //set a rs485 port to a recieve mode
  if (Serial2.available()) 
  {
    string = "";
    delay(100);
    rs485_recieve_artefact_mode();
  }

  if((counter_red != counter_red_prev) && counter_red != 10) //when counter red was changed - play a sound
  {
    mp3_play(11); //play bell song
    counter_red_prev = counter_red;
    delay(1000); //wait to song end (delay must be changed)
    int track_number = random(30, 40); //
    mp3_play(track_number); //play random track
  }

  if((counter_green != counter_green_prev) && counter_green != 10) //when counter green was changed - play a sound
  {
    mp3_play(12);
    delay(1000);
    counter_green_prev = counter_green;
    int track_number = random(30, 40);
    mp3_play(track_number);
  }

  Num1_red_Write(counter_green);
  Num2_green_Write(counter_red);

  if(millis()-timer_game >= 1500000) //25 min(1500s) has left
  {
    digitalWrite(M_3_syrene1, HIGH);
    digitalWrite(M_4_syrene2, HIGH);
    if(millis()-timer_game >= 1510000) //+10sec
    {
      digitalWrite(M_3_syrene1, LOW);
      digitalWrite(M_4_syrene2, LOW);
    }
  }

  if(millis()-timer_game >= 1560000) //26 min(1560s) has left
  {
    digitalWrite(M_3_syrene1, HIGH);
    digitalWrite(M_4_syrene2, HIGH);
    if(millis()-timer_game >= 1570000) //+10sec
    {
      digitalWrite(M_3_syrene1, LOW);
      digitalWrite(M_4_syrene2, LOW);
    }
  }

  if(millis()-timer_game >= 1620000) //27 min(1620s) has left
  {
    digitalWrite(M_3_syrene1, HIGH);
    digitalWrite(M_4_syrene2, HIGH);
    if(millis()-timer_game >= 1630000) //+10sec
    {
      digitalWrite(M_3_syrene1, LOW);
      digitalWrite(M_4_syrene2, LOW);
    }
  }

  if(millis()-timer_game >= 1680000) //28 min(1680s) has left
  {
    digitalWrite(M_3_syrene1, HIGH);
    digitalWrite(M_4_syrene2, HIGH);
    if(millis()-timer_game >= 1690000) //+10sec
    {
      digitalWrite(M_3_syrene1, LOW);
      digitalWrite(M_4_syrene2, LOW);
    }
  }

  if(millis()-timer_game >= 1740000) //29 min(1740s) has left
  {
    digitalWrite(M_3_syrene1, HIGH);
    digitalWrite(M_4_syrene2, HIGH);
    if(millis()-timer_game >= 1750000) //+10sec
    {
      digitalWrite(M_3_syrene1, LOW);
      digitalWrite(M_4_syrene2, LOW);
    }
  }

  if(millis()-timer_game >= 1800000) //30 min(1800s) has left
  {
    if(is_game_over == false)
    {
      digitalWrite(timer_set_30, HIGH);
      digitalWrite(timer_start, HIGH);

      mp3_play(13);

      digitalWrite(T_3_exit_door_led_projector, HIGH);
      digitalWrite(T_1_smoke_machine, LOW);
      digitalWrite(T_2_disco_lamp, LOW);

      digitalWrite(M_3_syrene1, HIGH);
      digitalWrite(M_4_syrene2, HIGH);
      is_game_over = true;
    }

    if(millis()-timer_game >= 1810000) //+10sec
    {
      digitalWrite(M_3_syrene1, LOW);
      digitalWrite(M_4_syrene2, LOW);
    }

    if(millis()-timer_game >= 1980000) //33 min has left
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
    mp3_play(46);

    is_timer_active = true;
  }

  digitalWrite(rs485_direction_pin, LOW); //set a rs485 port to a recieve mode
  if (Serial2.available())
  {
    string = "";
    delay(20);
    if(is_game_over == false) //dont recieve anything while game is over
    {
      rs485_recieve_lasertag_mode(); // is_game_over here is a little bit 'kostyl" because it had to be done quickly
      //rs485_test();
    }
  }

  //change a score on a display every X poins! and play a bell sound every time
  counter_red = counter_red_gun/counter_gun_divider;
  counter_green = counter_blue_gun/counter_gun_divider;

  if(counter_red != counter_red_prev) //when counter red was changed - play a sound
  {
    mp3_play(11); //play bell son
    counter_red_prev = counter_red;
    delay(1000); //wait to song end (delay must be changed)
    int track_number = random(41, 50); //
    mp3_play(track_number); //play random track
  }

  if(counter_green != counter_green_prev) //when counter green was changed - play a sound
  {
    mp3_play(12);
    delay(1000);
    counter_green_prev = counter_green;
    int track_number = random(41, 50);
    mp3_play(track_number);
  }

  Num1_red_Write(counter_red); // send to a Num1_red_Write function a counter_red value - refresh display!
  Num2_green_Write(counter_green);


  if(millis()-timer_game >= 900000) //15 min(900s) has left
  {
    if(is_game_over == false)
    {

      digitalWrite(timer_set_30, HIGH);
      digitalWrite(timer_start, HIGH);
      mp3_play(13); //play gong song
      delay(4000);
      mp3_set_volume(25);
      mp3_play(5);
      delay(100);

      digitalWrite(T_3_exit_door_led_projector, HIGH);
      digitalWrite(T_1_smoke_machine, LOW);
      digitalWrite(T_2_disco_lamp, LOW);

      digitalWrite(M_3_syrene1, HIGH);
      is_game_over = true;
    }

    if(millis()-timer_game >= 910000) //+10sec
    {
      digitalWrite(M_3_syrene1, LOW);
    }

    if(millis()-timer_game >= 1080000) //18 min has left
    {
      digitalWrite(T_3_exit_door_led_projector, LOW);
    }
  }

  if((counter_red == 10 || counter_green == 10) && is_game_over == false) //victory!!!
  {

    is_disco = false;
    is_exit_projector = false;
    is_T4 = false;

    if(is_game_over == false)
    {
      digitalWrite(timer_start, HIGH);
      mp3_play(13); //play gong song
      delay(4000);
      mp3_set_volume(25);
      mp3_play(5);
      delay(100);
      is_game_over = true;
    }

    digitalWrite(M_1_rgb_led_1, LOW); //power off
    digitalWrite(M_2_rgb_led_2, LOW); //power off
    digitalWrite(M_3_syrene1, LOW); //power off
    digitalWrite(M_4_syrene2, LOW); //power off
    digitalWrite(T_1_smoke_machine, LOW);
    digitalWrite(T_2_disco_lamp, LOW);

    digitalWrite(T_3_exit_door_led_projector, HIGH); //set a p4
    Serial.println("Exit_projector on");
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

      if (red1_flag == false && string.equals(red1)){red1_flag = true;counter_red +=1;mp3_play(11);}
      if (red2_flag == false && string.equals(red2)){red2_flag = true;counter_red +=1;mp3_play(11);}
      if (red3_flag == false && string.equals(red3)){red3_flag = true;counter_red +=1;mp3_play(11);}
      if (red4_flag == false && string.equals(red4)){red4_flag = true;counter_red +=1;mp3_play(11);}
      if (red5_flag == false && string.equals(red5)){red5_flag = true;counter_red +=1;mp3_play(11);}
      if (red6_flag == false && string.equals(red6)){red6_flag = true;counter_red +=1;mp3_play(11);}
      if (red7_flag == false && string.equals(red7)){red7_flag = true;counter_red +=1;mp3_play(11);}
      if (red8_flag == false && string.equals(red8)){red8_flag = true;counter_red +=1;mp3_play(11);}
      if (red9_flag == false && string.equals(red9)){red9_flag = true;counter_red +=1;mp3_play(11);}
      if (red10_flag == false && string.equals(red10)){red10_flag = true;counter_red +=1;mp3_play(11);}
      if (red11_flag == false && string.equals(red11)){red11_flag = true;counter_red +=1;mp3_play(11);}
      if (red12_flag == false && string.equals(red12)){red12_flag = true;counter_red +=1;mp3_play(11);}
      if (red13_flag == false && string.equals(red13)){red13_flag = true;counter_red +=1;mp3_play(11);}
      if (red14_flag == false && string.equals(red14)){red14_flag = true;counter_red +=1;mp3_play(11);}
      if (red15_flag == false && string.equals(red15)){red15_flag = true;counter_red +=1;mp3_play(11);}
      if (red16_flag == false && string.equals(red16)){red16_flag = true;counter_red +=1;mp3_play(11);}
      if (red17_flag == false && string.equals(red17)){red17_flag = true;counter_red +=1;mp3_play(11);}
      if (red18_flag == false && string.equals(red18)){red18_flag = true;counter_red +=1;mp3_play(11);}
      if (red19_flag == false && string.equals(red19)){red19_flag = true;counter_red +=1;mp3_play(11);}
      if (red20_flag == false && string.equals(red20)){red20_flag = true;counter_red +=1;mp3_play(11);}
      if (red21_flag == false && string.equals(red21)){red21_flag = true;counter_red +=1;mp3_play(11);}
      if (red22_flag == false && string.equals(red22)){red22_flag = true;counter_red +=1;mp3_play(11);}
      if (red23_flag == false && string.equals(red23)){red23_flag = true;counter_red +=1;mp3_play(11);}
      if (red24_flag == false && string.equals(red24)){red24_flag = true;counter_red +=1;mp3_play(11);}

      if (green1_flag == false && string.equals(green1)){green1_flag = true;counter_green +=1;mp3_play(12);}
      if (green2_flag == false && string.equals(green2)){green2_flag = true;counter_green +=1;mp3_play(12);}
      if (green3_flag == false && string.equals(green3)){green3_flag = true;counter_green +=1;mp3_play(12);}
      if (green4_flag == false && string.equals(green4)){green4_flag = true;counter_green +=1;mp3_play(12);}
      if (green5_flag == false && string.equals(green5)){green5_flag = true;counter_green +=1;mp3_play(12);}
      if (green6_flag == false && string.equals(green6)){green6_flag = true;counter_green +=1;mp3_play(12);}
      if (green7_flag == false && string.equals(green7)){green7_flag = true;counter_green +=1;mp3_play(12);}
      if (green8_flag == false && string.equals(green8)){green8_flag = true;counter_green +=1;mp3_play(12);}
      if (green9_flag == false && string.equals(green9)){green9_flag = true;counter_green +=1;mp3_play(12);}
      if (green10_flag == false && string.equals(green10)){green10_flag = true;counter_green +=1;mp3_play(12);}
      if (green11_flag == false && string.equals(green11)){green11_flag = true;counter_green +=1;mp3_play(12);}
      if (green12_flag == false && string.equals(green12)){green12_flag = true;counter_green +=1;mp3_play(12);}
      if (green13_flag == false && string.equals(green13)){green13_flag = true;counter_green +=1;mp3_play(12);}
      if (green14_flag == false && string.equals(green14)){green14_flag = true;counter_green +=1;mp3_play(12);}
      if (green15_flag == false && string.equals(green15)){green15_flag = true;counter_green +=1;mp3_play(12);}
      if (green16_flag == false && string.equals(green16)){green16_flag = true;counter_green +=1;mp3_play(12);}
      if (green17_flag == false && string.equals(green17)){green17_flag = true;counter_green +=1;mp3_play(12);}
      if (green18_flag == false && string.equals(green18)){green18_flag = true;counter_green +=1;mp3_play(12);}
      if (green19_flag == false && string.equals(green19)){green19_flag = true;counter_green +=1;mp3_play(12);}
      if (green20_flag == false && string.equals(green20)){green20_flag = true;counter_green +=1;mp3_play(12);}
      if (green21_flag == false && string.equals(green21)){green21_flag = true;counter_green +=1;mp3_play(12);}
      if (green22_flag == false && string.equals(green22)){green22_flag = true;counter_green +=1;mp3_play(12);}
      if (green23_flag == false && string.equals(green23)){green23_flag = true;counter_green +=1;mp3_play(12);}
      if (green24_flag == false && string.equals(green24)){green24_flag = true;counter_green +=1;mp3_play(12);}

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
        Serial.println(string);
        //red gun's codes
        if (string.equals(red_gun1) && last_gun_num != 1)
        {
          counter_red_gun +=1;
          last_gun_num = 1;
        }

        if (string.equals(red_gun2) && last_gun_num != 2)
        {
          counter_red_gun +=1;
          last_gun_num = 2;
        }

        if (string.equals(red_gun3) && last_gun_num != 3)
        {
          counter_red_gun +=1;
          last_gun_num = 3;
        }

        if (string.equals(red_gun4) && last_gun_num != 4)
        {
          counter_red_gun +=1;
          last_gun_num = 4;
        }

        if (string.equals(red_gun5) && last_gun_num != 5)
        {
          counter_red_gun +=1;
          last_gun_num = 5;
        }

        if (string.equals(red_gun6) && last_gun_num != 51)
        {
          counter_red_gun +=1;
          last_gun_num = 51;
        }

        //blue gun's codes
        if (string.equals(blue_gun1) && last_gun_num != 6)
        {
          counter_blue_gun +=1;
          last_gun_num = 6;
        }

        if (string.equals(blue_gun2) && last_gun_num != 7)
        {
          counter_blue_gun +=1;
          last_gun_num = 7;
        }

        if (string.equals(blue_gun3) && last_gun_num != 8)
        {
          counter_blue_gun +=1;
          last_gun_num = 8;
        }

        if (string.equals(blue_gun4) && last_gun_num != 9)
        {
          counter_blue_gun +=1;
          last_gun_num = 9;
        }

        if (string.equals(blue_gun5) && last_gun_num != 10)
        {
          counter_blue_gun +=1;
          last_gun_num = 10;
        }

        if (string.equals(blue_gun6) && last_gun_num != 11)
        {
          counter_blue_gun +=1;
          last_gun_num = 11;
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