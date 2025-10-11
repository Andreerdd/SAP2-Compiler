# SAP2-Interpreter
Interpreter do microprocessador "Simple-As-Possible 2".

## Como usar?
Obtenha o arquivo executável específico para seu caso _(Windows/Linux, ARM, ...)_ e execute-o da
forma como será mostrada abaixo.

## Comando:
O comando para executar o programa é: 
```bash
[arquivo_executavel] [arquivo] [parametros]
```
Observações:
- O `[arquivo_executavel]` vai estar em função do sistema do usuário (Windows/Linux, ARM, ...).
Então, por exemplo, se o seu sistema for Windows (x64 ou x86), seu comando se aparentaria com:
```bash
./sap2-interpreter-windows.exe arquivo.asm
```
- O arquivo, necessariamente, vem antes dos parâmetros.

Um exemplo de comando para o sistema operacional Ubuntu seria:
```bash
./sap2-interpreter-linux.exe test.asm
```

### Parâmetros:
Os parâmetros que podem ser usados são são:
- `--inicio <hexadecimal>`  ou `-i <hexadecimal>`: onde o contador de programa começará (no endereço `<hexadecimal>`);
- `--limite <numero>` ou `-l <numero>`: quantidade máxima de instruções que serão executadas (dado pelo valor `<numero>`);
- `--saida-limpa` ou `-sl`: desativa a impressão da tabela que mostra a memória no fim do programa (ao usar `HLT`);
- `--debug`, `-d`, `--passo-a-passo` ou `-p`: ativa o modo de depuração, que executa uma instrução 
por vez, mostrando informações a cada execução (como o valor dos registradores e dos flag).

Por exemplo, caso queira executar s:
```bash
./sap2-interpreter-windows test.asm --inicio 1000H
```
- Nesse caso, o programa compilará o arquivo test.asm e começará a anotar as instruções a
  partir do endereço de memória `1000H`.
    - Observação: nesse programa, todo número hexadecimal termina com H.

Outro exemplo que usaria o modo de depuração e a saída limpa:
```bash
./sap2-interpreter-windows test.asm --inicio 2000H -sl -d
```

É interessante citar que os parâmetros são escritos não importa, apenas que o próximo texto em
seguida deles seja seu(s) parâmetro(s) _(se houver)_. 
Por exemplo,
`
./sap2-interpreter-linux codigo.asm --inicio 1000H -sl
`
funcionaria, ao mesmo tempo que
`
./sap2-interpreter-linux codigo.asm 1000H --inicio -sl
`
não funcionaria.


## Código exemplar:
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

## Instruções
As instruções aceitas pelo programa são: 

![Codigos_de_Operacao.jpeg](SAP2/Codigos_de_Operacao.jpeg)

