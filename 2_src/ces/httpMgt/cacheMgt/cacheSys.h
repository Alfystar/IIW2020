//
// Created by alfystar on 22/07/20.
//

#ifndef HTTP_IMAGESERVER_CACHESYS_H
#define HTTP_IMAGESERVER_CACHESYS_H

// Gestione della directory cache e img con interfaccia tale che, data immagine sorgente, e caratteristiche di uscita
// o la elabora e la salva o la richiama, e passa ad httpMgt il path (relativo o full da vedere) della immagine che deve essere inviata

// aggiungere un thread shrader per la gestione della pulizia di memoria, all'interno del file-system


// C++17 per leggere tutti i file di una cartella e sotto file
//#include <string>
//#include <iostream>
//#include <filesystem>
//namespace fs = std::filesystem;
//
//int main()
//{
//	std::string path = "/path/to/directory";
//	for (const auto & entry : fs::directory_iterator(path))
//		std::cout << entry.path() << std::endl;
//}
//
// Tramite stat, dato il path di ogni file, possiamo ottenere l'ultimo accesso e con quello creare una coda di priorità, per le eliminazioni


#include  <shared_mutex>
// Per impostare la priorità dei lockExlusive sui lockShared, per il thread shrader
//pthread_rwlockattr_setkind_np(std::shared_lock<>::native_handle,PTHREAD_RWLOCK_PREFER_WRITER_NP)

class cacheSys{

};


#endif //HTTP_IMAGESERVER_CACHESYS_H
