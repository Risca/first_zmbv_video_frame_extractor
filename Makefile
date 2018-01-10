.PHONY: all clean

all: extract

extract: main.c
	gcc $^ -o $@

clean:
	rm -f extract
