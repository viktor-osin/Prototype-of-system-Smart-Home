#include <Servo.h>
#include "Ultrasonic.h"
#include <SPI.h>
#include <Wire.h>
#define fadePin 5 //ванная
#define fadePin1 9 //периметр
#define fadePin2 3 //гостиная
#define fadePin3 6 //коридор
 
int val;
byte newinfo, ledDoorLap;
int clocktime;
int cltime;
int valPIR = 0;
int valpir;
int vallight;
int pirPin1 = A1;                      //контакт для подключения датчика 1 к Arduino
int pirPin2 = A2;                      //контакт для подключения датчика 2 к Arduino

int lightcycle;
int lkorstate;                         //переменные для света в коридоре
int valK;                              //переменная для таймера (накапливает секунды)
long previousMillis1 = 0;              //переменная для сравнения текущего и сохраненного времени

int lzalstate;                         //переменнные для света в комнате
int valC;
long previousMillis2 = 0;

int ltulstate;
int valT;
long previousMillis3 = 0;

int lightG, lightV, lightK;
int ledDoor = 13;
int relay1 = 2;                   //объявление переменных для выходов сигналов реле
int relay2 = 10; 
int relay3 = 4; 
int relay4 = 12;   
int a, b, c, d, k1=0, k2=0, k3=0, k4=0, s1=0, kOK=0, pas=0, MD=0, npas=1, nom=0, InvPas=0, smv=0, smvon=0, PIR3;
int key1, key2, key3, key4, keyOK, passw, kmanl, sig=0, lig=0, key11, zal1=0, zal11=0, kom2=0, kom22=0, tul=0, tul2=0;
char incomingByte;
Ultrasonic ultrasonic(7, 11);
Servo servo;

void setup(){
  Wire.begin(4);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);
  pinMode(A3, OUTPUT);
  pinMode(ledDoor, OUTPUT);
  pinMode(pirPin1, INPUT);                //определить входной контакт для датчика 1
  pinMode(pirPin2, INPUT);                //определить входной контакт для датчика 2
  pinMode(relay1, OUTPUT);                //выходной контакт реле 1
  digitalWrite(relay1, HIGH);             //поддержание высокого уровня на выходе (реле выключено)
  pinMode(relay2, OUTPUT);                //аналогично реле 1...
  digitalWrite(relay2, HIGH);
  pinMode(relay3, OUTPUT);
  digitalWrite(relay3, HIGH);
  pinMode(relay4, OUTPUT);
  digitalWrite(relay4, HIGH);
  
  pinMode(fadePin, OUTPUT); 
  pinMode(fadePin1, OUTPUT);
  pinMode(fadePin2, OUTPUT);
  pinMode(fadePin3, OUTPUT);
  analogWrite(fadePin, 0);
  analogWrite(fadePin1, 0);
  analogWrite(fadePin2, 0);
  analogWrite(fadePin3, 0);

  servo.attach(8);
  servo.write(90);
 
 /* if(EEPROM.read(5) == 5){                //использование сохраненного пароля, если он менялся
  sim1=EEPROM.read(0);                   //в функции "новый пароль", при его изменении, в ячейку памяти 5
  sim2=EEPROM.read(1);                   //записывается 5, и при включении проверяется, если 5 есть, значит
  sim3=EEPROM.read(2);                   //присвоить переменным пароля значения, записанные в EEPROM
  sim4=EEPROM.read(3);
}*/
}


/**************************************************** АВТОМАТИЧЕСКОЕ УПРАВЛЕНИЕ СВЕТОМ (датчики) ********************************************************************************************/

void lightin(){
  while(lig == 0){
    if(analogRead(fadePin3) == HIGH ){Serial.println("ON - koridor");} else {Serial.println("OFF - koridor");}
    if(analogRead(fadePin) == HIGH ){Serial.println("ON - zal");} else {Serial.println("OFF - zal");}
    if(analogRead(fadePin2) == HIGH ){Serial.println("ON - tual");} else{Serial.println("OFF - tual");}
    lig = 1;
  }
  if (lig == 1){ //отключение света при переходе из ручного управления в автоматическое
  lightcycle++; //отсчет времени горения света
  Serial.println(lightcycle);
  if(lightG == 1) //если был включен свет в гостиной
  {
    if(lzalstate == 0 && lightcycle == 100) //если не было движения с датчиков и прошло достаточно времени
    {
      Wire.beginTransmission(5); // transmit to device #5
      Wire.write(21);              // sends one byte  
      Wire.endTransmission();
      for(int k=150; k>=0; k--){analogWrite(fadePin, k); delay(10);} //то гасим свет
      Serial.println("OFF - gost");
      lightG = 0;
    }
  }
  if(lightV == 1)
  {
    if(ltulstate == 0 && lightcycle == 100)
    {
      Wire.beginTransmission(5); // transmit to device #5
      Wire.write(31);              // sends one byte  
      Wire.endTransmission();
      for(int k=150; k>=0; k--){analogWrite(fadePin2, k); delay(10);}
      Serial.println("OFF - vanna");
      lightV = 0;
    }
  }
  if(lightK == 1)
  {
    if(lkorstate == 0 && lightcycle == 100)
    {
      Wire.beginTransmission(5); // transmit to device #5
      Wire.write(22);              // sends one byte  
      Wire.endTransmission();
      for(int k=150; k>=0; k--){analogWrite(fadePin3, k); delay(10);}
      Serial.println("OFF - koridor");
      lightK = 0;
    }
  }
  if (lightcycle >= 101) {lig = 2; lightcycle = 0;}
  }

  if (digitalRead(pirPin1) == HIGH)
  {
    previousMillis1 = millis();
    lzalstate = 1;
    valC = 0;
    while(zal11 == 0){              //то включаем бесконечный цикл для разового вывода на экран (ON - zal)
      Wire.beginTransmission(5); // transmit to device #5
      Wire.write(12);              // sends one byte  
      Wire.endTransmission();
      if(lightG != 1){
        for(int i=0; i<=150; i++){analogWrite(fadePin, i); delay(10);}
        Serial.println("ON - zal"); 
        lightG = 1;
      }  //переменная для работы общего откл/вкл света (команда 15/51)
      zal1 = 0;                          //обнуляем переменную для включения второго цикла
      zal11++;                           //выходим из цикла
    }                                   // далее аналогично для других комнат
  }
    if (digitalRead(pirPin2) == HIGH)
    {
    previousMillis2 = millis();
    lkorstate = 1;
    valK = 0;
     while(kom22 == 0){
       Wire.beginTransmission(5); // transmit to device #5
       Wire.write(11);              // sends one byte  
       Wire.endTransmission();
       if(lightK != 1){
         for(int i=0; i<=150; i++){analogWrite(fadePin3, i); delay(10);}
         Serial.println("ON - koridor");  
         lightK = 1;
       }
       kom2 = 0;
       kom22++;
    }
  }
 
   float dist_cm = ultrasonic.Ranging(CM); 	//значение с датчика в см
   if (dist_cm <= 16){                           //если оно меньше или равно 16см то
   previousMillis3 = millis();                    //включаем свет и запускаем таймер на 15(7,5) сек
   ltulstate = 1;
   valT = 0;
   while(tul2 == 0){
      Wire.beginTransmission(5); // transmit to device #5
      Wire.write(13);              // sends one byte  
      Wire.endTransmission();
      if(lightV != 1){
        for(int i=0; i<=150; i++){analogWrite(fadePin2, i);delay(10);}
        Serial.println("ON - tual");  
        lightV = 1;
      }
      tul = 0;
      tul2++;
    }
 }
    
    if (ltulstate == 1 && dist_cm > 16)
    {
    previousMillis3 = millis();    
    valT++;                       // работает счет
    }else{valT=0;}
   
    if (valT >= 20)                   //кол-во секунд, при достижении этого значения lzal выключается
    {
      Wire.beginTransmission(5); // transmit to device #5
      Wire.write(31);              // sends one byte  
      Wire.endTransmission();
      for(int k=150; k>=0; k--){analogWrite(fadePin2, k); delay(10);}
      Serial.println("OFF - tual");
      lightV = 0;
      smv=1;
      ltulstate = 0;
      valT=0;
      tul2=0;
    }
    
    if(smv >= 1){
      delay(500);
      digitalWrite(A3, HIGH);
      smvon++;
    }
    
    if(smvon == 15){
      smv=0;
      smvon=0;
    } 
    
    if (lzalstate == 1 && digitalRead(pirPin1) < HIGH)
    {
      previousMillis1 = millis();    
      valC++;                       // работает счет
    }
    else
    {
      valC=0;
    }
  
    if (valC >= 15)                   //кол-во секунд, при достижении этого значения lzal выключается
    {
      Wire.beginTransmission(5); // transmit to device #5
      Wire.write(21);              // sends one byte  
      Wire.endTransmission();
      for(int k=150; k>=0; k--){analogWrite(fadePin, k); delay(10);}
      Serial.println("OFF - zal");
      lightG = 0;
      lzalstate = 0;
      valC=0;
      zal11=0;
    }

    if (lkorstate == 1 && digitalRead(pirPin2) < HIGH)
    {
      previousMillis2 = millis();    
      valK++;                         // работает счет
    }
    else
    {
      valK=0;
    }
  
  
    if (valK >= 15)                      //кол-во секунд, при достижении этого значения lkor выключается
    {
        Wire.beginTransmission(5); // transmit to device #5
        Wire.write(22);              // sends one byte  
        Wire.endTransmission();
        for(int k=150; k>=0; k--){analogWrite(fadePin3, k); delay(10);}
        Serial.println("OFF - koridor");
        lightK = 0;
        lkorstate = 0;
        valK=0;
        kom22=0;
    }
    
    
  delay(500);
  digitalWrite(A3, LOW);
}


/*********************************************************** СИГНАЛИЗАЦИЯ ************************************************************************************************/

void PIR(){
  while(sig == 0)
  {
     valK++;       // работает счет
     if (valK >= 15)                      //кол-во секунд, при достижении этого значения lkor выключается
      {
      AllLightOff();
      sig = 1;
      }
      delay(1000);
  }
  ledDoorLap;
  ledDoorLap++;
  if (ledDoorLap >= 5){digitalWrite(ledDoor, HIGH); delay(200); ledDoorLap=0; }
 
  int PIR1 = digitalRead(pirPin1);             //считываем состояние датчика
  int PIR2 = digitalRead(pirPin2);
  float dist_cm = ultrasonic.Ranging(CM); 	// get distance
  if (dist_cm <= 16){PIR3 = HIGH;} 
  valPIR = PIR1 + PIR2 + PIR3;
  if (valPIR == HIGH) {     //если есть движение
    if (PIR1 == HIGH) {Serial.println("Motion is detected in the ZAL");}      //то сказать где движение обнаружено
    if (PIR2 == HIGH) {Serial.println("Motion is detected in the KORIDOR");} 
    if (PIR3 == HIGH) {Serial.println("Motion is detected in the TUALET");}
    Serial.println("Alarm ON!");             //передать на компьютер "Motion!"

  byte x = 109; //cообщение о сигнализации
  Wire.beginTransmission(5); // transmit to device #5
  Wire.write(x);              // sends one byte  
  Wire.endTransmission(); 
  delay(10);
  sig = 0;
  PIR3=LOW;   
  Serial.println("Press and hold any key...");
    while (s1 == 0){
        for(int i=0; i<=2; i++){
       // tone(A0, 2093, 400);
          analogWrite(fadePin1, 255); //периметр вкл
          digitalWrite(ledDoor, HIGH);
          delay(330); 
          analogWrite(fadePin1, 0); // периметр откл
          digitalWrite(ledDoor, LOW);
          delay(330);         
        }
        
        if(val == 101){
          nom=0;   //обратное включения While в PIR, надпись (no motion)
          s1++;
          newinfo = 1;  //обновление информации для вклчения автосвета
          val = 3;
          loop();
       }

    }

  }
  
  delay(300);
  digitalWrite(ledDoor, LOW);
}
  

/***************************************************** ГЛАВНАЯ ЦИКЛИЧЕСКАЯ ФУНКЦИЯ ***********************************************************************************************/

void loop(){
    float temph[2];
    int clock[2];
    //если пришла новая информация
    if(newinfo == 1){
       switch(val){
         //отключение автоматического управения светом
         case 1: 
             valpir = 0;
             vallight = 0;
             tul2=1; kom22=1; zal11=1;
             lkorstate = 0;
             valK=0;
             kom22=0;
             lzalstate = 0;
             valC=0;
             zal11=0;
             ltulstate = 0;
             valT=0;
             tul2=0; 
             transmitToArduinoUno();   //передать информацию на Arduino UNO
         break;
         
         //запуск автоматического управления светом
         case 3:
            Serial.println("light");
            nom=0;   //обратное включениe While в PIR, надпись (no motion)
            sig=0;   //обратное включение While в PIR, сигнал активации охраны
            lig=0;   //обратное включение While в lightin, сигнал активации автосвета
            valpir = 0;
            vallight = 1;
            lightin();
         break;
         //запуск автоматического управления светом
         case 101:
            Serial.println("light");
            nom=0;   //обратное включениe While в PIR, надпись (no motion)
            sig=0;   //обратное включение While в PIR, сигнал активации охраны
            lig=0;   //обратное включение While в lightin, сигнал активации автосвета
            valpir = 0;
            vallight = 1;
            lightin();
         break;
         
         //запуск режима охраны
         case 4: 
            Serial.println("secutity");
            nom=0;   //обратное включения While в PIR, надпись (no motion)
            sig=0;   //обратное включение While в PIR, сигнал активации охраны
            lig=0;   //обратное включение While в lightin, сигнал активации автосвета
            vallight = 0;
            valpir = 1;
            s1 = 0;
             tul2=1; kom22=1; zal11=1;
            lkorstate = 0;
            valK=0;
            kom22=0;
            lzalstate = 0;
            valC=0;
            zal11=0;
            ltulstate = 0;
            valT=0;
            tul2=0; 
            PIR();
         break;
         
         //открыть крышку серверного отделения
         case 9:
              analogWrite(fadePin1, 255);
              delay(300);
              analogWrite(fadePin1, 0);
              for(int i = 90; i<=166; i++){
                  servo.write(i);
                  delay(17);
              }
              transmitToArduinoUno();   //передать информацию на Arduino UNO
         break;
         
         //закрыть крышку серверного отделения
         case 10:
             for(int i = 166; i>=90; i--){
                 servo.write(i);
                 delay(17);
             }
             transmitToArduinoUno();   //передать информацию на Arduino UNO
         break;
         
         //плавно зажечь свет в коридоре 
         case 11:
             Serial.println(11); 
             for(int i=0; i<=150; i++){
               analogWrite(fadePin3, i);//коридор
               delay(10);
             }
             lightK = 1;
             transmitToArduinoUno();   //передать информацию на Arduino UNO
         break;
         //плавно погасить свет в коридоре
         case 22:
             //digitalWrite(relay2, HIGH);
              for(int k=150; k>=0; k--){
                analogWrite(fadePin3, k);//коридор
                delay(10); 
              } 
              lightK = 0;
              transmitToArduinoUno();   //передать информацию на Arduino UNO
         break;
         //плавно зажечь свет в гостиной
         case 12:
             //digitalWrite(relay3, LOW);
              Serial.println(12);
              for(int i=0; i<=150; i++){
                analogWrite(fadePin, i);//гостиная
                delay(10);
              }
              lightG = 1;
              transmitToArduinoUno();   //передать информацию на Arduino UNO
         break;
         //плавно погасить свет в гостиной
         case 21:
             //digitalWrite(relay3, HIGH);
             Serial.println(21);
             for(int k=150; k>=0; k--){
                analogWrite(fadePin, k);//гостиная
                delay(10);
              } 
              lightG = 0;
              transmitToArduinoUno();   //передать информацию на Arduino UNO
         break;
         //плавно зажечь свет в ванной
         case 13:
             //digitalWrite(relay4, LOW); 
              Serial.println(13);
              for(int i=0; i<=150; i++){
                  analogWrite(fadePin2, i);//ванная
                  delay(10);
              }
              lightV = 1;
              transmitToArduinoUno();   //передать информацию на Arduino UNO
         break;
         //плавно погасить свет в ванной
         case 31:
              // digitalWrite(relay4, HIGH);
              Serial.println(31);
              for(int k=150; k>=0; k--){
                  analogWrite(fadePin2, k);//ванная
                  delay(10);
              }   
              lightV = 0;
              transmitToArduinoUno();   //передать информацию на Arduino UNO
         break;
        //зажечь красный свет по периметру
         case 14:
              // digitalWrite(relay1, LOW); 
              Serial.println(14);
              analogWrite(fadePin1, 255);//периметр  
              transmitToArduinoUno();   //передать информацию на Arduino UNO
         break;
         //погасить красный свет по периметру
         case 41:
             //digitalWrite(relay1, HIGH);
              Serial.println(41);
              analogWrite(fadePin1, 0);//периметр
              transmitToArduinoUno();   //передать информацию на Arduino UNO
         break;
         
         //плавно зажечь ВЕСЬ свет
         case 15:
              for(int i=0; i<=150; i++)
              {
                  if(lightG == 0)analogWrite(fadePin, i);
                  if(lightV == 0)analogWrite(fadePin2, i);
                  if(lightK == 0)analogWrite(fadePin3, i);
                  delay(10);
              }
              lightG = 1;
              lightV = 1;
              lightK = 1;
              transmitToArduinoUno();   //передать информацию на Arduino UNO
         break;
         
         //плавно погасить весь свет
         case 51:
             AllLightOff();   
         break;    
      }
      newinfo = 0; //обнуление переменной новой информации
    }
    
    if(vallight == 1){lightin();}
    if(valpir == 1){PIR();}
    delay(30); 
} 

//функция выключения всего света
void AllLightOff(){
for(int k=150; k>=0; k--)
    {
      if(lightG == 1)analogWrite(fadePin, k);//гостиная
      if(lightV == 1) analogWrite(fadePin2, k); //ванная
      if(lightK == 1) analogWrite(fadePin3, k);//коридор
      delay(10);
    }
    lightG = 0;
    lightV = 0;
    lightK = 0;
    transmitToArduinoUno();   //передать информацию на Arduino UNO
}
  
void receiveEvent(int howMany)
{
  while(1 < Wire.available()) // loop through all but the last
  {

  }
 val = Wire.read();    // receive byte as an integer
  Serial.println(val); 
  if(val != 99) newinfo = 1; //команда 99 относится только к Arduino Uno
}  

void transmitToArduinoUno()
{
    Wire.beginTransmission(5); // transmit to device #5
    Wire.write(val);              // sends one byte  
    Wire.endTransmission();
}


