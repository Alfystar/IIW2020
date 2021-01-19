# How to compile badAlpha server:

## Librerie da installare dai Repository Linux 

#### Pre-requisiti
Nel progetto è stato utilizzato CMake per la creazione automatizzata del makefile ed il collegamento di tutte le dipendenze. 
Risulta quindi necessario installare `cmake`, oltre al `make` e al compilatore C++ (il pacchetto build-essential include la maggior parte delle dipendenze per C/C++)
```bash
sudo apt install cmake make g++ build-essential
```

- Per aggiungere la libreria format.h necessaria ad assemblare le stringhe
```bash
sudo add-apt-repository universe
sudo apt update
sudo apt install libfmt-dev
```
- Per la libreria boost (utility avanzate di C++)
```bash
sudo apt install libboost-all-dev
```

- Per usare le facility di image resize

```bash
sudo apt install webp ffmpeg
```

#### Compilazione del server

Una volta soddisfatti i prerequisiti, **recarsi nella directory dei sorgenti** **`2_src`** e segliere quale tra le 2 versioni principali compilare:

##### 1) Server Resize dinamico
```bash
mkdir build && cd build
cmake .. -DIMAGE_RESIZE_EN=True -DDAD_RECREATE_EN=True -DDEBUG_LOG_EN=False -DDEBUG_VERBOSE_EN=False
make -j 8   #Per parallelizzare su 8 thread la compilazione
```
##### 2) Server Basic
```bash
mkdir build && cd build
cmake .. -DIMAGE_RESIZE_EN=False -DDAD_RECREATE_EN=False -DDEBUG_LOG_EN=False -DDEBUG_VERBOSE_EN=False
make -j 8   #Per parallelizzare su 8 thread la compilazione
```

La presenza di questi parametri è causata dal fatto che il programma è stato sviluppato utilizzando diversi `#define` al fine di rendere parametrica l'implementazione di determinate porzioni di codice; aggiungendo nella chiamata del cmake da terminale queste opzioni è possibile scegliere la versione da compilare del server. La sintassi dei comandi è la seguente:
```bash
cmake .. -D<VariableName>=True/False ...
```
Dove `VariableName` è una delle seguenti variabili:
- IMAGE_RESIZE_EN := `Attiva la sezione di progetto che modifica le immagini in base allo user agent`
- DAD_RECREATE_EN := `Sistema di resilienza che rigenera il server in caso di crash inaspettato`
- DEBUG_LOG_EN := `Abilita la generazione dei file di log per il debug dinamico`
- DEBUG_VERBOSE_EN := `Abilita la scrittura sul terminale dei log rapidi, per tenere traccia delle operazioni svolte`

È inoltre possibile **concatenare più opzioni**, ottenendo un eseguibile il cui **nome dipenderà dalle caratteristiche scelte** (come scritto sopra). Nel seguente esempio,
```bash
cmake .. -DIMAGE_RESIZE_EN=True -DDAD_RECREATE_EN=True -DDEBUG_LOG_EN=False -DDEBUG_VERBOSE_EN=False
```
il nome sarà `badAlphaWeb_img_dadRecreate`.

### Esecuzione del server
Per l'esecuzione del server base (da variare il nome in base al compilato eseguito) recarsi dentro la directory **`build`** generata al punto precedente ed eseguire:
```bash
./badAlphaWeb_basic 8080 ../../Web/
```

Il programma supporta vari parametri da terminale, descritte dentro l'help e qui riportate:
```
./
Per eseguire correttamente il programma, digitare:
         1)  ./badAlphaWeb_basic <PORT> [OPTION]                := Home server nel PWD 
  ---->  2)  ./badAlphaWeb_basic <PORT> <serverHome> [opzioni]  := Home server spostata alla directory desiderata
         3)  ./badAlphaWeb_basic -h  o  badAlpha --help         := Per leggere questa guida
OPTION List: (In caso di ripetizioni, l'ultimo comando ha effetto)
         -w <NumberOfWorker>    := numero di worker istanziati dal server, di default = 8 and MUST be >=0
```

Tra di esse, il secondo formato è preferibile, altrimenti la `pwd` diventerà la `serverHome` del processo.

---
[HOME](https://github.com/Alfystar/IIW2020/blob/master/README.md)