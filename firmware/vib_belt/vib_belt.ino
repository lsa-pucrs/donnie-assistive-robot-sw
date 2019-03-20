#define DWORD unsigned long
#define WORD  word
#define BYTE  byte

#define GRID_LINES   5
#define GRID_COLUMNS 7

#define OUTPUT_LEVELS 16

struct SGridCell {
  
  BYTE bLed;
  
  BYTE bOutputLevel;
};

/*SGridCell m_a2stGrid[GRID_LINES][GRID_COLUMNS] = { //using 5x7 hardware

  { { 15, 0 }, { 25, 0 }, { 48, 0 }, {  9, 0 }, { 36, 0 }, { 46, 0 }, { 19, 0 } },
  { { 14, 0 }, {  5, 0 }, { 50, 0 }, {  8, 0 }, { 35, 0 }, { 13, 0 }, { 18, 0 } },
  { { 16, 0 }, {  6, 0 }, { 49, 0 }, { 32, 0 }, { 37, 0 }, { 12, 0 }, { 51, 0 } },
  { { 52, 0 }, {  7, 0 }, { 22, 0 }, { 33, 0 }, { 38, 0 }, { 11, 0 }, {  2, 0 } },
  { { 17, 0 }, { 24, 0 }, { 23, 0 }, { 34, 0 }, { 53, 0 }, { 10, 0 }, {  3, 0 } },
};*/
SGridCell m_a2stGrid[GRID_LINES][GRID_COLUMNS] = { //usuing 2x7 hardware

  { { 2, 0 }, { 3 , 0 }, { 4 , 0 }, {  5 , 0 }, { 6 , 0 }, { 7 , 0 }, { 24 , 0 } },//not implemented
  { { 8,0 }, { 9, 0 }, { 28, 0 }, {  30, 0 }, { 32, 0 }, { 34, 0 }, { 36, 0 } },
  { { 26, 0 }, { 10, 0 }, { 11, 0 }, {  12, 0 }, { 13, 0 }, { 14, 0 }, { 15, 0 } },//not implemented
  { { 40,0 }, { 42, 0 }, { 44, 0 }, {  46, 0 }, { 48, 0 }, { 50, 0 }, { 52, 0 } },
  { { 16,0 }, { 17, 0 }, { 18, 0 }, {  19, 0 }, { 20, 0 }, { 21, 0 }, { 22, 0 } },//not implemented
};
//Protocol: send 'Z' in the beginin of array and set to 'P' the not implemented lines.
//Exemple: data[36] = {'Z',V11,V12,V13,V14,V15,V16,V17,V21,V22,V23...V54,V56,V57}
//Value 'A' is Max. Vibration, 'B' Moderate and 'C' Low. 'P' stop the motor.
//You can use 'D', 'E', end 'F', but it's hard to identify the vibration

void setup() {

  Serial.begin(9600);
  
  for(byte bPin = 0; bPin < 54; bPin++) {
    
    boolean bnFound = false;
    
    for(int nY0 = 0; nY0 < GRID_LINES; nY0++) { //Percorre as linhas
      
      for(int nX0 = 0; nX0 < GRID_COLUMNS; nX0++) { //Percorre as colunas
        
        if (bPin == m_a2stGrid[nY0][nX0].bLed) { //Se o pino procurado for encontrado
          
          bnFound = true;
          break;
        }
      }
      
      if (bnFound == true)
        break;
    }
    
    if (bnFound == true) {
      
      digitalWrite(bPin, LOW);    //Inicializa em baixo
      
      if (bPin != 4)
        pinMode(bPin, OUTPUT);
    }
  }

/*
  for(int nY = 0; nY < GRID_LINES; nY++) {
    
    for(int nX = 0; nX < GRID_COLUMNS; nX++) {
      
      pinMode(m_a2stGrid[nY][nX].bLed, OUTPUT);
      digitalWrite(m_a2stGrid[nY][nX].bLed, LOW);    
    }
  }
*/
}

DWORD m_wwFrameCount = 0;

DWORD m_wwTimeOutCount = 0;

BYTE m_abFrameData[GRID_LINES * GRID_COLUMNS];
int  m_nFrameDataPos = -1;

void loop() {

  
  int nSerialAvailable = Serial.available(); //Vê quantos bytes tem aguardando na Serial
  
  if (nSerialAvailable > 0) { //Se tiver algo na Serial
    
    //\\Serial.println(nSerialAvailable);
    
    for(int nCount = 0; nCount < nSerialAvailable; nCount++) { //Varrer cada byte na Serial
      Serial.println(nCount);
      BYTE bNew = Serial.read(); //Ler um byte da Serial
      
      //Serial.print(bNew);
        
      //Primeira coisa que faz é procurar 'Z', entrando no else do if seguinte
      if (m_nFrameDataPos >= 0) { //Verificando os comandos
        
        if ((bNew >= 'A') && (bNew < 'A' + OUTPUT_LEVELS)) { //Verifica se está dentro dos niveis aceitos de OUTPUT - 'A    Serial.println("while");    Serial.println("while");' até 'P'
        
          //Determinar nivel numerico do OUTPUT
          if (bNew != ('A' + OUTPUT_LEVELS - 1))
            m_abFrameData[m_nFrameDataPos] = 1 + bNew - 'A';
          else //Caso 'P'
            m_abFrameData[m_nFrameDataPos] = 255;
          
          m_nFrameDataPos++; //Próxima posição da Serial
          
          if (m_nFrameDataPos == GRID_LINES * GRID_COLUMNS) { //Após ultima posição da Serial ser verificada 
            
            int nPos = 0;
            
            //Percorre a tabela de pinos modificando os valores de OUTPUT
            for(int nY = 0; nY < GRID_LINES; nY++) {
              
              for(int nX = 0; nX < GRID_COLUMNS; nX++) {
                
                m_a2stGrid[nY][nX].bOutputLevel = m_abFrameData[nPos];
                nPos++;
              }
            }
            
            m_nFrameDataPos = -1; //end of frame
            
            m_wwTimeOutCount = 100;
          }
        }
        else {
          Serial.println("Novo");
          Serial.println("E2");
          
          m_nFrameDataPos = -1; //error
        }
      }
      else { //Procurando inicio dos comandos pros motores
        
        if (bNew == 'Z') {
          
          m_nFrameDataPos = 0; //sync ok - prepara para poder verificar os comandos
          
          //Serial.println("START");
        }
        else {
          
          Serial.println("E1");
        }
      }
    }
  }
  
  if (m_wwTimeOutCount > 0) {
 
    m_wwTimeOutCount--;
  
    for(int nY = 0; nY < GRID_LINES; nY++) {
      
      for(int nX = 0; nX < GRID_COLUMNS; nX++) {
        
        if ((m_a2stGrid[nY][nX].bOutputLevel != 255) &&
            ((m_wwFrameCount % m_a2stGrid[nY][nX].bOutputLevel) == 0)) { //Duty cicle baseado no nivel do OUTPUT de cada pino
          
          digitalWrite(m_a2stGrid[nY][nX].bLed, HIGH);
        }
        else {
          
          digitalWrite(m_a2stGrid[nY][nX].bLed, LOW);    
        }
      }
    }
  }
  else { //Caso tenha passado determinado tempo sem receber instruções pela Serial
    
    for(int nY = 0; nY < GRID_LINES; nY++) {
      
      for(int nX = 0; nX < GRID_COLUMNS; nX++) {
        
        digitalWrite(m_a2stGrid[nY][nX].bLed, LOW);    
      }
    }
  }
  
  m_wwFrameCount++;
}

