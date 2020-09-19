//
// Created by alfystar on 12/09/20.
//

#ifndef HTTP_IMAGESERVER_HEADERFORM_H
#define HTTP_IMAGESERVER_HEADERFORM_H


#include <cstdio>
#include <string>

/// Stringhe parametriche da compilare e montare per creare l'header
// I campi {} vengono scritti parametricamente tramite format

namespace CES {
    using namespace std;
    const string startHead = "HTTP/1.1 {}\r\n"               // Status code della risposta (in base ad essa si scelgono i campi futuri)
                             "Date: {}\r\n";                 // Data

    const string bodyDataHead = "Content-Type: {}\r\n"          // Tipo di file image/png or text/html
                                "Last-Modified: {}\r\n";        // Ultima modifica dello stesso

    const string redirectHead = "Location: {}\r\n";             // full-url o path-relativo della risorsa (es: /index.html)

    const string endHead = "Content-Length: {}\r\n"        // Lunghezza in byte della comunicazione
                           "Server: BadAlpha 0.1 (unix)\r\n"
                           "Accept-Ranges: bytes\r\n"
                           "Keep-Alive: timeout=5, max=99\r\n"
                           "Connection: Keep-Alive\r\n"
                           "\r\n";
}
#endif //HTTP_IMAGESERVER_HEADERFORM_H
