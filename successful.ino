#include <Servo.h>
#include <rgb_lcd.h>
#include <WiFiNINA.h>
#include <ThingSpeak.h>

// 定义红外感应器的端口
const int sensorIn = 2;
const int sensorOut = 3;

// 定义伺服电机的端口
const int servoPin = 9;

// 初始化伺服电机
Servo gateServo;

// 初始化LCD
rgb_lcd lcd;

// 网络凭据
char ssid[] = "1234567890"; // 您的WiFi网络名
char pass[] = "1234567890"; // 您的WiFi密码

// ThingSpeak信息
unsigned long channelID = 2388115; // 替换为您的ThingSpeak频道ID
const char * writeAPIKey = "L8BXVTEWYPYON8QU"; // 替换为您的写入API密钥

// 定义车位数量和其他变量
int totalSpots = 110;
int availableSpots = 97;
int currentParkingCount;

bool carAtInSensor = false;
bool carAtOutSensor = false;

WiFiClient client;

void setup() {

    Serial.begin(9600);
  while (!Serial) {
    ; // 等待串口连接
  }

  // 尝试连接到WiFi网络
  Serial.print("Connecting to ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    delay(2000);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");

  // 设置红外感应器端口为输入
  pinMode(sensorIn, INPUT);
  pinMode(sensorOut, INPUT);

  // 初始化伺服电机
  gateServo.attach(servoPin);
  gateServo.write(0); // 初始化闸门为关闭状态

  // 初始化LCD
  lcd.begin(16, 2);
  lcd.setRGB(255, 255, 255); // 设置背光颜色为白色

  // 显示初始停车信息
  updateLCD();

    // 初始化ThingSpeak
  ThingSpeak.begin(client);
}

void loop() {
  carAtInSensor = digitalRead(sensorIn);
  carAtOutSensor = digitalRead(sensorOut);

if (carAtInSensor && carAtOutSensor) {
    // 暂时不执行任何操作
    delay(100); // 短暂延迟以防止重复触发
  }
  else if (carAtInSensor && availableSpots > 0) {
    // 车辆进入
    openGate();
    delay(3000); // 等待车辆通过
    closeGate();
    availableSpots--;
    updateLCD();
    delay(100); // 防止重复触发
  }

  else if (carAtOutSensor && availableSpots < totalSpots) {
    // 车辆离开
    openGate();
    delay(3000); // 等待车辆通过
    closeGate();
    availableSpots++;
    updateLCD();
    delay(100); // 防止重复触发
  }
  currentParkingCount = totalSpots - availableSpots;
  ThingSpeak.writeField(channelID, 1,availableSpots , writeAPIKey);
  ThingSpeak.writeField(channelID, 2,totalSpots , writeAPIKey);
  ThingSpeak.writeField(channelID, 3,currentParkingCount , writeAPIKey);
}

void openGate() {
  gateServo.write(90); // 打开闸门
}

void closeGate() {
  gateServo.write(0); // 关闭闸门
}

void updateLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Total Spots: ");
  lcd.print(totalSpots);
  lcd.setCursor(0, 1);
  lcd.print("Available: ");
  lcd.print(availableSpots);


   // 在串口监视器上打印剩余车位数
  Serial.print("availableSpots: ");
  Serial.println(availableSpots);
}