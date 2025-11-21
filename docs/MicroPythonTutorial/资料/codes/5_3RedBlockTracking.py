from machine import I2C, Pin, PWM
import time
import random
from Sengo2 import *

# 初始化I2C (根据实际接线调整引脚)
i2c = I2C(0, scl=Pin(21), sda=Pin(20), freq=400000)  # 使用参考代码中的引脚

# 等待Sengo2初始化 (重要!)
time.sleep(3)

# 创建Sengo2对象
sentry = Sengo2()

# 开始传感器
if sentry.begin(i2c) != SENTRY_OK:
    print("Sentry initialization failed!")
    while True: pass

# 设置色块检测参数 - 只检测红色和蓝色
sentry.SetParamNum(sengo2_vision_e.kVisionBlob, 1)  # 设置1组参数

# 红色参数组
sentry.SetParam(
    sengo2_vision_e.kVisionBlob,
    [0, 0, 5, 5, color_label_e.kColorRed],  # 参数格式
    param_id=1
)

# 开启色块检测功能
if sentry.VisionBegin(sengo2_vision_e.kVisionBlob) == SENTRY_OK:
    print("Color block detection enabled")
    

# right wheel
pin1=Pin(14,Pin.OUT)
pin2=PWM(Pin(16))
pin2.freq(50)

# left wheel
pin3=Pin(15,Pin.OUT)
pin4=PWM(Pin(17))
pin4.freq(50)

# As a function of the car going forward.
def car_forward(): 
  pin1.value(0)
  pin2.duty_u16(20000) 
  pin3.value(0)
  pin4.duty_u16(20000)

# As a function of the car going backwards.
def car_back(): 
  pin1.value(1)
  pin2.duty_u16(30000)  
  pin3.value(1)
  pin4.duty_u16(30000)

# As a function of the car going left.
def car_left(): 
  pin1.value(0)
  pin2.duty_u16(5000)  
  pin3.value(1)
  pin4.duty_u16(45000)
# As a function of the car going right.
def car_right(): 
  pin1.value(1)
  pin2.duty_u16(45000)  
  pin3.value(0)
  pin4.duty_u16(5000)

# As a function of the car stopping.
def car_stop(): 
  pin1.value(0)
  pin2.duty_u16(0)  
  pin3.value(0)
  pin4.duty_u16(0)
    

# 主检测循环
try:
    while True:
        # 获取检测到的色块数量
        obj_num = sentry.GetValue(sengo2_vision_e.kVisionBlob,sentry_obj_info_e.kStatus)
        
        if obj_num:
            # 获取位置与大小
            x = sentry.GetValue(sengo2_vision_e.kVisionBlob,sentry_obj_info_e.kXValue,1)
            y = sentry.GetValue(sengo2_vision_e.kVisionBlob,sentry_obj_info_e.kYValue,1)
            w = sentry.GetValue(sengo2_vision_e.kVisionBlob,sentry_obj_info_e.kWidthValue,1)
            h = sentry.GetValue(sengo2_vision_e.kVisionBlob,sentry_obj_info_e.kHeightValue,1)
            if y >= 80:
                #小车后退
                car_back()
            elif x <= 20:
                #小车左转
                car_left()
            elif x >= 80:
                #小车右转
                car_right()
            elif w <= 15 and h <= 15:
                #小车前进
                car_forward()
            elif w >= 70 and h >= 70:
                #小车后退
                car_back()
            else:
                car_stop()
        else: car_stop()
        time.sleep(0.1)  # 短暂延时

except KeyboardInterrupt:
    sentry.VisionEnd(sengo2_vision_e.kVisionBlob)
    print("The program has stopped")


