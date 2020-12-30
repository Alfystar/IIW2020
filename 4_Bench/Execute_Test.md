# Benchmark:
Per analizzare le performance del server, la scelta è ricaduta su un tool scritto in Go, recente e multithread.
Per scaricare ed installare Go, seguire la guida su:
https://golang.org/doc/install?download=go1.15.2.linux-amd64.tar.gz

È inoltre necessario aver installato Python 3.8 per poter eseguire lo script di test, che chiama in sequenza il tool modificandone i parametri, salva i risultati ottenuti e ne grafica i valori.

# Go-HttpBench
Il programma di benchmark è reperibile all'url:
https://github.com/parkghost/gohttpbench.git
Per poterlo installare sulla macchina:

```bash
cd <Path d'installazione>      # verrà qui creato un eseguibile
go get github.com/parkghost/gohttpbench
go build -o gb github.com/parkghost/gohttpbench
```
Nella directory `<Path d'installazione>` verrà creato l'eseguibile `gb`. Per eseguirlo lanciare da terminale:
```bash
./gb <PARAMETRI>
```
Per una descrizione più dettagliata dei parametri, riferirsi al README sul repository del progetto originale.

# Run benchmark script
Anziché eseguire il tool variando i parametri manualmente, è stato scritto uno script in python per la gestione statica delle chiamate al programma.

Esso esegue autonomamente il server `BadAlpha` con i vari parametri, e gli stessi test su Apache2.

Alla fine del test i risultati verranno raccolti i dati e salvati in dei file .dat nel filesystem e generato un file che contiene i loro path.
Infine viene chiamato lo script Matlab passando l'elenco dei file .dat per elaborarli ed e produrre i grafici utili alla relazione (esportati come imagini png). 

Da dentro la directory `4_Bench`, dopo aver compilato l'eseguibile versione base, eseguire il test con questi argv e aspettare la terminazione dello stesso.
```bash
./testRunner.py -bas ../2_src/build/badAlphaWeb_basic ../Web
```


# Matlab Elaboration
Partendo da un file contenente tutti i path dei file .dat, si fa il plot del *numero e tempo di richiesta* (in ordinata) al variare del *numero di connessioni parallele* (in ascissa).

Un altro grafico mostra invece la *percentuale del numero di richieste* elaborate entro un determinato *tempo*.

Il tutto viene generato partendo da un file di testo contenente i *full path* delle tabelle di benchmark.

Per avviare la generazione dei grafici è necessario avere installato `Matlab 2020b` e chiamare da `4_Bench` il comando: (si potrebbe aspettare un pò causa caricamento in ram dell'ambiente)

```bash
matlab -batch "datPlotter('<Path file "matlabList.txt">')"

```
