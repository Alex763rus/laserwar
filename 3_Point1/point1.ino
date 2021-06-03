#include <iarduino_IR_RX.h>

int pinRedTeam = 6;
int pinBlueTeam = 7;
int pinIk = 8;
int pinSound = 9;

iarduino_IR_RX IR(pinIk);

bool isRed;
bool isEndGame;
unsigned long timerRedTeam;
unsigned long timerBlueTeam;
unsigned long startPaintPoint;
unsigned long startBlink;      //время запуска моргания
int countMinuteRed;
unsigned long countMinuteTimeRed;
int countMinuteBlue;
unsigned long countMinuteTimeBlue;

int modeSetting; //переменная для выполнения настройки игры

void setup(){
  Serial.begin(9600);
  IR.begin();
  IR.protocol(IR_CLEAN);
 // IR.protocol("BeN@bpLKKXLNXJLJ@@@@@D@@`"); //протокол автоматов
  pinMode(pinRedTeam, OUTPUT);
  pinMode(pinBlueTeam, OUTPUT);
  pinMode(pinSound, OUTPUT);

  init(600000);//600000 - 10 минут
}
void init(long gameLength){
  isRed = false;
  isEndGame = false;
  timerRedTeam = gameLength;
  timerBlueTeam = gameLength;
  startPaintPoint = 0;
  startBlink = 0;
  
  countMinuteRed = 1;          //1 минута. Переменная для воспроизведения едеминутного звука
  countMinuteBlue = 1;         //1 минута. Переменная для воспроизведения едеминутного звука
  countMinuteTimeRed = 60000;  //1 минута. Переменная для воспроизведения едеминутного звука
  countMinuteTimeBlue = 60000; //1 минута. Переменная для воспроизведения едеминутного звука
  modeSetting = 0;
  hello();
}
boolean checkEndGame(){
  if(startPaintPoint == 0){
    return false;
  }
  if(isRed && ((millis() - startPaintPoint) > timerRedTeam)){
    return true;
  }
  if(!isRed && ((millis() - startPaintPoint) > timerBlueTeam)){
    return true;
  }
  return false;
}

void loop(){
  if(isEndGame){
    endGame();
    return;
  }
  if(startPaintPoint == 0 && modeSetting == 0){
    blinkColors();
  }
  isEndGame = checkEndGame();
  if(IR.check()){
    if(IR.data == 16769565){//menu
      modeSetting = 1;
      setColorAll();
      playMusic(1);
    }
    if(modeSetting == 1){
      if(IR.data == 16724175){//1
        init(300000);//5 минут
      }
      if(IR.data == 16718055){//2
        init(600000);//10 минут
      }
      if(IR.data == 16743045){//3
        init(900000);//15 минут
      }
      if(IR.data == 16716015){//4
        init(1200000);//20 минут
      }
      return;
    }
    if((millis() - startPaintPoint) < 2000 || modeSetting != 0){
      return;
    }
    if(startPaintPoint != 0){
      if(isRed){
        setColorBlue();
        timerRedTeam = timerRedTeam - (millis() - startPaintPoint);
      }else{
        setColorRed();
        timerBlueTeam = timerBlueTeam - (millis() - startPaintPoint);
      }
    }
    startPaintPoint = millis();
    playMusic(1);
  }
  if(startPaintPoint != 0){
    playCountMinute();
  }
}
void playCountMinute(){
  if(isRed && (timerRedTeam - (millis() - startPaintPoint)) < (timerRedTeam - countMinuteTimeRed)){
    playMusic(countMinuteRed);
    ++countMinuteRed;
    countMinuteTimeRed = countMinuteRed * 60000; 
  }
  if(!isRed && (timerBlueTeam - (millis() - startPaintPoint)) < (timerBlueTeam - countMinuteTimeBlue)){
    playMusic(countMinuteBlue);
    ++countMinuteBlue;
    countMinuteTimeBlue = countMinuteBlue * 60000;
  }
}
void hello(){
  playMusic(1);
  setColorAll();
  delay(1000);
  playMusic(2);
  setColorNone();
}
void blinkColors(){
  if((millis() - startBlink) > 5000){
    if(isRed){
      setColorBlue();
    }else{
      setColorRed();
    }
    startBlink = millis();
  } 
}
void endGame(){
  setColorAll();
  analogWrite(pinSound, 500);
  delay(5000);
  analogWrite(pinSound, 0);
  if(isRed){
    digitalWrite(pinRedTeam, HIGH);
    digitalWrite(pinBlueTeam, LOW);
  }else{
    digitalWrite(pinRedTeam, LOW);
    digitalWrite(pinBlueTeam, HIGH);
  }
  delay(1000);
}
void setColorRed(){
  digitalWrite(pinRedTeam, HIGH);
  digitalWrite(pinBlueTeam, LOW);
  isRed = true;
}
void setColorBlue(){
  digitalWrite(pinRedTeam, LOW);
  digitalWrite(pinBlueTeam, HIGH);
  isRed = false;
}
void setColorAll(){
  digitalWrite(pinRedTeam, HIGH);
  digitalWrite(pinBlueTeam, HIGH);
}
void setColorNone(){
  digitalWrite(pinRedTeam, LOW);
  digitalWrite(pinBlueTeam, LOW);
}
void playMusic(int countRepeat){
  for(int i = 0; i < countRepeat; ++i){
    analogWrite(pinSound, 500);
    delay(300);
    analogWrite(pinSound, 0);
    delay(300);
  }
}
