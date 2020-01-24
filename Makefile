cc := gcc -o
cFlags += -g
cSwitch += -Wall

OBJ := $(patsubst %.c , %.o ,$(wildcard *.c))


op.exe: $(OBJ)
	$(cc) $@ $^ $(cFlags) $(cSwitch)
clean:
	rm *.o *.exe
