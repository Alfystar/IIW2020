# IIW2020
Progetto di IIW Web Server

Sviluppato in C++ per il corso di Ingegneria di Internet

Tenuto dal professore: Francesco Lo Presti

La richiesta del progetto è reperibile [qui](https://github.com/Alfystar/IIW2020/blob/master/WebServer_Project.pdf)

### Sviluppato da:
- Alfano Emanuele
- Badalamenti Filippo

#### Guide di supporto
Per poter replicare i test trovate:
- [Guida per la compilazione del progetto](https://github.com/Alfystar/IIW2020/blob/master/ProjectCompiling.md) per creare correttamente l'eseguibile

- [Guida per installare apache2](https://github.com/Alfystar/IIW2020/blob/master/Install_Apache2.md) per replicare la nostra condizione di test

- [Guida per eseguire i test di benckmark](https://github.com/Alfystar/IIW2020/blob/master/4_Bench/Execute_Test.md) per replicare i nostri risultati

#### Features
- Automatic redirect to HOME if path empty
- Errors: 404 Page, 406 Page, ...
- Moving throw the fileSystem (no security)
- Log System, visualizzabile in real time mediante: `tail -f logDb.txt -n 50 -s 1`
- Storico dei debug, accedibile dentro la cartella Debug/
- No busy Wait to all CODE

### Supporter Browser
Il progetto è stato testato con successo su:
- FireFox browser
- Chromium
- Chrome
- links2 ` sudo apt install links2 `
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


### Codice Open-Source usato:
Per l'estrazione delle informazioni dall'header di una richiesta, sono stati prese le librerie scritte in questo progetto, e riadattate al nostro codice
- https://gitlab.com/eidheim/Simple-Web-Server
