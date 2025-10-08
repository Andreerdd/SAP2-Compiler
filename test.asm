; Carrega os registradores
MVI A, FH
MVI B, 2H
MVI C, 3H

; Faz algumas operações
ADD B ; A = A+B
OUT 0H

ADD C ; A = (A+B)+C
OUT 0H

SUB B ; A = (A+B+C)-B = A+C
SUB C

OUT 0H
HLT