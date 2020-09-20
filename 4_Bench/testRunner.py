#!/usr/bin/python3.8
"""
Runner dei test sviluppato per python3.8
Necessita dell'eseguibile "gb" della stessa directory per poter eseguire i test
Esegue e salva i test eseguiti con gb e raccoglie i dati in dei file .dat,
analizzabili in seguito come più piace.

Il programma esegue 2 volte il test
    1) Apache
        pause for setup
    2) BadAlpha

"""

import sys
import subprocess
import os
import time
from datetime import datetime

# Parametri Obbligatori
url = "127.0.0.1"
rsc = "/index.html"
apachePort = 80
badAlphaPort = 8080
outDir = "./"

# Parametri Facoltativi


# Separatore delle varie colonne
sep = "\t";


def help():
    print("Usage:\t./testRunner.py         #Esegue con i parametri di default")
    print("      \t./testRunner.py -h      #Esegue questo Help")
    print("      \t./testRunner.py <urlServer> <resource>")
    print("      \t./testRunner.py <urlServer> <resource> <Port Apache> <Port BadAlpha>")
    print("      \t./testRunner.py <urlServer> <resource> <Port Apache> <Port BadAlpha> <Save Dir>")
    exit(0);


def paramNumExtract(stdOut, paramFind, postParamFind,  idStart=0):
    startIndex = stdOut.find(paramFind,idStart)
    endIndex = stdOut.find(postParamFind, startIndex)
    param = stdOut[startIndex + len(paramFind):endIndex]
    return float(param), endIndex

def runTest(parallelCon, totCon, cpu, url, port, path):
    """
     ./gb -c 100 -n 100000 -k http://127.0.0.1:80/index.html
     ./gb -c <parallelCon> -n <Total Con> -G <CPU used> -k http://<url>:<port>/<resource Path>

    :param parallelCon: @int    Numero Connessioni in parallelo
    :param totCon: @int         Numero Connessioni Totali
    :param cpu: @int            CPU usate nel test
    :param url: @string
    :param port: @int
    :param path: @string
    :return:
    """
    global sep
    command = "./gb -k -c " + str(parallelCon) + " -n " + str(totCon) + \
              " -G " + str(cpu) + " http://" + url + ":" + str(port) + "/" + path
    print(command)

    # Esegue il comando e ritorna lo
    stdOut = subprocess.check_output(command, shell=True, text=True)
    print(stdOut)

    startIndex = 0
    nReqSec, startIndex = paramNumExtract(stdOut, "Requests per second:    ", "[#/sec] (mean)", startIndex)
    reqTime, startIndex = paramNumExtract(stdOut, "(mean)\nTime per request:       ", "[ms]", startIndex)
    speedTransf, startIndex = paramNumExtract(stdOut, "HTML Transfer rate:     ", "[Kbytes/sec] received", startIndex)

    print("Connessioni parallele: " + str(parallelCon) )
    print("CPU usate: " + str(cpu))
    print("Requests per second: " + str(nReqSec) + "[#/sec]")
    print("Time per request (every connection): " + str(reqTime) + "[ms]")
    print("HTML Transfer rate: " + str(speedTransf) + "[Kbytes/sec]")

    line = str(cpu) + sep + str(parallelCon) + sep + str(reqTime) + sep + str(parallelCon) + sep + str(parallelCon)+"\n"

    return


def main():
    global url, rsc, apachePort, badAlphaPort, outDir

    if (len(sys.argv) >= 2):
        if sys.argv[1] == "-h":
            help()

    if (len(sys.argv) >= 3):
        url = sys.argv[1]
        rsc = sys.argv[2]

    if (len(sys.argv) >= 5):
        if sys.argv[3].isnumeric() and sys.argv[4].isnumeric():
            apachePort = int(sys.argv[3])
            badAlphaPort = int(sys.argv[4])
            if (apachePort == 0 or badAlphaPort == 0):
                print("Parametri 3 e 4 invalidi!!!")
                help()
        else:
            print("Parametri 3 e 4 NON NUMERI!!!")
            help()

    if (len(sys.argv) >= 6):
        outDir = sys.argv[5]

    runTest(100, 10000, 2, url, apachePort, rsc)

    return 0


if __name__ == '__main__':
    main()
