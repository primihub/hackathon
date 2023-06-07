all: injector

injector: injector.o
	$(CC) $(CFLAGS) $< -O3 -Wall -l:libcapstone.a -o $@ -pthread

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ -Wall

clean:
	rm *.o injector
