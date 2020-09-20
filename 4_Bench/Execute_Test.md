# Benchmark:
Abbiamo optato per un tool scritto in go, recente e multithread, per analizzare le performance del server.
Per scaricare ed installare Go, seguire la guida su:
https://golang.org/doc/install?download=go1.15.2.linux-amd64.tar.gz


# Go-HttpBench
Il programma di Benchmark è reperibile all'url:
https://github.com/parkghost/gohttpbench.git
Per poterlo installare sulla macchina

```
cd <Path dove installare>      # verrà qui creato un eseguibile
go get github.com/parkghost/gohttpbench
go build -o gb github.com/parkghost/gohttpbench
```
Nella directory `<Path dove installare>` verrà creato l'eseguibile `gb`.
Per esegurlo quindi lanciare da terminale:
```
./gb <PARAMETRI>
```
Per una descrizione migliore guardare readmi sul repository del progetto.

# Repeate Benchmark script
Per eseguire i test ripetutamente e con i parametri ipotizzati, è stato scritto uno script in python che gestisca le chiamate del programma con i diversi parametri.


# Matlab Elaboration
Per estrapolare dei dati dai vari bechmark effettuati, è stato creato uno script di Matlab che preso il file xml, genera i grafici di cui abbiamo bisogno.
