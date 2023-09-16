MVI H,0AH ;an example input for our keys
MOV A,H ; set our input to acu
RLC ; rotate to left *4
RLC 
RLC 
RLC 
ADD H ; add H to four time routated H
MOV H,A ; set H to A
MOV D,A ; set H to A
ROTATE: MOV A,D ;set D to A for loop
    RLC ;rotate A for first time
    MOV D,A ; renew A cuz we want countine from here not first 
    ANI 01H ; and to make our output
    OUT 10H ;Output the D0 bit

JMP ROTATE ;Jump to ROTATE to change logic level
