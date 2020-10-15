# How to install Apache2 from repository:

```
sudo apt install apache2
```

##### Nel caso si verificasse "digimend-dkms error 1":

```
sudo apt install libncurses-dev flex bison openssl libssl-dev dkms libelf-dev libudev-dev libpci-dev libiberty-dev autoconf
sudo dpkg --remove --force-remove-reinstreq digimend-dkms
```

quindi riprovare ad installare il server


# How to setup Apache2
Di default la home del server è in:
```
/var/www/html
```
La directory viene creata da apache2 durante l'installazione, ma è di proprietà del root
Cambiamo la proprietà dandola a `www-data`, ovvero l'utente con cui il server si fa il login sulla macchina; per cambiare l'utente:

```
sudo chown www-data:www-data /var/www/html
sudo chown www-data:www-data /var/www/html/index.html
```

Per una maggiore comodità è quindi necessario aggiungere il proprio utente al gruppo `www-data` con cui eseguire apache2:

```
sudo usermod -a -G www-data <UserName>
```

quindi **riavviare** la macchina per rendere effettive le modifiche.

Adesso cambiamo i permessi del gruppo `www-data` per modificare il contenuto di index.html ed in generale della directory. Da dentro `/var/www/html` digitiamo:
```
sudo chmod g+w .
sudo chmod g+w index.html
sudo mv index.html defaultIndex.html 
```


A questo punto, copiare dentro la cartella `/var/www/html` il proprio web server:
```
cp <PATH Web Page>/* . -r   ("*" per indicare tutto il contenuto)

```

# Start Apache2 daemon
Per avviare il demone di apache2 e poter accedere alla pagina:
```
sudo service apache2 reload     # Ricarica configurazioni (se presenti cambiamenti) e avvia il server  
```
A questo punto è possibile accedere al server gestito da Apache2 in **localhost** dalla porta di default 80.

In generale per avviare e chiudere il server da ora in poi si deve usare
```
sudo service apache2 restart    # Riavvia il server qualunque sia il suo stato
sudo service apache2 stop       # Chiude il server se è avviato
sudo service apache2 reload     # Ricarica configurazioni (se presenti cambiamenti) a SERVER AVVIATO 
```
