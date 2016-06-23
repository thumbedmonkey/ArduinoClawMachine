#include "MeOrion.h"
MeStepper stepperX(PORT_1);
MeStepper stepperY(PORT_2);
MeDCMotor dc(M1);
MeDCMotor dcClaw(M2);
int x_min_pin = mePort[PORT_3].s1;
int x_max_pin = mePort[PORT_3].s2;
int y_min_pin = mePort[PORT_6].s1;
int y_max_pin = mePort[PORT_4].s2;
int motor_pin = mePort[PORT_6].s2;
int motion_x_pin = mePort[PORT_7].s1;
int motion_y_pin = mePort[PORT_7].s2;
int motor_x_pin = mePort[PORT_8].s1;
int motor_y_pin = mePort[PORT_8].s2;
int x_min=0,x_max=0,y_min=0,y_max=0;
int motor_limit = 0;
void setup(){
  Serial.begin(115200);
  stepperX.setMaxSpeed(80000);
  stepperY.setMaxSpeed(80000);
  stepperX.setAcceleration(15000);
  stepperY.setAcceleration(15000);
  pinMode(x_min_pin,INPUT_PULLUP);
  pinMode(x_max_pin,INPUT_PULLUP);
  pinMode(y_min_pin,INPUT_PULLUP);
  pinMode(y_max_pin,INPUT_PULLUP);
  pinMode(motion_x_pin,INPUT);
  pinMode(motion_y_pin,INPUT);
  pinMode(motor_pin,INPUT_PULLUP);
}
void loop(){
  x_min = digitalRead(x_min_pin);
  x_max = digitalRead(x_max_pin);
  y_min = digitalRead(y_min_pin);
  y_max = digitalRead(y_max_pin);
  motor_limit = digitalRead(motor_pin);
  //Serial.println(motor_limit<<4|x_min<<3|x_max<<2|y_min<<1|y_max,BIN);
  //int motionX = floor((analogRead(motion_x_pin)-400)/2)*2;
  //int motionY = floor((analogRead(motion_y_pin)-500)/2)*2;
  int m = (analogRead(motion_y_pin));
  int motionX=0,motionY=0;
  if(m<10){
    if(x_max==1){
      motionX = 100;
      motionY = -100;
    }
  }else if(m<500){
    if(y_min==1){
      motionX = 100;
      motionY = 100;
    }
  }else if(m<700){
    if(y_max==1){
      motionX = -100;
      motionY = 100;
    }
  }else if(m<800){
    if(x_min==1){
      motionX = -100;
      motionY = -100;
    }
  }
  m = (analogRead(motor_y_pin));
  Serial.println(m);
  if(m<10){
    if(motor_limit==1){
      dc.run(100);
    }else{
      dc.run(-100);
      delay(500);
      dc.run(0);
      delay(100);
    }
    dcClaw.run(0);
  }else if(m>400&&m<500){
    dcClaw.run(200);
      dc.run(0);
  }else if(m>500&&m<700){
      
    if(motor_limit==1){
      dc.run(-100);
    }else{
      dc.run(100);
      delay(500);
      dc.run(0);
      delay(100);
    }
    dcClaw.run(0);
  }else if(m>700&&m<800){
    dcClaw.run(-200);
      dc.run(0);
  }else{
      dc.run(0);
      dcClaw.run(0);
  }
  //motionX = abs(motionX)<100?0:motionX;
  //motionY = abs(motionY)<100?0:motionY;
  stepperX.setSpeed(motionX*500);
  stepperY.setSpeed(motionY*500);
  stepperX.runSpeed();
  stepperY.runSpeed();
}

