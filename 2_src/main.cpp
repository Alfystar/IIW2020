//
// Created by alfystar on 14/07/20.
//
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <utility.hpp>
#include <sstream>
#include <fstream>
#include <fmt/format.h>


#include <NCS.h>

#define SERV_PORT   8080
#define BACKLOG       10
#define MAXLINE     1024

using namespace std;


int sendData (int sckt, const void *data, int datalen){
	const char *ptr = static_cast<const char *>(data);
	while (datalen > 0){
		int bytes = send (sckt, ptr, datalen, 0);
		if (bytes <= 0) return -1;
		ptr += bytes;
		datalen -= bytes;
	}
	return 0;
}

int sendStr (int sckt, const std::string &s){
	return sendData (sckt, s.c_str (), s.size ());
}


int main (int argc, char *argv[]){

//	int listensd, connsd;
//	struct sockaddr_in servaddr;

//	// crea il socket di ascolto del processo verso la rete
//	if ((listensd = socket (AF_INET, SOCK_STREAM, 0)) < 0){
//		perror ("errore in socket");
//		exit (1);
//	}
//
//	//Rende riutilizzabile la porta
//	int enable = 1;
//	if (setsockopt (listensd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof (int)) < 0)
//		perror ("setsockopt(SO_REUSEADDR) failed");
//
//
//	// Configuro i parametri con la quale deve funzionare la socket, attraverso la bind
//	memset ((void *)&servaddr, 0, sizeof (servaddr));
//	servaddr.sin_family = AF_INET;
//	servaddr.sin_addr.s_addr = htonl (INADDR_ANY); /* il server accetta
//        connessioni su una qualunque delle sue intefacce di rete */
//	servaddr.sin_port = htons (SERV_PORT); /* numero di porta del server */
//
//	// assegna l'indirizzo e la porta da far guardare alla socket
//	if ((bind (listensd, (struct sockaddr *)&servaddr, sizeof (servaddr))) < 0){
//		perror ("errore in bind");
//		exit (1);
//	}
//
//	// Comunico al SO la dimensione della coda di attesa che deve gestire
//	if (listen (listensd, BACKLOG) < 0){
//		perror ("errore in listen");
//		exit (1);
//	}
//
//	// Attende sia presente almeno una connessione (che ha già superato l'handShake a 3 vie)
//	// Si ottiene un Soket connesso
//	if ((connsd = accept (listensd, (struct sockaddr *)NULL, NULL)) < 0){
//		perror ("errore in accept");
//		exit (1);
//	}

	NCS::Accept *a = NCS::ncsGetAccept();
	int connsd = a->getLastFd();

	char buff[MAXLINE];

	read (connsd, buff, MAXLINE);

	//Definiamo una stringa dal buffer letto
	std::string string1 = buff;
	// Trasformiamo la stringa in uno streambuf
	std::stringbuf *buffer = new stringbuf (string1);
	// Creiamo l'oggetto che analizza la richiesta
	SimpleWeb::RequestMessage *header = new SimpleWeb::RequestMessage ();
	//Creiamo dallo streambuf un inputStream per farlo analizzare
	std::istream *istream1 = new istream (buffer);
	//Impostiamo i campi di lavoro
	std::string method;
	std::string path;
	std::string query_string;
	std::string version;
	SimpleWeb::CaseInsensitiveMultimap cim;
	//Analizziamo la richiesta e la salviamo sui campo
	header->parse (*istream1, method, path, query_string, version, cim);

	//Cerchiamo nel dizionario e troviamo l'indice
	auto it = cim.find ("Accept");
	cout << "Iterator points to " << it->first << " = " << it->second << endl;

//		int start = it->second.find("image/");
//		int end = it->second.find(",",start);
//		std::string imgRequest = it->second.substr(start, end-start);
//		cout << "\nImg request :\n" << imgRequest << "\n";

	//Otteniamo Separate tutte le accetazioni ammissibili

	int start = 0;
	int end = 0;
	cout << "Img request :\n";
	end = it->second.find (",", start + 1);
	std::string imgRequest = it->second.substr (start, end - start);
	cout << imgRequest << "\n";
	while (start < end){
		start = end;
		end = it->second.find (",", start + 1);
		std::string imgRequest = it->second.substr (start + 1, end - start - 1);
		cout << imgRequest << "\n";
	}

	// Creazione del campo DATA
	SimpleWeb::Date *dat = new SimpleWeb::Date ();
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now ();
	std::string dataString = dat->to_string (now);
	cout << dataString << "\n\n\n\n";

	// Append modifica la stringa corrente e ritorna il riferimento a se stesa
	string newString = dataString.append ("Aggiungo altro \n");
	cout << dataString << "\n";
	cout << newString << "\n";


	//Funzione che dato il path di un file, ritorna l'ultima modifica dello stesso
	//Da trasformare per ritornare la stringa e non printarla
//		void getFileCreationTime(char *path) {
//			struct stat attr;
//			stat(path, &attr);
//			printf("Last modified time: %s", ctime(&attr.st_mtime));
//		}

	//Verrà letto da un file, e probabilmente modificato \n con \r\n
	std::string b = "<html>\r\n"
	                "<body>\r\n"
	                "<h1>Hello, World!</h1>\r\n"
	                "</body>\r\n"
	                "</html>\r\n\r\n";

	// Ci saranno i campi che dovranno essere {} e parametricamente montati
	std::string h = "HTTP/1.1 200 OK\r\n"
	                "Date: Mon, 27 Jul 2009 12:28:53 GMT\r\n"
	                "Server: Apache/2.2.14 (Win32)\r\n"
	                "Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT\r\n"
				    "Content-Length: {}\r\n"
	                "Content-Type: text/html\r\n"
	                //	                "Connection: Closed\r\n"
	                "Keep-Alive: timeout=5, max=99\r\n"
	                "Connection: Keep-Alive\r\n"
	                "\r\n"
				    "{}";   //Body
	// Unirà l'header con il bodi e ritornerà la stringa di invio HTML
	string mes = fmt::format(h, b.length(),b);

	cout<<"try send 1° response\n";

	if (sendStr (connsd, mes) == -1){
		cout<<"sendStr 1° come error\n";
		close (connsd);
	}



	///#################################################################################################

	// Aspettiamo la connesione per il favicon.ico
//	if ((connsd = accept (listensd, (struct sockaddr *)NULL, NULL)) < 0){
//		perror ("errore in accept");
//		exit (1);
//	}
	read (connsd, buff, MAXLINE);

	//Definiamo una stringa dal buffer letto
	string1 = buff;
	// Trasformiamo la stringa in uno streambuf
	buffer = new stringbuf (string1);
	//Creiamo dallo streambuf un inputStream per farlo analizzare
	istream1 = new istream (buffer);

	//Analizziamo la richiesta e la salviamo sui campo
	header->parse (*istream1, method, path, query_string, version, cim);

	//Cerchiamo nel dizionario e troviamo l'indice
	it = cim.find ("Accept");
	cout << "Iterator points to " << it->first << " = " << it->second << endl;

	//Otteniamo Separate tutte le accetazioni ammissibili

	start = 0;
	end = 0;
	cout << "Img request :\n";
	end = it->second.find (",", start + 1);
	imgRequest = it->second.substr (start, end - start);
	cout << imgRequest << "\n";
	while (start < end){
		start = end;
		end = it->second.find (",", start + 1);
		std::string imgRequest = it->second.substr (start + 1, end - start - 1);
		cout << imgRequest << "\n";
	}

//    "Connection: Keep-Alive\n"
	string s = "HTTP/1.1 200 OK\r\n"
	    "Date: Wed, 15 Jul 2020 10:09:52 GMT\r\n"
	    "Server: Apache/2.2.22 (Debian)\r\n"
	    "Last-Modified: Thu, 16 Mar 2006 09:33:16 GMT\r\n"
	    "Accept-Ranges: bytes\r\n"
	    "Content-Length: 1035\r\n"
	    "X-Content-Type-Options: nosniff\r\n"
	    "Connection: Closed\r\n"
	    "Content-Type: image/png\r\n"
	    "\r\n";

	if (sendStr (connsd, s) == -1){
		cout<<"sendStr 1° come error\n";
		close (connsd);
	}

	std::string filename = "/home/alfystar/Documenti/IIW2020/2_src/rfc.png";

//		if (!fileExists (filename)) // <- you need to implement this
//		{
//			if (sendStr (socket, "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: keep-alive\r\n\r\n") ==
//			    -1){
//				close (socket);
//			}
//		}
//		else{
	int socket = connsd;
	std::ifstream in (filename, std::ios::binary);
//			if (!in.is_open ()){
//				std::cout << "failed to open file" << std::endl;
//				if (sendStr (socket, "HTTP/1.1 500 Error\r\nContent-Length: 0\r\nConnection: keep-alive\r\n\r\n") == -1){
//					close (socket);
//				}
//			}else{
//				std::cout << "file:" << filename << " not just Open" << std::endl;

	in.seekg (0, std::ios::end);
	std::size_t length = in.tellg ();
	in.seekg (0, std::ios::beg);

//				if (in.fail ()){
//					std::cout << "failed to get size of file" << std::endl;
//					if (sendStr (socket, "HTTP/1.1 500 Error\r\nContent-Length: 0\r\nConnection: keep-alive\r\n\r\n") == -1){
//						close (socket);
//					}
//					std::cout << "fail n1\n";
//				}else if (sendStr (socket, "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string (length) +
//				                          "\r\nConnection: keep-alive\r\nContent-Type: image/png\r\n\r\n") == -1){
//					close (socket);
//					std::cout << "fail n2\n";
//				}else
	if (length > 0){
		std::cout << "success\n";
		char data[4096];
		do{
			if (!in.read (data, std::min (length, sizeof (data)))){
				close (socket);
				break;
			}

			int bytes = in.gcount ();
			if (sendData (socket, data, bytes) == -1){
				close (socket);
				break;
			}

			length -= bytes;
		}
		while (length > 0);
		std::cout << "end While\n";
	}
//				}
//			}
//		}



//		/* scrive sul socket di connessione il contenuto di buff */
//		if (write (connsd, s.c_str (), strlen (s.c_str ())) != strlen (s.c_str ())){
//			perror ("errore in write");
//			exit (1);
//		}


//	close (listensd);

	if (close (connsd) == -1){  /* chiude la connessione */
		perror ("errore in close");
		exit (1);
	}
	return 0;
}