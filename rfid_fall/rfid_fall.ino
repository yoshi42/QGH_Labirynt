#include <SPI.h>
#include <MFRC522.h> // библиотека "RFID".
#include <SoftwareSerial.h>

#define SSerialRX        6  //Serial Receive pin
#define SSerialTX        7  //Serial Transmit pin
#define SSerialTxControl 8   //RS485 Direction control

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);
unsigned long uidDec, uidDecTemp;  // для храниения номера метки в десятичном формате

SoftwareSerial RS485Serial(SSerialRX, SSerialTX); // RX, TX

void setup() {
  Serial.begin(9600);
  RS485Serial.begin(9600); 

  Serial.println("Waiting for card...");
  SPI.begin();  //  инициализация SPI / Init SPI bus.
  mfrc522.PCD_Init();     // инициализация MFRC522 / Init MFRC522 card.
}
void loop() 
{
  digitalWrite(SSerialTxControl, LOW);
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
  	Serial.println("Card UID: ");
  	Serial.println(uidDec); // Выводим UID метки в консоль.

  	digitalWrite(SSerialTxControl, HIGH); // переводим устройство в режим приёмника
  	RS485Serial.print(uidDec); // Выводим UID метки в консоль.
  	RS485Serial.println("#"); // Выводим UID метки в консоль.
  }
}