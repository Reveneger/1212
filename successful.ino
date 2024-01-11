#include <Servo.h>
#include <rgb_lcd.h>
#include <WiFiNINA.h>
#include <ThingSpeak.h>


const int sensorIn = 2;
const int sensorOut = 3;

const int servoPin = 9;

Servo gateServo;

rgb_lcd lcd;


char ssid[] = "1234567890"; 
char pass[] = "1234567890"; 

unsigned long channelID = 2388115; 
const char * writeAPIKey = "L8BXVTEWYPYON8QU"; 


int totalSpots = 110;
int availableSpots = 97;
int currentParkingCount;

bool carAtInSensor = false;
bool carAtOutSensor = false;

WiFiClient client;

void setup() {

    Serial.begin(9600);
  while (!Serial) {
    ; 
  }


  Serial.print("Connecting to ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    delay(2000);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");


  pinMode(sensorIn, INPUT);
  pinMode(sensorOut, INPUT);


  gateServo.attach(servoPin);
  gateServo.write(0);


  lcd.begin(16, 2);
  lcd.setRGB(255, 255, 255); 

  updateLCD();


  ThingSpeak.begin(client);
}

void loop() {
  carAtInSensor = digitalRead(sensorIn);
  carAtOutSensor = digitalRead(sensorOut);

if (carAtInSensor && carAtOutSensor) {
  
    delay(100); 发
  }
  else if (carAtInSensor && availableSpots > 0) {
    
    openGate();
    delay(3000); 
    closeGate();
    availableSpots--;
    updateLCD();
    delay(100); 
  }

  else if (carAtOutSensor && availableSpots < totalSpots) {
    
    openGate();
    delay(3000); 
    closeGate();
    availableSpots++;
    updateLCD();
    delay(100); 
  }
  currentParkingCount = totalSpots - availableSpots;
  ThingSpeak.writeField(channelID, 1,availableSpots , writeAPIKey);
  ThingSpeak.writeField(channelID, 2,totalSpots , writeAPIKey);
  ThingSpeak.writeField(channelID, 3,currentParkingCount , writeAPIKey);
}

void openGate() {
  gateServo.write(90); 
}

void closeGate() {
  gateServo.write(0); 
}

void updateLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Total Spots: ");
  lcd.print(totalSpots);
  lcd.setCursor(0, 1);
  lcd.print("Available: ");
  lcd.print(availableSpots);



  Serial.print("availableSpots: ");
  Serial.println(availableSpots);
}
