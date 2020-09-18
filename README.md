# IIW2020
Progetto di IIW Web Server

Sviluppato in C++ per il corso di Ingegneria di Internet

Tenuto dal professore: Francesco Lo Presti

### Sviluppato da:
- Alfano Emanuele
- Badalamenti Filippo

#### Repository Linux da Installare

- Per aggiungere la libreria format.h e montare le stringhe
    
```
    sudo add-apt-repository universe
    sudo apt update
    sudo apt install libfmt-dev
```

    
- Per la libreria boost (utility avanzate di C++)


```
    sudo apt-get install libboost-all-dev
```


#### Features
- Redirect to home
- Error 404 Page
- Moving throw the fileSystem (no security)
- Log System
- No busy Wait to all CODE

### Tool di supporto:
- FireFox browser
- Chromium
- links2
```
	sudo apt install links2
```
- curl
- gdb (debug codice C++)
- [Visual Paradigm](https://www.visual-paradigm.com/download/community.jsp) community version
- ImageMagic, che necessita delle librerie:
```
	sudo apt install webp, ffmpeg    
```


### Librerie in uso:
- Boost (per operazioni avanzate su stringhe e path)
- Format (per creare parametricamente le stringhe)


### Progetti di Inspirazione
- https://gitlab.com/eidheim/Simple-Web-Server
- 
