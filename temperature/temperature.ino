#include <LiquidCrystal_I2C.h>  // 液晶显示屏库
#include <DFRobot_DHT11.h>      // 温湿度传感器库

// 初始化 LCD 对象，参数：地址、列数、行数（根据实际情况设置地址，常见为 0x27 或 0x20）
LiquidCrystal_I2C lcd(0x20, 16, 2);

// 初始化温湿度传感器对象
DFRobot_DHT11 DHT;

// 定义温湿度传感器连接的数字引脚
#define DHT11_PIN 4

void setup() {
  // 初始化 LCD 屏幕
  lcd.init();                  // 初始化 LCD
  lcd.backlight();             // 打开 LCD 背景灯

  // 设置串口通信波特率
  Serial.begin(9600);

  // LCD 屏显示固定内容
  lcd.setCursor(0, 0);         // 设置光标到第 1 行，第 1 列
  lcd.print("Humidity(%):");   // 显示湿度提示
  lcd.setCursor(0, 1);         // 设置光标到第 2 行，第 1 列
  lcd.print("Temp(C):");       // 显示温度提示

  // 串口监视器输出固定表头
  Serial.println("Type,\tHumidity(%),\tTemperature(C)");
}

void loop() {
  // 从 DHT11 传感器读取数据
  DHT.read(DHT11_PIN);

  // 获取湿度和温度数据
  float humidity = DHT.humidity;      // 获取湿度值
  float temperature = DHT.temperature; // 获取温度值

  // 将温湿度数据打印到串口监视器
  Serial.print("DHT11,\t");
  Serial.print(humidity, 1);          // 湿度，保留 1 位小数
  Serial.print(",\t");
  Serial.println(temperature, 1);     // 温度，保留 1 位小数

  // 在 LCD 屏显示湿度
  lcd.setCursor(12, 0);               // 设置光标到第 1 行第 13 列
  lcd.print("    ");                  // 清除之前的数据
  lcd.setCursor(12, 0);
  lcd.print(humidity, 1);             // 显示湿度值

  // 在 LCD 屏显示温度
  lcd.setCursor(10, 1);               // 设置光标到第 2 行第 11 列
  lcd.print("    ");                  // 清除之前的数据
  lcd.setCursor(10, 1);
  lcd.print(temperature, 1);          // 显示温度值

  // 延迟 1 秒，模拟实时刷新
  delay(1000);
}