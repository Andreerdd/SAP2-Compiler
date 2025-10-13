; Código que simula um sinal de trânsito.

; Nesse código, você não conseguirá ver os 3 estados diferentes
; pois o limite de tempo padrão (10 segundos) te impedirá.
; Para alterá-lo, apenas adicione o parâmetro -lt <tempo> na
; execução do programa (<tempo> precisa ser o tempo em
; milissegundos, como `30000` para 30 segundos).


; Esse rótulo guarda um valor de um endereço de memória
SAVE: 1234H

; Esse rótulo guarda um endereço de memória. Assim, JMP AGAIN
; levaria para essa instrução abaixo e as outras abaixo dela.
AGAIN: MVI A, 1EH
STA SAVE
MVI A, 10H
OUT 04H
LOOPGR: CALL D1SEC
LDA SAVE
DCR A
STA SAVE
JNZ LOOPGR
MVI A, 05H
STA SAVE
MVI A,20H
OUT 04H
LOOPYE: CALL D1SEC
LDA SAVE
DCR A
STA SAVE
JNZ LOOPYE
MVI A, 14H
STA SAVE
MVI A, 40H
OUT 04H
LOOPRE: CALL D1SEC
LDA SAVE
DCR A
STA SAVE
JNZ LOOPRE
JMP AGAIN
HLT

; Delay de 1 segundo
D1SEC: MVI A, 0AH
loop1: MVI B, 64H
loop2: MVI C, 47H
loop3: DCR C
JNZ loop3
DCR B
JNZ loop2
DCR A
JNZ loop1
RET