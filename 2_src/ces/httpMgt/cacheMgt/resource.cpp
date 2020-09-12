//
// Created by filippob on 11/09/20.
//

#include "resource.h"

using namespace CES;


Resource::Resource(string &p, float &qValue) {

    // lock shredder_mutex

    string scale = to_string((int)round(qValue*100));
    string tmp  = p.substr(0, p.length()-4) + "_" + scale + "_.jpg";
    path = tmp.c_str();

    // lock open_mutex

    if ((fd = open(path, O_CREAT|O_EXCL, S_IRUSR|S_IWUSR) )== -1) { // provo a creare il file
        if (errno != EEXIST) {
            perror("[Resource]Error occurred at 1st open: ");
        }
        cout << path << " already exists"; // a questo punto il file è stato creato
    }

    // unlock open_mutex

}

Resource::~Resource() {
//destroy the resource, then close the fd and delete the file

    close(fd);
    // unlock shredder_mutex


}


string Resource::getPath() {

    if (!path) return string(path); //todo: chiedere a manu il ritorno di stringhe
    else return path;
}