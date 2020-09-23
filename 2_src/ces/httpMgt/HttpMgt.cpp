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
        return ConClosed;
    }

    //La libraries boost are case insensitive
    Method code;
    if (boost::iequals(hHeader->method, "GET"))
        code = Get;
    else if (boost::iequals(hHeader->method, "HEAD"))
        code = Head;
    else
        code = Other;

    // Redirect to error page if the method aren't management
    if (code == Other){ //Connection valid but requested method not management
        hHeader->path = "/web/sys/406.html";
    }

    HtmlMessage mes(*hHeader);
    Action ret;
    switch (code){
        case Get:
            ret = sendGet(c, mes);
            break;
        case Head:
            ret = sendHead(c, mes);
            break;
        case Other:
            ret = sendMethodInvalid(c, mes, hHeader);
            break;
        default:
            cerr << "[HttpMgt::connectionRequest] Value on the Switch unexpected!!\n";
            ret = ConClosed;
    }

    if (mes.conType == KeepAlive)
        return ret;

    return ConClosed;
}

Action HttpMgt::sendGet (NCS::Connection *c, HtmlMessage &msg){
    Action actionRet;

    actionRet = stringSend(c, msg.header);
    if (actionRet == ConClosed){
        return ConClosed;
    }

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

Action HttpMgt::sendHead (NCS::Connection *c, HtmlMessage &msg){
    Action actionRet = stringSend(c, msg.header);

    return actionRet;
}

Action HttpMgt::sendMethodInvalid (NCS::Connection *c, HtmlMessage &msg, NCS::Connection::httpHeader *hHeader){
    msg.status = StatusCode::client_error_not_acceptable;
    msg.body = fmt::format(msg.body, hHeader->method);
    return sendGet(c, msg); //Strutturalmente come una get, ma è un codice di errore
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
    std::size_t len = msg.lenBody;
    if (len > 0){
        char data[4096];
        do{
            if (!msg.inStream->read(data, std::min(len, sizeof(data)))){
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
            len -= bytes;
        }while (len > 0);
    }
    return RequestComplete;
}