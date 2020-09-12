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

HttpMgt::HttpMgt(){

}

Action HttpMgt::connectionRequest(NCS::Connection *c){
	NCS::Connection::httpHeader *hHeader = c->readHttpHeader();
	if(!hHeader){
		Log::out << "hHeader = null \n";
		delete c;
		return ConClosed;
	}
	Action actionRet;


	htmlMessage mes(hHeader->path);


	//La libreria boost è case insensitive
	if(boost::iequals(hHeader->method, "GET")){

		actionRet = stringSend(c, mes.header);
		if(actionRet != RequestComplete)
			return actionRet;

		switch(mes.typePayload){
			case text:
				actionRet = stringSend(c, mes.body);
				if(actionRet != RequestComplete)
					return actionRet;
				break;
			case imageData:
			case rawData:
				actionRet = rawSend(c, mes);
				if(actionRet != RequestComplete)
					return actionRet;
				break;
			case noBody:
				break;
		}
	}
	else if(boost::iequals(hHeader->method, "HEAD")){
		actionRet = stringSend(c, mes.header);
		if(actionRet != RequestComplete)
			return actionRet;
	}
	else{
		//connessione valida ma richiesta non gestita
		// todo: header dove si dice operazione invalida
	}

	return RequestComplete;
}

Action HttpMgt::stringSend(NCS::Connection *c, string &msg){
	if(c->sendStr(msg) == -1){
		switch(errno){
			case EPIPE:
				perror("Epipe sendStr");
				Log::db << "sendStr come brokenPipe error\n";
				delete c;
				return ConClosed;
			default:
				Log::db << "sendStr come error " << strerror(errno) << "\n";
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
				Log::db << "Reading image from filesystem get error " << strerror(errno) << "\n";
				return ConClosed;
			}

			int bytes = msg.inStream->gcount();

			if(c->sendData(data, bytes) == -1){
				switch(errno){
					case EPIPE:
						perror("Epipe sendData");
						Log::db << "Send image to socket get brokenPipe error\n";
						delete c;
						return ConClosed;
					default:
						Log::db << "Send image to socket get error " << strerror(errno) << "\n";
						delete c;
						return ConClosed;
				}
			}

			lenght -= bytes;
		}
		while(lenght > 0);
		Log::out << "end While\n";
	}
	return RequestComplete;

}