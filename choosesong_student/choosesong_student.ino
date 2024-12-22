#include <IRremote.h>


//歌曲
int Eulogyofjoytune[]=                 //根据简谱列出各频率
{
  350,350,393,441,
  441,393,350,330,
  294,294,330,350,
  350,330,330,
  350,350,393,441,
  441,393,350,330,
  294,294,330,350,
  330,294,294,
  330,330,350,294,
  330,350,393,350,294,
  330,350,393,350,330,
  294,330,221,-1,
  350,350,393,441,
  441,393,350,393,330,
  294,294,330,350,
  330,294,294
};
int lstune[]={
  262,262,393,393,
  441,441,393,
  350,350,330,330,
  294,294,262,
  393,393,350,350,
  330,330,294,
  393,393,350,350,
  330,330,294,
  262,262,393,393,
  441,441,393,
  350,350,330,330,
  294,294,262
};

//节拍
float Eulogyofjoybeat[]=                   //根据简谱列出各节拍
{
  1,1,1,1,
  1,1,1,1,
  1,1,1,1,
  1+0.5,0.5,1+1,
  1,1,1,1,
  1,1,1,1,
  1,1,1,1,
  1+0.5,0.5,1+1,
  1,1,1,1,
  1,0.5,0.5,1,1,
  1,0.5,0.5,1,1,
  1,1,1,1,
  1,1,1,1,
  1,1,1,0.5,0.5,
  1,1,1,1,
  1+0.5,0.5,1+1,
};
float lsbeat[]={
  1,1,1,1,
  1,1,1+1,
  1,1,1,1,
  1,1,1+1,
  1,1,1,1,
  1,1,1+1,
  1,1,1,1,
  1,1,1+1,
  1,1,1,1,
  1,1,1+1,
  1,1,1,1,
  1,1,1+1
};
int fsjtune[]={
  556,441,556,441,556,441,350,
  393,495,441,393,556,
  556,441,556,441,556,441,350,
  393,495,441,393,350,
  393,393,495,495,441,350,556,
  393,495,441,393,556,
  556,441,556,441,556,441,350,
  393,495,441,393,556
};
float fsjbeat[]={
  0.5,0.5,0.5,0.5,0.5,0.5,1,
  0.5,0.5,0.5,0.5,1+1,
  0.5,0.5,0.5,0.5,0.5,0.5,1,
  0.5,0.5,0.5,0.5,1+1,
  0.5,0.5,0.5,0.5,0.5,0.5,1,
  0.5,0.5,0.5,0.5,1+1,
  0.5,0.5,0.5,0.5,0.5,0.5,1,
  0.5,0.5,0.5,0.5,1+1
};
int hbtune[]={
  221,221,248,221,294,278,
  221,221,221,221,248,221,
  294,278,330,294,221,221,
  441,350,294,278,278,248,
  393,393,350,294,330,294
};
float hbbeat[]={
  0.5,0.5,1,1,1,1,
  0.5,0.5,1,1,1,1,
  0.5,0.5,1,1,1,1,
  0.5,0.5,1,1,1,1,
};

//设置接口
int buzzerpin = 10;
int ledpin = 13;
float speed =1.0;
int IR_RECEIVE_PIN = 9;

void setup() {
  pinMode(buzzerpin,OUTPUT);
  pinMode(ledpin,INPUT);
  Serial.begin(9600);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
}
//播放歌曲的函数
void start(int len,int buzzpin,int lepin,int *tune,float *beat,float speed){
  for(int x=0;x<len;x++)
  {
    tone(buzzpin,tune[x]);
    digitalWrite(lepin,HIGH); 
    delay(500*beat[x]/speed);//与前一代码不同之处，这里将原来的500分为了400和100，分别控制led的开与关，对于蜂鸣器来说依然是500.
    digitalWrite(lepin,LOW);
    noTone(buzzpin);   
    delay(100);
  }
}
void loop() {
  String select;
  while(Serial.available()>0){
    select+=char(Serial.read());
    delay(2);
  }
  if(select!=""){
    int len;
    if(select=="Ode to Jo"){
      Serial.println("Next, we will play the Ode to Jo");
      len=sizeof(Eulogyofjoytune)/sizeof(Eulogyofjoytune[0]);
      start(len,buzzerpin,ledpin,Eulogyofjoytune,Eulogyofjoybeat,speed);
    }else if(select=="little star"){
      Serial.println("Next, we will play the little star");
      len=sizeof(lstune)/sizeof(lstune[0]);
      start(len,buzzerpin,ledpin,lstune,lsbeat,speed);}
    else if(select=="Whitewashre"){
      Serial.println("Next, we will play the Whitewasher");
      len=sizeof(fsjtune)/sizeof(fsjtune[0]);
      start(len,buzzerpin,ledpin,fsjtune,fsjbeat,speed);}
    else if(select=="Happy Birthday"){
      Serial.println("Next, we will play the Happy Birthday");
      len=sizeof(fsjtune)/sizeof(hbtune[0]);
      start(len,buzzerpin,ledpin,hbtune,hbbeat,speed);}
    else if(select=="Speed up"){
      speed+=0.25;
      Serial.println("Next, we will speed up");
      Serial.println(speed);
    }
    else if(select=="Speed down"){
      speed-=0.25;
      Serial.println("Next, we will speed down");
      Serial.println(speed);
    }
    else if(select=="Stop"){
      digitalWrite(ledpin,LOW);
      noTone(buzzerpin);
      Serial.println("stop playing");
    }
    else{
      Serial.println("Sorry, there is no such song in the music library.");
    }
  }
  
}
