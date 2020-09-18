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
#include <Utility.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <Log.h>
#include <Connection.h>

#include "headerForm.h"

#include <Resource.h>


namespace CES {
    using namespace std;
    enum payloadType : char {
        text, imageData, rawData, noBody
    };
    struct imgRequest_ {
        float qFactor;      // numero di default1 o 0 se image/ non è presente
        string fileType;    // "*" o "nome Tipo" se c'è, "" se image/ non è presente e neanche */*
    };
    typedef struct imgRequest_ imgRequest;

    class HtmlMessage {
    public:
        SimpleWeb::StatusCode status = SimpleWeb::StatusCode::success_ok; // se non succede nulla si mantiene
        string header;
        string pathBody;
        std::size_t lenBody = 0;
        string body;
        ifstream *inStream = nullptr;

        payloadType typePayload = noBody;

    private:
        string redirect;
    public:

        HtmlMessage() = delete;

        HtmlMessage(NCS::Connection::httpHeader &hHeader);  //Crea la classe e l'header

        ~HtmlMessage();

    private:
        void htmlPageLoad();

        // In caso di errore viene cambiato status, così da montare conseguentemente l'header
        void imageOpen();               //Carica immagine
        void headerMount();             //header normali pagine HTML

        void discoverFileTypeRequest();

        string dataNow();

        string lastChangeFile(const char *path);

        string lastChangeFile(string &path);

        bool fileExists(string path);

        void acceptExtractor(NCS::Connection::httpHeader &hHeader, imgRequest &img);
    };

}


#endif //HTTP_IMAGESERVER_HTMLMESSAGE_H
