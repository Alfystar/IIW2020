//
// Created by alfystar on 20/07/20.
//

#include "Worker.h"

using namespace CES;

Worker::Worker(string &name){
	q = NCS::ncsGetQueue();
	myName = name;
	tJob = new std::thread(thWorker, this);

}

void Worker::thWorker(Worker *j){
	std::cout << "Worker::thWorker " << j->myName << " Start work\n";
	HttpMgt httpMgt;
	NCS::Connection *c;
//	NCS::Connection::httpHeader *hHeader;
	for(;;){
		c = j->q->popReadyCon();
		Action ret = httpMgt.connectionRequest(c);
		switch(ret){
			case ConClosed:
				// Connessione chiusa dentro httpMgt
				break;
			case RequestComplete:
			default:
				j->q->pushWaitCon(c);
				break;
		}


		//c = j->q->popReadyCon();

//		hHeader = c->readHttpHeader();
//		if(!hHeader){
//			cout << "hHeader = null \n";
//			exit(-1);
//		}
//		//Cerchiamo nel dizionario e troviamo l'indice
//		auto it = hHeader->cim.find("Accept");
//		cout << "Iterator points to " << it->first << " = " << it->second << endl;
//
////		int start = it->second.find("image/");
////		int end = it->second.find(",",start);
////		std::string imgRequest = it->second.substr(start, end-start);
////		cout << "\nImg request :\n" << imgRequest << "\n";
//
//		//Otteniamo Separate tutte le accetazioni ammissibili
//
//		int start = 0;
//		int end = 0;
//		cout << "Img request :\n";
//		end = it->second.find(",", start + 1);
//		std::string imgRequest = it->second.substr(start, end - start);
//		cout << imgRequest << "\n";
//		while(start < end){
//			start = end;
//			end = it->second.find(",", start + 1);
//			imgRequest = it->second.substr(start + 1, end - start - 1);
//			cout << imgRequest << "\n";
//		}
//
//		it = hHeader->cim.find("User-Agent");
//		cout << "Iterator points to " << it->first << " = " << it->second << endl;
//
//
//		// Creazione del campo DATA
//		SimpleWeb::Date *dat = new SimpleWeb::Date();
//		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
//		std::string dataString = dat->to_string(now);
//		cout << dataString << "\n\n\n\n";
//
//		// Append modifica la stringa corrente e ritorna il riferimento a se stesa
//		string newString = dataString.append("Aggiungo altro \n");
//		cout << dataString << "\n";
//		cout << newString << "\n";
//
//
//		//Funzione che dato il path di un file, ritorna l'ultima modifica dello stesso
//		//Da trasformare per ritornare la stringa e non printarla
////		void getFileCreationTime(char *path) {
////			struct stat attr;
////			stat(path, &attr);
////			printf("Last modified time: %s", ctime(&attr.st_mtime));
////		}
//
//		//Verrà letto da un file, e probabilmente modificato \n con \r\n
//		std::string b = "<html>\r\n"
//		                "<body>\r\n"
//		                "<h1>Hello, World!</h1>\r\n"
//		                "</body>\r\n"
//		                "</html>\r\n\r\n";
//
//		// Ci saranno i campi che dovranno essere {} e parametricamente montati
//		std::string h = "HTTP/1.1 200 OK\r\n"
//		                "Date: Mon, 27 Jul 2009 12:28:53 GMT\r\n"
//		                "Server: Apache/2.2.14 (Win32)\r\n"
//		                "Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT\r\n"
//		                "Content-Length: {}\r\n"
//		                "Content-Type: text/html\r\n"
//		                //	                "Connection: Closed\r\n"
//		                "Keep-Alive: timeout=5, max=99\r\n"
//		                "Connection: Keep-Alive\r\n"
//		                "\r\n"
//		                "{}";   //Body
//		// Unirà l'header con il bodi e ritornerà la stringa di invio HTML
//		string mes = fmt::format(h, b.length(), b);
//
//		cout << "try send 1° response\n";
//
//		if(c->sendStr(mes) == -1){
//			Log::db << "sendStr 1° come error " << strerror(errno) << "\n";
//			delete c;
//			continue;
//		}
//
//
//		///#################################################################################################
//
//		// Aspettiamo la connesione per il favicon.ico
////	if ((connsd = accept (listensd, (struct sockaddr *)NULL, NULL)) < 0){
////		perror ("errore in accept");
////		exit (1);
////	}
//
//		delete hHeader;
//		hHeader = c->readHttpHeader();
//
//		//Cerchiamo nel dizionario e troviamo l'indice
//		it = hHeader->cim.find("Accept");
//		cout << "Iterator points to " << it->first << " = " << it->second << endl;
//
//		//Otteniamo Separate tutte le accetazioni ammissibili
//
//		start = 0;
//		end = 0;
//		cout << "Img request :\n";
//		end = it->second.find(",", start + 1);
//		imgRequest = it->second.substr(start, end - start);
//		cout << imgRequest << "\n";
//		while(start < end){
//			start = end;
//			end = it->second.find(",", start + 1);
//			imgRequest = it->second.substr(start + 1, end - start - 1);
//			cout << imgRequest << "\n";
//		}
//
////    "Connection: Keep-Alive\n"
//		string s = "HTTP/1.1 200 OK\r\n"
//		           "Date: Wed, 15 Jul 2020 10:09:52 GMT\r\n"
//		           "Server: Apache/2.2.22 (Debian)\r\n"
//		           "Last-Modified: Thu, 16 Mar 2006 09:33:16 GMT\r\n"
//		           "Accept-Ranges: bytes\r\n"
//		           "Content-Length: 1035\r\n"
//		           "X-Content-Type-Options: nosniff\r\n"
//		           "Connection: Closed\r\n"
//		           "Content-Type: image/png\r\n"
//		           "\r\n";
//
//		if(c->sendStr(s) == -1){
//			Log::db << "sendStr 2° come error " << strerror(errno) << "\n";
//			delete c;
//			continue;
//		}
//
//		//todo: da argv farsi passare il pwd, dove sarà presente la cartella delle risorse
//		std::string filename = "/home/alfystar/Documenti/IIW2020/2_src/rfc.png";
//
////		if (!fileExists (filename)) // <- you need to implement this
////		{
////			if (sendStr (socket, "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: keep-alive\r\n\r\n") ==
////			    -1){
////				close (socket);
////			}
////		}
////		else{
////		int socket = c->fd;
//		std::ifstream in(filename, std::ios::binary);
////			if (!in.is_open ()){
////				std::cout << "failed to open file" << std::endl;
////				if (sendStr (socket, "HTTP/1.1 500 Error\r\nContent-Length: 0\r\nConnection: keep-alive\r\n\r\n") == -1){
////					close (socket);
////				}
////			}else{
////				std::cout << "file:" << filename << " not just Open" << std::endl;
//
//		in.seekg(0, std::ios::end);
//		std::size_t length = in.tellg();
//		in.seekg(0, std::ios::beg);
//
////				if (in.fail ()){
////					std::cout << "failed to get size of file" << std::endl;
////					if (sendStr (socket, "HTTP/1.1 500 Error\r\nContent-Length: 0\r\nConnection: keep-alive\r\n\r\n") == -1){
////						close (socket);
////					}
////					std::cout << "fail n1\n";
////				}else if (sendStr (socket, "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string (length) +
////				                          "\r\nConnection: keep-alive\r\nContent-Type: image/png\r\n\r\n") == -1){
////					close (socket);
////					std::cout << "fail n2\n";
////				}else
//		if(length > 0){
//			std::cout << "success\n";
//			char data[4096];
//			do{
//				if(!in.read(data, std::min(length, sizeof(data)))){
//					delete c;
//					Log::db << "Reading image file get error " << strerror(errno) << "\n";
//					continue;
//				}
//
//				int bytes = in.gcount();
//
//				if(c->sendData(data, bytes) == -1){
//					delete c;
//					Log::db << "Send image file get error " << strerror(errno) << "\n";
//					continue;
//				}
//
//				length -= bytes;
//			}
//			while(length > 0);
//			std::cout << "end While\n";
//		}
//		j->q->pushWaitCon(c);

	}

}
