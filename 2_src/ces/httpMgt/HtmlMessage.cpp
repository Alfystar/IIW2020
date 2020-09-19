//
// Created by alfystar on 11/09/20.
//

#include "HtmlMessage.h"

using namespace CES;

HtmlMessage::HtmlMessage (NCS::Connection::httpHeader &hHeader){
    pathBody = ".";
    pathBody.append(hHeader.path);

    if (pathBody.compare("./") == 0){
        //redirect to /index.html
        //HTTP/1.1 301 Moved Permanently
        //Location: ./index.html
        status = SimpleWeb::StatusCode::redirection_moved_permanently;
        redirect = "/index.html";
        typePayload = noBody;
    }
    else if (!fileExists(pathBody)){
        Log::err << "HtmlMessage::HtmlMessage file don't Exist" << std::endl;
        status = SimpleWeb::StatusCode::client_error_not_found;
        typePayload = text;
        pathBody = "./web/sys/404.html";
    }
    else{
        this->discoverFileTypeRequest();
    }

    imgRequest request;
    switch (typePayload){
        case text:
            htmlPageLoad();
            if (status == SimpleWeb::StatusCode::client_error_not_found){
                body = fmt::format(body, hHeader.path);
            }
            break;
        case imageData:
            acceptExtractor(hHeader, request);
            if (request.fileType.compare("") == 0){ // Non vuole immagini in risposta, ma la risorsa è un immagine
                status = SimpleWeb::StatusCode::client_error_unsupported_media_type;
                typePayload = noBody;
            }
            else{
                Resource rsc(pathBody, request.fileType, request.qFactor);
                pathBody = rsc.getPath();
                this->imageOpen();    // in caso di errore cambia il tipo di typePayload in text e ci scrive l'errore
            }
            break;
        case rawData:
            this->imageOpen();    // in caso di errore cambia il tipo di typePayload in text e ci scrive l'errore
            break;
        case noBody:
            // niente da caricare
            break;
        default:
            cout << "HtmlMessage::HtmlMessage raggiunto default!!!" << std::endl;
            break;
    }
    headerMount();
}

HtmlMessage::~HtmlMessage (){
    if (!inStream)
        delete inStream;
}

void HtmlMessage::htmlPageLoad (){
    // Verifico esistenza File
    if (!fileExists(pathBody)){
        Log::err << "HtmlMessage::htmlPageLoad file don't Exist" << std::endl;
        status = SimpleWeb::StatusCode::client_error_bad_request;
        typePayload = noBody;
        return;
    }

    //Provo ad aprire il file
    ifstream textFile (pathBody, std::ios::in);
    if (!textFile.is_open()){
        Log::err << "HtmlMessage::htmlPageLoad failed to open file" << std::endl;
        status = SimpleWeb::StatusCode::server_error_internal_server_error;
        typePayload = noBody;
        return;
    }

    //Provo a ottenere la dimensione del file
    textFile.seekg(0, std::ios::end);
    std::size_t len = textFile.tellg();
    textFile.seekg(0, std::ios::beg);

    if (textFile.fail()){
        Log::out << "HtmlMessage::htmlPageLoad failed to set at 0 seek of size" << std::endl;
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
        if (!textFile.getline(lineBuff, std::min(len, sizeof(lineBuff)))){
            Log::err << "HtmlMessage::htmlPageLoad Reading text from filesystem get error " << strerror(errno) << "\n";
            status = SimpleWeb::StatusCode::server_error_internal_server_error;
            typePayload = noBody;
            return;
        }

        bytes = textFile.gcount();

        if (textFile.rdstate() == ifstream::eofbit){ //Raggiunto End-Of-File Prematuramente
            Log::err << "HtmlMessage::htmlPageLoad Raggiunto End-Of-File Prematuramente" << endl;
            status = SimpleWeb::StatusCode::server_error_internal_server_error;
            typePayload = noBody;
            return;
        }

        if (textFile.rdstate() == ifstream::failbit){
            if (bytes == 0){ //non ho estratto nulla e c'è un problema sullo stream
                Log::err << "HtmlMessage::htmlPageLoad Non ha estratto nulla" << endl;
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
    }while (len > 0);
    body.append("\r\n\r\n");
    lenBody = body.length();
}

void HtmlMessage::imageOpen (){
    // Verifico esistenza File
    if (!fileExists(pathBody)) // <- you need to implement this
    {
        Log::err << "HtmlMessage::imageOpen file don't Exist" << std::endl;
        status = SimpleWeb::StatusCode::client_error_not_found;
        typePayload = noBody;
        return;
    }

    //Provo ad aprire il file
    inStream = new std::ifstream(pathBody, std::ios::binary);
    if (!inStream->is_open()){
        Log::err << "HtmlMessage::imageOpen failed to open file" << std::endl;
        status = SimpleWeb::StatusCode::server_error_internal_server_error;
        typePayload = noBody;

        delete inStream;
        return;
    }

    //Provo a ottenere la dimensione del file
    inStream->seekg(0, std::ios::end);
    lenBody = inStream->tellg();
    inStream->seekg(0, std::ios::beg);

    if (inStream->fail()){
        Log::err << "HtmlMessage::imageOpen failed to get size of file" << std::endl;
        status = SimpleWeb::StatusCode::server_error_internal_server_error;
        typePayload = noBody;

        delete inStream;
        return;
    }
}

void CES::HtmlMessage::headerMount (){
    header = fmt::format(startHead, SimpleWeb::status_code(status), dataNow());

    // Redirect header
    switch (status){
        case SimpleWeb::StatusCode::redirection_moved_permanently:
            header.append(fmt::format(redirectHead, redirect));
            break;
        default:
            break;
    }

    // Body information header
    switch (typePayload){
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

inline void HtmlMessage::discoverFileTypeRequest (){
    string file_extension = boost::filesystem::extension(pathBody);
    // boost è case insensitive
    if (file_extension.compare("") == 0){
        typePayload = noBody;
    }
    else if (boost::iequals(file_extension, ".html") || boost::iequals(file_extension, ".htm") ||
             boost::iequals(file_extension, ".txt")){

        typePayload = text;
    }
    else if (boost::iequals(file_extension, ".JPEG") || boost::iequals(file_extension, ".JFIF") ||
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

string HtmlMessage::dataNow (){
    // Creazione del campo DATA
    SimpleWeb::Date date;
    chrono::system_clock::time_point now = std::chrono::system_clock::now();
    string dataString = date.to_string(now);
    return dataString;
}

//Funzione che dato il path di un file, ritorna l'ultima modifica dello stesso
//Da trasformare per ritornare la stringa e non printarla
string HtmlMessage::lastChangeFile (const char *path){
    struct stat attr;
    stat(path, &attr);
    char *dataStr = ctime(&attr.st_mtime);
    dataStr[strlen(dataStr) - 2] = '\0';    //Rimuovo 'accapo automatico
    string lastDate = dataStr;
    return lastDate;
}

string HtmlMessage::lastChangeFile (string &path){
    return lastChangeFile(path.c_str());
}

inline bool HtmlMessage::fileExists (string &path){
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

void HtmlMessage::acceptExtractor (NCS::Connection::httpHeader &hHeader, imgRequest &img){
    //https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Accept
    //Accept: ... , <MIME_type>/<MIME_subtype>;q=<(q-factor weighting)>, ...

    auto it = hHeader.cim.find("Accept");
    #ifdef DEBUG_LOG
    Log::db << "[HtmlMessage::acceptExtractor] " << it->first << " = " << it->second << endl;
    #endif
    size_t start = it->second.find("image/");
    if (start == string::npos){  // se non trovo "<MIME_type>" => non accetta "image/"
        start = it->second.find("*/*");
        if (start == string::npos){  // se non trovo "<MIME_type>" => non accetta "image/"
            img.fileType = "";
            img.qFactor = 0;
            return;
        }
        else{
            img.fileType = "*";
            img.qFactor = 1;
            return;
        }
    }

    // Se "image/" è presente è necessario trocare qFactor & fileType
    size_t end = it->second.find(",", start);
    if (end == string::npos){ // se non trovo "," => ultima richiesta
        end = it->second.length();
    }
    start += 6; // mi sposto appena oltre lo "/" di "image/"
    size_t qIndex = it->second.find(";q=", start);
    if (qIndex == string::npos || qIndex > end){ // se non trovo ";q=", o è oltre => non specificato quality factor
        img.qFactor = 1;        //Quality factor di default
        img.fileType = it->second.substr(start, end - start);
    }
    else{  // se trovo ;q= => nel mezzo è presente il  <MIME_subtype>
        img.fileType = it->second.substr(start, qIndex - start);
        qIndex += 3;  //mi sposto appena oltre l'"=" di ";q="
        string q = it->second.substr(qIndex, end - qIndex);
        try{
            img.qFactor = std::stof(q);
        }catch (std::exception &e){
            cerr << e.what();
            cerr << "HtmlMessage::acceptExtractor qFactor string isn't number\n";
        }
    }
    return;
}