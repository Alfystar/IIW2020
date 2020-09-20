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
- Automatic redirect to HOME if path empty
- Errors: 404 Page, 406 Page, ...
- Moving throw the fileSystem (no security)
- Log System, visualizzabile in real time mediante:
```
	tail -f logDb.txt -n 50 -s 1    
```
- Storico dei debug, accedibile dentro la cartella Debug/
- No busy Wait to all CODE

### Supporter Browser
- FireFox browser
- Chromium
- Chorme
- links2
```
	sudo apt install links2
```
- curl

### Tool di supporto:
- gdb (debug codice C++)
- [Visual Paradigm](https://www.visual-paradigm.com/download/community.jsp) community version
- ImageMagic, che necessita delle librerie:
```
	sudo apt install webp, ffmpeg    
```
- Go-HttpBench reperibile all'url https://github.com/parkghost/gohttpbench

### Librerie in uso:
- Boost (per operazioni avanzate su stringhe e path)
- Format (per creare parametricamente le stringhe)


### Progetti di Inspirazione
- https://gitlab.com/eidheim/Simple-Web-Server
