# How to install from repository apache2:

```
sudo apt install apache2
```

##### in caso di errorore di "digimend-dkms error 1" 

```
sudo apt install libncurses-dev flex bison openssl libssl-dev dkms libelf-dev libudev-dev libpci-dev libiberty-dev autoconf
sudo dpkg --remove --force-remove-reinstreq digimend-dkms
```

quindi riprovare ad installare il server


# How to setup apache2
Di default la home del server è in
```
/var/www/html
```
La directory viene creata da apache2 durante l'installazione, ma è di proprietà del root
Cambiamo la proprietà dandola a `www-data`, ovvero l'utente con cui il server si logga nella macchina, per cambiare l'utente:

```
sudo chown www-data:www-data /var/www/html
sudo chown www-data:www-data /var/www/html/index.html
```

Per una maggiore comodità è quindi necessario aggiungere il proprio utente al gruppo `www-data` con cui esegue apache2

```
sudo usermod -a -G www-data <UserName>
```

Quindi Riavviare la macchina per rendere effettive le modifiche

Passiamo ora al dare il diritto al gruppo `www-data` di modificare index.html e la directory per rinominarlo e copiarci i file:
Da dentro `/var/www/html` digtiamo:
```
sudo chmod g+w .
sudo chmod g+w index.html
sudo mv index.html defaultIndex.html 
```


A questo punto copiare dentro la catella `/var/www/html` il proprio web server:
Da dentro `/var/www/html` digtiamo:
```
cp <PATH Web Page>/* . -r   #L'* server per dire TUTTO

```

# Start apache2 Deamon
Per avviare il demone di apache2 e poter accedere alla pagina:
```
sudo service apache2 reload     # Ricarica configurazioni (se presenti cambiamenti) e avvia il server  
```
A questo punto è possibile accedere dalla porta di default 80, al server gestito da apache2, in localHost

In generale per avviare e chiudere il server da ora in poi si deve usare
```
sudo service apache2 restart    # Avvia il server dopo essere stato stoppato
sudo service apache2 stop       # Chiude il server se è avviato
sudo service apache2 reload     # Ricarica configurazioni (se presenti cambiamenti) a SERVER AVVIATO 
```

# Httpperf usage
Per usare

