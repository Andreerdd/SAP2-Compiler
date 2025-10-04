; --- Programa Principal ---
MVI A, 07H      ; Inicia A com 7

CALL DUPLICAR   ; Chama a sub-rotina. A deve se tornar 14 (0EH)
OUT 0H          ; Saída 0: Deve exibir 0EH

CALL DUPLICAR   ; Chama de novo. A deve se tornar 28 (1CH)
OUT 1H          ; Saída 1: Deve exibir 1CH

HLT             ; Fim do programa principal

; --- Definição da Sub-rotina ---
DUPLICAR:
    MOV B, A    ; Copia A para B (B=A)
    ADD B       ; Adiciona B ao Acumulador (A = A + B, ou seja, A = A + A)
    RET         ; Retorna para o programa principal