//
// Created by alfystar on 14/07/20.
//
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SERV_PORT   8080
#define BACKLOG       10
#define MAXLINE     1024

using namespace std;

int main(int argc, char *argv[]){

	cout << "Hello World " << SERV_PORT << "\n";
	int listensd, connsd;
	struct sockaddr_in    servaddr;
	char buff[MAXLINE];

	// crea il socket
	if ((listensd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("errore in socket");
		exit(1);
	}

	int enable = 1;
	if (setsockopt(listensd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		perror("setsockopt(SO_REUSEADDR) failed");

	memset((void *)&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); /* il server accetta
        connessioni su una qualunque delle sue intefacce di rete */
	servaddr.sin_port = htons(SERV_PORT); /* numero di porta del server */

	// assegna l'indirizzo al socket
	if ((bind(listensd, (struct sockaddr *) &servaddr, sizeof(servaddr))) < 0) {
		perror("errore in bind");
		exit(1);
	}

	if (listen(listensd, BACKLOG) < 0 ) {
		perror("errore in listen");
		exit(1);
	}

	for ( ; ; ) {
		if ((connsd = accept(listensd, (struct sockaddr *)NULL, NULL)) < 0) {
			perror("errore in accept");
			exit(1);
		}

		read(connsd,buff, MAXLINE);
		std::cout<<buff;

		std::string s =
				"HTTP/1.1 200 OK\r\n"
				"Date: Mon, 27 Jul 2009 12:28:53 GMT\r\n"
				"Server: Apache/2.2.14 (Win32)\r\n"
				"Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT\r\n"
				"Content-Length: 88\r\n"
				"Content-Type: text/html\r\n"
				"Connection: Closed\r\n"
				"\r\n"
				"<html>\r\n"
				"<body>\r\n"
				"<h1>Hello, World!</h1>\r\n"
				"</body>\r\n"
				"</html>\r\n\r\n";
		std::cout<<s;

		/* scrive sul socket di connessione il contenuto di buff */
		if (write(connsd, s.c_str(), strlen(s.c_str())) != strlen(s.c_str())) {
			perror("errore in write");
			exit(1);
		}

		close(listensd);

		if (close(connsd) == -1) {  /* chiude la connessione */
			perror("errore in close");
			exit(1);
		}
		exit(0);
	}
	exit(0);
	return 0;
}