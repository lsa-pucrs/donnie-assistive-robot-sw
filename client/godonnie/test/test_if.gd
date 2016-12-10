criar x
criar y

falar "Iniciando teste do SE ENTÃO SENÃO"

se x == 0 entao pf 5 gd 90 senao pt 5 ge 90 fim se

se y == 0 entao falar "y também é falso"  senao    	 falar "porém y é verdade"   fim   se

se y == 0 entao  falar "y também é falso"  
senao   falar "porém y é verdade"   
fim se

se y == 0 entao 
  falar "y também é falso"  
senao    	 
  falar "porém y é verdade"   
fim se

falar "Iniciando teste do SE ENTÃO SENÃO encadeado"

se x == 0 entao 
  falar "x é falso"
  esperar 1
  se y == 0 entao 
	falar "y também é falso"
  senao 
    falar "porém y é verdade"
  fim se
  esperar 1
  pf 2 
senao 
  falar "x é verdade"
  esperar 1
  se y == 0 entao 
	falar "mas y é falso"
  senao 
    falar "y também é verdade"
  fim se
  esperar 1
  pt 2 
fim se

falar "fim do teste"
