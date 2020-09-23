# How to compiling badAlpha server:

#### Pre-requisiti
Per il progetto è stato usato cmake per la creazione automatizzata del make e il collegamento di tutte le dipendenze. 
Risulta quindi necessario installare `cmake`, oltre al `make` e il compilatore C++
```
sudo apt install cmake, make, g++
```

#### Compilazione della soluzione
Arrivati a questo punto, **partendo da dentro la directory dei sorgenti** basterà eseguire:
```
mkdir build && cd build
cmake ..
make -j 8   #Per parallelizzare su 8 processi la compilazione
```

Il programma è stato sviluppato usando vari `#define` che permettessero di rendere parametriche le aggiunte del codice, dal cmake è possibile selezionare l'estenzione che più aggrada, per poter selezionare le opzioni da attivare così da avere un compilato con quelle caratteristiche basterà digitare al posto di `cmake`:
```
cmake .. -D<VariableName>=True

```
Dove `VariableName` è uno delle seguenti variabili:
- IMAGE_RESIZE_EN
- DAD_RECREATE_EN
- DEBUG_LOG_EN
- DEBUG_VERBOSE_EN

##### È in oltre possibile creare una composizione di caratteristiche
Ad esempio
```
cmake .. -DIMAGE_RESIZE_EN=True -DDAD_RECREATE_EN=True

```
E successivamente eseguire nuovamente il `make` per ottenere l'eseguibile.
Il nome dell'eseguibile è dinamico così da non cancellare le istanze precedenti
E si otterà come risultato: `badAlphaWeb_img_dadRecreate` in questo caso particolare
