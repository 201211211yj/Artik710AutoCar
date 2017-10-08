
#include <Servo.h>  //Servo ��� 

Servo myservo_degree;  //���� ��ȯ���ִ� ����
Servo motor;           //������ ����ϴ� ����ȸ�� ����

enum {LEFT=1, RIGHT=2, FRONT=3, BACK=4, AUTO=5, STOP=6, ENGINEON=8, ENGINEOFF=9}; //�����Ͱ�

#define SERVO_DEFAULT 95 //���� �ٶ󺸴� �������� 
#define ALTER_DIR 20    //�ѹ� �Է½� ���ϴ� ����
#define DELAY_VAR 10    //������ ���ϴ� �ӵ� ������
#define CONTROL_SPEED 50  //�ӵ��������� 

int razer,temp; //������ ���� �� ���ϱ� ���� ������
int data; //Bluetooth, console�κ��� ���� ������
int bt_data;  //Bluetooth�κ��� �޴� ������
int mega_data;  //console�κ��� �޴� ������
int current_servo_pos = SERVO_DEFAULT;     //���� �������� ������
int servo_pos = SERVO_DEFAULT; //���ϴ� �������� ������
int echoPin = 5;  //������ echo ���� number
int trigPin = 6;  //������ trig ���� number
//int cloudambPin = 31; // cloud1 ��� ���� number
//int cloudpolicePin = 33; // cloud2 ��� ���� number
float duration, distance; //������ ���� ������
int degree;   //��ƽ���κ��� ���޹��� ������ ������ ���� ��
int predegree;  //degree �� ������ , ���� �� ���̴� ������
int FromARTIK(); //��ƽ���� ����ó���� �޾ƿ��� �Լ�
float Distance(); //������ ������ ���޹޴� �Լ�
void BT_MEGA_data(); //��������� ���� ������ ������
void DefaultPosition(); //FRONT, BACK ��� �� ���� ������ �������ִ� �Լ�
//void Cloud_Police();  //������ Ŭ�����ȣ ������ ó�����ִ� �Լ�
//void Cloud_Amb();     //������ Ŭ�����ȣ ������ ó�����ִ� �Լ�
int motorspeed;

void setup()  //�ʱⰪ
{
  Serial1.begin(9600);  //Serial1 == Bluetooth ���
  Serial.begin(9600); //�ø��� ��� ����

  for(int i = 53; i>=39;)     //ARTIK���� ����ó������ �޴� �ɼ���
  {
    pinMode(i,INPUT);
    i=i-2;
  }

  pinMode(trigPin, OUTPUT); //������ trig�ɿ��� ����
  pinMode(echoPin, INPUT);  //������ echo�ɿ��� ����
  
  myservo_degree.attach(3);//���� ���� ��
  motor.attach(9);         //���� ���� ���� ��  
  motor.write(90);  //motor stop 
  myservo_degree.write(servo_pos); //���� ����Ʈ ������ ����  
}

//�ܼ�, bluetooth data  <--->  �Ƶ��̳�
//ó���� ��� �������ֱ�. 7�� ���������� , 8�� �������

void loop()
{     
  distance = Distance(); //���� ���� �տ� ��ü���� �Ÿ� �ľ�
  BT_MEGA_data();  //������� �Ǵ� �ø��� ������ �޾ƿ���
  /*
   =========================================================
   �������� ��� 
   =========================================================
  */  
  if(data == ENGINEON) //��������κ��� �޾ƿ� �����Ͱ� �������� ��� while�� ����
  {
    while(true)
    { 
      BT_MEGA_data(); 

      distance = Distance();

      if(data == FRONT ) //while�� ����� ��������κ��� �޾ƿ� �����Ͱ� FRONT�ΰ��
      {
        DefaultPosition(); //�������� ������ ������ �����ϰ� ����� �Լ�
        motor.write(90-CONTROL_SPEED); //�������͸�  ������ ����ŭ�� �ӵ��� ����
      }
      else if(data == STOP )  //STOP == 6  //���� �����Ͱ� STOP�� ���
      {
        motor.write(90); //�����    
      }  
      else if (data == RIGHT) //RIGHT == 2  //���� �����Ͱ� RIGHT�� ���
      {  
        for(servo_pos = current_servo_pos ; servo_pos < current_servo_pos + ALTER_DIR ; servo_pos++) //���簢������ ����������ŭ ���������� ȸ��
        {
          myservo_degree.write(servo_pos); //tell servo to go to position in variable 'pos'
          delay(DELAY_VAR); //waits DELAY_VARms for th e servo to reach the position
        }
      }
      else if(data == LEFT) //���� �����Ͱ� LEFT�� ���
      {      
        for(servo_pos = current_servo_pos ; servo_pos > current_servo_pos - ALTER_DIR ; servo_pos--) //���簢������ ����������ŭ �������� ȸ��
        {
          myservo_degree.write(servo_pos); //tell servo to go to position in variable 'pos'
          delay(DELAY_VAR); //waits DELAY_VARms for the servo to reach the position
        }
      }
      else if (data == BACK) //���� �����Ͱ� BACK
      {
        DefaultPosition();
        motor.write(90+CONTROL_SPEED); //���� ����
      }
     
      if(distance < 10.0) //������ �Ÿ��� ���� �ӵ�����
      { 
        temp=0;   //���� �ѹ��� ƥ ��츦 ����ϴ� ����
        razer=0;
        
        while(true) //10�Ʒ��� �����İ��� ���� ���ѷ����� ����
        { 
          temp++;
          distance = Distance();
          
          if(distance<10.0)
            razer++;
          if(razer == 4)    //5���߿� 4���̻� 10�Ʒ��� �����İ��� ������ Emergency ����ϰ� ����
          {
            Serial.println("9");
            motor.write(90);
            break; 
          } 
          if(temp > 6) 
            break; 
        }   
      }
     
      if(data == AUTO || data == ENGINEOFF) //���� ���� �����Ͱ� AUTO ���������̰ų� ENGINEOFF�� ��� while�� Ż��
        break;
      current_servo_pos = servo_pos;  //���ư� ��ŭ�� ���������� ���� �������� ������ ����
      data=0;     //DATA���� �ߺ����� �ԷµǴ� ��츦 �����ϱ����� 0���� ����
    }
  }

  /*
   =========================================================
   �������� ��� 
   =========================================================
  */ 
    
  if(data==AUTO) //�������� ���
  {
    while(true)
    {              
      BT_MEGA_data();
     // Cloud_Amb();     //������ Ŭ���� �Լ� ȣ��
     // Cloud_Police();  //������ Ŭ���� �Լ� ȣ��
      degree = FromARTIK() - 90; //��ƽ���� ����ó���� �����͸� ������ return�� �������� ������ ���̰����� �ٲٱ����� -90�� ����

      
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
      myservo_degree.write(SERVO_DEFAULT-degree);   //���鰢���� ������ ����� degree���� ���� ������ �־���
      distance = Distance();  // distance �� �޾ƿ���

      if(distance < 10.0) //������ �Ÿ����� �� ���豸��
      { 
        temp=0;
        razer=0;
        
        while(true) //�ϵ���� �� ���� ���� �� error �����͸� ó�����ֱ� ���� ��Ǭ��.
        { 
          temp++; 
          distance = Distance();
          
          if(distance<10.0)
            razer++;
          if(razer == 4)    //Emergency ����ϰ� ����
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
        motor.write(motorspeed);  //�����Ÿ��� �������̸� ������ �ӵ��� ����
          
      if(data != AUTO)
      {
        motor.write(90); //�ӵ� ����� ����
        current_servo_pos = SERVO_DEFAULT; //���� ���� ������ ����Ʈ������ ����
        myservo_degree.write(current_servo_pos); //���簢���� �������Ϳ� ���� 
        data=ENGINEON; //������ ���� ENGINEON���� ����
        break; //AUTO ���Ϲ� Ż��
      }
    }
  }
} //void loop finish
/*
void Cloud_Amb()
{
  int clouddata_amb;
  clouddata_amb = digitalRead(cloudambPin); // Ŭ���� ���� �� �� read

  if(clouddata_amb == HIGH)  //���� ���� �ִٸ� STOP
  {
   delay(5000);
   data = 6;
   motor.write(90);
  }
}

void Cloud_Police()
{
  int clouddata_police;
  clouddata_police = digitalRead(cloudpolicePin); //Ŭ���� ���� ��ư� �� read

  if(clouddata_police == HIGH) // ���� ���� �ִٸ� ���������忡�� ����������� ����
    data = 8;
}
*/
float Distance()                          //�����İ� �޾Ƽ� cm�� ����ؼ� �����ϴ� �Լ�
{
  digitalWrite(trigPin, HIGH);
  delay(10);
  digitalWrite(trigPin, LOW);   //echoPin �� HIGH�� ������ �ð��� ���� �Ѵ�.
  duration = pulseIn(echoPin, HIGH); 
  //HIGH ���� �� �ð�(�����İ� ���´ٰ� �ٽ� ���� �ð�)�� ������ �Ÿ��� ��� �Ѵ�.
  distance = ((float)(340 * duration) / 10000) / 2;
  return distance;  
}

void BT_MEGA_data()           //�������or�ø��󰪹޴� �Լ�
{
  if(Serial1.available())   //������� ��Ű��� �����Ѵٸ�
  {
    bt_data= Serial1.read()-48; //Serial1.read() ������ -48
    data= bt_data;
    Serial.println(bt_data);    //Console�� ���
  }  
  else if (Serial.available())   //�ܼ� ��Ű��� �����Ѵٸ�
  {
    mega_data = Serial.read()-48;
    data = mega_data;
    Serial.println(mega_data);
  }
}

void DefaultPosition()                //������������ Default�� ���ִ� �Լ�
{
  if (current_servo_pos > SERVO_DEFAULT) //SERVO_DEFAULT ���� ũ�ٴ°� ��ȸ��
  {
    for(servo_pos = current_servo_pos; servo_pos != SERVO_DEFAULT; servo_pos--) //SERVO_DEFAULT�� �����ش�
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

int FromARTIK()               //ARTIK���� ����ó�����޾Ƽ� ���� �������ִ� �Լ�
{
  int decimal = 0;
  int bin = 128;
  
  for(int i = 53; i >= 39;)   //������ �������� �ɿ��� ������ �о��
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