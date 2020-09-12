//
// Created by alfystar on 22/07/20.
//

#ifndef HTTP_IMAGESERVER_HTTPMGT_H
#define HTTP_IMAGESERVER_HTTPMGT_H

// Sistema che presa una connessione http, ne gestisce tutta quanta l'elaborazione e le risposte in base alle domande
// se la richesta è senza ulr mandare redirection_moved_permanently verso la home
// client:=     Host: en.wikipedia.org
// ma noi volgliamo venga richiesta una home, quindi nell'header di risposta tra i vari campi:
// server:=     location: https://en.wikipedia.org/wiki/Main_Page

// chi sviluppa dovrà decidere se usare eccezioni o codici di ritorni (in questo ultimo caso si suggeriscono gli enum)

//Al worker deve arrivare il feedback se la connessione va mantenuta e quindi mandata indietro alla queue o scartata

#include <cstdio>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <boost/algorithm/string.hpp>

#include <NCS.h>
#include <Connection.h>

#include <Log.h>
#include "htmlMessage.h"

namespace CES{
	using namespace std;
	enum Action{RequestComplete, ConClosed};

	class HttpMgt{
	public:
		HttpMgt();

		Action connectionRequest(NCS::Connection *c);

	private:
		Action stringSend(NCS::Connection *c, string &msg);

		Action rawSend(NCS::Connection *c, htmlMessage &msg);
	};
}

#endif //HTTP_IMAGESERVER_HTTPMGT_H
