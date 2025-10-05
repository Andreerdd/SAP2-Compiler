# SAP2-Compiler
"Simple-As-Possible" microprocessor compiler

Parâmetros:
- --inicio \<hexadecimal>  ou -i \<hexadecimal>: onde o contador de programa começará
- --limite \<numero> ou -l \<numero>: quantidade máxima de instruções que serão executadas
- --saida-limpa ou -sl: desativa a impressão da memória no HLT
- --debug, -d, --passo-a-passo ou -p: ativa o modo de depuração, que executa cada instrução passo a passo

código exemplar:
```asm
; Carrega os registradores
MVI A, 1H
MVI B, 2H
MVI C, 3H

; Faz algumas operações
ADD B ; A = A+B
OUT 0H

ADD C ; A = (A+B)+C
OUT 0H

SUB B ; A = (A+B+C)-B = A+C
OUT 0H
```