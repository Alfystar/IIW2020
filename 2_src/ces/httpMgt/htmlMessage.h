//
// Created by alfystar on 11/09/20.
//

#ifndef HTTP_IMAGESERVER_HTMLMESSAGE_H
#define HTTP_IMAGESERVER_HTMLMESSAGE_H


#include <cstdio>
#include <string>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <fmt/format.h>
#include <utility.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <Log.h>


namespace CES{
	using namespace std;
	enum payloadType : char{text, imageData, rawData, noBody};

	class htmlMessage{
	public:
		SimpleWeb::StatusCode status = SimpleWeb::StatusCode::success_ok;
		string header;
		string pathBody;
		std::size_t lenBody = 0;
		string body;
		ifstream *inStream = nullptr;

		payloadType typePayload = noBody;

	private:
		static string compilingHeader;
	public:


		htmlMessage() = delete;

		htmlMessage(string &path);  //Crea la classe e l'header

		~htmlMessage();

	private:
		void htmlPageLoad();

		void
		imageOpen();               //Cerca di caricare l'immagine, in caso contrario imposta lo status e il messaggio di riposta diventa senza body, solo l'errore
		void headerMount();             //header normali pagine HTML

		void discoverFileTypeRequest();

		string dataNow();

		string getFileCreationTime(const char *path);

		string getFileCreationTime(string &path);

		bool fileExists(string path);
	};

}


#endif //HTTP_IMAGESERVER_HTMLMESSAGE_H
