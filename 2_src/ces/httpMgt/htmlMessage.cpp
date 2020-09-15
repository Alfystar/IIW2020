//
// Created by alfystar on 11/09/20.
//

#include "htmlMessage.h"

using namespace CES;

htmlMessage::htmlMessage(NCS::Connection::httpHeader &hHeader){

	pathBody = ".";
	pathBody.append(hHeader.path);

	this->discoverFileTypeRequest();

	float q = 1;
	switch(typePayload){
		case text:
			htmlPageLoad();
			break;
		case imageData:
			q = qualityFactor(hHeader);
			cout << q << endl;
			this->imageOpen();    // in caso di errore cambia il tipo di typePayload in text e ci scrive l'errore
			break;
		case rawData:
			this->imageOpen();    // in caso di errore cambia il tipo di typePayload in text e ci scrive l'errore
			break;
		case noBody:
		default:
			if(pathBody.compare("./") == 0){
				//redirect to /index.html
				//HTTP/1.1 301 Moved Permanently
				//Location: ./index.html
				status = SimpleWeb::StatusCode::redirection_moved_permanently;
				redirect = "/index.html";
			}
			else{
				//richiesta invalida, ritornare un errore
				status = SimpleWeb::StatusCode::client_error_not_found;
			}
			break;
	}

	if(status == SimpleWeb::StatusCode::client_error_not_found){
		pathBody = "./404.html";
		htmlPageLoad();
		body = fmt::format(body, hHeader.path);
		status = SimpleWeb::StatusCode::client_error_not_found;
	}

	headerMount();

}

htmlMessage::~htmlMessage(){
	if(!inStream)
		delete inStream;
}


void htmlMessage::htmlPageLoad(){
	// Verifico esistenza File
	if(!fileExists(pathBody)) // <- you need to implement this
	{
		Log::err << "htmlMessage::htmlPageLoad file don't Exist" << std::endl;
		status = SimpleWeb::StatusCode::client_error_bad_request;
		typePayload = noBody;
		return;
	}

	//Provo ad aprire il file
	ifstream textFile(pathBody, std::ios::in);
	if(!textFile.is_open()){
		Log::err << "htmlMessage::htmlPageLoad failed to open file" << std::endl;
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
			Log::err << "htmlMessage::htmlPageLoad Reading text from filesystem get error " << strerror(errno) << "\n";
			status = SimpleWeb::StatusCode::server_error_internal_server_error;
			typePayload = noBody;
			return;
		}

		bytes = textFile.gcount();

		if(textFile.rdstate() == ifstream::eofbit){ //Raggiunto End-Of-File Prematuramente
			Log::err << "htmlMessage::htmlPageLoad Raggiunto End-Of-File Prematuramente" << endl;
			status = SimpleWeb::StatusCode::server_error_internal_server_error;
			typePayload = noBody;
			return;
		}

		if(textFile.rdstate() == ifstream::failbit){
			if(bytes == 0){ //non ho estratto nulla e c'è un problema sullo stream
				Log::err << "htmlMessage::htmlPageLoad Non ha estratto nulla" << endl;
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
	lenBody = body.length();
	status = SimpleWeb::StatusCode::success_ok;
}

void htmlMessage::imageOpen(){
	// Verifico esistenza File
	if(!fileExists(pathBody)) // <- you need to implement this
	{
		Log::err << "htmlMessage::imageOpen file don't Exist" << std::endl;
		status = SimpleWeb::StatusCode::client_error_not_found;
		typePayload = noBody;
		return;
	}

	//Provo ad aprire il file
	inStream = new std::ifstream(pathBody, std::ios::binary);
	if(!inStream->is_open()){
		Log::err << "htmlMessage::imageOpen failed to open file" << std::endl;
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
		Log::err << "htmlMessage::imageOpen failed to get size of file" << std::endl;
		status = SimpleWeb::StatusCode::server_error_internal_server_error;
		typePayload = noBody;

		delete inStream;
		return;
	}
	status = SimpleWeb::StatusCode::success_ok;
}

void CES::htmlMessage::headerMount(){

	header = fmt::format(startHead, SimpleWeb::status_code(status), dataNow());

	// Redirect header
	switch(status){
		case SimpleWeb::StatusCode::redirection_moved_permanently:
			header.append(fmt::format(redirectHead, redirect));
			break;
		default:
			break;
	}

	// Body information header
	switch(typePayload){
		case text:
			header.append(fmt::format(bodyDataHead, "text/html", lastChangeFile(pathBody)));
			break;
		case imageData:
		case rawData:
			header.append(fmt::format(bodyDataHead, "image/png", lastChangeFile(pathBody)));
			break;
		default:
			break;
	}

	// Ending header
	header.append(fmt::format(endHead, lenBody));

}


inline void htmlMessage::discoverFileTypeRequest(){
	string file_extension = boost::filesystem::extension(pathBody);
	// boost è case insensitive
	if(file_extension.compare("") == 0){
		typePayload = noBody;
	}
	else if(boost::iequals(file_extension, ".html") || boost::iequals(file_extension, ".htm") ||
	        boost::iequals(file_extension, ".txt")){

		typePayload = text;
	}
	else if(boost::iequals(file_extension, ".JPEG") || boost::iequals(file_extension, ".JFIF") ||
	        boost::iequals(file_extension, ".JPG") || boost::iequals(file_extension, ".Exif") ||
	        boost::iequals(file_extension, ".TIFF") || boost::iequals(file_extension, ".GIF") ||
	        boost::iequals(file_extension, ".BMP") || boost::iequals(file_extension, ".PNG") ||
	        boost::iequals(file_extension, ".WebP") || boost::iequals(file_extension, ".ico")){//image
		typePayload = imageData;
	}
	else{
		typePayload = rawData;
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
string htmlMessage::lastChangeFile(const char *path){
	struct stat attr;
	stat(path, &attr);
	char *dataStr = ctime(&attr.st_mtime);
	dataStr[strlen(dataStr) - 2] = '\0';    //Rimuovo 'accapo automatico
	string lastDate = dataStr;
	return lastDate;
}

string htmlMessage::lastChangeFile(string &path){
	return lastChangeFile(path.c_str());
}


inline bool htmlMessage::fileExists(string path){
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0);
}

float htmlMessage::qualityFactor(NCS::Connection::httpHeader &hHeader){

	// Piccolo script che si scorre tutti i paramentri del campo Accept dell'header request
	//Cerchiamo nel dizionario e troviamo l'indice
	auto it = hHeader.cim.find("Accept");
	cout << "Iterator points to " << it->first << " = " << it->second << endl;
//
	int start = it->second.find("image/");
	int qIndex = it->second.find("q=", start);
	int end = it->second.find(",", end);
	std::string imgRequest = it->second.substr(start, end - start);
	cout << "\nImg request :\n" << imgRequest << "\n";

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
	return 0;
}
