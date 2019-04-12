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

  { { 2, 0 }, { 3 , 0 }, { 4 , 0 }, {  5 , 0 }, { 6 , 0 }, { 7 , 0 }, { 8 , 0 } },//not implemented
  { { 24,0 }, { 26, 0 }, { 28, 0 }, {  30, 0 }, { 32, 0 }, { 34, 0 }, { 36, 0 } },
  { { 9, 0 }, { 10, 0 }, { 11, 0 }, {  12, 0 }, { 13, 0 }, { 14, 0 }, { 15, 0 } },//not implemented
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
    
    for(int nY0 = 0; nY0 < GRID_LINES; nY0++) {
      
      for(int nX0 = 0; nX0 < GRID_COLUMNS; nX0++) {
        
        if (bPin == m_a2stGrid[nY0][nX0].bLed) {
          
          bnFound = true;
          break;
        }
      }
      
      if (bnFound == true)
        break;
    }
    
    if (bnFound == true) {
      
      digitalWrite(bPin, LOW);    
      
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

  int nSerialAvailable = Serial.available();
  
  if (nSerialAvailable > 0) {
    
    //\\Serial.println(nSerialAvailable);
    
    for(int nCount = 0; nCount < nSerialAvailable; nCount++) {
      
      BYTE bNew = Serial.read();
      
      //Serial.print(bNew);
        
      if (m_nFrameDataPos >= 0) {
        
        if ((bNew >= 'A') && (bNew < 'A' + OUTPUT_LEVELS)) {
        
          if (bNew != ('A' + OUTPUT_LEVELS - 1))
            m_abFrameData[m_nFrameDataPos] = 1 + bNew - 'A';
          else
            m_abFrameData[m_nFrameDataPos] = 255;
          
          m_nFrameDataPos++;
          
          if (m_nFrameDataPos == GRID_LINES * GRID_COLUMNS) {
            
            int nPos = 0;
            
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
          
          Serial.println("E2");
          
          m_nFrameDataPos = -1; //error
        }
      }
      else {
        
        if (bNew == 'Z') {
          
          m_nFrameDataPos = 0; //sync ok
          
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
            ((m_wwFrameCount % m_a2stGrid[nY][nX].bOutputLevel) == 0)) {
          
          digitalWrite(m_a2stGrid[nY][nX].bLed, HIGH);
        }
        else {
          
          digitalWrite(m_a2stGrid[nY][nX].bLed, LOW);    
        }
      }
    }
  }
  else {
    
    for(int nY = 0; nY < GRID_LINES; nY++) {
      
      for(int nX = 0; nX < GRID_COLUMNS; nX++) {
        
        digitalWrite(m_a2stGrid[nY][nX].bLed, LOW);    
      }
    }
  }
  
  m_wwFrameCount++;
}

