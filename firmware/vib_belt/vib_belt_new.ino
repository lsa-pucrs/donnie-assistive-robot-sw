#define PINOUT_LENGTH 6

#define SERIAL_DATA_LENGTH 35

#define OUTPUT_LEVELS 16

struct MotorPin_Out {

  char motPin;
  
  char motOutputLevel;

};

//Arduino MEGA
MotorPin_Out arduinoPins[PINOUT_LENGTH] = {
  { 8 , 0 }, { 9 , 0 }, { 10 , 0 }, { 11 , 0 }, { 12 , 0 }, { 13 , 0 }
};

//Arduino NANO
/*MotorPin_Out arduinoPins[PINOUT_LENGTH] = {
  { 3 , 0 }, { 5 , 0 }, { 6 , 0 }, { 9 , 0 }, { 10 , 0 }, { 11 , 0 }
};*/

void setup() {
  
  Serial.begin(9600);  

  for(char mPin = 0; mPin < PINOUT_LENGTH; mPin++) {
   
    pinMode(arduinoPins[mPin].motPin, OUTPUT);
    
    digitalWrite(arduinoPins[mPin].motPin, LOW);

  }
}

int countTimeOut = 0;

int  m_nFrameDataPos = -1;

void loop() {
  
  //Enquanto a Serial esta vazia
  while ( !Serial.available() ) {
    
    //Decrementa o tempo de TimeOut caso seja diferente de zero
    if ( countTimeOut ){
      delay(1);
      
      --countTimeOut;

      //Caso chegue a zero, desliga todos os motores
      if ( !countTimeOut ) {

        for( char n = 0; n < PINOUT_LENGTH; n++) {

          analogWrite(arduinoPins[n].motPin, LOW);

        }
      }
    }
  }

  countTimeOut = 0;
  
  delay (10);
  
  //Vê quantos chars tem aguardando na Serial
  int serialAvailable = Serial.available(); 
  
  //Obtenção e manipulação com os valores da Serial
  for ( int nCount = 0; nCount < serialAvailable; nCount++ ) {
    Serial.println(nCount);
    char newValue = Serial.read();

    //Verificando os comandos caso tenho sido encontrado 'Z' previamente
    if (m_nFrameDataPos >= 0) { 
      
      //Unicos valores relevantes ao Arduino estao entre 8ª e 13ª posiçao inclusive
      if ( (m_nFrameDataPos >= (PINOUT_LENGTH + 1) ) && (m_nFrameDataPos < (2 * PINOUT_LENGTH + 1) ) ) {
        
        //Verifica se está dentro dos niveis aceitos de OUTPUT - 'A' até 'P'
        if ( (newValue >= 'A') && (newValue < 'A' + OUTPUT_LEVELS)) {
          
          //Determinar nivel numerico do OUTPUT
          arduinoPins[m_nFrameDataPos - 7].motOutputLevel = 255 - (newValue - 'A') * (OUTPUT_LEVELS + 1);
          
        }
        else {
          
          Serial.println("E2");
          
          m_nFrameDataPos = -1; //error
        }
      }
      
      //Próxima posição da Serial
      m_nFrameDataPos++; 
      
      //Após ultima posição da Serial ser verificada 
      if (m_nFrameDataPos == SERIAL_DATA_LENGTH) { 

        //Fim de um frame de dados da Serial
        m_nFrameDataPos = -1; 
        
        countTimeOut = 200;
      }
    }
    //Procurando inicio dos comandos pros motores
    else { 
      Serial.println("Novo");
      if (newValue == 'Z') {
        
        //sync ok - Prepara para poder verificar os comandos
        m_nFrameDataPos = 0; 
      }
      else {
        
        Serial.println("E1");
      }
    }
    
    if ( countTimeOut ) { break; }
  }
  
  
  
  
  if (countTimeOut > 0) {
 
    countTimeOut--;
  
    for( char mPin = 0; mPin < PINOUT_LENGTH; mPin++) {
      
      analogWrite(arduinoPins[mPin].motPin, arduinoPins[mPin].motOutputLevel);

    }

  }
  else { //Caso tenha passado determinado tempo sem receber instruções pela Serial
    
    for( char mPin = 0; mPin < PINOUT_LENGTH; mPin++) {

      analogWrite(arduinoPins[mPin].motPin, LOW);

    }

  }



}
