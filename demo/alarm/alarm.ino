#include <LiquidCrystal_I2C.h> // 液晶显示屏库
#include <IRremote.h>         // 红外遥控库

// 初始化 LCD 对象，地址为 0x27，16 列 2 行
LiquidCrystal_I2C lcd(0x20, 16, 2);
// 红外接收引脚
#define IR_RECEIVE_PIN 10
// 蜂鸣器引脚
#define BUZZER_PIN 8
// 定义按键的编码
#define START 0xFF00BF00
#define ONE 0xEF10BF00
#define TWO 0xEE11BF00
#define RIGHT 0xF906BF00
#define LEFT 0xFB04BF00
#define INCREASE 0xFE01BF00
#define CLOCK_TIME 5

// 全局时间变量
int Y1 = 2, Y2 = 0, Y3 = 2, Y4 = 4; // 年份 2024
int M1 = 1, M2 = 2;                 // 月份 12
int D1 = 0, D2 = 4;                 // 日期 03
int H1 = 2, H2 = 1;                 // 小时 00
int MI1 = 1, MI2 = 6;               // 分钟 00
int S1 = 0, S2 = 0;                 // 秒数 00
int Date = 2;                       // 星期
// 闹钟相关变量
int Alarm_H1 = 0, Alarm_H2 = 0;     // 闹钟小时
int Alarm_MI1 = 0, Alarm_MI2 = 0;   // 闹钟分钟
boolean alarm_flag = false;         // 闹钟模式标志
boolean buzzer_flag = false;        // 蜂鸣器标志
// 其他标志变量
boolean modify_flag = false;        // 修改模式标志
boolean light_flag = true;          // 背景灯标志
boolean serial_buzzer_flag = false;        // 蜂鸣器标志
// 光标位置
int x = 0, y = 0;
int modifyIndex = 0; //更改位置索引 
int pos[] = {0, 1, 2, 3, 5, 6, 8, 9, 16, 17, 19, 20, 22, 23}; // 光标位置数组

// 初始化
void setup() {
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  Serial.begin(9600);            // 设置波特率
  lcd.init();                    // 初始化 LCD
  lcd.backlight();               // 打开背景灯
  lcd.clear();
  pinMode(BUZZER_PIN, OUTPUT);   // 设置蜂鸣器为输出模式
  digitalWrite(BUZZER_PIN, LOW); // 初始化蜂鸣器状态
}

// 主循环
void loop() {
  // 处理红外信号
  if (IrReceiver.decode()) {
    handleIRSignal();            // 调用处理红外信号的函数
    IrReceiver.resume();         // 准备接收下一个信号
  }

  handleSerial(); // 处理串口输入

  // 根据模式切换显示逻辑
  if (!modify_flag && !alarm_flag) {
    calendarShow();              // 显示时间
    tita();                      // 时间递增
    judgeAlarm();
    delay(1000);                 // 模拟时间流动
  }
  // 蜂鸣器状态控制
  if (buzzer_flag || serial_buzzer_flag) {
    digitalWrite(BUZZER_PIN, HIGH);
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }
}

// 显示时间到 LCD
void calendarShow() {
  lcd.setCursor(0, 0);
  lcd.print(Y1); lcd.print(Y2); lcd.print(Y3); lcd.print(Y4);
  lcd.print("/"); lcd.print(M1); lcd.print(M2);
  lcd.print("/"); lcd.print(D1); lcd.print(D2);

  lcd.setCursor(0, 1);
  lcd.print(H1); lcd.print(H2); lcd.print(":");
  lcd.print(MI1); lcd.print(MI2); lcd.print(":");
  lcd.print(S1); lcd.print(S2);

  lcd.setCursor(10, 1);
  const char *days[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
  getweek();
  lcd.print(days[Date]);
}

// 时间递增逻辑
void tita() {
  S2++;
  if (S2 == 10) {
    S2 = 0; S1++;
    if (S1 == 6) {
      S1 = 0; MI2++;
      if (MI2 == 10) {
        MI2 = 0; MI1++;
        if (MI1 == 6) {
          MI1 = 0; H2++;
          if (H2 == 10 || (H1 == 2 && H2 == 4)) {
            H2 = 0; H1++;
            if (H1 == 3) {
              H1 = 0; date();
            }
          }
        }
      }
    }
  }
}

// 日期递增逻辑
void date() {
  Date = (Date + 1) % 7; // 星期数递增
  int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if ((Y4 % 4 == 0 && Y4 % 100 != 0) || Y4 % 400 == 0) {
    daysInMonth[1] = 29; // 闰年
  }

  if (D2 + D1 * 10 > daysInMonth[M1 * 10 + M2 - 1]) {
    D2 = 1; D1 = 0; M2++;
    if (M2 == 10) {
      M2 = 0; M1++;
      if (M1 == 2) {
        M1 = 0; Y4++;
      }
    }
  }
}

void handleIRSignal() {
  switch (IrReceiver.decodedIRData.decodedRawData) { 
  case START:
    light_flag = !light_flag;
    if (light_flag) lcd.backlight();
    else lcd.noBacklight();
    break;
  case ONE:
    modify_flag = !modify_flag;
    alarm_flag = false;
    buzzer_flag = false;
    if (modify_flag) xblink();
    else xNoblink();
    break;
  case TWO:
    alarm_flag = !alarm_flag;
    modify_flag = false;
    buzzer_flag = false;
    if (alarm_flag){
      xblink();
      setClock();
    }else {
      lcd.clear();
      xNoblink();
    }
    break;
  case LEFT:
    move(-1);
    break;
  case RIGHT:
    move(1);
    break;
  case INCREASE:
    addTime();
    move(-1);
    move(1);
    break;
  }
  Serial.println(IrReceiver.decodedIRData.decodedRawData,HEX);
}

// 光标闪烁
void xblink() {
  lcd.setCursor(x, y);
  lcd.blink();
}

// 取消光标闪烁
void xNoblink() {
  lcd.noBlink();
}

void move(int step) {
  if (modify_flag) {
    modifyIndex += step;
    if (modifyIndex > 13) {
      modifyIndex = 0;
    }
    if (modifyIndex < 0) {
      modifyIndex = 13;
    }
    x = pos[modifyIndex] % 16;
    y = pos[modifyIndex] / 16;
    lcd.setCursor(x, y);
  }
  if (alarm_flag) {
    modifyIndex += step;
    if (modifyIndex > 11) {
      modifyIndex = 8;
    }
    if (modifyIndex < 8) {
      modifyIndex = 11;
    }
    x = pos[modifyIndex] % 16;
    y = pos[modifyIndex] / 16;
    lcd.setCursor(x, y);
  }
}

void addTime(){
  if (modify_flag && !alarm_flag){
    switch (modifyIndex){
      case 0:
        Y1++;
        if (Y1 == 10) Y1 = 0;
        lcd.print(Y1);
        break;
      case 1:
        Y2++;
        if (Y2 == 10) Y2 = 0;
        lcd.print(Y2);
        break;
      case 2:
        Y3++;
        if (Y3 == 10) Y3 = 0;
        lcd.print(Y3);
        break;
      case 3:
        Y4++;
        if (Y4 == 10) Y4 = 0;
        lcd.print(Y4);
        break;
      case 4:
        M1++;
        if (M1 == 2) M1 = 0;
        lcd.print(M1);
        break;
      case 5:
        M2++;
        if (M2 == 10) M2 = 0;
        lcd.print(M2);
        break;
      case 6:
        D1++;
        if (D1 == 4) D1 = 0;
        lcd.print(D1);
        break;
      case 7:
        D2++;
        if (D2 == 10) D2 = 0;
        lcd.print(D2);
        break;
      case 8:
        H1++;
        if (H1 == 3) H1 = 0;
        lcd.print(H1);
        break;
      case 9:
        H2++;
        if (H2 == 10) H2 = 0;
        lcd.print(H2);
        break;
      case 10:
        MI1++;
        if (MI1 == 6) MI1 = 0;
        lcd.print(MI1);
        break;
      case 11:
        MI2++;
        if (MI2 == 10) MI2 = 0;
        lcd.print(MI2);
        break;
      case 12:
        S1++;
        if (S1 == 6) S1 = 0;
        lcd.print(S1);
        break;
      case 13:
        S2++;
        if (S2 == 10) S2 = 0;
        lcd.print(S2);
        break;
      default:
        break;
    }
  }
  else if(!modify_flag && alarm_flag){
    switch (modifyIndex){
      case 8:
        Alarm_H1++;
        if (Alarm_H1 == 3) Alarm_H1 = 0;
        lcd.print(Alarm_H1);
        break;
      case 9:
        Alarm_H2++;
        if (Alarm_H2 == 10) Alarm_H2 = 0;
        lcd.print(Alarm_H2);
        break;
      case 10:
        Alarm_MI1++;
        if (Alarm_MI1 == 6) Alarm_MI1 = 0;
        lcd.print(Alarm_MI1);
        break;
      case 11:
        Alarm_MI2++;
        if (Alarm_MI2 == 10) Alarm_MI2 = 0;
        lcd.print(Alarm_MI2);
        break;
      default:
        break;
    }
  }
}

void judgeAlarm(){
  if (H1 == Alarm_H1 && H2 == Alarm_H2 && MI1 == Alarm_MI1 && MI2 == Alarm_MI2){
    buzzer_flag = true;
    if (S1*10 + S2 > CLOCK_TIME){
      buzzer_flag = false;
    }
  }else{
    buzzer_flag = false;
  }

}

void getweek(){
  int year, month, day;
  year = Y1 * 1000 + Y2 * 100 + Y3 * 10 + Y4;
  month = M1 * 10 + M2;
  day = D1 * 10 + D2;
  if (month == 1 || month == 2) {
    month += 12;
    year--;
  }
  int week = (day + 2 * month + 3 * (month + 1) / 5 + year + year / 4 - year / 100 + year / 400) % 7;
  Date = week;
}

void setClock(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.println("Setting clock:  ");
  lcd.setCursor(0, 1);
  lcd.print(Alarm_H1); lcd.print(Alarm_H2); lcd.print(":");
  lcd.print(Alarm_MI1); lcd.print(Alarm_MI2);
  move(1);
}

void adjustTime(char* token){
  token = strtok(NULL, " ");
      Y1 = token[0] - '0';
      Y2 = token[1] - '0';
      Y3 = token[2] - '0';
      Y4 = token[3] - '0';
      token = strtok(NULL, " ");
      M1 = token[0] - '0';
      M2 = token[1] - '0';
      token = strtok(NULL, " ");
      D1 = token[0] - '0';
      D2 = token[1] - '0';
      token = strtok(NULL, " ");
      H1 = token[0] - '0';
      H2 = token[1] - '0';
      token = strtok(NULL, " ");
      MI1 = token[0] - '0';
      MI2 = token[1] - '0';
      token = strtok(NULL, " ");
      S1 = token[0] - '0';
      S2 = token[1] - '0';

      String feed_back = "adjust_success: ";
      feed_back += String(Y1) + String(Y2) + String(Y3) + String(Y4) + " ";
      feed_back += String(M1) + String(M2) + " ";
      feed_back += String(D1) + String(D2) + " ";
      feed_back += String(H1) + String(H2) + " ";
      feed_back += String(MI1) + String(MI2) + " ";
      feed_back += String(S1) + String(S2);
      Serial.println(feed_back);    // 返回反馈格式：adjust_success: xxxx xx xx xx xx xx
}

void setAlarm(char* token){
  token = strtok(NULL, " ");
  Alarm_H1 = token[0] - '0';
  Alarm_H2 = token[1] - '0';
  token = strtok(NULL, " ");
  Alarm_MI1 = token[0] - '0';
  Alarm_MI2 = token[1] - '0';

  String feed_back = "set_alarm_success: ";
  feed_back += String(Alarm_H1) + String(Alarm_H2) + " ";
  feed_back += String(Alarm_MI1) + String(Alarm_MI2);
  Serial.println(feed_back);    // 返回反馈格式：set_alarm_success: xx xx
}

void printOnScreen(char* token){
  token = strtok(NULL, " ");
  String str = token;
  while (token != NULL){
    token = strtok(NULL, " ");
    if (token != NULL){
      str += " ";
      str += token;
    }
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(str);
  modify_flag = true;
  if (str == "_exit"){
    modify_flag = false;
  }
}

void switchAlarm(char* token){
  serial_buzzer_flag = !serial_buzzer_flag;
}

void handleSerial(){
  String str_read;
  while(Serial.available()>0){
    str_read+=char(Serial.read());
    delay(2);
  } // 读取串口输入
  if (str_read !=""){
    char* token = strtok(str_read.c_str(), " ");  // 以空格为分隔符
    if (token != NULL && strcmp(token, "adjust_time") == 0){
      adjustTime(token);  // 调整时间
    }
    else if(token != NULL && strcmp(token, "set_alarm") == 0){
      setAlarm(token);    // 设置闹钟
    }
    else if (token != NULL && strcmp(token, "print_on_screen") == 0){
      printOnScreen(token);    // 打印到屏幕
    }
    else if (token != NULL && strcmp(token, "switch_alarm") == 0){
      switchAlarm(token);    // 切换闹钟
    }
    str_read = "";    // 清空串口输入
  }
}