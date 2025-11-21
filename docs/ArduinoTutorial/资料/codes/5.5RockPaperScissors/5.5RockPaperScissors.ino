#include <Arduino.h>
#include <Sentry.h>

typedef Sengo2 Sengo;

#define SENGO_I2C
// #define SENGO_UART

#ifdef SENGO_I2C
#include <Wire.h>
#endif
#ifdef SENGO_UART
#include <SoftwareSerial.h>
#define TX_PIN 11
#define RX_PIN 10
SoftwareSerial mySerial(RX_PIN, TX_PIN);
#endif

#define VISION_TYPE Sengo::kVisionAprilTag
Sengo sengo;

#define ML 4
#define ML_PWM 6
#define MR 2
#define MR_PWM 5

void setup() {
  sentry_err_t err = SENTRY_OK;

  Serial.begin(9600);

  Serial.println("Waiting for sengo initialize...");
#ifdef SENGO_I2C
  Wire.begin();
  while (SENTRY_OK != sengo.begin(&Wire)) {
    yield();
  }
#endif  // SENGO_I2C
#ifdef SENGO_UART
  mySerial.begin(9600);
  while (SENTRY_OK != sengo.begin(&mySerial)) {
    yield();
  }
#endif  // SENGO_UART
  Serial.println("Sengo begin Success.");
  err = sengo.VisionSetMode(VISION_TYPE, Sengo::kVisionModeFamily16H5);
  err = sengo.VisionSetLevel(VISION_TYPE, kLevelSpeed);
  if (err) {
    Serial.print("sengo.VisionSetMode Error: 0x");
    Serial.println(err, HEX);
  }
  err = sengo.VisionBegin(VISION_TYPE);
  Serial.print("sengo.VisionBegin(kVisionAprilTag) ");
  if (err) {
    Serial.print("Error: 0x");
  } else {
    Serial.print("Success: 0x");
  }
  Serial.println(err, HEX);

  pinMode(ML, OUTPUT);      //设置左电机方向控制引脚为输出
  pinMode(ML_PWM, OUTPUT);  //设置左电机方向控制引脚为输出
  pinMode(MR, OUTPUT);      //设置左电机方向控制引脚为输出
  pinMode(MR_PWM, OUTPUT);  //设置左电机方向控制引脚为输出
}

void loop() {

  int randomNumber = random(3);  //随机生成0，1，2数字
  int result = 0;

  int obj_num = sengo.GetValue(VISION_TYPE, kStatus);
  if (obj_num) {
    for (int i = 1; i <= obj_num; ++i) {
      int userNumber = sengo.GetValue(VISION_TYPE, kLabel, i);
      Serial.print("randomNumber：");
      Serial.print(randomNumber);

      result = (userNumber - randomNumber + 3) % 3;
      Serial.print("   result:");
      Serial.print(result);
      Serial.print("   userNumber：");
      Serial.println(userNumber);
      if (result == 0) {
        Serial.println("平局");
        car_stop();
      } else if (result == 1) {
        Serial.println("你输了！");
        car_forward();
        delay(500);
        car_stop();
      } else if (result == 2) {
        Serial.println("你赢了！");
        car_back();
        delay(500);
        car_stop();
      }
    }
    delay(2000); 
  }
}

//小车前进代码
void car_forward() {
  digitalWrite(ML, LOW);
  analogWrite(ML_PWM, 100);
  digitalWrite(MR, LOW);
  analogWrite(MR_PWM, 100);
}

//小车后退代码
void car_back() {
  digitalWrite(ML, HIGH);
  analogWrite(ML_PWM, 150);
  digitalWrite(MR, HIGH);
  analogWrite(MR_PWM, 150);
}

//小车停止代码
void car_stop() {
  digitalWrite(ML, LOW);
  analogWrite(ML_PWM, 0);
  digitalWrite(MR, LOW);
  analogWrite(MR_PWM, 0);
}