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
String buffers = "";
char *str;
String s="";
int clawStatus = 0;
long clawTime = 0;
long lastTime=0;
int leapmotionX,leapmotionY,leapmotionZ,leapmotionW;
int i,mode;
long zTime = 0;
long zTarget = 0;
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
  while(true){
    motor_limit = digitalRead(motor_pin);
    dc.run(50);
    if(motor_limit==0){
      dc.run(-50);
      delay(500);
      dc.run(0);
      break;
    }
  }
}
      
void loop(){
  
  if(micros()-lastTime>400000){
    lastTime = micros();
    leapmotionX = 0;
    leapmotionY = 0;
    leapmotionZ = 0;
    leapmotionW = 100;
    mode = 0;
  }
  int motionX=0,motionY=0;
  if(Serial.available()){
    char c = Serial.read();
    if(c=='\n'){
      //x=10&y=10&z=10
      
      lastTime = micros();
      char *p = (char*)buffers.c_str();
      i = 0;
      while ((str = strtok_r(p, "&", &p)) != NULL) {
        s=str;
        float v = s.substring(2,s.length()).toFloat();
        if(i==0){
          leapmotionX = v/5;
        }else if(i==1){
          leapmotionY = v/5;
        }else if(i==2){
          leapmotionZ = v*20;
        }else if(i==3){
          leapmotionW = v;
        }
        i++;
      }
      Serial.print(leapmotionX);
      Serial.print(":");
      Serial.print(leapmotionY);
      Serial.print(":");
      Serial.print(leapmotionZ);
      Serial.print(":");
      Serial.println(leapmotionW);
      mode= 1;
      buffers = "";
    }else{
      buffers+=c;
    }
  }
  x_min = digitalRead(x_min_pin);
  x_max = digitalRead(x_max_pin);
  y_min = digitalRead(y_min_pin);
  y_max = digitalRead(y_max_pin);
  motor_limit = digitalRead(motor_pin);
  if(mode==0){
    //Serial.println(motor_limit<<4|x_min<<3|x_max<<2|y_min<<1|y_max,BIN);
    //int motionX = floor((analogRead(motion_x_pin)-400)/2)*2;
    //int motionY = floor((analogRead(motion_y_pin)-500)/2)*2;
    int m = (analogRead(motion_y_pin));
    if(m<10){
      if(x_max==1){
        motionX = -100;
        motionY = 100;
      }
    }else if(m<500){
      if(y_min==1){
        motionX = -100;
        motionY = -100;
      }
    }else if(m<700){
      if(y_max==1){
        motionX = 100;
        motionY = -100;
      }
    }else if(m<800){
      if(x_min==1){
        motionX = 100;
        motionY = 100;
      }
    }
    m = (analogRead(motor_y_pin));
    //Serial.println(m);
    if(m<10){
      if(motor_limit==1){
        zTarget -= 1;
        if(zTarget<0){
          zTarget = 0;
        }
      }else{
        dc.run(-100);
        delay(500);
        dc.run(0);
        delay(100);
        zTime = 0;
        zTarget = 0;
      }
      dcClaw.run(0);
    }else if(m>400&&m<500){
      dcClaw.run(200);
      dc.run(0);
    }else if(m>500&&m<700){
      if(motor_limit==1){
        zTarget += 1;
        if(zTarget>6000){
          zTarget = 6000;
        }
      }else{
        dc.run(100);
        delay(500);
        dc.run(0);
        delay(100);
        zTime = 0;
        zTarget = 0;
      }
      dcClaw.run(0);
    }else if(m>700&&m<800){
      dcClaw.run(-200);
        dc.run(0);
    }else{
        dc.run(0);
        dcClaw.run(0);
    }
    if(zTarget>zTime){
        dc.run(-100);
        zTime += 1;
    }else if(zTarget<zTime){
        dc.run(100);
        zTime -= 1;
    }else{ 
      dc.run(0);
    }
  }else{
    if(abs(leapmotionX)<10){
      leapmotionX = 0;
    }
    if(abs(leapmotionY)<10){
      leapmotionY = 0;
    }
    if(leapmotionY<0&&x_max==0){
      leapmotionY = 0;
    }
    if(leapmotionY>0&&y_max==0){
      leapmotionY = 0;
    }
    if(leapmotionX<0&&x_min==0){
      leapmotionX = 0;
    }
    if(leapmotionX>0&&y_min==0){
      leapmotionX = 0;
    }
    motionX = leapmotionY-leapmotionX;
    motionY = -leapmotionY-leapmotionX;
    zTarget = min(6000,max(0,6000-leapmotionZ));
    if(zTarget>zTime){
        dc.run(-100);
        zTime += 1;
    }else if(zTarget<zTime){
        dc.run(100);
        zTime -= 1;
    }else{ 
      dc.run(0);
    }
    if(motor_limit==0){
      dc.run(-100);
      delay(500);
      dc.run(0);
      delay(100);
      zTime = 0;
      zTarget = 0;
    }
    if(leapmotionW>90&&clawStatus!=1){
      dcClaw.run(255);
      clawStatus = 1;
      clawTime = micros();
    }else if(leapmotionW<60&&clawStatus!=-1){
      dcClaw.run(-180);
      clawStatus = -1;
      clawTime = micros();
    }else{
      if(micros()-clawTime>1200000){
        dcClaw.run(0);
      }
    }
  }
  //motionX = abs(motionX)<100?0:motionX;
  //motionY = abs(motionY)<100?0:motionY;
  Serial.println(zTime);
  stepperX.setSpeed(motionX*100);
  stepperY.setSpeed(motionY*100);
  stepperX.runSpeed();
  stepperY.runSpeed();
}

