# SAP2-Compiler
"Simple-As-Possible" microprocessor compiler


código exemplar:
```asm
; Carrega os registradores
MVI A, 1H
MVI B, 2H
MVI C, 3H

; A+B
ADD B
OUT 0H

; A+B+C
ADD C
OUT 0H

; Finaliza o programa
HLT
```