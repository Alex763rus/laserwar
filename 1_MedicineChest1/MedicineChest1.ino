#include <iarduino_IR_RX.h> 
#include "GyverTM1637.h"

int pinIk = 7;
int pinDisplayCLK = 2;
int pinDisplayDIO = 3;
int pinSound = 9;
int pinReleResp = 10;
int pinButton = 11;

int pinReleKill = 5;
int pinEcho     = 4;
int pinTrig     = 6;

iarduino_IR_RX IR(7);
GyverTM1637 disp(pinDisplayCLK, pinDisplayDIO);

int modeGame;    //1 - по кнопке (по умолчанию). 2 - по времени. 3 - заданное количество раз. 4 - через время активируется кнопка
int respDelay;   //время задержки между восстановлением для режима №2
int countResp;   //Количество восстановлений для режима №3

int tmpRespDelay;//переменная время задержки между восстановлением для режима №2
int modeSetting; //переменная для выполнения настройки игры
int brSetting;   //переменная для настройки яркости дисплея

long duration;
int currentCm;
int savedCm;

void setup(){
  Serial.begin(9600);
  pinMode(pinSound, OUTPUT);
  pinMode(pinReleResp, OUTPUT);
  pinMode(pinReleKill, OUTPUT);
  pinMode(pinButton, INPUT_PULLUP);
  pinMode(pinTrig,OUTPUT);
  pinMode(pinEcho,INPUT);

  IR.begin();
  digitalWrite(pinReleResp,HIGH);
  digitalWrite(pinReleKill,LOW);

  disp.clear();
  brSetting = 0;
  disp.brightness(brSetting);  // яркость, 0 - 7
  modeGame = 1;
  runningText();
  disp.displayInt(modeGame);
  modeSetting = 0;
  respDelay = 77;
  tmpRespDelay = 0;
  countResp = 7;
  savedCm = 0;
  playMusic(1);

}

void kill(){
  digitalWrite(pinReleKill,HIGH);
  delay(2000);
  digitalWrite(pinReleKill,LOW);
}
void resp(){
  //включает пульт восстановления
  digitalWrite(pinReleResp,LOW);
  delay(2000);
  digitalWrite(pinReleResp,HIGH);
}
void testSystem(){
  playMusic(5);
  byte welcome_banner[] = {_8, _8, _8, _8};
  disp.runningString(welcome_banner, sizeof(welcome_banner), 300);
  disp.displayInt(modeGame);
}
void runningText() {
  byte welcome_banner[] = {_H, _E, _L, _L, _O, };
  disp.runningString(welcome_banner, sizeof(welcome_banner), 300);
}
void printEndGame() {
  byte welcome_banner[] = {_0, _0, _0, _0};
  disp.runningString(welcome_banner, sizeof(welcome_banner), 300);
  disp.scrollByte(welcome_banner, 200);
}

void playMusic(int countRepeat){
  for(int i = 0; i < countRepeat; ++i){
    analogWrite(pinSound, 500);
    delay(300);
    analogWrite(pinSound, 0);
    delay(300);
  }
}

void setRespDelay(int respDelayIn){
  respDelay = respDelayIn;
  tmpRespDelay = respDelayIn;
  modeSetting = 0;
  playMusic(2);
}
void setCountResp(int countRespIn){
  countResp = countRespIn;
  modeSetting = 0;
  playMusic(2);
}
void setModeGame(int modeGameIn){
  modeGame = modeGameIn;
  if(modeGameIn == 1 || modeGameIn == 5){
    modeSetting = 0;
    disp.displayInt(modeGame);
    playMusic(2);
  }else{
    modeSetting = modeGameIn;
    disp.displayInt(modeGame);
    playMusic(1);
  }
}
void settingGame(){
  //16724175 - 1
  //16718055 - 2
  //16743045 - 3
  //16716015 - 4
  //16726215 - 5
  //16734885 - 6
  //16728765 - 7
  //16730805 - 8
  //16732845 - 9
  //16769565 - menu
  //16754775 - OK
  //16720605 - test
  //16712445 - +
  //16750695 - -
  //16753245 - вкл/выкл

  if(IR.check()){
    if(IR.data == 16720605){//test
      testSystem();
    }
    if(IR.data == 16753245){//вкл/выкл
       playMusic(1);
       resp();
    }
    if(IR.data == 16750695){//-
      if(brSetting > 0){
        --brSetting;
        disp.brightness(brSetting);
        playMusic(1);
      }else{
        playMusic(2);
      }
    }
    if(IR.data == 16712445){//+
      if(brSetting < 7){
        ++brSetting;
        disp.brightness(brSetting);
        playMusic(1);
      }else{
        playMusic(2);  
      }
    }
    if(IR.data == 16769565){//menu
      modeSetting = 1;
      playMusic(1);
      disp.clear();
    }
    if(modeSetting == 1){
      //выбираем режим:
      if(IR.data == 16724175){//1
        setModeGame(1);
      }
      if(IR.data == 16718055){//2
        setModeGame(2);
        return;
      }
      if(IR.data == 16743045){//3
        setModeGame(3);
        return;
      }
      if(IR.data == 16716015){//4
        setModeGame(4);
        return;
      }
      if(IR.data == 16726215){//5
        savedCm = 0;
        Serial.print("7771");
        setModeGame(5);
      }
    }
    if(modeSetting == 2){
      if(IR.data == 16724175){//1
        setRespDelay(30);
      }
      if(IR.data == 16718055){//2
        setRespDelay(60);
      }
      if(IR.data == 16743045){//3
         setRespDelay(90);
      }
      if(IR.data == 16716015){//4
        setRespDelay(120);
      }
      if(IR.data == 16726215){//5
         setRespDelay(150);
      }
      if(IR.data == 16734885){//6
        setRespDelay(180);
      }
      if(IR.data == 16728765){//7
        setRespDelay(300);
      }
      disp.displayInt(respDelay);
    }
    if(modeSetting == 3){
      if(IR.data == 16724175){//1
        setCountResp(2);
      }
      if(IR.data == 16718055){//2
        setCountResp(4);
      }
      if(IR.data == 16743045){//3
        setCountResp(6);
      }
      if(IR.data == 16716015){//4
        setCountResp(8);
      }
      if(IR.data == 16726215){//5
        setCountResp(10);
      }
      if(IR.data == 16734885){//6
        setCountResp(15);
      }
      if(IR.data == 16728765){//7
        setCountResp(20);
      }
      disp.displayInt(countResp);
    }
    if(modeSetting == 4){
      if(IR.data == 16724175){//1
        setRespDelay(30);
      }
      if(IR.data == 16718055){//2
        setRespDelay(60);
      }
      if(IR.data == 16743045){//3
         setRespDelay(90);
      }
      if(IR.data == 16716015){//4
        setRespDelay(120);
      }
      if(IR.data == 16726215){//5
         setRespDelay(150);
      }
      if(IR.data == 16734885){//6
        setRespDelay(180);
      }
      if(IR.data == 16728765){//7
        setRespDelay(300);
      }
      disp.displayInt(respDelay);
    }
    Serial.println(IR.data);
  }
}
void logicGameMode1(){
  if(digitalRead(pinButton) == 0){//нажата кнопка
    resp();
  }
}
void logicGameMode2(){
  if(tmpRespDelay == 0){
    playMusic(1);
    resp();
    tmpRespDelay = respDelay;
  }
  --tmpRespDelay;
  disp.displayInt(tmpRespDelay);
  delay(1000);
}
void logicGameMode3(){
  if(digitalRead(pinButton) == 0){//нажата кнопка
    if(countResp>0){
      resp();
      --countResp;
      disp.displayInt(countResp);
    }
    if(countResp == 0){
      playMusic(2);
      printEndGame();
    }
  } 
}
void logicGameMode4(){
  if(tmpRespDelay == 0){
    if(digitalRead(pinButton) == 0){
      resp();
      tmpRespDelay = respDelay;
      disp.displayInt(countResp);
    }
  }else{
    --tmpRespDelay;
    delay(1000);
    if(tmpRespDelay == 0){
      playMusic(1);
    }
  }
  disp.displayInt(tmpRespDelay);
}
void logicGameMode5(){
  digitalWrite(pinTrig, LOW);
  delayMicroseconds(5);
  digitalWrite(pinTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinTrig, LOW);
  duration  = pulseIn(pinEcho, HIGH);
  currentCm = (duration/2)/29.1;

  Serial.println(savedCm);
  disp.displayInt(currentCm);
 
  if(savedCm == 0){
    if(digitalRead(pinButton) == 0){//нажата кнопка, фиксируем расстояние
      savedCm = currentCm - 20;     //на всякий случай - 20 см запас
      playMusic(2);
    }
  }else{
     if(currentCm < savedCm){
        playMusic(2);
        kill();
     }
  }
  delay(1000);
}
void loop(){
  settingGame();
  if(modeSetting != 0){
    return; 
  }
  if(modeGame == 1){
    logicGameMode1();
  }
  if(modeGame == 2){
    logicGameMode2();
  }
  if(modeGame == 3){
    logicGameMode3();
  }
  if(modeGame == 4){
    logicGameMode4();
  }
  if(modeGame == 5){
    logicGameMode5();
  }
}
