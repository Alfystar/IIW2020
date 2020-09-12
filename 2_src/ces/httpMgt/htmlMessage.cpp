//
// Created by alfystar on 11/09/20.
//

#include "htmlMessage.h"

using namespace CES;

htmlMessage::htmlMessage(std::string &path){

	pathBody = ".";
	pathBody.append(path);

	this->discoverFileTypeRequest();


	switch(typePayload){
		case text:
			htmlPageLoad();
			break;
		case imageData:
		case rawData:
			imageOpen();    // in caso di errore cambia il tipo di typePayload in text e ci scrive l'errore
			break;
		case noBody:
		default:
			if(path.compare("")){
				//todo: implementare redirect to /index.html
				//HTTP/1.1 301 Moved Permanently
				//Location: http://www.example.org/index.asp
			}
			else{
				//todo: richiesta invalida, ritornare un errore
				status = SimpleWeb::StatusCode::client_error_bad_request;
			}

			break;
	}

	headerMount();

}

htmlMessage::~htmlMessage(){
//	if (!body)
//		delete body;
	if(!inStream)
		delete inStream;
//	switch(typePayload){
//		case text:
//			delete body;
//			break;
//		case noBody:
//			break;
//		case imageData:
//		case rawData:
//		default:
//			delete inStream;
//			break;
//	}
}


void htmlMessage::htmlPageLoad(){
	// Verifico esistenza File
	if(!fileExists(pathBody)) // <- you need to implement this
	{
		Log::out << "htmlMessage::htmlPageLoad file don't Exist" << std::endl;
		status = SimpleWeb::StatusCode::client_error_bad_request;
		typePayload = noBody;
		return;
	}

	//Provo ad aprire il file
	ifstream textFile(pathBody, std::ios::in);
	if(!textFile.is_open()){
		Log::out << "htmlMessage::htmlPageLoad failed to open file" << std::endl;
		status = SimpleWeb::StatusCode::server_error_internal_server_error;
		typePayload = noBody;
		return;
	}
	//Provo a ottenere la dimensione del file
	textFile.seekg(0, std::ios::end);
	std::size_t len = textFile.tellg();
	textFile.seekg(0, std::ios::beg);

	if(textFile.fail()){
		Log::out << "htmlMessage::htmlPageLoad failed to set at 0 seek of size" << std::endl;
		status = SimpleWeb::StatusCode::server_error_internal_server_error;
		typePayload = noBody;
		return;
	}

	// Estraggo il testo e aggiungo a fine linea "\r\n"
	typePayload = text;
	char lineBuff[4096];
	body = "";
	int bytes;
	do{
		if(!textFile.getline(lineBuff, std::min(len, sizeof(lineBuff)))){
			Log::out << "htmlMessage::htmlPageLoad Reading text from filesystem get error " << strerror(errno) << "\n";
//			delete body;
			status = SimpleWeb::StatusCode::server_error_internal_server_error;
			typePayload = noBody;
			return;
		}

		bytes = textFile.gcount();

		if(textFile.rdstate() == ifstream::eofbit){ //Raggiunto End-Of-File Prematuramente
			Log::out << "htmlMessage::htmlPageLoad Raggiunto End-Of-File Prematuramente" << endl;
//			delete body;
			status = SimpleWeb::StatusCode::server_error_internal_server_error;
			typePayload = noBody;
			return;
		}

		if(textFile.rdstate() == ifstream::failbit){
			if(bytes == 0){ //non ho estratto nulla e c'è un problema sullo stream
				Log::out << "htmlMessage::htmlPageLoad Non ha estratto nulla" << endl;
//				delete body;
				status = SimpleWeb::StatusCode::server_error_internal_server_error;
				typePayload = noBody;
				return;
			}
			else{  // in 4096 caratteri non è stato trovato '\n'
				//Tutto ok, copio il buffer senza mettere "\r\n"
				body.append(lineBuff);
			}
		}

		body.append(lineBuff);
		body.append("\r\n");
		len -= bytes;
	}
	while(len > 0);
	body.append("\r\n\r\n");
	status = SimpleWeb::StatusCode::success_ok;
}

void htmlMessage::imageOpen(){
	// Verifico esistenza File
	if(!fileExists(pathBody)) // <- you need to implement this
	{
		Log::out << "htmlMessage::imageOpen file don't Exist" << std::endl;

		status = SimpleWeb::StatusCode::client_error_bad_request;
		typePayload = noBody;
		return;
	}

	//Provo ad aprire il file
	inStream = new std::ifstream(pathBody, std::ios::binary);
	if(!inStream->is_open()){
		Log::out << "htmlMessage::imageOpen failed to open file" << std::endl;
		status = SimpleWeb::StatusCode::server_error_internal_server_error;
		typePayload = noBody;

		delete inStream;
		return;
	}

	//Provo a ottenere la dimensione del file
	inStream->seekg(0, std::ios::end);
	lenBody = inStream->tellg();
	inStream->seekg(0, std::ios::beg);

	if(inStream->fail()){
		Log::out << "htmlMessage::imageOpen failed to get size of file" << std::endl;
		status = SimpleWeb::StatusCode::server_error_internal_server_error;
		typePayload = noBody;

		delete inStream;
		return;
	}
	status = SimpleWeb::StatusCode::success_ok;
}


string htmlMessage::compilingHeader = "HTTP/1.1 {}\r\n"
                                      "Date: {}\r\n"
                                      "Server: BadAlpha 0.1 (unix)\r\n"
                                      "Last-Modified: {}\r\n"
                                      "Accept-Ranges: bytes\r\n"
                                      "Content-Length: {}\r\n"
                                      "Content-Type: {}\r\n"         //image/png or text/html
                                      "Keep-Alive: timeout=5, max=99\r\n"
                                      "Connection: Keep-Alive\r\n"
                                      "\r\n";


void CES::htmlMessage::headerMount(){
	// I campi {} vengono scritti parametricamente

	switch(typePayload){
		case text:
			lenBody = body.length();
			header = fmt::format(compilingHeader, SimpleWeb::status_code(status), dataNow(),
			                     getFileCreationTime(pathBody), lenBody, "text/html");
			break;
		case noBody:
			header = fmt::format(compilingHeader, SimpleWeb::status_code(status), dataNow(), "", 0, "");
			break;
		case imageData:
		case rawData:
		default:
			header = fmt::format(compilingHeader, SimpleWeb::status_code(status), dataNow(),
			                     getFileCreationTime(pathBody), lenBody, "image/png");
			break;
	}

}


inline void htmlMessage::discoverFileTypeRequest(){
	string file_extension = boost::filesystem::extension(pathBody);
	// boost è case insensitive
	if(file_extension.compare("") == 0){
		typePayload = text;
	}
	else if(boost::iequals(file_extension, ".html") || boost::iequals(file_extension, ".htm") ||
	        boost::iequals(file_extension, ".txt")){

		typePayload = text;
	}
	else if(boost::iequals(file_extension, ".JPEG") || boost::iequals(file_extension, ".JFIF") ||
	        boost::iequals(file_extension, ".JPEG") || boost::iequals(file_extension, ".Exif") ||
	        boost::iequals(file_extension, ".TIFF") || boost::iequals(file_extension, ".GIF") ||
	        boost::iequals(file_extension, ".BMP") || boost::iequals(file_extension, ".PNG") ||
	        boost::iequals(file_extension, ".WebP") || boost::iequals(file_extension, ".ico")){//image
		typePayload = imageData;
	}
	else{
		typePayload = noBody;
	}
}


string htmlMessage::dataNow(){
	// Creazione del campo DATA
	SimpleWeb::Date date;
	chrono::system_clock::time_point now = std::chrono::system_clock::now();
	string dataString = date.to_string(now);
	return dataString;
}

//Funzione che dato il path di un file, ritorna l'ultima modifica dello stesso
//Da trasformare per ritornare la stringa e non printarla
string htmlMessage::getFileCreationTime(const char *path){
	struct stat attr;
	stat(path, &attr);
	char *dataStr = ctime(&attr.st_mtime);
	dataStr[strlen(dataStr) - 2] = '\0';    //Rimuovo 'accapo automatico
	string lastDate = dataStr;
	return lastDate;
//	printf("Last modified time: %s", ctime(&attr.st_mtime));
}

string htmlMessage::getFileCreationTime(string &path){
	return getFileCreationTime(path.c_str());
}


inline bool htmlMessage::fileExists(string path){
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0);
}