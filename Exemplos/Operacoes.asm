; Código que faz umas operações básicas para mostrar
; as operações mais fundamentais

; Carrega os registradores
MVI A, 1H
MVI B, 2H
MVI C, 3H

; Faz algumas operações e imprime os resultados a cada operação
ADD B ; A = A+B
OUT 0H

ADD C ; A = (A+B)+C
OUT 0H

SUB B ; A = (A+B+C)-B = A+C
OUT 0H