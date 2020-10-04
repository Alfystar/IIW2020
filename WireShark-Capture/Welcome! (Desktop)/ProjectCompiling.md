ProjectCompiling

# How to compile badAlpha server:

## Librerie da installare dai Repository Linux 

#### Pre-requisiti
Per il progetto è stato usato CMake per la creazione automatizzata del makefile ed il collegamento di tutte le dipendenze. 
Risulta quindi necessario installare `cmake`, oltre al `make` e al compilatore C++ (il pacchetto build-essential include la maggior parte delle dipendenze per C/C++)
```
sudo apt install cmake, make, g++, build-essential
```

- Per aggiungere la libreria format.h necessaria ad assemblare le stringhe
```
    sudo add-apt-repository universe
    sudo apt update
    sudo apt install libfmt-dev
```
- Per la libreria boost (utility avanzate di C++)
```
    sudo apt-get install libboost-all-dev
```

#### Compilazione del server
Una volta soddisfatti i prerequisiti, **recarsi nella directory dei sorgenti** ed eseguire:
```
mkdir build && cd build
cmake ..
make -j 8   #Per parallelizzare su 8 thread la compilazione
```

Il programma è stato sviluppato utilizzando diversi `#define` al fine di rendere parametrica l'implementazione di determinate porzioni di codice; aggiungendo nella chiamata del cmake da terminale alcune opzioni è possibile scegliere la modalità operativa del server desiderata. La sintassi dei comandi è la seguente:
```
cmake .. -D<VariableName>=True/False
cmake .. -DIMAGE_RESIZE_EN=False -DDAD_RECREATE_EN=False -DDEBUG_LOG_EN=False -DDEBUG_VERBOSE_EN=False
```
Dove `VariableName` è una delle seguenti variabili:
- IMAGE_RESIZE_EN
- DAD_RECREATE_EN
- DEBUG_LOG_EN
- DEBUG_VERBOSE_EN

È inoltre possibile **concatenare più opzioni**, ottenendo un eseguibile il cui nome dipenderà dalle caratteristiche scelte. Nel seguente esempio,
```
cmake .. -DIMAGE_RESIZE_EN=True -DDAD_RECREATE_EN=True -DDEBUG_LOG_EN=False -DDEBUG_VERBOSE_EN=False
```
il nome sarà `badAlphaWeb_img_dadRecreate`.

### Esecuzione del server
Una volta effettuata la compilazione, digitare sul terminale una delle seguenti tre opzioni:
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
