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
import time
from systemUtility import *
from experiment import *

# Parametri Obbligatori
IP = "127.0.0.1"
apachePort = 80
badAlphaPort = 8080
outDir = "./benchMark"


def help():
    print("Usage:\t./testRunner.py         #Esegue con i parametri di default")
    print("      \t./testRunner.py -h      #Esegue questo Help")
    print("      \t./testRunner.py <urlServer>")
    print("      \t./testRunner.py <urlServer> <Port Apache> <Port BadAlpha>")
    print("      \t./testRunner.py <urlServer> <Port Apache> <Port BadAlpha> <Save Dir>")
    exit(0)

def argvExtractor():
    global IP, apachePort, badAlphaPort, outDir
    if len(sys.argv) >= 2:
        if sys.argv[1] == "-h":
            help()

    if len(sys.argv) >= 2:
        IP = sys.argv[1]

    if len(sys.argv) >= 4:
        if sys.argv[2].isnumeric() and sys.argv[3].isnumeric():
            apachePort = int(sys.argv[2])
            badAlphaPort = int(sys.argv[3])
            if apachePort == 0 or badAlphaPort == 0:
                print("Parametri 3 e 4 invalidi!!!")
                help()
        else:
            print("Parametri 3 e 4 NON NUMERI!!!")
            help()

    if len(sys.argv) >= 5:
        outDir = sys.argv[4]


def main():
    global IP, apachePort, badAlphaPort, outDir

    argvExtractor()

    # Inizio dell'esperimento

    savePath = initExpDir(outDir)
    #todo Creare il file ElencoOutputDir
    nameOutList = "elencoDat.txt"

    try:
        os.remove(nameOutList)
        print("% s removed successfully" % nameOutList)
    except OSError as error:
        print(error)
        print("File path can not be removed")

    ElencoOutputDir = open(nameOutList, "wt")

    runBadAlphaExperiment(savePath, IP, badAlphaPort, "index.html", ElencoOutputDir)
    runBadAlphaExperiment(savePath, IP, badAlphaPort, "web/img/caffe.jpg", ElencoOutputDir)
    runBadAlphaExperiment(savePath, IP, badAlphaPort, "web/img/computerAi.webp", ElencoOutputDir)
    runBadAlphaExperiment(savePath, IP, badAlphaPort, "web/img/enterprise.png", ElencoOutputDir)
    runBadAlphaExperiment(savePath, IP, badAlphaPort, "web/img/car.jpg", ElencoOutputDir)

    time.sleep(1)
    # input("Press Enter to continue...")
    runBadAlphaExperiment(savePath, apachePort, "apache2")

    return 0

if __name__ == '__main__':
    main()
