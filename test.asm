; Carrega os registradores
MVI A, FH
MVI B, 2H
MVI C, 3H

; Faz algumas operações
ADD B ; A = A+B
OUT 0H ; mostra o resultado

ADD C ; A = (A+B)+C
OUT 0H ; mostra o resultado

SUB B ; A = (A+B+C)-B = A+C
SUB C ; A = (A+C) - A = A (valor inicial)

OUT 0H ; mostra o resultado
HLT ; finaliza o programa