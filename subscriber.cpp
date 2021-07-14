#include "helpers.h"

using namespace std;

// transforma inputul de la stdin in mesaj ce va fi trimis catre server
int command(char* buffer, tcp2srv_t* msg2srv) {

    // setat 0 la finalul bufferului
    // altfel unsubscribe va primi un topic eronat, plin pana la [50]
    buffer[strlen(buffer) - 1] = 0;

		// sparge buffer in tokeni ce vor fi prelucrati
    char *token = strtok(buffer, " ");  // delimitator spatiu
		if (token == NULL)  // nu am avut o comanda valida
      return 0;
    if (!strcmp(token, "subscribe"))
       msg2srv->type = 1;
    else if (!strcmp(token, "unsubscribe"))
      msg2srv->type = 0;

    token = strtok(NULL, " ");
		if (token == NULL)
      return 0;
    if (strlen(token) > 50)
      return 0;
    strcpy(msg2srv->topic, token);

    if (msg2srv->type == 1) { // SF apare doar la subscribe
        token = strtok(NULL, " ");
				if (token == NULL)
          return 0;
        if (token[0] == '1')
          msg2srv->SF = 1;
        else if (token[0] == '0')
          msg2srv->SF = 0;
        else return 0;
    }

    return 1;
}

void usage(char *file)
{
	fprintf(stderr, "Usage: %s subscriber_id server_address server_port\n", file);
	exit(0);
}

int main(int argc, char *argv[])
{
	int sockfd, n, ret;
	struct sockaddr_in serv_addr;
	char buffer[BUFLEN];
	int flag = 1;
	tcp2srv_t msg2srv;
	srv2tcp_t* rcvmsg;

	fd_set set;
	fd_set tmp_fds;

	FD_ZERO(&set);
	FD_ZERO(&tmp_fds);

	if (argc < 4) {
		usage(argv[0]);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	DIE(sockfd < 0, "socket");

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[3]));

	ret = inet_aton(argv[2], &serv_addr.sin_addr);
	DIE(ret = 0, "inet_aton");

	ret = connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
	DIE(ret < 0, "connect");

  // se trimite serverului ID-ul clientului
	ret = send(sockfd, argv[1], strlen(argv[1]) + 1, 0);
	DIE(ret < 0, "send");

	FD_SET(0, &set);
	FD_SET(sockfd, &set);

	int fdmax = sockfd;

	while (1) {

		tmp_fds = set;

		ret = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);
		DIE(ret < 0, "select");

		if (FD_ISSET(0, &tmp_fds)) {

		  // se citeste de la tastatura
			memset(buffer, 0, BUFLEN);
			fgets(buffer, BUFLEN - 1, stdin);

			// s-a primit <exit>
			if (strncmp(buffer, "exit", 4) == 0)
				break;

			// se trimite mesaj la server
			if(command(buffer, &msg2srv) == 1){

				n = send(sockfd, (char*) &msg2srv, sizeof(msg2srv), 0);
      	DIE(n < 0, "send");

				if (strncmp(buffer, "subscribe", 9) == 0)
					printf("Server: Subscribed to %s\n", msg2srv.topic);
				else if (strncmp(buffer, "unsubscribe", 11) == 0)
						printf("Server: Unsubscribed from %s\n", msg2srv.topic);
			}
		}

			if (FD_ISSET(sockfd, &tmp_fds)){

  		// s-a primit mesaj de la server
			memset(buffer, 0, BUFLEN);
			n = recv(sockfd, buffer, BUFLEN, 0);
			DIE(n < 0, "recv");

			// serverul a fost inchis -> se inchide clientul
			if (n == 0)
					break;

      // se afiseaza mesajul primit de la server
			rcvmsg = (srv2tcp_t*)buffer;
      if(rcvmsg->type > 0){
        fprintf(stderr, "Received: %s:%hu - %s - %d - %s\n",
								rcvmsg->IP, rcvmsg->udp_port, rcvmsg->topic,
								rcvmsg->type, rcvmsg->data);
      }
		}
	}

	close(sockfd);

	return 0;
}
