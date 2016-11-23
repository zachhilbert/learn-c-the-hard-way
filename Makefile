CFLAGS=-Wall -g

all: clean code

clean:
	rm -rf ex

code:
	cc $(CFLAGS) ex${EXNUM}.c -o ex

