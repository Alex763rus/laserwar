#include <iarduino_IR_RX.h>

int pinRedTeam = 6;
int pinBlueTeam = 7;
int pinIk = 8;
int pinSound = 9;

iarduino_IR_RX IR(pinIk);

enum { RED  //точка захвачена красной командой
     , BLUE //точка захвачена синей командой
     } pointColor; //Текущий цвет точки
     
enum { BLINK    //точка не захвачена, моргает
     , GAME     //точка захвачена, игра идет
     , END_GAME //время истекло, конец игры 
     } modes;   //Текущее состояние игры
     
unsigned long timerRedTeam;    //Таймер для красной команды
unsigned long timerBlueTeam;   //Таймер для синей команды
unsigned long startPaintPoint; //Время последней перекраски точки
unsigned long startBlink;      //время запуска моргания
unsigned long gameLength;      //время на игру для каждой команды

unsigned long countMinuteRed;  //Количество минут, которое удержала красная команда
unsigned long countMinuteBlue; //Количество минут, которое удержала синяя команда
/*
int modeSetting; //переменная для выполнения настройки игры
*/

void setup(){
  Serial.begin(9600);
  IR.begin();
  IR.protocol(IR_CLEAN);
  pinMode(pinRedTeam, OUTPUT);
  pinMode(pinBlueTeam, OUTPUT);
  pinMode(pinSound, OUTPUT);

  init(600000);//600000 - 10 минут
}
void init(long gameLen){
  gameLength = gameLen;
  pointColor = RED;
  modes = BLINK;
  timerRedTeam = 0;
  timerBlueTeam = 0;
  startPaintPoint = 0;
  startBlink = 0;
  countMinuteRed = 1;
  countMinuteBlue = 1;
  hello();
}
void loop(){
  //printInfo(); //открыть для детализации
  if(modes == END_GAME){
    endGame();
    return;
  }
  if(modes == BLINK){
    blinkColors();
  } 
  if(IR.check()){
    playMusic(1);
    if(!isIntervalPassed(startPaintPoint, 2000)){ //после последнего перекрашивания блокировка на перекраску 2 секунды
       return;
    }
    if(modes == BLINK){
       modes = GAME;
       startPaintPoint = millis();
       return;
    }
    if(modes == GAME){
      if(pointColor == RED){
        timerRedTeam = timerRedTeam + (millis() - startPaintPoint);
      } else{
        timerBlueTeam = timerBlueTeam + (millis() - startPaintPoint);
      }
      changeColor();
      setActualColor();
      startPaintPoint = millis();
      playMusic(1);
    }
  }
  checkEndGame();
  playCountMinute();
}
void printInfo(){
  Serial.print( " timerRedTeam:");    Serial.print(gameLength - timerRedTeam);
  Serial.print( " countMinuteRed:");  Serial.print(countMinuteRed);
  Serial.print( " timerBlueTeam:" );  Serial.print(gameLength - timerBlueTeam);
  Serial.print( " countMinuteBlue:"); Serial.print(countMinuteBlue);
  Serial.print( " pointColor:" );     Serial.print(pointColor);
  Serial.print( " startPaintPoint:" );Serial.print(startPaintPoint);
  Serial.println("");
  delay(3000);
}
void checkEndGame(){
  if(modes == BLINK){
    return;
  }
  if(pointColor == RED){
    if((timerRedTeam + (millis() - startPaintPoint)) > gameLength){
      modes = END_GAME;
    }
  }
  if(pointColor == BLUE){
    if((timerBlueTeam + (millis() - startPaintPoint)) > gameLength){
      modes = END_GAME;
    }
  }
}
void blinkColors(){
  if(isIntervalPassed(startBlink, 5000)){
    changeColor();
    setActualColor();
    startBlink = millis();
  } 
}
void changeColor(){
  if(pointColor == RED){
    pointColor = BLUE;
  }else{
    pointColor = RED;
  }
}
bool isIntervalPassed(long t1, long interval){
  return ((millis() - t1) > interval);
}
void playCountMinute(){
  if(!modes == GAME){
    return;
  }
  if(pointColor == RED){
    if((timerRedTeam + (millis() - startPaintPoint)) > countMinuteRed*60000){
      playMusic(countMinuteRed);
      ++countMinuteRed;
    }
  }
  if(pointColor == BLUE){
    if((timerBlueTeam + (millis() - startPaintPoint)) > countMinuteBlue*60000){
      playMusic(countMinuteBlue);
      ++countMinuteBlue;
    }
  }
}
void hello(){
  playMusic(1);
  setColorAll();
  delay(1000);
  playMusic(2);
  setColorNone();
}
void endGame(){
  setColorAll();
  analogWrite(pinSound, 500);
  delay(2000);
  analogWrite(pinSound, 0);
  setActualColor();
  delay(2000);
}
void setActualColor(){
  if(pointColor == RED){
    setColorRed();
  }else{
    setColorBlue();
  }
}
void setColorRed(){
  digitalWrite(pinRedTeam, HIGH);
  digitalWrite(pinBlueTeam, LOW);
}
void setColorBlue(){
  digitalWrite(pinRedTeam, LOW);
  digitalWrite(pinBlueTeam, HIGH);
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
