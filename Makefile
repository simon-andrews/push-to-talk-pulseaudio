CFLAGS=-Wall -Wextra -pedantic
LIBS=-lX11

ptt_pa: ptt_pa.c
	$(CC) $(CFLAGS) ptt_pa.c -o ptt_pa $(LIBS)

clean:
	rm ptt_pa

.PHONY: clean
