criar a
criar pos_antes
criar pos_depois
criar error
criar repetitions=4

falar "Iniciando teste de movimentos de 90 graus"
esperar 3
pos_antes = posicao a
a=0 enquanto a<repetitions faca gd 90 ge 90 a = a+1 fim enquanto
pos_depois = posicao a
error = pos_depois-pos_antes
falar error
esperar 1

falar "Iniciando teste de movimentos de 15 graus"
esperar 3
pos_antes = posicao a
a=0 enquanto a<repetitions faca gd 15 ge 15 a = a+1 fim enquanto
pos_depois = posicao a
error = pos_depois-pos_antes
falar error
esperar 1

falar "Iniciando teste de movimentos de 160 graus"
esperar 3
pos_antes = posicao a
a=0 enquanto a<repetitions faca gd 160 ge 160 a = a+1 fim enquanto
pos_depois = posicao a
error = pos_depois-pos_antes
falar error
esperar 1

falar "terminei"
