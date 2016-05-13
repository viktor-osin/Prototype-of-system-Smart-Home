#include <Wire.h>  //библиотека для работы с I2C
#include <SPI.h>   
#include <Time.h>  //библиотека для работы с модулем времени
#include <DS1307RTC.h>  // a basic DS1307 library that returns time as a time_t
#include "DHT.h"  //библиотека для работы датчика температуры и влажности
#define DHTPIN 6  //датчик подключен ко входу 6
#define DHTTYPE DHT22  
#include <Wtv020sd16p.h> //библиотека работы мп3 модуля
#include <Bounce.h>      //библиотека для правильного срабатывания прерывания (дребезг кнопки и помехи)
#include <MFRC522.h>     //библиотека RFID
#define SS_PIN 10        //RFID Константы
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.
Bounce bouncer = Bounce(3, 40); //создаем экземпляр класса Bounce для 2 вывода
//Card UID: 83 00 D8 00
//Card UID: 62 7E 34 00

int s1=0, pas=0, sig, value1, ihome, r, time;
 int  k1=0, k2=0, k3=0, k4=0, key1=0, key2=0, key3=0, key4=0;
int passw, t1, t2, c1, c2, ch,  cm,  sgv, playv, previousMillis2, valK, start, priem; 
byte ventil = 1, shtora = 1; 
int sim1=12345;    //стартовый пароль по умолчанию
int raw = 0, sigvoice = 0, konder = 1;
float t, h; //константы температуры и влажности
byte val, PassRFID; //константа входящих кодов
//блок констант для определния работающих объектов в приложении (расшифровка в блокноте)
byte AUO, AUZ, AUV, OR, RUK, RUG, RUT, RUP, RUO, SO, AINF, RUZ, RUV;

int resetPin = 7;  //пины для мп3 модуля
int clockPin = 8; 
int dataPin = 5; 
int busyPin = 4; 
Wtv020sd16p wtv020sd16p(resetPin,clockPin,dataPin,busyPin);

float temph[2]; //массив для температуры и влажности
int clock[2];   //массив для времени
int voice[4];   //массив для голосового оповещения
long randomCode, key5=0;
int Npasport = 0000; //номер паспорта для восстановления доступа

char incomingByte;  // входящие данные
int cltime, photorez, morning, evening, massage, clocktime, question;
int npas,npas1,betanew,pasnew, soundVoice = 1;
int dvig = A0;    //реле двигателя
int polar = A1;   //реле полярности
int kond = A2;    //двигатель вентиляции
int kondStop;     
float kondTemp = 28;     //стандартное значение температуры включения вентиляции
const int pinPhoto = A3; //фоторезистор
DHT dht(DHTPIN, DHTTYPE);
byte uidCard[4] = {0x83, 0x00, 0xD8, 0x00};




void setup(){
  Wire.begin(5);                // задать Arduino Uno адрес №5 в шине I2C
  Wire.onReceive(receiveEvent); // регистрация прихода сообщения
  
  Serial.begin(9600);           //открываем сериал для послыки сообщений приложению
   delay(2);
   randomSeed(analogRead(0));   //функция рандомного выбора кода активации
  digitalWrite(10, HIGH);
  pinMode(pinPhoto, INPUT);
  pinMode(dvig, OUTPUT);
  pinMode(kond, OUTPUT);
  pinMode(polar, OUTPUT);
  digitalWrite(3, HIGH);
  digitalWrite(polar, LOW);
  wtv020sd16p.reset();
  dht.begin();
  SPI.begin();   // инициализация SPI щины
  mfrc522.PCD_Init(); // инициализация MFRC522 card
  //значение констант интерфейса приложения по умолчанию
  AUO=0, AUZ=1 , AUV=1, OR=0, RUK=0, RUG=0, RUT=0, RUP=0, RUO=0, SO=0, AINF=1, RUZ=0, RUV=0;
  attachInterrupt(1, zvonok, FALLING);  //прерывание для срабатывания функции кнопки звонка
/*  setSyncProvider(RTC.get);   //настройка модуля реального времени
  setTime(18,18,0,12,9,2015);
  RTC.set(now());*/
  while (!Serial) ; //ждем,если Аrduino не открыло сериал порт
  setSyncProvider(RTC.get);
}


void loop() {
  while(start == 0){   //ждем пока приложение пошлет код старта цикла
    if(Serial.available() > 0) val = Serial.parseInt(); //записать в val сообщение от приложения
    if(val == 255) start = 1;    //если сообщение равно 255, то запускаем главный цикл
  }
    
  if(passw == 1){ //если введен верный пароль, то включаем тело главного цикла 
     switch(priem){  //прием сообщений от Arduino Nano
       case 4: OR = 1; break; //записать включение охраны в константу интерфейса приложения
       case 40: OR = 0; break;
       case 109: sigvoice = 1; priem = 0; break; //если пришло сообщение 109, то включить сигнализацию
       case 1: sendMessage("AUO_OFF"); AUO = 0;  priem = 0; break;
     
       case 9: sendMessage("SO_ON"); SO = 1; priem = 0; break;
       case 10: sendMessage("SO_OFF"); SO = 0; priem = 0; break;
     
       case 11: sendMessage("KORIDOR_ON"); RUK = 1; priem = 0; break;
       case 22: sendMessage("KORIDOR_OFF"); RUK = 0; priem = 0; break;
     
       case 13: sendMessage("VAN_ON"); RUT = 1; priem = 0; break;
       case 31: sendMessage("VAN_OFF"); RUT = 0; priem = 0; break;
     
       case 12: sendMessage("GOST_ON"); RUG = 1; priem = 0; break;
       case 21: sendMessage("GOST_OFF"); RUG = 0; priem = 0; break;
     
       case 14: sendMessage("PERIM_ON"); RUP = 1; priem = 0; break;
       case 41: sendMessage("PERIM_OFF"); RUP = 0; priem = 0; break;
    
       case 15: sendMessage("ALL_ON"); RUO = 1; priem = 0; break;
       case 51: sendMessage("ALL_OFF"); RUO = 0; priem = 0; break;
       }
  if(OR == 1)RFIDStopSig(); //если охранный режим включен, то считываем показания ридера
  if(ihome == 1)RFIDOR();
  if(sigvoice == 1)
  { 
    while(sgv == 0){
    wtv020sd16p.playVoice(226);
    delay(100);
    wtv020sd16p.playVoice(226);
    sgv = 1;
    }
    sgv++;
    if(sgv >= 900)
    {
      sgv = 0;
      if(Serial.available() > 0) sig = Serial.parseInt();
      if(sig == sim1)sigvoice = 0;  //если введен верный пароль, то откл сигнализацию
      else
      { 
        wtv020sd16p.playVoice(226);
        delay(100);
        wtv020sd16p.playVoice(226);
      }
    } 
  }
  
//контроль температуры, времени, занавесок и вентиляции
  clocktime++;
  if(clocktime >= 500)
  {
    clocktime = 0;
    float t = dht.readTemperature();
    float h = dht.readHumidity(); 
    int ch = hour();
    int cm = minute();
    photorez = analogRead(pinPhoto);
    temph[0]=float(t);
    temph[1]=float(h);
    clock[0]=int(ch);
    clock[1]=int(cm);
 
    value1 = temph[0];
    sendInfoMessages();
    
    if(AUZ == 1) winlight();  //если вкл. автоматич.упр.занавесками, то переходим к функции
    
    if(ventil == 1)
    { //если вентиляция вкл, то запустить функцию кондиционирования
     r = (temph[0]) - (kondTemp); //замеряем разниццу температур
     kondition();
    }
    if(ventil == 4)
    {
     ventil = 0;
     digitalWrite(kond, LOW); //выключаем вентиляцию и обнуляем переменные
     sendMessage("VENTIL_OFF");
     RUV = 0;
     valK = 0;
     konder = 1;
     kondStop = 0;
    }
    
  }

if(Serial.available() > 0)  //если что-то пришло в сериал
{
  sig = Serial.parseInt();  //считать значение сериал 
  val = sig;                //записать в константу входящих кодов
    
    if(sig == sim1)         //если введены символы пароля, то выключить сигнализацию
    {
       sigvoice = 0;
       val = 101;
       Wire.beginTransmission(4); // открываем передачу устройству №4 (Arduino Nano)
       Wire.write(val);           //передаем код 101 (отключение световой индикации)
       Wire.endTransmission();    //закрываем передачу
       sendMessage("SIGNAL_OFF"); //передаем приложению сообщение об отключении сигнализации
       OR = 0;                    //записываем в константу состояния сигнализации 0
          if(soundVoice == 1)     //если разрешен звук, то прозносим фразу о правильности введенного пароля
          { 
            wtv020sd16p.playVoice(228);
            delay(100); 
            wtv020sd16p.playVoice(228);
            delay(50);
          }
     }
  
  Wire.beginTransmission(4); // открываем передачу устройству №4 (Arduino Nano)
  Wire.write(val);           //передаем состояние val
  Wire.endTransmission();    //закрываем передачу
 
    switch(val) //в зависимости от введенной команды val выбрать:
    {
      case 99: updateint(); break; //запрос на обновление интерфейса приложения
      case 4: RFIDOR(); //если включен охранный режим, перейти в функцию его выполнения
      case 6: soundVoice = 1;  AINF = 1; break; //включить автоинформатор
      case 60: soundVoice = 0; AINF = 0; break; //выключить автоинформатор
      
      case 2:                                  //разрешить автоматическую вентиляцию
        AUV = 1;
        if((temph[0]) >= (kondTemp)) //если температура превышает заданную, включить вентиляцию
           kondStop = 1;
        else kondStop = 0;
 
        if((kondTemp) > (temph[0])){
           sendMessage("VENTIL_OFF");
           digitalWrite(kond, LOW); //выключаем вентиляцию и обнуляем переменные
           RUV = 0;
           kondStop = 0;
       }
        valK = 0; //обнуляем счетчик отключения вентиляции
        ventil = 1;  
        konder = 1; 
        val=0;
      break;
        
      case 20: ventil = 4; AUV = 0; val = 0; break; //запретить автоматическую вентиляцию
      
      case 16: if(soundVoice == 1)                  //включить вентиляцию
             { 
                wtv020sd16p.playVoice(241);
                delay(100); 
                wtv020sd16p.playVoice(241);
                delay(1000);
             }
          sendMessage("VENTIL_ON");  
          digitalWrite(kond, HIGH);
          RUV = 1;
          ventil = 0;
          break;
          
      case 61: sendMessage("VENTIL_OFF");  digitalWrite(kond, LOW); RUV = 0; break; //выключить вентиляцию
      case 1: AUO=0; break;
      case 3:                  //автоматическое управление светом включено (послан сигнал на Arduino Nano)
        if (soundVoice == 1)
        {
          wtv020sd16p.playVoice(2);
          delay(50);
          wtv020sd16p.playVoice(2);
        }
      AUO=1; 
      break;
      
      case 5:                    //задание температуры включния вентиляции
        while(Serial.available() == 0){}
        kondTemp = Serial.parseInt(); //записываем значение требуемой температуры
        wtv020sd16p.playVoice(kondTemp+90); 
        delay(50); 
        valK = 0; //обнуляем счетчик отключения вентиляции
        if((kondTemp) > (temph[0])){
            sendMessage("VENTIL_OFF");
            digitalWrite(kond, LOW); //выключаем вентиляцию и обнуляем переменные
            RUV = 0;
            valK = 0;
            konder = 1;
            kondStop = 0;
        }
      break;
      
      case 87:                    //проиграть определенную запись
        while(Serial.available() == 0){}
        int mp3;
        mp3 = Serial.parseInt(); //задать номер записи
        wtv020sd16p.playVoice(mp3);
        delay(10);
        wtv020sd16p.playVoice(mp3);
      break;
      
      case 8: shtora = 1; evening = 0; morning = 0; AUZ = 1; break; //вкл автоматич управление занавесками
      case 80: shtora = 0; AUZ = 0; break;         //выкл автоматич управление занавесками
      
      case 17: digitalWrite(dvig, HIGH); delay(1800); digitalWrite(dvig, LOW); sendMessage("ZANAV_OPEN"); RUZ = 1; break; //открыть занавески
      case 71: digitalWrite(polar, HIGH);                     //закрыть занавески
               delay(50);
               digitalWrite(dvig, HIGH);
               delay(1600);
               digitalWrite(dvig, LOW);
               digitalWrite(polar, LOW);
               sendMessage("ZANAV_CLOSE");
               RUZ = 0;
      break;
      
      case 404:   //сброс системы (пароль 12345 и начало цикла заново)
            k1=0, k2=0, k3=0, k4=0, key1=0, key2=0, key3=0, key4=0;
            sim1 = 12345;
            PassRFID = 1;
            passw=0;
      break;
                       
      case 7:                             //автонформатор (температура, влажность, время)
        int t = dht.readTemperature();
        int h = dht.readHumidity(); 
        int ch = hour();
        int cm = minute();
        voice[0]=int(t);
        voice[1]=int(h);
        voice[2]=int(ch);
        voice[3]=int(cm);
        t1 = (voice[0]) + 90;
        t2 = (voice[1]) - 5;
        c1 = (voice[2]) + 140;
        if(c1 == 140) c1 = 165;
        c2 = (voice[3]) + 166;
    
        wtv020sd16p.playVoice(4);
        delay(1900);
        wtv020sd16p.playVoice(t1);
        delay(10);
        wtv020sd16p.playVoice(t2);
        delay(3300); 
        wtv020sd16p.playVoice(140); 
        delay(10); 
        wtv020sd16p.playVoice(c1);
        delay(1900); 
        wtv020sd16p.playVoice(c2);
        delay(10); 
      break;  
    }  
  }
  delay(10);
 }else RFID(); //если пароль не верный, то функция введения пароля
}


void password(){ //функция введения пароля с 3 попытками и восстановлением в случае утери
  while (k3 == 0){
   //   Serial.println("Welcome! Press the password:");
       if(soundVoice == 1){ 
    wtv020sd16p.playVoice(227);
     delay(200);
     wtv020sd16p.playVoice(227);
     delay(1000);
       }
     k3 = 1;
  }
     while (k1 == 0){
    if(Serial.available() > 0){
      key1 = Serial.parseInt();
      k1++;
      if (key1 == sim1){   
        passw = 1;
   //      Serial.println("Password in correct..");
          if(soundVoice == 1){ 
        wtv020sd16p.playVoice(228);
      delay(100); 
      wtv020sd16p.playVoice(228);
      delay(50);
      }
       
    }}}
    if(key1 != sim1 && k1 != 10){
      k1=0; key1=0;
  //Serial.println("Invalid password! :)"); 
      wtv020sd16p.playVoice(229);
      delay(100); 
      wtv020sd16p.playVoice(229);
      delay(50);
   
    }
    
    if (passw == 0){
    while (k1 == 0){
    if(Serial.available() > 0){
      key2 = Serial.parseInt();
      k1++;
      
      if (key2 == sim1){   //если первая цифра совпадает, то ставим "+"
        passw = 1;
     //    Serial.println("Password in correct..");
        wtv020sd16p.playVoice(228);
      delay(100); 
      wtv020sd16p.playVoice(228);
      delay(50);
          
       
    }}}
     if(key2 != sim1&& k1 != 10){
      k1=0; key2=0;
  //Serial.println("Invalid password! :)"); 
      wtv020sd16p.playVoice(230);
      delay(100); 
      wtv020sd16p.playVoice(230);
      delay(50);
    }}
    
     if (passw == 0){
    while (k1 == 0){
    if(Serial.available() > 0){
      key3 = Serial.parseInt();
      k1++;
      if (key3 == sim1){   //если первая цифра совпадает, то ставим "+"
        passw = 1;
     //    Serial.println("Password in correct..");
       
        wtv020sd16p.playVoice(228);
      delay(100); 
      wtv020sd16p.playVoice(228);
      delay(50);
    }}}
     if(key3 != sim1&& k1 != 10){
      k1=0; key3=0;
    //  Serial.println("Invalid password! :)");
      wtv020sd16p.playVoice(231);
      delay(100); 
      wtv020sd16p.playVoice(231);
      delay(50);  
    }}
    
     if (passw == 0){
    while (k1 == 0){ 
    if(Serial.available() > 0){
      key4 = Serial.parseInt();
      k1++;
      if (key4 == sim1){   //если первая цифра совпадает, то ставим "+"
        passw = 1;
     //    Serial.println("Password in correct.."); 
        wtv020sd16p.playVoice(228);
      delay(100); 
      wtv020sd16p.playVoice(228);
      delay(50);
       
    }}}
     if(key4 != sim1 && k1 != 10){
      k1=10; key4=0;
  //Serial.println("Invalid password! :)"); 
      wtv020sd16p.playVoice(232);
      delay(100); 
      wtv020sd16p.playVoice(232);
      delay(50);
   
    }}
     
       if (passw == 0){
         randomCode = random(99999);
         //Serial.println(randomCode);
       while (k2 == 0){
    if(Serial.available() > 0){
      key5 = Serial.parseInt();
      k2++;
      if (key5 == randomCode){ 
   //   Serial.println("Code in correct.."); 
      wtv020sd16p.playVoice(233);
      delay(100); 
      wtv020sd16p.playVoice(233);
      delay(50);
      question = 1;
      k2 = 10; 
      k4 = 0;
    }}}
    if(key5 != randomCode && k2 != 10){
     // randomCode = random(999999);
     // Serial.println(randomCode);
      k2=0; key5=0;
  //Serial.println("Invalid code!"); 
      wtv020sd16p.playVoice(236);
      delay(100); 
      wtv020sd16p.playVoice(236);
      delay(50);   
   
    }}
    
        if(question == 1){
       while (k4 == 0){
    if(Serial.available() > 0){
      key5 = Serial.parseInt();
      k4++;
      if (key5 == Npasport){ 
   //     Serial.println("Nomber in correct..");
        
        wtv020sd16p.playVoice(234);
      delay(100); 
      wtv020sd16p.playVoice(234);
      delay(50);
       k4 = 10;
       key5=0;
       npas = 0;
       newpass();
       
    }}}
    if(key5 != Npasport && k4 != 10){
     // randomCode = random(999999);
     // Serial.println(randomCode);
      k4=0; key5=0; k2 = 10;
  //Serial.println("Invalid code!"); 
      wtv020sd16p.playVoice(237);
      delay(100); 
      wtv020sd16p.playVoice(237);
      delay(50);   
    }}}

void newpass(){    //функция введения нового пароля
    while (npas == 0){
    if(Serial.available() > 0){
      sim1 = Serial.parseInt();
      npas++;
   //    Serial.println("New password safe."); 
        passw = 0;
        k1=0, k2=0, key1=0, key2=0, key3=0, key4=0;
        betanew=0, question=0;
       wtv020sd16p.playVoice(235);
      delay(100); 
      wtv020sd16p.playVoice(235);
      delay(50);
    }}}

void RFID(){   //функция считвания карты при входе
  while (k3 == 0){
   //   Serial.println("Welcome! Press the password:");
       if(soundVoice == 1){ 
    wtv020sd16p.playVoice(227);
     delay(200);
     wtv020sd16p.playVoice(227);
     delay(1000);
       }
     k3 = 1;
  }
 if ( ! mfrc522.PICC_IsNewCardPresent()) {
  return;
 }
 if ( ! mfrc522.PICC_ReadCardSerial()) {
  return;
 }    
        for (byte i = 0; i < 4; i++) {
          if (uidCard[i] != mfrc522.uid.uidByte[i])
            return;           
        }
  Wire.beginTransmission(4); // открываем передачу устройству №4 (Arduino Nano)
  Wire.write(101);           //передаем код 101 (отключение световой индикации)
  Wire.endTransmission();    //закрываем передачу
  if(soundVoice == 1){ 
    wtv020sd16p.playVoice(228);
    delay(100); 
    wtv020sd16p.playVoice(228);
    delay(50);
  }
  passw = 1; 
}

void RFIDStopSig(){  //функция считывания карты для отключния охранного режима
while(time <= 10){time++; delay(1000);}
 if ( ! mfrc522.PICC_IsNewCardPresent()) {
  return;
 }
 if ( ! mfrc522.PICC_ReadCardSerial()) {
  return;
 }    
        for (byte i = 0; i < 4; i++) {
          if (uidCard[i] != mfrc522.uid.uidByte[i])
            return;           
        }
       sigvoice = 0;
       Wire.beginTransmission(4); // открываем передачу устройству №4 (Arduino Nano)
       Wire.write(101);           //передаем код 101 (отключение световой индикации)
       Wire.endTransmission();    //закрываем передачу
       sendMessage("SIGNAL_OFF"); //передаем приложению сообщение об отключении сигнализации
       OR = 0;                    //записываем в константу состояния сигнализации 0
          if(soundVoice == 1)     //если разрешен звук, то прозносим фразу о правильности введенного пароля
          { 
            wtv020sd16p.playVoice(228);
            delay(100); 
            wtv020sd16p.playVoice(228);
            delay(50);
          }
      /* Wire.beginTransmission(4); // открываем передачу устройству №4 (Arduino Nano)
       Wire.write(101);           //передаем код 101 (отключение световой индикации)
       Wire.endTransmission();    //закрываем передачу*/
       time = 0;
}

void RFIDOR(){   //функция включеня охранного режима
  while(ihome == 0){
  if(soundVoice == 1)     //если разрешен звук, то прозносим фразу
          { 
            wtv020sd16p.playVoice(1);
            delay(5000);
          } 
     ihome = 1;     
  }
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
  return;
   }
  if ( ! mfrc522.PICC_ReadCardSerial()) {
  return;
  }    
        for (byte i = 0; i < 4; i++) {
          if (uidCard[i] != mfrc522.uid.uidByte[i])
            return;           
        }
       Wire.beginTransmission(4); // открываем передачу устройству №4 (Arduino Nano)
       Wire.write(44);           //передаем код 44 (включение охранного режима)
       Wire.endTransmission();   //закрываем передачу
       ihome = 0;
}
     
void kondition()  //функция кондиционирования
{
 if((temph[0]) >= (kondTemp)) //если температура превышает заданную, включить вентиляцию
 {
      while(konder == 1)
      {
         if(soundVoice == 1)
         { 
            wtv020sd16p.playVoice(241);
            delay(100); 
            wtv020sd16p.playVoice(241);
            delay(1000);
         }
      sendMessage("VENTIL_ON");
      RUV = 1;   
      digitalWrite(kond, HIGH);
      konder = 2; //выход из бесконечного цикла
      kondStop = 1; //переменная для разрешения на отключение вентиляции
     }
 }

  if ( (r <= -1) && (kondStop == 1) ) //если разница температур достигает 1, то отключить вентиляцию
  {  
    valK++;                         // работает счет
  }
  if (valK >= 6)                      //кол-во проходов, при достижении этого значения вентиляция выключается
  {
    sendMessage("VENTIL_OFF");
    digitalWrite(kond, LOW);
    RUV = 0;
    valK = 0;
    konder = 1;
    kondStop = 0;
  } 
}

void winlight() //функция слежения за уровнем освещенности и положением штор
{
  if(photorez >= 300)
  {
  evening = 0;
  morning++;
    while(morning == 1)
    {
      if(OR != 1)
      {
        if(shtora == 1){
        digitalWrite(dvig, HIGH);
        delay(2300);
        digitalWrite(dvig, LOW);
         sendMessage("ZANAV_OPEN");
         RUZ = 1;
        }
    morning = 2;
    }
   }
  }
  else
  {
  morning=0;
  evening++;
      while(evening == 1){
        if(OR != 1){
               if(shtora == 1){
               digitalWrite(polar, HIGH);
               delay(50);
               digitalWrite(dvig, HIGH);
               delay(2100);
               digitalWrite(dvig, LOW);
               digitalWrite(polar, LOW);
                sendMessage("ZANAV_CLOSE");
                RUZ = 0;
               }
        }
     evening = 2;
     }
  }
}


void zvonok()  //нажата кнопка звонка (прерывание)
{
  if (bouncer.update()) {     //если произошло событие
    if (bouncer.read()==0) {    //если кнопка нажата
       wtv020sd16p.playVoice(3);
       delay(50);
       wtv020sd16p.playVoice(3);
   //    Serial.println("pressed");  //вывод сообщения о нажатии
       bouncer.rebounce(500);      //повторить событие через 500мс
       }
     //  else Serial.println("released"); //вывод сообщения об отпускании
    
  }
}

void sendMessage(char* message) //функция передачи сообщения приложению Windows
{
  int messageLen = strlen(message);
  if (messageLen < 256)
  {
    Serial.write(messageLen);
    Serial.print(message);
  }
}

void sendInfoMessages()
{
//int value1 = analogRead(A0);
  char value2[8] = "";
  char value3[16] = "";
  itoa(value1, value2, 10);
  strcat(value3, "IN=");
  strcat(value3, value2);
  sendMessage(value3);
}

void updateint(){ //функция обновления интерфейса приложения
  if(OR == 1) {sendMessage("SIG_ON"); AUO = 0;} else sendMessage("SIGNAL_OFF"); delay(10);
 
  if(AUO == 1) sendMessage("AUO_ON"); else sendMessage("AUO_OFF"); delay(10);
  if(AUZ == 1) sendMessage("AUZ_ON"); else sendMessage("AUZ_OFF"); delay(10);
  if(AUV == 1) sendMessage("AUV_ON"); else sendMessage("AUV_OFF"); delay(10);
  
  if(RUK == 1) sendMessage("KORIDOR_ON"); else sendMessage("KORIDOR_OFF"); delay(10);
  if(RUG == 1) sendMessage("GOST_ON"); else sendMessage("GOST_OFF"); delay(10);
  if(RUT == 1) sendMessage("VAN_ON"); else sendMessage("VAN_OFF"); delay(10);
  if(RUP == 1) sendMessage("PERIM_ON"); else sendMessage("PERIM_OFF"); delay(10);
  if(RUO == 1) sendMessage("ALL_ON"); else sendMessage("ALL_OFF"); delay(10);
  if(SO == 1) sendMessage("SO_ON"); else sendMessage("SO_OFF"); delay(10);
  if(AINF == 1) sendMessage("AUTOINF_ON"); else sendMessage("AUTOINF_OFF"); delay(10);
  if(RUZ == 1) sendMessage("ZANAV_OPEN"); else sendMessage("ZANAV_CLOSE"); delay(10);
  if(RUV == 1) sendMessage("VENTIL_ON"); else sendMessage("VENTIL_OFF"); delay(10);
  sendMessage("I_AM_READY_END");
}

void receiveEvent(int howMany) //функция приема сообщения от устройств по шине I2C
{
  while(1 < Wire.available()) 
  { }
  priem = Wire.read();   //записываем принятое сообщение в константу
}  
