import serial
import time, requests
from bs4 import BeautifulSoup
from datetime import datetime
import threading


POWER =             "FF00BF00"          # 电源
START_PAUSE =       "FA05BF00"          # 开始/暂停
FAST_FORWARD_LEFT = "FB04BF00"          # 左快进
FAST_FORWARD_RIGHT = "F906BF00"         # 右快进
DOWN =              "F708BF00"          # 下
UP =                "F50ABF00"          # 上
EQ =                "F20DBF00"          # EQ
VOLUME_UP =         "FE01BF00"          # 音量+
VOLUME_DOWN =       "F609BF00"          # 音量-
ST_REPT =           "F10EBF00"          # st/rept
FUNC_STOP =         "FD02BF00"          # func/stop
ALT_0 =             "F30CBF00"          # 按键0
ALT_1 =             "EF10BF00"          # 按键1
ALT_2 =             "EE11BF00"          # 按键2
ALT_3 =             "ED12BF00"          # 按键3
ALT_4 =             "EB14BF00"          # 按键4
ALT_5 =             "EA15BF00"          # 按键5
ALT_6 =             "E916BF00"          # 按键6
ALT_7 =             "E718BF00"          # 按键7
ALT_8 =             "E619BF00"          # 按键8
ALT_9 =             "E51ABF00"          # 按键9
button_mapping = {"FF00BF00":"电源", "FA05BF00":"开始/暂停", "FB04BF00":"左快进", "F906BF00":"右快进", "F708BF00":"下", "F50ABF00":"上", "F20DBF00":"EQ", "FE01BF00":"音量+", "F609BF00":"音量-", "F10EBF00":"st/rept", "FD02BF00":"func/stop", "F30CBF00":"按键0", "EF10BF00":"按键1", "EE11BF00":"按键2", "ED12BF00":"按键3", "EB14BF00":"按键4", "EA15BF00":"按键5", "E916BF00":"按键6", "E718BF00":"按键7", "E619BF00":"按键8", "E51ABF00":"按键9"}

# 定义闹钟时钟的按键编码
class AlarmClock:
    def __init__(self,port,baudrate):
        self.ser = serial.Serial(port, baudrate)
        time.sleep(2)

    def send_and_receive_time(self):
        try:
            # 获取当前时间并格式化为指定格式
            now = datetime.now()
            time_str = "adjust_time {year:04d} {month:02d} {day:02d} {hour:02d} {minute:02d} {second:02d}".format(
                year=now.year,
                month=now.month,
                day=now.day,
                hour=now.hour,
                minute=now.minute,
                second=now.second
            )
            self.ser.write(time_str.encode("utf-8"))  # 将字符串编码为字节流发送
            time.sleep(2)  # 等待2s，确保串口数据发送完毕
            response = self.ser.readline().decode('utf-8').strip()  # 读取返回数据并解码、去除首尾空白字符
            print(response)
        except serial.SerialException as e:
            print(f"串口通信出现错误: {e}")
        except Exception as ex:
            print(f"其他错误: {ex}")

    def set_alarm(self,alarm_time_hour=8, alarm_time_minute=0):
        try:
            alarm_str = "set_alarm {hour:02d} {minute:02d}".format(
                hour=alarm_time_hour,
                minute=alarm_time_minute,
            )
            self.ser.write(alarm_str.encode("utf-8"))
            time.sleep(2)
            response = self.ser.readline().decode('utf-8').strip()
            print(response)
        except serial.SerialException as e:
            print(f"串口通信出现错误: {e}")
        except Exception as ex:
            print(f"其他错误: {ex}")

    def _print_on_screen(self,content):
        try:
            content = "print_on_screen " + content
            self.ser.write(content.encode("utf-8"))
            time.sleep(2)
        except serial.SerialException as e:
            print(f"串口通信出现错误: {e}")
        except Exception as ex:
            print(f"其他错误: {ex}")

    def print_on_screen(self,content,times=5):
        self._print_on_screen(content)
        time.sleep(times)
        self._print_on_screen("_exit")

    def switch_alarm(self):
        try:
            content = "switch_alarm"
            self.ser.write(content.encode("utf-8"))
            time.sleep(2)
        except serial.SerialException as e:
            print(f"串口通信出现错误: {e}")
        except Exception as ex:
            print(f"其他错误: {ex}")

    def wait_for_input(self):
        try:
            response = self.ser.readline().decode('utf-8').strip()
            if response.endswith("00") and (response.startswith("F") or response.startswith("E")):
                print("接收到的按键：", button_mapping.get(response, "未知按键")) 
                return response

        except serial.SerialException as e:
            print(f"串口通信出现错误: {e}")
        except Exception as ex:
            print(f"其他错误: {ex}")

    def start_auto_adjust_time(self,interval_seconds=0,interval_minutes=1,interval_hours=0):
        interval = interval_seconds + interval_minutes * 60 + interval_hours * 3600
        while True:
            self.send_and_receive_time()
            time.sleep(interval)

    def close(self):
        self.ser.close()

def start_wait_for_input(clock):
    while True:
        response = clock.wait_for_input()
        if response == FUNC_STOP:
            clock.close()
            break
        if response == START_PAUSE:
            print("暂停")
        if response == ALT_3:
            print("获取北京天气")
            status, temperature = get_beijing_weather()
            clock.print_on_screen(f"Beijing is {status}",3)
            clock.print_on_screen(f"Temp:{temperature}",3)
        if response == FUNC_STOP:
            print("退出")
        if response == ALT_4:
            print("开始闹铃")
            clock.switch_alarm()
        if response == ALT_5:
            print("调整时间")
            clock.send_and_receive_time()


def get_beijing_weather():
    weather_mapping = {"晴": "sunny","多云": "cloudy","阴": "overcast","小雨": "light rain","中雨": "moderate rain","大雨": "heavy rain","暴雨": "storm","雷阵雨": "thunderstorm","雾": "fog","雪": "snow","雨夹雪": "sleet","霾": "haze"}
    url = "https://www.tianqi.com/beijing/"
    try:
        headers = {
            'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.3'
        }
        response = requests.get(url, headers=headers)
        html = BeautifulSoup(response.text, "html.parser")
        weather = html.find("dd", class_="weather")
        if weather :=weather.find("span" ):
            status = weather.find("b").extract()
            temperature = weather.get_text()
            status = weather_mapping.get(status.get_text(), "unknown")
            return status,temperature.replace("℃","").replace("~","/").replace(" ","")

    except Exception as e:
        print(f"发生错误：{e}")

if __name__ == "__main__":
    clk = AlarmClock("/dev/cu.usbmodem11401",9600)
    start_wait_for_input(clk)