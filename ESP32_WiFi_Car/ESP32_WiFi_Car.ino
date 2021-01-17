#include "WiFi.h"
#include "SPIFFS.h"
#include "ESPAsyncWebServer.h"

#define motorEnableA 35
#define motorEnableB 34

#define motorInput1 27
#define motorInput2 14
#define motorInput3 12
#define motorInput4 13

const int freq = 30000;
const int pwmChannelLeft = 0;
const int pwmChannelRight = 1;
const int resolution = 8;
int dutyCycle = 200;

const char* ssid = "SuperSecret";
const char* password = "SuperSecret";

AsyncWebServer server(80);


/*
INDIVIDUAL WHEEL MOVE FUNCTIONS
*/
void moveLeftWheelForward(int speed) {
  digitalWrite(motorInput1, LOW);
  digitalWrite(motorInput2, HIGH);
  ledcWrite(pwmChannelLeft, speed);
}

void moveRightWheelForward(int speed) {
  digitalWrite(motorInput3, LOW);
  digitalWrite(motorInput4, HIGH);
  ledcWrite(pwmChannelRight, speed);
}

void moveLeftWheelBack(int speed) {
  digitalWrite(motorInput1, HIGH);
  digitalWrite(motorInput2, LOW);
  ledcWrite(pwmChannelLeft, speed);
}

void moveRightWheelBack(int speed) {
  digitalWrite(motorInput3, HIGH);
  digitalWrite(motorInput4, LOW);
  ledcWrite(pwmChannelRight, speed);
}

/*
STOP FUNCTIONS
*/
void stopLeft() {
  digitalWrite(motorInput1, LOW);
  digitalWrite(motorInput2, LOW);
}

void stopRight() {
  digitalWrite(motorInput3, LOW);
  digitalWrite(motorInput4, LOW);
}

void stop() {
  stopLeft();
  stopRight();
}

void setup() {
  Serial.begin(115200);
  delay(10000);   
  
  if(!SPIFFS.begin()){
     Serial.println("An Error has occurred while mounting SPIFFS");
     return;
  }

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/joystick.html", "text/html");
  });

  server.on("/vjoy.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/virtualjoystick.js", "text/javascript");
  });
  //server.on("/jsData.html", handleJSData);

  server.on("/jsData.html", HTTP_GET, [](AsyncWebServerRequest *request){
    
    int paramsNr = request->params();
    AsyncWebParameter* x_coord = request->getParam(0);
    AsyncWebParameter* y_coord = request->getParam(1);
    
    int x = x_coord->value().toInt() * 10;
    int y = y_coord->value().toInt() * 10;
    
    int leftSpeed = abs(y);
    int rightSpeed = abs(y);
    //set the direction based on y being negative or positive 
    //adjust to speed of each each motor depending on the x-axis
    //it slows down one motor and speeds up the other proportionately 
    //based on the amount of turning
    leftSpeed = constrain(leftSpeed + x/2, 0, 255);
    rightSpeed = constrain(rightSpeed - x/2, 0, 255);
  
    //use the speed and direction values to turn the motors
    //if either motor is going in reverse from what is expected,
    //just change the 2 digitalWrite lines for both motors:
    //!ydir would become ydir, and ydir would become !ydir
  
    if (leftSpeed < 20) {
      stopLeft();
    }
    else if (y < 0) {
      moveLeftWheelBack(leftSpeed);
    }
    else if (y > 0) {
      moveLeftWheelForward(leftSpeed);
    }
    if (rightSpeed < 20) {
      stopRight();
    }
    else if (y < 0) {
      moveRightWheelBack(rightSpeed);
    }
    else if (y > 0) {
      moveRightWheelForward(rightSpeed);
    }
    
    //return an HTTP 200
    request->send(200, "text/plain", "");   

  });
  
  server.begin();

  // Arduino output pins
  pinMode(motorEnableA, OUTPUT);
  pinMode(motorEnableB, OUTPUT);
  pinMode(motorInput1, OUTPUT);
  pinMode(motorInput2, OUTPUT);
  pinMode(motorInput3, OUTPUT);
  pinMode(motorInput4, OUTPUT);

  ledcSetup(pwmChannelLeft, freq, resolution);
  ledcAttachPin(motorEnableA, pwmChannelLeft);
  ledcSetup(pwmChannelRight, freq, resolution);
  ledcAttachPin(motorEnableB, pwmChannelRight);
  
  Serial.print("Setup is done now");
}

void loop() {
  // put your main code here, to run repeatedly:
  //moveLeftWheelForward(150);
  //moveRightWheelForward(150);
  //moveRightWheelBack(150);
  //moveLeftWheelBack(150);
  //delay(2000);
  //stopLeft();
  delay(10000);
}
