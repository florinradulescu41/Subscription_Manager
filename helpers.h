#ifndef _HELPERS_H
#define _HELPERS_H 1

#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <string>
#include <cstdio>
#include <vector>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <netdb.h>
#include <zconf.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unordered_map>
#include <unordered_set>

using namespace std;

// Nota: Macro-ul de verificare al erorilor este cel folosit in laboratoare
#define DIE(assertion, call_description)	\
	do {									\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",	\
					__FILE__, __LINE__);	\
			perror(call_description);		\
			exit(EXIT_FAILURE);				\
		}									\
	} while(0)

#define BUFLEN 1552	// dimensiunea maxima a calupului de date
#define MAX_CLIENTS	20	// numarul maxim de clienti in asteptare

// structura pentru un mesaj primit de catre server de la un client UDP
struct __attribute__((packed)) udp2srv_t {
    char topic[50];
    uint8_t type;
    char data[1501];
};

// structura pentru un mesaj transmis la server de catre un client TCP
struct __attribute__((packed)) tcp2srv_t {
    int type; //subscribe = 1; unsubscribe = 0;
    char topic[51];
    int SF;
};

// structura pentru un mesaj transmis de la server catre un client TCP
struct __attribute__((packed)) srv2tcp_t {
    char IP[16];
    uint16_t udp_port;
    char topic[51];
    int type;	// 1 = INT; 2 = SHORT_REAL; 3 = FLOAT; 4 = STRING
    char data[1501];
};

int command(char*, tcp2srv_t*);	//	in subscriber.cpp
int parse_message(udp2srv_t* rcvmsg, srv2tcp_t* msgsnd);	//in server.cpp

#endif
