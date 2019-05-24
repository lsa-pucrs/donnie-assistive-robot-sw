#ifndef motors_h
#define motors_h

#include <Arduino.h>
#include <Wire.h>

//! Ticks/Pulses counter var, updated in interrupt
volatile int counterL;
volatile int counterR;

//! Routine called to set the digital outputs and anothers motors pre sets
void motors_config();
//! Routine called to update motor's data or PID control
void motorsUpdate();
//! Process the motor command that cames from donnie's protocol
void motorsCommandProcess(uint8_t *data);
//! Set Motors Turn from motor command 
void setMotorsTurn(uint8_t newMotorRTurn, uint8_t newMotorLTurn);
//! Set Motors speed from motor command 
void setMotorsSpeed(uint8_t newMotorRSpeed, uint8_t newMotorLSpeed);
//! Set Motors Turn on or off from motor command 
void setMotorsActive(uint8_t newMotorRSpeed, uint8_t newMotorLSpeed);
//! Set right wheel speed and direction
/*! Speed range: 0 to 255
    Direction = 1: Turn Clock-wise*/
void setRightWheelSpeedAndDirection(int8_t speed, int8_t direction);
void setLeftWheelSpeedAndDirection(int8_t speed, int8_t direction);
//! Ticks/Pulses counter interruption procedure
void pololuR();
void pololuL();
//! Stop the robot
void stop();




//?to daniel: tem algum motivo de nao comecar tudo setado em zero ??
//int motorsDefaultSpeed = 195; //daniel
int motorsDefaultSpeed = 120; //marques
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
    void init_Motors(int defSpeed, int newSpeedMax, int newSpeedMin);
    void setSpeed(int SpeedL, int SpeedR, int newSpeedMax, int newSpeedMin);
    void turn_right(int PIDSpeedL, int PIDSpeedR);
    void turn_left(int PIDSpeedL, int PIDSpeedR);
    void move_backward(int PIDSpeedL, int PIDSpeedR);
    void move_forward(int SpeedLeft, int SpeedRight);
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
  offsetR=0;//3;
	offsetL=0;//-3;

  i=0;// -> mudar de nome, i é mais genérico que a vida
  setpoint=5; // valor de ticks que a gente quer controlar o PID. o "padrão", o nosso 100%
  iterations=500;
  number_of_cicles_wanted=20;
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
            //setLeftWheelSpeedAndDirection(PIDSpeedR, 1);
            //setRightWheelSpeedAndDirection(PIDSpeedL, 1);
            setLeftWheelSpeedAndDirection(motorsDefaultAngularSpeed-3, 1);
            setRightWheelSpeedAndDirection(motorsDefaultAngularSpeed, 1);
    //  }
    //  else
    //  {
    //        setLeftWheelSpeedAndDirection(0, 0);
    //        setRightWheelSpeedAndDirection(0, 0); 
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
            //setLeftWheelSpeedAndDirection(PIDSpeedR, 0);
            //setRightWheelSpeedAndDirection(PIDSpeedL, 0);
            setLeftWheelSpeedAndDirection(motorsDefaultAngularSpeed-3, 0);
            setRightWheelSpeedAndDirection(motorsDefaultAngularSpeed, 0);
  //    }
  //    else
  //    {
  //          setLeftWheelSpeedAndDirection(0, 0);
  //          setRightWheelSpeedAndDirection(0, 0); 
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
  //setLeftWheelSpeedAndDirection(PIDSpeedR+-1*offsetR, 1);
	//setRightWheelSpeedAndDirection(PIDSpeedL+-1*offsetL, 0);
  setLeftWheelSpeedAndDirection(motorsDefaultSpeed, 1);
  setRightWheelSpeedAndDirection(motorsDefaultSpeed, 0);
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
  //setLeftWheelSpeedAndDirection(PIDSpeedR+offsetR, 0);
	//setRightWheelSpeedAndDirection(PIDSpeedL+offsetL, 1);
  setLeftWheelSpeedAndDirection(motorsDefaultSpeed-5, 0);
  setRightWheelSpeedAndDirection(motorsDefaultSpeed, 1);
}

void Motors::calibrate_motors(int Speed)
{
	if(i<iterations)
	{
		//setRightWheelSpeedAndDirection(Speed, 1);
    //setLeftWheelSpeedAndDirection(Speed, 0);
    setRightWheelSpeedAndDirection(motorsDefaultSpeed, 1);
    setLeftWheelSpeedAndDirection(motorsDefaultSpeed, 0);
		i++;
	}
	else
	{
    setRightWheelSpeedAndDirection(0, 1); //para motores
    setLeftWheelSpeedAndDirection(0, 0);
		float control=iterations/number_of_cicles_wanted;
		float mediaR = counterR/control;
		float mediaL = counterL/control;

		setpoint = (mediaR+mediaL)/2;
		i=0; 
		systemMsg("setpoint!!!!!!!!!!!!!!!!!!!!"+String(setpoint));
		counterL=0;
		counterR=0; 
	}

}


/*
This function controls how the robot moves. The turn of the wheel indicates if the robot is moving forward, backward,left or right*/
void Motors::control_movement()
{

  if((motorRActive==1) && (motorLActive==1)){
	  if(motorLTurn==1 && motorRTurn==0) //move forward
	  {
	    move_forward(motorLspeed,motorRspeed);
	    // counterL=0;
	    // counterR=0;
	  }
	  else if ((motorLTurn==0) && (motorRTurn==1))//mofe backward
	  {
	    move_backward(motorLspeed,motorRspeed);
	    // counterL=0;
	    // counterR=0;
	  }
	  else if(motorLTurn==1 && motorRTurn==1)//turn right
	  {
	    turn_right(motorsDefaultAngularSpeed,motorsDefaultAngularSpeed);
	    // counterL=0;
	    // counterR=0;
	  }
	  else if(motorLTurn==0 && motorRTurn==0)//turn left
	  {
	    turn_left(motorsDefaultAngularSpeed,motorsDefaultAngularSpeed);
	    // counterL=0;
	    // counterR=0;
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//!A function to setup the environmnet of the motors. This fucntion initialize the motor with the right values and the pin interrupts.
void motors_config(){
  //AFMS.begin();
  pinMode(LBIN1,OUTPUT);
  pinMode(LBIN2,OUTPUT);
  pinMode(LPWMB,OUTPUT);
  pinMode(RAIN1,OUTPUT);
  pinMode(RAIN2,OUTPUT);
  pinMode(RPWMA,OUTPUT);
  attachInterrupt(digitalPinToInterrupt(pololuAL), pololuL, FALLING);
  pinMode(pololuBL, INPUT);
  attachInterrupt(digitalPinToInterrupt(pololuAR), pololuR, FALLING);
  pinMode(pololuBR, INPUT);

  counterL = 0; //reset ticks
  counterR = 0; //reset ticks

  //m.init_Motors(motorsDefaultSpeed,10,10);
}

void motorsUpdate()
{
  m.control_movement();
}

void motorsCommandProcess(uint8_t *data){ 
    setMotorsTurn(data[0],data[2]); //determina o sentido dos motores 
    if((motorRActive==0) && (motorLActive==0)) m.setSpeed(motorRspeed, motorLspeed, 10, 10); //setMotorsSpeed(data[1],data[3]); //determina a velocidade dos motores
    setMotorsActive(data[1],data[3]); //determina se os motores esta ligado ou nao
}


void setMotorsTurn(uint8_t newMotorLTurn, uint8_t newMotorRTurn){ 
  if(newMotorLTurn==0x0F) motorLTurn=1;
  else motorLTurn=0;

  if(newMotorRTurn==0xF0) motorRTurn=1;
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

//! Pin interrupts for the two pololu encoders. When the wheels turns forward, the counter is incremented and when it turns backwards, is decremented.
void pololuL()//interrupcao pro encoder 1
{
  if(digitalRead(pololuBL)==1)
   counterL--;
  else
   counterL++;
}
void pololuR() //interrupcao pro encoder2
{ 
  if(digitalRead(pololuBR)==1)
    counterR++;
  else
    counterR--;
}

//TODO change name to setRightMotorPWMAndDirection, because the speed will be count in m/s
void setRightWheelSpeedAndDirection(int8_t speed, int8_t direction) {  
  if(direction == 0) { 
    digitalWrite(LBIN1, HIGH); 
    digitalWrite(LBIN2, LOW); 
  }
  else{
    digitalWrite(LBIN1, LOW); 
    digitalWrite(LBIN2, HIGH); 
  }
  analogWrite(LPWMB, speed); 
  digitalWrite(STBY, HIGH); //disable standby  
}

//TODO change name to setLeftMotorPWMAndDirection
void setLeftWheelSpeedAndDirection(int8_t speed, int8_t direction) { 
  if(direction == 1) { 
      digitalWrite(RAIN1, HIGH); 
      digitalWrite(RAIN2, LOW);
  }
  else{
      digitalWrite(RAIN1, LOW); 
      digitalWrite(RAIN2, HIGH);
  }
  analogWrite(RPWMA, speed); 
  digitalWrite(STBY, HIGH); //disable standby    
}

void stop(){
  digitalWrite(STBY, LOW);   //enable standby   
}

#endif