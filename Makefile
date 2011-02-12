GCC=gcc
AR=ar
CFLAGS=-Wall -O3 -g3
#CFLAGS=-O3 -Wall

MOLY_O=l.o d.o f.o dw.o
MOLY_A=molybdenum.a

all: clean m 

clean:
	rm -f $(MOLY_O) $(MOLY_A)

m: $(MOLY_O)
	$(AR) -q $(MOLY_A) $(MOLY_O)

test: m test.o
	gcc $(CFLAGS) test.o $(MOLY_A) -o test.out -lpthread


%.o:%.c
	$(GCC) -c $(INCLUDE) $(CFLAGS) -o $@ $< 
