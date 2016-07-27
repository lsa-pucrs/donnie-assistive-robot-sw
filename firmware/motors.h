#ifndef motors_h
#define motors_h

#include <Arduino.h>
#include <Wire.h>



//?to daniel: tem algum motivo de nao comecar tudo setado em zero ??
int motorsDefaultSpeed = 195;
int motorsDefaultAngularSpeed = 60;
int motorRActive=0;
int motorRspeed=motorsDefaultSpeed;//setado a 195
int motorRTurn=0;
int motorLActive=0;
int motorLspeed=motorsDefaultSpeed;//setado a 195
int motorLTurn=0;






class Motors
{
  private:
    int defaultSpeed;//= 195;
    int speedMax;//= 200;
    int SpeedMin;//= 190;
    volatile int16_t PIDSpeedL;
    volatile int16_t PIDSpeedR;
    double tempo;//=0;
    double controletempo;//=0;
    int erro;
    int D;
    int I;                                      
    int ultimoL;//=0
    int ultimoR;//=0
    int8_t offsetR;
		int8_t offsetL;
		float kP;
		float kI; // nao utilizado
		float kD; // nao utilizado
    //na declaração da classe:
    int i;// -> mudar de nome, i é mais genérico que a vida
    float setpoint; // valor de ticks que a gente quer controlar o PID. o "padrão", o nosso 100%
    int iterations;
    int number_of_cicles_wanted;
	  float PR;
	  float PL;
    /*
    iterations = número de vezes "ciclos" que ele vai testar 
    number_of_cicles_wanted = tipo quantos ciclos que o PID vai atualizar..achoq ue a gente vai tirar isso mas so pra organizar a idéia.
    No caso, a gente vai definir quantas vezes a gente quer realizar o teste. digamos que vai ser 1000x ciclos, então, 1000 iterações. Mas a gente vai executar o PID de 10 em 10 ciclos,
    logo o numero de ciclos que a gente quer é 10.  O wanted ticks vai calcular a média das duas rodas (pra ter um valor aproximado de quanto a gente quer), dividido pela divisão de iterações/numerodeciclosdesejados.
    Exemplo:
    Executamos 1000x e queremos de 10 em 10. os ticks deram 432 e 425.
    Vai ser a média desses dois, dividido por 100. 
    */


  public:

    
    int MOTOR_R;
    int MOTOR_L;
    volatile int counterL;//=0
    volatile int counterR;//=0
    
    void init_Motors(int defSpeed, int newSpeedMax, int newSpeedMin);
    void setSpeed(int SpeedL, int SpeedR, int newSpeedMax, int newSpeedMin);
    void turn_right(int PIDSpeedL, int PIDSpeedR);
    void turn_left(int PIDSpeedL, int PIDSpeedR);
    void move_backward(int PIDSpeedL, int PIDSpeedR);
    void move_forward(int SpeedLeft, int SpeedRight);
    void stop();
    void moveRightWheel(int motor, int speed, int direction);
    void moveLeftWheel(int motor, int speed, int direction);
    void control_movement();
    int16_t getSpeedR();
    int16_t getSpeedL();
		void calibrate_motors(int Speed);
		void setKP(float kP);
		void setKI(float kI);
		void setKD(float kD);
		void setSetpoint(float setpoint);
		void setOffsetR(int8_t offsetR);
		void setOffsetL(int8_t offsetR);

};
/*
A method to initialize the motor control variables. The parameters are:

7- defSpeed : a default speed to the wheels
14- SPEEDHIGH : how much the speed can vary upwards.
15- SPEEDLOW : how much the speed can vary downwards.
*/
void Motors::init_Motors(int defSpeed, int newSpeedMax, int newSpeedMin)
{

  MOTOR_R=1;
  MOTOR_L=1;
  counterL = 0;
  counterR = 0;
  defaultSpeed = defSpeed;
  PIDSpeedL= defSpeed;
  PIDSpeedR =defSpeed;
  speedMax = newSpeedMax; //200;
  SpeedMin = newSpeedMin;//190;
  tempo = 0;
  controletempo = 0;
  PR;
  PL;
  D = 0; 
  I = 0;
  kP=1; //0.05;
	kD=0; //0.05;
	kI=0; //0.05;

  erro=0;                                      
  ultimoL = 0;
  ultimoR = 0;
  offsetR=3;
	offsetL=-3;

  i=0;// -> mudar de nome, i é mais genérico que a vida
  setpoint=5; // valor de ticks que a gente quer controlar o PID. o "padrão", o nosso 100%
  iterations=500;
  number_of_cicles_wanted=20;


  pinMode(LBIN1,OUTPUT);
  pinMode(LBIN2,OUTPUT);
  pinMode(LPWMB,OUTPUT);
  pinMode(RAIN1,OUTPUT);
  pinMode(RAIN2,OUTPUT);
  pinMode(RPWMA,OUTPUT);
  //pinMode(pololuAL, INPUT);
  pinMode(pololuBL, INPUT);
  //pinMode(pololuAR, INPUT);
  pinMode(pololuBR, INPUT);
}
/*
This method will set the speed of the wheels and defines what are their max and min values. The parameters are:
1- PIDSpeedL : the speed of the left wheel.
2- PIDSpeedR : the speed of the right wheel.
3- SPEEDHIGH : how much the speed can vary upwards.
4- SPEEDLOW : how much the speed can vary downwards.
*/
void Motors::setSpeed(int SpeedLeft, int SpeedRight, int newSpeedMax, int newSpeedMin)
{
  PIDSpeedL = SpeedLeft;
  PIDSpeedR = SpeedRight;
  speedMax = newSpeedMax; //200;
  SpeedMin = newSpeedMin;//190;
}

/*
	offsetR-=1;
	offsetL+=1;
	systemMsg("offsetR:"+String(offsetR));
	systemMsg("offsetL:"+String(offsetL));
		offsetR+=1;
	offsetL-=1;
	systemMsg("offsetR:"+String(offsetR));
	systemMsg("offsetL:"+String(offsetL));


	calibrate_motors(195);
	*/

void Motors::turn_right(int PIDSpeedL, int PIDSpeedR)// 1,-1 para direita, -1,1 para esquerda
{

   //int number_of_ticks =  map(angle, 0,360,0,23);
   if((motorRActive==1) && (motorLActive==1))
   {
     // if((1*counterL<number_of_ticks) || (-1*counterR<number_of_ticks))
     // {
            moveRightWheel(MOTOR_R, PIDSpeedR, 1);
            moveLeftWheel(MOTOR_L, PIDSpeedL, 1);
    //  }
    //  else
    //  {
    //        moveRightWheel(MOTOR_R, 0, 0);
    //        moveLeftWheel(MOTOR_L, 0, 0); 
    //  }
   }  
}
void Motors::turn_left(int PIDSpeedL, int PIDSpeedR)// 1,-1 para direita, -1,1 para esquerda
{

   //int number_of_ticks =  map(angle, 0,360,0,23);
   if((motorRActive==1) && (motorLActive==1))
   {
     // if((-1*counterL<number_of_ticks) || (1*counterR<number_of_ticks))
      //{
            moveRightWheel(MOTOR_R, PIDSpeedR, 0);
            moveLeftWheel(MOTOR_L, PIDSpeedL, 0);
  //    }
  //    else
  //    {
  //          moveRightWheel(MOTOR_R, 0, 0);
  //          moveLeftWheel(MOTOR_L, 0, 0); 
  //    }
   }  
}

void Motors::move_backward(int PIDSpeedL, int PIDSpeedR)
{
  if(i>number_of_cicles_wanted)
	{
		if(counterR != counterL) 
		{
			PR = (setpoint - (-1*(counterR-ultimoR)))*kP;
		  PL = (setpoint - (-1*(counterL-ultimoL)))*kP;
	    PIDSpeedL=PIDSpeedL+PL;
	    PIDSpeedR=PIDSpeedL+PR;

			if(PIDSpeedR > 255) 
			{
				PIDSpeedR=255;
			}	
			if(PIDSpeedL > 255) 
			{
				PIDSpeedL = 255;
			}
			if(PIDSpeedR < 0)
			{
				PIDSpeedR=0;
			}
			if(PIDSpeedL < 0)
			{
				PIDSpeedL = 0;
			}
		}
		ultimoR = counterR;
		ultimoL = counterL;
		i=0;
	}
	else
	{
		i++;
	}
  moveRightWheel(MOTOR_R, PIDSpeedR+-1*offsetR, 1);
	moveLeftWheel(MOTOR_L, PIDSpeedL+-1*offsetL, 0);
}


void Motors::move_forward(int SpeedL, int SpeedR)
{
	/*systemMsg("KP:"+String(kP)+"\n");
	systemMsg("KI:"+String(kI)+"\n");
	systemMsg("KD:"+String(kD)+"\n");
	systemMsg("setpoint:"+String(setpoint)+"\n");
	systemMsg("offsetR:"+String(offsetR)+"\n");
	systemMsg("offsetL:"+String(offsetL)+"\n");
	systemMsg("\n");*/
	if(i>number_of_cicles_wanted)
	{
		if(counterR != counterL) 
		{
			PR = (setpoint - (counterR-ultimoR))*kP;
		  PL = (setpoint - (counterL-ultimoL))*kP;
	    PIDSpeedL=PIDSpeedL+PL;
	    PIDSpeedR=PIDSpeedL+PR;

			if(PIDSpeedR > 255) 
			{
				PIDSpeedR=255;
			}	
			if(PIDSpeedL > 255) 
			{
				PIDSpeedL = 255;
			}
			if(PIDSpeedR < 0)
			{
				PIDSpeedR=0;
			}
			if(PIDSpeedL < 0)
			{
				PIDSpeedL = 0;
			}
		}
		ultimoR = counterR;
		ultimoL = counterL;
		i=0;
	}
	else
	{
		i++;
	}
  moveRightWheel(MOTOR_R, PIDSpeedR+offsetR, 0);
	moveLeftWheel(MOTOR_L, PIDSpeedL+offsetL, 1);
}

void Motors::calibrate_motors(int Speed)
{
	if(i<iterations)
	{
    moveLeftWheel(MOTOR_L, Speed, 1);
    moveRightWheel(MOTOR_R, Speed, 0);
		i++;
	}
	else
	{
    moveLeftWheel(MOTOR_L, 0, 1); //para motores
    moveRightWheel(MOTOR_R, 0, 0);
		float control=iterations/number_of_cicles_wanted;
		float mediaR = counterR/control;
		float mediaL = counterL/control;

		setpoint = (mediaR+mediaL)/2;
		i=0; 
		systemMsg("setpoint!!!!!!!!!!!!!!!!!!!!!"+String(setpoint));
		counterL=0;
		counterR=0; 
	}

}

void Motors::moveRightWheel(int motor, int speed, int direction) 
{ 
  boolean inPinR1;
  boolean inPinR2; 
  if(direction == 1) 
  { 
    inPinR1 = HIGH; 
    inPinR2 = LOW; 
  }
  else
  {
    inPinR1 = LOW;
    inPinR2 = HIGH; 
  }

      digitalWrite(RAIN1, inPinR1); 
      digitalWrite(RAIN2, inPinR2);
      analogWrite(RPWMA, speed); 
      digitalWrite(STBY, HIGH); //disable standby
      
}
void Motors::moveLeftWheel(int motor, int speed, int direction) 
{ 
  boolean inPinL1;
  boolean inPinL2; 
  if(direction == 0) 
  { 
    inPinL1 = HIGH; 
    inPinL2 = LOW; 
  }
  else
  {
    inPinL1 = LOW;
    inPinL2 = HIGH; 
  }

  digitalWrite(LBIN1, inPinL1); 
  digitalWrite(LBIN2, inPinL2); 
  analogWrite(LPWMB, speed); 

      
}

void Motors::stop(){
  //enable standby  
  digitalWrite(STBY, LOW); 
  counterR=0;
  counterL=0;
}

/*
This function controls how the robot moves. The turn of the wheel indicates if the robot is moving forward, backward,left or right*/
void Motors::control_movement()
{

  if((motorRActive==1) && (motorLActive==1)){
	  if(motorLTurn==1 && motorRTurn==0) //move forward
	  {
	    move_forward(motorLspeed,motorRspeed);
	    //counterL=0;
	    //counterR=0;
	  }
	  else if ((motorLTurn==0) && (motorRTurn==1))//mofe backward
	  {
	    move_backward(motorLspeed,motorRspeed);
	    //counterL=0;
	    //counterR=0;
	  }
	  else if(motorLTurn==1 && motorRTurn==1)//turn right
	  {
	    turn_right(motorsDefaultAngularSpeed,motorsDefaultAngularSpeed);
	    //counterL=0;
	    //counterR=0;
	  }
	  else if(motorLTurn==0 && motorRTurn==0)//turn left
	  {
	    turn_left(motorsDefaultAngularSpeed,motorsDefaultAngularSpeed);
	    //counterL=0;
	    //counterR=0;
	  }
	}
	else stop();
}

int16_t Motors::getSpeedR(){
	return PIDSpeedR;
}
int16_t Motors::getSpeedL(){
	return PIDSpeedL;
}
void Motors::setSetpoint(float setpoint_){
	setpoint = setpoint_;
}
void Motors::setOffsetR(int8_t offsetR_){
	offsetR = offsetR_;
}
void Motors::setOffsetL(int8_t offsetL_){
	offsetL = offsetL_;
}
void Motors::setKP(float kP_){
	kP = kP_;
}
void Motors::setKI(float kI_){
	kI = kI_;
}
void Motors::setKD(float kD_){
	kD = kD_;
}





Motors m;


/*
 Pin interrupts for the two pololu encoders. When the wheels turns forward, the counter is incremented and when it turns backwards, is decremented.
 */
void pololuL()//interrupcao pro encoder 1
{
  if(digitalRead(pololuBL)==1)
   m.counterL++;
  else
   m.counterL--;
}
void pololuR() //interrupcao pro encoder2
{ 
    if(digitalRead(pololuBR)==1)
     m.counterR--;//counter2++;
    else
     m.counterR++;//counter2--;
}


/*
 A function to setup the environmnet of the motors. This fucntion initialize the motor with the right values and the pin interrupts.
 */
void setup_Environment(int interrup1, int interrup2)
{
  //Serial.begin(9600);
  //AFMS.begin();
  m.init_Motors(motorsDefaultSpeed,10,10);
  attachInterrupt(interrup1, pololuL, FALLING);
  attachInterrupt(interrup2, pololuR, FALLING);
}



void motors_config()
{
  setup_Environment(0,1);
}

void motorsUpdate()
{
  m.control_movement();
}




void motorsCommandProcess(uint8_t *data);
void setMotorsTurn(uint8_t newMotorRTurn, uint8_t newMotorLTurn);
//void setMotorsSpeed(uint8_t newMotorRSpeed, uint8_t newMotorLSpeed);
void setMotorsActive(uint8_t newMotorRSpeed, uint8_t newMotorLSpeed);
//void motorsUpdate();
//void setNeckServoPos(uint8_t *data);

void motorsCommandProcess(uint8_t *data){ 
    setMotorsTurn(data[0],data[2]); //determina o sentido dos motores 
    if((motorRActive==0) && (motorLActive==0)) m.setSpeed(motorRspeed, motorLspeed, 10, 10); //setMotorsSpeed(data[1],data[3]); //determina a velocidade dos motores
    setMotorsActive(data[1],data[3]); //determina se os motores esta ligado ou nao
}


void setMotorsTurn(uint8_t newMotorRTurn, uint8_t newMotorLTurn){ 
	if(newMotorRTurn==0x0F) motorLTurn=1;
	else motorLTurn=0;

	if(newMotorLTurn==0x0F) motorRTurn=1;
	else motorRTurn=0;
}
/*
void setMotorsSpeed(uint8_t newMotorRSpeed, uint8_t newMotorLSpeed){
	/*motorRspeed = newMotorRSpeed; 
    motorLspeed = newMotorLSpeed;
}*/

void setMotorsActive(uint8_t newMotorRSpeed, uint8_t newMotorLSpeed){ 
    if(newMotorRSpeed) motorRActive = 1;
    else motorRActive =0;
    if(newMotorLSpeed) motorLActive = 1;
    else motorLActive = 0;
}

/*
void motorsUpdate(){
	if(motorRActive){
		if(motorRTurn==1 && motorLTurn==0){
			moveFoward(255);
		}
		else if(motorRTurn==0 && motorLTurn==1){
			moveBackward(255);
		}
		else if(motorRTurn==1 && motorLTurn==1){
			rotateLeft(255);
		}
		else if(motorRTurn==0 && motorLTurn==0){
			rotateRight(255);
		}
	}
	else stop();
}
/*
void setNeckServoPos(uint8_t *data){
	neckServo.write(data[1]);
}*/

#endif