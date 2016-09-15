
	
void motors_power_off();
void motors_set_vel(double x, double yaw);
void motorsDataProcess(double x, double yaw);



const int speedSize = 8;

int     motorRActive = 0;   //valor definido com commando vindo do driver
uint8_t motorRSpeed = 0;
int     motorRTurn= 0; //1 = rigth; 0 = left

int     motorLActive = 0;
uint8_t motorLSpeed = 0;
int     motorLTurn = 0; //1 = rigth; 0 = left


void motors_power_off(){

    
}


void motors_set_vel(double x, double yaw){
	//motorsConvertSpeed(speedSize);

}

void motorsDataProcess(uint8_t cmd){ // converte 1 byte to 2 speed //uint8_t cmd, int size){
    //motorRSpeed = cmd & 0x0f; //mantem nible da direita
    //motorLSpeed = cmd >> 4; //mantem nible da esquerda
    //motorLSpeed = motorLSpeed & 0x0f; //evita de 1000000 virar 1111000 ao ser deslocado para esquerda

    //setMotorsTurn(); //determina o sentido dos motores 
    //setMotorsActive(); //determina se os motores esta ligado ou nao

    //converte do tamanho indicado para 8 bits cada. (-1 para discontar a direcao)
    //os speeds vao ficar entre 0 e 255 ... caso necessario outros valores mudar nessa funcao
    //motorsConvertSpeed(speedSize-1); 

    //motorsToPrint(); 
}

//void motorsUpdate(){
//    //setMotorsActive(); //determina se os motores esta ligado ou nao
//    //if(motorRActive){
//        if(motorRTurn) myMotor1->run(BACKWARD);
//        else myMotor1->run(FORWARD);
//        myMotor1->setSpeed(motorRSpeed);
//    //}
//    //if(motorLActive){
//        if(motorLTurn) myMotor2->run(FORWARD);
//        else myMotor2->run(BACKWARD);
//        myMotor2->setSpeed(motorRSpeed);
//    //}
//}
//
////setMotorsTurn() zera o bit de sentido da speed e define o 
////valor das variveis motorXTurn. Se esse bit for relevante 
////eh so nao chamar essa funcao.
//void setMotorsTurn(){ 
//    if((motorRSpeed >> speedSize-1) & 1)motorRTurn = 1;
//   else motorRTurn =0;
//
//   if((motorLSpeed >> speedSize-1) & 1) motorLTurn = 1;
//   else motorLTurn = 0;
//
//   motorRSpeed = motorRSpeed & ~(1 << speedSize-1); //zera o bit que define o sentido do speed
//   motorLSpeed = motorLSpeed & ~(1 << speedSize-1); 
//}

//exemplo 1000 pode ser um motor desligado caso 
//nao seja usada a funcao setMotorsTurn()
//e se for usada 000 é um motor desligado 
//e é na setMotorsActive() que que sera definida a lógica
//void setMotorsActive(){ 
//    if(motorRSpeed)motorRActive = 1;
//    else motorRActive =0;
//    if(motorLSpeed) motorLActive = 1;
//    else motorLActive = 0;
//}

//void motorsConvertSpeed(uint8_t sourceBitlengh){
//    motorRSpeed = map(motorRSpeed,0,pow(2,sourceBitlengh),0,pow(2,8)-1);
//    motorLSpeed = map(motorLSpeed,0,pow(2,sourceBitlengh),0,pow(2,8)-1);
//}
