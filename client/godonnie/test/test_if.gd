criar x
criar y

falar "Iniciando teste do SE ENT�O SEN�O"

se x == 0 entao pf 5 gd 90 senao pt 5 ge 90 fim se

se y == 0 entao falar "y tamb�m � falso"  senao    	 falar "por�m y � verdade"   fim   se

se y == 0 entao  falar "y tamb�m � falso"  
senao   falar "por�m y � verdade"   
fim se

se y == 0 entao 
  falar "y tamb�m � falso"  
senao    	 
  falar "por�m y � verdade"   
fim se

falar "Iniciando teste do SE ENT�O SEN�O encadeado"

se x == 0 entao 
  falar "x � falso"
  esperar 1
  se y == 0 entao 
	falar "y tamb�m � falso"
  senao 
    falar "por�m y � verdade"
  fim se
  esperar 1
  pf 2 
senao 
  falar "x � verdade"
  esperar 1
  se y == 0 entao 
	falar "mas y � falso"
  senao 
    falar "y tamb�m � verdade"
  fim se
  esperar 1
  pt 2 
fim se

falar "fim do teste"
