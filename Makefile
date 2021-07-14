# Protocoale de comunicatii
# Tema2
# Radulescu Florin, 323 CC
# Makefile

CFLAGS = -Wall -g

# Portul pe care asculta serverul (se poate completa)
PORT =

# Adresa IP a serverului (se poate completa)
IP_SERVER =

all: server subscriber

# Compileaza server.cpp
server: server.cpp

# Compileaza subscriber.cpp
subscriber: subscriber.cpp

.PHONY: clean run_server run_subscriber

# Ruleaza serverul
run_server:
	./server ${PORT}

# Ruleaza clientul TCP
run_subscriber:
	./subscriber ${IP_SERVER} ${PORT}

clean:
	rm -f server subscriber
