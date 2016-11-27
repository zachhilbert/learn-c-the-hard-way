CFLAGS=-Wall -g

exnum=${EXNUM}
ifneq ($(exnum), )
	file=ex${EXNUM}.c
	outfile=ex
else
	file=${SRC}.c
	outfile=${SRC}
endif

all: clean code

clean:
	rm -rf $(outfile)

code:
	cc $(CFLAGS) $(file) -o $(outfile)
