
#include <Servo.h>  //Servo 헤더 

Servo myservo_degree;  //각도 변환해주는 서보
Servo motor;           //엔진을 담당하는 무한회전 서보

enum {LEFT=1, RIGHT=2, FRONT=3, BACK=4, AUTO=5, STOP=6, ENGINEON=8, ENGINEOFF=9}; //데이터값

#define SERVO_DEFAULT 95 //정면 바라보는 서보각도 
#define ALTER_DIR 20    //한번 입력시 변하는 각도
#define DELAY_VAR 10    //각도가 변하는 속도 딜레이
#define CONTROL_SPEED 50  //속도조절위한 

int razer,temp; //초음파 센서 값 비교하기 위한 데이터
int data; //Bluetooth, console로부터 받은 데이터
int bt_data;  //Bluetooth로부터 받는 데이터
int mega_data;  //console로부터 받는 데이터
int current_servo_pos = SERVO_DEFAULT;     //현재 서보모터 각도값
int servo_pos = SERVO_DEFAULT; //변하는 서보모터 각도값
int echoPin = 5;  //초음파 echo 센서 number
int trigPin = 6;  //초음파 trig 센서 number
//int cloudambPin = 31; // cloud1 통신 센서 number
//int cloudpolicePin = 33; // cloud2 통신 센서 number
float duration, distance; //초음파 센서 데이터
int degree;   //아틱으로부터 전달받은 차선과 차량의 각도 값
int predegree;  //degree 의 이전값 , 비교할 때 쓰이는 데이터
int FromARTIK(); //아틱에서 영상처리값 받아오는 함수
float Distance(); //초음파 센서값 전달받는 함수
void BT_MEGA_data(); //블루투스를 통해 얻어오는 데이터
void DefaultPosition(); //FRONT, BACK 기능 시 서보 각도를 변경해주는 함수
//void Cloud_Police();  //경찰차 클라우드신호 있을때 처리해주는 함수
//void Cloud_Amb();     //구급차 클라우드신호 있을때 처리해주는 함수
int motorspeed;

void setup()  //초기값
{
  Serial1.begin(9600);  //Serial1 == Bluetooth 통신
  Serial.begin(9600); //시리얼 통신 시작

  for(int i = 53; i>=39;)     //ARTIK에서 영상처리값을 받는 핀설정
  {
    pinMode(i,INPUT);
    i=i-2;
  }

  pinMode(trigPin, OUTPUT); //초음파 trig핀에서 전달
  pinMode(echoPin, INPUT);  //초음파 echo핀에서 수신
  
  myservo_degree.attach(3);//서보 모터 핀
  motor.attach(9);         //엔진 서보 모터 핀  
  motor.write(90);  //motor stop 
  myservo_degree.write(servo_pos); //서보 디폴트 값으로 세팅  
}

//콘솔, bluetooth data  <--->  아두이노
//처음에 모드 지정해주기. 7번 자율주행모드 , 8번 수동모드

void loop()
{     
  distance = Distance(); //현재 차량 앞에 물체와의 거리 파악
  BT_MEGA_data();  //블루투스 또는 시리얼 데이터 받아오기
  /*
   =========================================================
   수동주행 모드 
   =========================================================
  */  
  if(data == ENGINEON) //블루투스로부터 받아온 데이터가 엔진온일 경우 while문 수행
  {
    while(true)
    { 
      BT_MEGA_data(); 

      distance = Distance();

      if(data == FRONT ) //while문 수행시 블루투스로부터 받아온 데이터가 FRONT인경우
      {
        DefaultPosition(); //서보모터 각도를 직선과 평행하게 만드는 함수
        motor.write(90-CONTROL_SPEED); //바퀴모터를  설정한 값만큼의 속도로 직진
      }
      else if(data == STOP )  //STOP == 6  //만약 데이터가 STOP인 경우
      {
        motor.write(90); //멈춘다    
      }  
      else if (data == RIGHT) //RIGHT == 2  //만약 데이터가 RIGHT인 경우
      {  
        for(servo_pos = current_servo_pos ; servo_pos < current_servo_pos + ALTER_DIR ; servo_pos++) //현재각도에서 각도범위만큼 오른쪽으로 회전
        {
          myservo_degree.write(servo_pos); //tell servo to go to position in variable 'pos'
          delay(DELAY_VAR); //waits DELAY_VARms for th e servo to reach the position
        }
      }
      else if(data == LEFT) //만약 데이터가 LEFT인 경우
      {      
        for(servo_pos = current_servo_pos ; servo_pos > current_servo_pos - ALTER_DIR ; servo_pos--) //현재각도에서 각도범위만큼 왼쪽으로 회전
        {
          myservo_degree.write(servo_pos); //tell servo to go to position in variable 'pos'
          delay(DELAY_VAR); //waits DELAY_VARms for the servo to reach the position
        }
      }
      else if (data == BACK) //만약 데이터가 BACK
      {
        DefaultPosition();
        motor.write(90+CONTROL_SPEED); //모터 후진
      }
     
      if(distance < 10.0) //초음파 거리에 의해 속도제어
      { 
        temp=0;   //값이 한번씩 튈 경우를 대비하는 변수
        razer=0;
        
        while(true) //10아래의 초음파값이 들어와 무한루프에 들어옴
        { 
          temp++;
          distance = Distance();
          
          if(distance<10.0)
            razer++;
          if(razer == 4)    //5번중에 4번이상 10아래의 초음파값이 들어오면 Emergency 출력하고 멈춤
          {
            Serial.println("9");
            motor.write(90);
            break; 
          } 
          if(temp > 6) 
            break; 
        }   
      }
     
      if(data == AUTO || data == ENGINEOFF) //만약 들어온 데이터가 AUTO 자율주행이거나 ENGINEOFF의 경우 while을 탈출
        break;
      current_servo_pos = servo_pos;  //돌아간 만큼의 서보각도를 현내 서보각도 값으로 저장
      data=0;     //DATA값이 중복으로 입력되는 경우를 방지하기위해 0으로 설정
    }
  }

  /*
   =========================================================
   자율주행 모드 
   =========================================================
  */ 
    
  if(data==AUTO) //자율주행 모드
  {
    while(true)
    {              
      BT_MEGA_data();
     // Cloud_Amb();     //응급차 클라우드 함수 호출
     // Cloud_Police();  //경찰차 클라우드 함수 호출
      degree = FromARTIK() - 90; //아틱에서 영상처리된 데이터를 각도로 return후 서보모터 값과의 차이값으로 바꾸기위해 -90을 해줌

      
      if(abs(degree)<21)
        motorspeed = 51+(abs(degree)/2);
      else
        motorspeed = 62;
        
        /*
      if(abs(degree)>75)
        motorspeed = 62;
      else
        motorspeed = 55;
       */
      myservo_degree.write(SERVO_DEFAULT-degree);   //정면각도에 위에서 계산한 degree값을 빼서 서보에 넣어줌
      distance = Distance();  // distance 값 받아오기

      if(distance < 10.0) //초음파 거리측정 시 위험구간
      { 
        temp=0;
        razer=0;
        
        while(true) //하드웨어 핀 정보 누락 및 error 데이터를 처리해주기 위해 다푼다.
        { 
          temp++; 
          distance = Distance();
          
          if(distance<10.0)
            razer++;
          if(razer == 4)    //Emergency 출력하고 멈춤
          {
            Serial1.println("9");
            motor.write(90);
            break;
          } 
          if(temp > 6)
            break;        
        }      
      }
      else
        motor.write(motorspeed);  //안전거리를 유지중이면 지정한 속도로 직진
          
      if(data != AUTO)
      {
        motor.write(90); //속도 제어로 정지
        current_servo_pos = SERVO_DEFAULT; //현재 서보 각도를 디폴트값으로 설정
        myservo_degree.write(current_servo_pos); //현재각도를 서보모터에 전달 
        data=ENGINEON; //데이터 값을 ENGINEON으로 변경
        break; //AUTO 와일문 탈출
      }
    }
  }
} //void loop finish
/*
void Cloud_Amb()
{
  int clouddata_amb;
  clouddata_amb = digitalRead(cloudambPin); // 클라우드 핀의 들어간 값 read

  if(clouddata_amb == HIGH)  //만약 값이 있다면 STOP
  {
   delay(5000);
   data = 6;
   motor.write(90);
  }
}

void Cloud_Police()
{
  int clouddata_police;
  clouddata_police = digitalRead(cloudpolicePin); //클라우드 핀의 들아간 값 read

  if(clouddata_police == HIGH) // 만약 값이 있다면 자율주행모드에서 수동주행모드로 변경
    data = 8;
}
*/
float Distance()                          //초음파값 받아서 cm로 계산해서 리턴하는 함수
{
  digitalWrite(trigPin, HIGH);
  delay(10);
  digitalWrite(trigPin, LOW);   //echoPin 이 HIGH를 유지한 시간을 저장 한다.
  duration = pulseIn(echoPin, HIGH); 
  //HIGH 였을 때 시간(초음파가 보냈다가 다시 들어온 시간)을 가지고 거리를 계산 한다.
  distance = ((float)(340 * duration) / 10000) / 2;
  return distance;  
}

void BT_MEGA_data()           //블루투스or시리얼값받는 함수
{
  if(Serial1.available())   //블루투스 통신값이 존재한다면
  {
    bt_data= Serial1.read()-48; //Serial1.read() 읽은값 -48
    data= bt_data;
    Serial.println(bt_data);    //Console에 출력
  }  
  else if (Serial.available())   //콘솔 통신값이 존재한다면
  {
    mega_data = Serial.read()-48;
    data = mega_data;
    Serial.println(mega_data);
  }
}

void DefaultPosition()                //바퀴정면으로 Default로 해주는 함수
{
  if (current_servo_pos > SERVO_DEFAULT) //SERVO_DEFAULT 보다 크다는게 좌회전
  {
    for(servo_pos = current_servo_pos; servo_pos != SERVO_DEFAULT; servo_pos--) //SERVO_DEFAULT로 맞춰준다
    {
       myservo_degree.write(servo_pos);              
       delay(DELAY_VAR);                         
    }
  }
  else
  {
    for(servo_pos = current_servo_pos; servo_pos != SERVO_DEFAULT; servo_pos++) 
    {
       myservo_degree.write(servo_pos);                 
       delay(DELAY_VAR);                         
    }
  }
}

int FromARTIK()               //ARTIK에서 영상처리값받아서 각도 리턴해주는 함수
{
  int decimal = 0;
  int bin = 128;
  
  for(int i = 53; i >= 39;)   //위에서 지정해준 핀에서 데이터 읽어옴
  {
    int num = digitalRead(i);
    decimal += num * bin;
    bin/=2;
    i=i-2;
  } 
  
  Serial1.print("degree");
  Serial1.println(decimal);
  
  return decimal;
}