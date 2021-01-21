#define motorEnableA 35
#define motorEnableB 34

#define motorInput1 27
#define motorInput2 14
#define motorInput3 12
#define motorInput4 13

class car_motors {

  private:
    int freq = 9000;
    int pwmChannelLeft = 0;
    int pwmChannelRight = 1;
    int resolution = 12;

    // True = forwards, False = backwards
    bool forwardDir = true;
    
  public:

    void setupCarMotors() {
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
    }

    void updateCarMotors(int x, int y, int sw) {
      // x, y each range from 0 to 4095
      if (sw == 0) {
        // toggle direction
        forwardDir = !forwardDir;
      }
      
      int leftSpeed = y;
      int rightSpeed = x;
    
      //use the speed and direction values to turn the motors  
      if (leftSpeed < 30) {
        stopLeft();
      }
      else if (!forwardDir) {
        moveLeftWheelBack(leftSpeed);
      }
      else if (forwardDir) {
        moveLeftWheelForward(leftSpeed);
      }
      if (rightSpeed < 30) {
        stopRight();
      }
      else if (!forwardDir) {
        moveRightWheelBack(rightSpeed);
      }
      else if (forwardDir) {
        moveRightWheelForward(rightSpeed);
      }   
      
    }

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

};
car_motors CM;
