criar a
criar pos_antes
criar pos_depois
criar error

falar "Iniciando teste de movimentos de 90 graus"
pos_antes = posicao a
a=0 enquanto a<8 faca gd 90 ge 90 a = a+1 fim enquanto
pos_depois = posicao a
error = pos_depois-pos_antes
falar error
esperar 1
falar "Iniciando teste de movimentos de 15 graus"
pos_antes = posicao a
a=0 enquanto a<8 faca gd 15 ge 15 a = a+1 fim enquanto
pos_depois = posicao a
error = pos_depois-pos_antes
falar error
esperar 1
falar "Iniciando teste de movimentos de 160 graus"
pos_antes = posicao a
a=0 enquanto a<8 faca gd 160 ge 160 a = a+1 fim enquanto
pos_depois = posicao a
error = pos_depois-pos_antes
falar error
esperar 1
