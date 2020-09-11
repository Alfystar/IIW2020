//
// Created by filippob on 11/09/20.
//

#include "resource.h"

using namespace CES;


Resource::Resource( float &qValue) {

    string scale = to_string((int)round(qValue*100));
    string tmp  = "filesCache/image_" + scale + "_.jpg";
    path = tmp.c_str();

    // open_mutex

    if ((fd = open(path, O_CREAT|O_EXCL, S_IRUSR|S_IWUSR) )== -1) { // provo a creare il file
        if (errno != EEXIST) {
            perror("[Resource]Error occurred at 1st open: ");
        }
        cout << path << " already exists"; // a questo punto il file è stato creato
    }



}

Resource::~Resource() {
//destroy the resource, then close the fd and delete the file



}


string Resource::getPath() {

    if (!path) return string(path); //todo: chiedere a manu il ritorno di stringhe
    else return path;
}