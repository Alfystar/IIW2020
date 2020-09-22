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

#matlab -batch "datPlotter('./benchMark/test/indexBenchMark.dat')"

import sys
import subprocess
import os
import time
from datetime import datetime

# Parametri Obbligatori
url = "127.0.0.1"
apachePort = 80
badAlphaPort = 8080
outDir = "./benchMark"

# Parametri Facoltativi


# Separatore delle varie colonne
sep = "\t";


def help():
    print("Usage:\t./testRunner.py         #Esegue con i parametri di default")
    print("      \t./testRunner.py -h      #Esegue questo Help")
    print("      \t./testRunner.py <urlServer>")
    print("      \t./testRunner.py <urlServer> <Port Apache> <Port BadAlpha>")
    print("      \t./testRunner.py <urlServer> <Port Apache> <Port BadAlpha> <Save Dir>")
    exit(0);


def initDatSave(dirOut):
    """
    La funzione crea dentro la directory <dirOut> una cartella con la data corrente per salvare l'esperimento
    e ci sposta il processo così da vedere:

    .
    ├── apache2         # per gli esperimenti sulla porta di apache2
    └── badAlpha        # per gli esperimenti sulla porta di badAlpha

    con "." --> <dirOut>/<timeStamp>/

    :param dirOut: @strint      Path della directory su cui salvare i risultati
    :return: @strint            Ritorno il path della direcory su cui salvare
    """
    start_time = datetime.now()

    dirOut += "/"
    dirName = start_time.isoformat(timespec='seconds')
    path = dirOut + dirName + "/"

    try:
        os.makedirs(path)
    except OSError:
        print("Creation of the directory %s failed" % path)
    else:
        print("Successfully created the directory %s" % path)

    return path


def initFileExperiment(pathSave, name):
    f = open(pathSave + "/" + name + "BenchMark.dat", "w")
    # line = str(cpu) + sep + str(parallelCon) + sep + str(nReqSec)+ sep + str(reqTime) + sep + str(speedTransf) + "\n"
    title = "Prove sperimentali del file <" + name + "> i dati sono raccolti nelle colonne:\n"
    title += "N°Worker\tN°Parallel Connection\tRequests [#ps]\tRequestTime [ms]\tHttpSpeedRate [KBps]\n"
    f.write(title)

    return f


def paramNumExtract(stdOut, paramFind, postParamFind, idStart=0):
    startIndex = stdOut.find(paramFind, idStart)
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
    :return : @string           Linea da appendere al file .Dat con i risultati di questo esperimento
    """
    global sep
    command = "./gb -k -t 10 -c " + str(parallelCon) + " -n " + str(totCon) + \
              " -G " + str(cpu) + " http://" + url + ":" + str(port) + "/" + path
    print(command)

    # Esegue il comando e ritorna lo
    stdOut = subprocess.check_output(command, shell=True, text=True)

    startIndex = 0
    nReqSec, startIndex = paramNumExtract(stdOut, "Requests per second:    ", "[#/sec] (mean)", startIndex)
    reqTime, startIndex = paramNumExtract(stdOut, "(mean)\nTime per request:       ", "[ms]", startIndex)
    speedTransf, startIndex = paramNumExtract(stdOut, "HTML Transfer rate:     ", "[Kbytes/sec] received", startIndex)

    # print("Connessioni parallele: " + str(parallelCon) )
    # print("CPU usate: " + str(cpu))
    # print("Requests per second: " + str(nReqSec) + "[#/sec]")
    # print("Time per request (every connection): " + str(reqTime) + "[ms]")
    # print("HTML Transfer rate: " + str(speedTransf) + "[Kbytes/sec]")

    line = str(cpu) + sep + str(parallelCon) + sep + str(nReqSec) + sep + str(reqTime) + sep + str(speedTransf) + "\n"

    return line


def testResource(fdWrite, port, rsc):
    # for nCpu in range(1, os.cpu_count()):
    for pCon in range(500, 4000, 500):
        line = runTest(pCon, 10000000, 4, url, port, rsc)
        fdWrite.write(line)
    return


def runExperiment(saveDir, port, service):
    path = saveDir + service
    try:
        os.makedirs(path)
    except OSError:
        print("Creation of the directory %s failed" % path)
    else:
        print("Successfully created the directory %s" % path)

    rsc = "index.html"
    name = rsc[rsc.rfind("/") + 1: rsc.rfind(".")]

    f = initFileExperiment(path, name)
    testResource(f, port, rsc)

    f.close()

    return


def main():
    global url, rsc, apachePort, badAlphaPort, outDir

    if (len(sys.argv) >= 2):
        if sys.argv[1] == "-h":
            help()

    if (len(sys.argv) >= 2):
        url = sys.argv[1]

    if (len(sys.argv) >= 4):
        if sys.argv[2].isnumeric() and sys.argv[3].isnumeric():
            apachePort = int(sys.argv[2])
            badAlphaPort = int(sys.argv[3])
            if (apachePort == 0 or badAlphaPort == 0):
                print("Parametri 3 e 4 invalidi!!!")
                help()
        else:
            print("Parametri 3 e 4 NON NUMERI!!!")
            help()

    if (len(sys.argv) >= 5):
        outDir = sys.argv[4]

    # Inizio dell'esperimento

    savePath = initDatSave(outDir)

    runExperiment(savePath, badAlphaPort, "badAlpha")
    time.sleep(1);
    # input("Press Enter to continue...")
    runExperiment(savePath, apachePort, "apache2")

    return 0


if __name__ == '__main__':
    main()
