#include <iarduino_IR_RX.h>

const double vers = 1.0;
const int pinRedTeam = 6;
const int pinBlueTeam = 7;
const int pinIk = 8;
const int pinSound = 9;
const int pinServo = 5;

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

int modeSetting; //переменная для выполнения настройки игры
int modeGame;    //1 - классический 2 - с определением кто стрелял
long gameLen;
const long MINUTE_5  = 300000;
const long MINUTE_10 = 600000;
const long MINUTE_15 = 900000;
const long MINUTE_20 = 1200000;
int countGamers;

const int MAX_COUNT_GAMERS = 18;
long gamerId[MAX_COUNT_GAMERS];

void setup(){
  
  Serial.begin(9600);
  IR.begin();
  IR.protocol(IR_CLEAN);
  pinMode(pinRedTeam, OUTPUT);
  pinMode(pinBlueTeam, OUTPUT);
  pinMode(pinSound, OUTPUT);

  modeSetting = 0;
  modeGame = 1;
  gameLen = 600000; //600000 = 10 минут
  //servo.attach(5);
  pinMode(pinServo, OUTPUT);
    
  Serial.println("HELLO!");
  init(gameLen);
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

  modeSetting = 0;
  clearGamerList();
  playMusic(2);

}
void clearGamerList(){
  for(int i = 0; i < MAX_COUNT_GAMERS; ++i){
    gamerId[i]=0;
  }
}
bool gamerIsExsists(int gamerIdIn){
  for(int i = 0; i < countGamers; ++i){
    if(gamerId[i]==0){
      if(i == (countGamers-1)){
        clearGamerList();
      }else{
        gamerId[i]=gamerIdIn;
      }
      return false;
    }
    if(gamerId[i]==gamerIdIn){
      return true;
    }    
  }
  return false;
}
void setGame(int modeGameIn){
  modeGame = modeGameIn;
  modeSetting = 2;
  playMusic(1);
}
void setTime(long timeIn){
  gameLen = timeIn;
  if(modeGame == 1){
    init(gameLen);
  } else{
    modeSetting = 3;
    playMusic(1);    
  }
}
void setCountGamers(int countGamersIn){
  countGamers = countGamersIn;
  init(gameLen);
}

void servoPulse(int angle) {
  for (int i = 0; i <= 50; i++){
    // convert angle to 500-2480 pulse width
    int pulseWidth = (angle * 11) + 500; 
    digitalWrite(pinServo, HIGH); // set the level of servo pin as high
    delayMicroseconds(pulseWidth); // delay microsecond of pulse width
    digitalWrite(pinServo, LOW); // set the level of servo pin as low
    delay(20 - pulseWidth / 1000);
  }
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
    Serial.println(IR.data);
    if(IR.data == 16769565){//menu
      modeSetting = 1;
      playMusic(1);
    }
    if(modeSetting!=0){
      isSettings();
      return;
    }
    if(!isIntervalPassed(startPaintPoint, 2000)){ //после последнего перекрашивания блокировка на перекраску 2 секунды
      playErrorMusic(2);
      return;
    }
    if((modeGame == 2) && gamerIsExsists(IR.data)){
      playErrorMusic(2);
      Serial.println("Игрок уже захватывал!");
      return;
    }
    playMusic(2);
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
      playErrorMusic(countMinuteRed);
      ++countMinuteRed;
    }
  }
  if(pointColor == BLUE){
    if((timerBlueTeam + (millis() - startPaintPoint)) > countMinuteBlue*60000){
      playErrorMusic(countMinuteBlue);
      ++countMinuteBlue;
    }
  }
}
void hello(){
  setColorAll();
  delay(1000);
  setColorNone();
  playMusic(2);
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
  servoSetRed();
}
void setColorBlue(){
  digitalWrite(pinRedTeam, LOW);
  digitalWrite(pinBlueTeam, HIGH);
  servoSetBlue();
}
void setColorAll(){
  digitalWrite(pinRedTeam, HIGH);
  digitalWrite(pinBlueTeam, HIGH);
  servoSetOff();
}
void setColorNone(){
  digitalWrite(pinRedTeam, LOW);
  digitalWrite(pinBlueTeam, LOW);
  servoSetOff();
}
void playMusic(int countRepeat){
  for(int i = 0; i < countRepeat; ++i){
    analogWrite(pinSound, 500);
    delay(300);
    analogWrite(pinSound, 0);
    delay(300);
  }
}
void servoSetRed(){
  servoPulse(0);
}
void servoSetBlue(){
  servoPulse(180);
}
void servoSetOff(){
  servoPulse(90);
}
void playErrorMusic(int countRepeat){
  for(int i = 0; i < countRepeat; ++i){
    analogWrite(pinSound, 500);
    delay(100);
    analogWrite(pinSound, 0);
    delay(100);
  }
}
void isSettings(){
  if(modeSetting == 1){
    //выбираем режим:
    if(IR.data == 16724175){//1
      setGame(1);
    }
    if(IR.data == 16718055){//2
      setGame(2);
    }
  }
  else if(modeSetting == 2){
    //выбираем время:
    if(IR.data == 16724175){//1
      setTime(MINUTE_5);
    }
    if(IR.data == 16718055){//2
      setTime(MINUTE_10);
    }
    if(IR.data == 16743045){//3
      setTime(MINUTE_15);
    }
    if(IR.data == 16716015){//4
      setTime(MINUTE_20);
    }
  }
  else if(modeSetting == 3){
    //указываем количество игроков для второго режима с отслеживанием захвата:
    if(IR.data == 16724175){//1
      setCountGamers(2);
    }
    if(IR.data == 16718055){//2
      setCountGamers(4);
    }
    if(IR.data == 16743045){//3
      setCountGamers(6);
    }
    if(IR.data == 16716015){//4
      setCountGamers(8);
    }
    if(IR.data == 16726215){//5
      setCountGamers(10);
    }
    if(IR.data == 16734885){//6
      setCountGamers(12);
    }
    if(IR.data == 16728765){//7
      setCountGamers(14);
    }
    if(IR.data == 16730805){//8
      setCountGamers(16);
    }
    if(IR.data == 16732845){//9
      setCountGamers(18);
    }
  }
}
