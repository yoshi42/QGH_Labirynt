/*Система управління лабіринтом.
Логіка роботи:

Вибір режиму Квач/Артефакт/Козаки
Старт

Команда на таймер: Старт 15, Старт 30, Ребут.
Команда від таймера: Кінець часу

Команда на зчитувач: старт, ресет
Команди від зчитувача карта зелена +1
Карта червона +1

*/
#include <DFPlayer_Mini_Mp3.h>
#include <SoftwareSerial.h>

int SSerialTxControl = 40;   //RS485 Direction control