#include "https.h"
const int pin_light = 2;
const int pin_infrared = 25;

////全局变量
double status;//当前可能性
bool active;//当前开关状态



//// 蜂鸣器模块
const int pin_beep = 15;
void beep_leave()
{
  for (int i = 0; i < 100000; i++)
  {
    digitalWrite(pin_beep, HIGH); // 23ok
    delay(0.1);
    digitalWrite(pin_beep, LOW); // 23ok
    delay(0.1);
  }
}

void beep_come()
{
  for (int i = 0; i < 100000; i++)
  {
    digitalWrite(pin_beep, HIGH); // 23ok
    delay(0.1);
    digitalWrite(pin_beep, LOW); // 23ok
    delay(0.1);
  }
}

//// 初始化


void setup()
{
  //初始化串口
  Serial.begin(115200);
  //初始化蜂鸣器
  pinMode(pin_beep, OUTPUT);
  //初始化LED灯
  pinMode(23, OUTPUT);
  pinMode(19, OUTPUT);
  pinMode(18, OUTPUT);

  digitalWrite(23, HIGH);
  digitalWrite(18, HIGH);
  digitalWrite(19, HIGH);
  //初始化传感器
  pinMode(pin_light, INPUT);
  pinMode(pin_infrared, INPUT);
  //初始化wifi
  WIFIandHTTPS_setup();
}






/**
 * @brief 闪灯一秒
 * 
 * @return ** void 
 */
void blink_1s()
{
  int rand_t;
  for (int i = 0; i < 10; i++)
  {
    rand_t = random(0, 100);
    if (rand_t < status)
      digitalWrite(19, HIGH);
    else
      digitalWrite(19, LOW);
    rand_t = random(0, 100);
    if (rand_t < status)
      digitalWrite(18, HIGH);
    else
      digitalWrite(18, LOW);
    rand_t = random(0, 100);
    if (rand_t < status)
      digitalWrite(23, HIGH);
    else
      digitalWrite(23, LOW);
    delay(100);
  }
}


void getStatus()
{
  int infrared = digitalRead(pin_infrared);
  status = status * 0.99;
  status += infrared * 20;

  if (status > 100)
    status = 100;
  if (status < 0)
    status = 0;
}

/**
 * @brief 发送当前状态
 *
 * @return ** void
 */

void sendStatus()
{
  if (active)
  {
    sendActive("open");
  }
  else
  {
    sendActive("close");
  }
}
/**
 * @brief 处理情况，临界点蜂鸣，同步状态
 *
 * @return ** void
 */
void dealStatus()
{
  if (active && status < 1)
  {
    beep_leave();
    active = false;
    sendStatus();
  }
  if (!active && status > 60)
  {
    beep_come();
    active = true;
    sendStatus();
  }
}



long long time_cnt = 0;
void loop()
{
  // 定期重启wifi
  time_cnt++;
  if (time_cnt > 3600)
  {
    WIFIandHTTPS_delete();
    WIFIandHTTPS_setup();
    time_cnt = 0;
  }

  // 获取数据
  getStatus();
  // 处理当前状态
  dealStatus();

  // 定期发送一次数据
  if (time_cnt % 300 == 0)
  {
    sendStatus();
  }
  // 输出当前状态
  Serial.println(status);

  // 闪灯一秒
  blink_1s();
}
