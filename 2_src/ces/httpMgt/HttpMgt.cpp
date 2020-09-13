//
// Created by alfystar on 22/07/20.
//

// Piccolo script che si scorre tutti i paramentri del campo Accept dell'header request
//	//Cerchiamo nel dizionario e troviamo l'indice
//	auto it = hHeader->cim.find("Accept");
//	Log::out << "Iterator points to " << it->first << " = " << it->second << endl;
//
////		int start = it->second.find("image/");
////		int end = it->second.find(",",start);
////		std::string imgRequest = it->second.substr(start, end-start);
////		Log::out << "\nImg request :\n" << imgRequest << "\n";
//
//	//Otteniamo Separate tutte le accetazioni ammissibili
//
//	int start = 0;
//	int end = 0;
//	Log::out << "Img request :\n";
//	end = it->second.find(",", start + 1);
//	std::string imgRequest = it->second.substr(start, end - start);
//	Log::out << imgRequest << "\n";
//	while(start < end){
//		start = end;
//		end = it->second.find(",", start + 1);
//		imgRequest = it->second.substr(start + 1, end - start - 1);
//		Log::db << imgRequest << "\n";
//	}
//
//	it = hHeader->cim.find("User-Agent");
//	Log::db << "Iterator points to " << it->first << " = " << it->second << endl;

#include "HttpMgt.h"

using namespace CES;
using namespace SimpleWeb;

HttpMgt::HttpMgt(){

}

Action HttpMgt::connectionRequest(NCS::Connection *c){
	NCS::Connection::httpHeader *hHeader = c->readHttpHeader();
	if(!hHeader){
		#ifdef DEBUG_LOG
		Log::db << "HttpMgt::connectionRequest hHeader = null \n";
		#endif
		delete c;
		return ConClosed;
	}
	Action actionRet;



	//La libreria boost è case insensitive
	SimpleWeb::StatusCode code;
	if(boost::iequals(hHeader->method, "GET"))
		code = StatusCode::success_ok;
	else if(boost::iequals(hHeader->method, "HEAD"))
		code = StatusCode::success_no_content;
	else
		code = StatusCode::client_error_not_acceptable;

	// Cambio la pagina se il metodo è non gestito
	if(code == StatusCode::client_error_not_acceptable){ //connessione valida ma richiesta non gestita
		hHeader->path = "/406.html";
	}

	htmlMessage mes(hHeader->path);


	switch(code){
		case StatusCode::success_ok: //get
			actionRet = send(c, mes);
			if(actionRet != RequestComplete)
				return actionRet;
			break;
		case StatusCode::success_no_content: // head
			actionRet = stringSend(c, mes.header);
			if(actionRet != RequestComplete)
				return actionRet;
			break;
		case StatusCode::client_error_not_acceptable:
			mes.status = code;
			mes.body = fmt::format(mes.body, hHeader->method);
			actionRet = send(c, mes);
			if(actionRet != RequestComplete)
				return actionRet;
			break;
			break;
		default:
			// Connessione invalida
			break;
	}

	return RequestComplete;
}

Action HttpMgt::send(NCS::Connection *c, htmlMessage &msg){
	Action actionRet;

	actionRet = stringSend(c, msg.header);
	if(actionRet != RequestComplete)
		return actionRet;

	switch(msg.typePayload){
		case text:
			actionRet = stringSend(c, msg.body);
			break;
		case imageData:
		case rawData:
			actionRet = rawSend(c, msg);
			break;
		case noBody:
			break;
	}
	return actionRet;
}

Action HttpMgt::stringSend(NCS::Connection *c, string &msg){
	if(c->sendStr(msg) == -1){
		switch(errno){
			case EPIPE:
				#ifdef DEBUG_LOG
				Log::db << "HttpMgt::stringSend sendStr come brokenPipe error\n";
				#endif
				delete c;
				return ConClosed;
			default:
				#ifdef DEBUG_LOG
				Log::db << "HttpMgt::stringSend sendStr come error " << strerror(errno) << "\n";
				#endif
				delete c;
				return ConClosed;
		}
	}
	return RequestComplete;
}

Action HttpMgt::rawSend(NCS::Connection *c, htmlMessage &msg){
	std::size_t lenght = msg.lenBody;
	if(lenght > 0){
		char data[4096];
		do{
			if(!msg.inStream->read(data, std::min(lenght, sizeof(data)))){
				delete c;
				#ifdef DEBUG_LOG
				Log::db << "HttpMgt::rawSend Reading image from filesystem get error " << strerror(errno) << "\n";
				#endif
				return ConClosed;
			}

			int bytes = msg.inStream->gcount();

			if(c->sendData(data, bytes) == -1){
				switch(errno){
					case EPIPE:
						perror("Epipe sendData");
						#ifdef DEBUG_LOG
						Log::db << "HttpMgt::rawSend Send image to socket get brokenPipe error\n";
						#endif
						delete c;
						return ConClosed;
					default:
						#ifdef DEBUG_LOG
						Log::db << "HttpMgt::rawSend Send image to socket get error " << strerror(errno) << "\n";
						#endif
						delete c;
						return ConClosed;
				}
			}

			lenght -= bytes;
		}
		while(lenght > 0);
	}
	return RequestComplete;

}


