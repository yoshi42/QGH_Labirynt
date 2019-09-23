#include <SPI.h>
#include <MFRC522.h> // библиотека "RFID".
#include <SoftwareSerial.h>
#include <RemoteReceiver.h>

#define SSerialRX        7  //Serial Receive pin
#define SSerialTX        6  //Serial Transmit pin
#define SSerialTxControl 8   //RS485 Direction control

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);
unsigned long uidDec, uidDecTemp;  // для храниения номера метки в десятичном формате

SoftwareSerial RS485Serial(SSerialRX, SSerialTX); // RX, TX

void setup() {
  Serial.begin(9600);
  RS485Serial.begin(9600); 
  RemoteReceiver::init(0, 1, showCode); //прерывание 0 - пин 2

  pinMode(SSerialTxControl, OUTPUT);
  digitalWrite(SSerialTxControl, LOW); // // переводим устройство в режим приемника, для предотвращения перегревания max385
  pinMode(3, INPUT); // второй выход data в приемнике вешаем на пин 3 (interrupt 1) на всякий случай, 
  //и назначаем его как вход, чтобы не мешался паразитными значениями

  //digitalWrite(SSerialTxControl, HIGH); // переводим устройство в режим передатчика
  //RS485Serial.println("Waiting for card..."); // Выводим UID метки в консоль.

  Serial.println("Waiting for card...");
  SPI.begin();  //  инициализация SPI / Init SPI bus.
  mfrc522.PCD_Init();     // инициализация MFRC522 / Init MFRC522 card.

  digitalWrite(SSerialTxControl, HIGH); // переводим устройство в режим передатчика
  //RS485Serial.println("Waiting for card..."); // Выводим UID метки в консоль.
  Serial.println("Waiting for card...");
}
void loop() 
{
  digitalWrite(SSerialTxControl, LOW); // переводим устройство в режим приемника, для предотвращения перегревания max385
  // Поиск новой метки
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  // Выбор метки
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  uidDec = 0;
  // Выдача серийного номера метки.
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    uidDecTemp = mfrc522.uid.uidByte[i];
    uidDec = uidDec * 256 + uidDecTemp;
  }
  if(uidDec != 0)
  {
  	digitalWrite(SSerialTxControl, HIGH); // переводим устройство в режим передатчика
  	//RS485Serial.print(uidDec); // Выводим UID метки в консоль.
  	//RS485Serial.println("#"); // Выводим стоповый байт.
  	//Serial.println("Card UID: ");
  	Serial.print(uidDec); // Выводим UID метки в консоль.
  	Serial.println("#"); // Выводим стоповый байт.
    delay(100);
  }
}

void showCode(unsigned long receivedCode, unsigned int period) 
{	
	//digitalWrite(SSerialTxControl, HIGH); // переводим устройство в режим передатчика
   	//Print the received code.
    Serial.print("Code: ");
    Serial.print(receivedCode);
    //Serial.println("#"); // Выводим стоповый байт.
    Serial.print(", period duration: ");
    Serial.print(period);
    Serial.println("us.");
}