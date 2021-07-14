#include "helpers.h"

using namespace std;

// transforma mesajul primit de la clientul UDP in mesaj pentru clientul TCP
int parse_message(udp2srv_t* rcvmsg, srv2tcp_t* msgsnd) {

    // severul poate interpreta doar 4 tipuri de mesaje
    if(rcvmsg->type > 3)
      return 0;

    long long int_num; // pentru verificarea numerelor intregi fara semn
    double real_num;	// pentru verificarea numerelor reale

    strncpy(msgsnd->topic, rcvmsg->topic, 50);
    msgsnd->topic[50] = 0; // fixare terminator

    if (rcvmsg->type == 0) {
      // numar intreg fara semn
			if(rcvmsg->data[0] > 1)
        return 0;
      int_num = ntohl(*(uint32_t*)(rcvmsg->data + 1));
      if(rcvmsg->data[0])
        int_num *= -1;
      sprintf(msgsnd->data, "%lld", int_num);
      msgsnd->type = 1; // INT

    } else if (rcvmsg->type == 1) {
      // numar real pozitiv cu 2 zecimale
      real_num = ntohs(*(uint16_t*)(rcvmsg->data));
      real_num /= 100;
      sprintf(msgsnd->data, "%.2f", real_num);
      msgsnd->type = 2; // SHORT_REAL

    } else if (rcvmsg->type == 2) {
      // numar real
			if(rcvmsg->data[0] > 1)
        return 0;
      real_num = ntohl(*(uint32_t*)(rcvmsg->data + 1));
      real_num /= pow(10, rcvmsg->data[5]);
      if (rcvmsg->data[0])
        real_num *= -1;
      sprintf(msgsnd->data, "%lf", real_num);
      msgsnd->type = 3; // FLOAT

    } else {
      // sir de caractere
      strcpy(msgsnd->data, rcvmsg->data);
			msgsnd->type = 4;  // STRING
		}

    return 1;
}

void usage(char *file)
{
	fprintf(stderr, "Usage: %s server_port\n", file);
	exit(0);
}

int main(int argc, char *argv[])
{

	int sockfd, newsockfd, portno, tcpsock, udpsock, max_fd;
	char buffer[BUFLEN];
	struct sockaddr_in serv_addr, udp_addr, tcp_addr, new_tcp;
	int n, i, ret;
	socklen_t clilen;
	bool EXIT = false;

	// tipurile de mesaje inspre/dinspre server:
	tcp2srv_t* tcp2srv;
	udp2srv_t* udp2srv;
	srv2tcp_t srv2tcp;

	// cheie = topic; valoare = file descriptori ai subscriberilor
	unordered_map<string, unordered_set<int>> topics;

	fd_set read_fds; // multimea de citire folosita in select()
	fd_set tmp_fds;  // multime folosita temporar
	int fdmax; // valoare maxima fd din multimea read_fds

	if (argc < 2) {
		usage(argv[0]);
	}

	// se goleste multimea de descriptori de citire (read_fds)
  // si multimea temporara (tmp_fds)
	FD_ZERO(&read_fds);
	FD_ZERO(&tmp_fds);

	// se creeaza socketul UDP
	udpsock = socket(PF_INET, SOCK_DGRAM, 0);
	DIE(udpsock < 0, "socket");

	// se creeaza socketul TCP
	tcpsock = socket(AF_INET, SOCK_STREAM, 0);
	DIE(tcpsock < 0, "socket");

	// numar port
	portno = atoi(argv[1]);
	DIE(portno == 0, "atoi");

	// se completeaza informatiile despre socketi
	memset((char *) &udp_addr, 0, sizeof(udp_addr));
	memset((char *) &tcp_addr, 0, sizeof(tcp_addr));
	udp_addr.sin_family = AF_INET;
	tcp_addr.sin_family = AF_INET;
	udp_addr.sin_port = htons(portno);
	tcp_addr.sin_port = htons(portno);
	udp_addr.sin_addr.s_addr = INADDR_ANY;
	tcp_addr.sin_addr.s_addr = INADDR_ANY;

	// bind la socketi
	ret = bind(udpsock, (struct sockaddr *) &udp_addr, sizeof(struct sockaddr));
	DIE(ret < 0, "bind");
	ret = bind(tcpsock, (struct sockaddr *) &tcp_addr, sizeof(struct sockaddr));
	DIE(ret < 0, "bind");

	ret = listen(tcpsock, MAX_CLIENTS);
	DIE(ret < 0, "listen");

	// setarea file descriptorilor
	FD_SET(tcpsock, &read_fds);
	FD_SET(udpsock, &read_fds);
	FD_SET(0, &read_fds);
	fdmax = tcpsock;

	while (!EXIT) {
		tmp_fds = read_fds;
		memset(buffer, 0, BUFLEN);

		ret = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);
		DIE(ret < 0, "select");

		for (i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &tmp_fds)) {

				if (!i){
					// s-a primit comanda de la stdin pe server
					fgets(buffer, BUFLEN - 1, stdin);
					if (!strcmp(buffer, "exit\n")) {
							EXIT = true;
							break;
					}

				} else if (i == udpsock) {

					// mesaj de la client UDP
					memset(buffer, 0, BUFLEN);
					n = recv(i, buffer, sizeof(buffer), 0);
					DIE(n < 0, "recv");

					srv2tcp.udp_port = ntohs(udp_addr.sin_port);
					strcpy(srv2tcp.IP, inet_ntoa(udp_addr.sin_addr));
					udp2srv = (udp2srv_t*)buffer;

					// decodeaza mesaj si trimite la clientii TCP abonati la topic
					if (parse_message(udp2srv, &srv2tcp) == 1) {
						if (topics.find(srv2tcp.topic) != topics.end()) {
							for (int i : topics[srv2tcp.topic]) {
								ret = send(i, (char*) &srv2tcp, sizeof(srv2tcp_t), 0);
								DIE(ret < 0, "send");
							}
						}
					}

				} else if (i == tcpsock) {

					  // a venit o cerere de conexiune pe socketul inactiv (cel cu listen),
						// pe care serverul o accepta
						clilen = sizeof(tcp_addr);
						newsockfd = accept(tcpsock, (struct sockaddr *) &new_tcp, &clilen);
						DIE(newsockfd < 0, "accept");

						// se dezactiveaza algoritmul lui Nae pentru conexiunea la clientul TCP
						int flag = 1;
            setsockopt(newsockfd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));

						// socketul intors de accept() e pus in multimea de file_descriptori
						FD_SET(newsockfd, &read_fds);
						if (newsockfd > fdmax) {
							fdmax = newsockfd;
						}

						n = recv(newsockfd, buffer, sizeof(buffer) , 0);
						DIE(n < 0, "recv");

						printf("New client %s, connected from %s:%hu.\n", buffer,
								inet_ntoa(tcp_addr.sin_addr), ntohs(tcp_addr.sin_port));

				} else {

					// s-au primit date pe unul din socketii de client TCP
					// asa ca serverul trebuie sa le receptioneze
					memset(buffer, 0, BUFLEN);
					n = recv(i, buffer, BUFLEN - 1, 0);
					DIE(n < 0, "recv");

					if (n == 0) {

						// conexiunea s-a inchis
            // se scoate fd-ul clientului din lista de topicuri
            for (unordered_map<string, unordered_set<int>>::iterator it = topics.begin(); it != topics.end(); it++){
		             topics[it->first].erase(i);
	          }
						printf("Client %d disconnected.\n", i);
						close(i);

						// se scoate din multimea de citire socketul inchis
						FD_CLR(i, &read_fds);

				  } else {

					  // s-a primit subscribe sau unsubscribe
						tcp2srv = (tcp2srv_t*)buffer;

            // subscribe -> se dauga clientul in setul topicului
						if (tcp2srv->type == 1) {
              topics[tcp2srv->topic].insert(i);

            // unsubscribe -> se sterge clienul din setul topicului
            } else
              topics[tcp2srv->topic].erase(i);
				  }
			  }
			}
		}
	}

  // inchidere socketi
	close(tcpsock);
	close(udpsock);

	return 0;

}
