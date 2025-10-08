INC= -I../src/blincolnlights -I../src/blincolnlights/pdp1
files := $(wildcard *.c)

%.so:%.o
	gcc -shared $< -o $@
%.o:%.c
	gcc $(INC) -fPIC -c $< -o $@

all: $(files:.c=.so)

clean:
	rm *.o *.so
