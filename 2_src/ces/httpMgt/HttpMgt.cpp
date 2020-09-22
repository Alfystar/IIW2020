//
// Created by alfystar on 22/07/20.
//

#include "HttpMgt.h"

using namespace CES;
using namespace SimpleWeb;

HttpMgt::HttpMgt (){}

Action HttpMgt::connectionRequest (NCS::Connection *c){
    NCS::Connection::httpHeader *hHeader = c->readHttpHeader();
    if (!hHeader){
        #ifdef DEBUG_LOG
        Log::db << "[HttpMgt::connectionRequest] hHeader = null \n";
        #endif
//        delete c;
        return ConClosed;
    }
    Action actionRet;

    //La libreria boost è case insensitive
    SimpleWeb::StatusCode code;
    if (boost::iequals(hHeader->method, "GET"))
        code = StatusCode::success_ok;
    else if (boost::iequals(hHeader->method, "HEAD"))
        code = StatusCode::success_no_content;
    else
        code = StatusCode::client_error_not_acceptable;

    // Cambio la pagina se il metodo è non gestito
    if (code == StatusCode::client_error_not_acceptable){ //connessione valida ma richiesta non gestita
        hHeader->path = "/web/sys/406.html";
    }

    HtmlMessage mes(*hHeader);

    switch (code){
        case StatusCode::success_ok: //get
            actionRet = send(c, mes);
            if (actionRet == RequestComplete)
                return RequestComplete;
            break;
        case StatusCode::success_no_content: // head
            actionRet = stringSend(c, mes.header);
            if (actionRet == RequestComplete)
                return RequestComplete;
            break;
        default:            // Connessione non gestita o errore, quindi da chiudere
            mes.status = code;
            mes.body = fmt::format(mes.body, hHeader->method);
            send(c, mes);
            break;
    }
    return ConClosed;
}

Action HttpMgt::send (NCS::Connection *c, HtmlMessage &msg){
    Action actionRet;

    actionRet = stringSend(c, msg.header);
    if (actionRet == ConClosed)
        return ConClosed;

    switch (msg.typePayload){
        case text:
            actionRet = stringSend(c, msg.body);
            break;
        case imageData:
        case rawData:
            actionRet = binarySend(c, msg);
            break;
        case noBody:
            break;
    }
    return actionRet;
}

Action HttpMgt::stringSend (NCS::Connection *c, string &msg){
    if (c->sendStr(msg) == -1){
        switch (errno){
            case EPIPE:
                #ifdef DEBUG_LOG
                Log::db << "[HttpMgt::stringSend] sendStr come brokenPipe error\n";
                #endif
                return ConClosed;
            default:
                #ifdef DEBUG_LOG
                Log::db << "[HttpMgt::stringSend] sendStr come error " << strerror(errno) << "\n";
                #endif
                return ConClosed;
        }
    }
    return RequestComplete;
}

Action HttpMgt::binarySend (NCS::Connection *c, HtmlMessage &msg){
    std::size_t lenght = msg.lenBody;
    if (lenght > 0){
        char data[4096];
        do{
            if (!msg.inStream->read(data, std::min(lenght, sizeof(data)))){
                //                delete c;
                #ifdef DEBUG_LOG
                Log::db << "[HttpMgt::rawSend] Reading image from filesystem get error " << strerror(errno) << "\n";
                #endif
                return ConClosed;
            }

            int bytes = msg.inStream->gcount();

            if (c->sendData(data, bytes) == -1){
                switch (errno){
                    case EPIPE:
                        perror("[HttpMgt::binarySend] Epipe sendData");
                        return ConClosed;
                    default:
                        return ConClosed;
                }
            }
            lenght -= bytes;
        }while (lenght > 0);
    }
    return RequestComplete;
}