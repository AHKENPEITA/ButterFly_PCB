//连线指导：左电机6；右电机7，红外11；

#include <Servo.h>
#include <IRremote.h>  //调用红外遥控对应的库
int RECV_PIN = 2;     //定义红外接口引脚
int pbIn = 0;
int time1 = 0, time2;
unsigned long current_time = 0;
unsigned long parameter_time = 0;

int amplitudeL = 45;//扇翅膀时左翼的最大整幅
int amplitudeR = 45;//扇翅膀时右翼的最大整幅
int pitch_amp = 20;

bool turnL = false;//左转弯时增加右翼振幅的flag
bool turnR = false;//右转弯时增加左翼振幅的flag


Servo myServoL;
Servo myServoR;
IRrecv irrecv(RECV_PIN);  //创建一个红外线接收对象irrecv
int servoPinL = 9;         //设置控制左舵机信号的输出引脚
int servoPinR = 10;        //设置控制右舵机信号的输出引脚
decode_results results;   //声明一个IRremote库函数独有的变量类型(decode_results)，results存放红外解码结果
int N = 72;
int stage = 0;
bool onoff = false;
int pitch = 0;
int yaw = 0;
double leftC = 0;
double rightC = 0;
bool ifReceive = false;
void stateChange();
double miniAngle[72]={};
double normalAngle[72]= {};
double largeAngle[72] = {};
double leftAngle;
double rightAngle;

void stateChange(){
  time2 = millis();
  if(time2 -time1>=90){
    time1=time2;
  }
  else time1=time2;
  ifReceive = irrecv.decode(&results);
  if (ifReceive){
    ir();
  }
}

void setup() {
  Serial.begin(9600);            //设置串口波特率
  myServoL.attach(servoPinL);
  myServoR.attach(servoPinR);
  attachInterrupt(pbIn,stateChange,CHANGE);
  myServoL.write(leftC);
  myServoR.write(rightC);
  Serial.println("Begin");
  Serial.println("Begin1");
  Serial.println("Begin2");
  for (int i=0;i<N;i++){
    miniAngle[i] = 45+sin(i/(double)N*2*PI)*30;
    normalAngle[i] = 45+sin(i/(double)N*2*PI)*45;
    largeAngle[i] = 45+sin(i/(double)N*2*PI)*60;
  }
  irrecv.enableIRIn();           // Start the receiver启动红外解码
  set_zero();
}
void ir(){
    if (results.value == 0xFFA25D) {//开机信号
      Serial.println(results.value, HEX);
      if (onoff == false) {
        Serial.println("On");
        onoff = true;
        current_time = millis();  //获取当下时间，并转为一周期内的等效值
      } else {
        Serial.println("StillOn");
      }
    } else if (results.value == 0xFFE21D) {//关机信号
      if (onoff == true) {
        Serial.println("Off");
        onoff = false;
        set_zero();
      } else {
        Serial.println("StillOff");
      }
    }
    if (results.value == 0xFF18E7){//俯仰+
      if (pitch<1){
        pitch+=1;
        set_pitch();
      }
    }else if (results.value == 0xFF4AB5){//俯仰-
      if (pitch>-1){
        pitch-=1;
        set_pitch();
      }
    }
    if (results.value == 0xFF10EF) {//偏航+
      if (yaw<1){
        yaw+=1;
        //set_yaw();
      }
    } else if (results.value == 0xFF5AA5) {//偏航-
      if (yaw>-1){
        yaw-=1;
        //set_yaw();
      }
    } 
    irrecv.resume();  // 等待接收下一组信号
}
void set_pitch () {
  switch(pitch){
    case -1:
      Serial.println("Down");
      leftC = pitch_amp;
      rightC = -pitch_amp;
      break;
    case 0:
      Serial.println("Float");
      leftC = 0;
      rightC = 0;
      break;
    case 1:
      Serial.println("Up");
      leftC = -pitch_amp;
      rightC = pitch_amp;
      break;
  }
}
void set_yaw(){
  switch(yaw){
    case -1:
      Serial.println("Left");
      //setLeftAngleArray(miniAngle);
      break;
    case 0:
      Serial.println("Straight");
      //setLeftAngleArray(normalAngle);
      //setRightAngleArray(normalAngle);
      break;
    case 1:
      Serial.println("Right");
      //setRightAngleArray(miniAngle);
      break;
  }
}


void set_zero(){
  myServoL.write(45);
  myServoR.write(45);
}


void loop() {
  if (onoff == true) {
    stage = (stage+1)%N;
    int stageL = stage;
    int stageR = (stage+(N/2))%N;
    switch (yaw){
      case -1:
        leftAngle = miniAngle[stageL]+leftC;
        rightAngle = largeAngle[stageR]+rightC;
        break;
      case 0:
        leftAngle = normalAngle[stageL]+leftC;
        rightAngle = normalAngle[stageR]+rightC;
        break;
      case 1:
        leftAngle = largeAngle[stageL]+leftC;
        rightAngle = miniAngle[stageR]+rightC;
        break;
    }
    
    myServoL.write(leftAngle);
    myServoR.write(rightAngle);
    delay(6);
  }
}