MVI A, 0AH
loop1: MVI B, 64H
loop2: MVI C, 47H
loop3: DCR C
JNZ loop3
DCR B
JNZ loop2
DCR A
JNZ loop1
HLT