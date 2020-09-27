#!/usr/bin/python3.8
"""
Runner dei test sviluppato per python3.8
Necessita dell'eseguibile "gb" della stessa directory per poter eseguire i test
Esegue e salva i test eseguiti con gb e raccoglie i dati in dei file .dat,
analizzabili in seguito come più piace.

Il programma esegue 2 volte il test
    1) BadAlpha (con vari workers)
    2) Apache
Confrontandoli per numero di connessioni contemporaneamente aperte in un tempo di test di 5 secondi

Il comando per avviare da dentro la directory 4_Bench e usare i parametri di default è:
./testRunner.py -bas ../2_src/cmake-build-debug/badAlphaWeb_basic ../Web



"""

import sys

from experiment import *

# Parametri Obbligatori
IP = "127.0.0.1"
apachePort = 80
badAlphaPort = 8080
outDir = "./benchMark"
badAlphaParam = []


def help():
    print("Usage:\t./testRunner.py {-bas --badAlphaServer <PathBadAlpha> <webHomePath>} [opzioni]")
    print("Non è obbligatorio l'ordine ma <-bas> DEVE essere specificato.")
    print("Liststa delle opzioni:")
    print("      \t-h                                                      #Esegue questo Help")
    print("      \t-u o --url <UrlServer>                                  #Url da testare")
    print("      \t-p o --port <Port Apache>;<Port BadAlpha>               #Porte da chiamare")
    print("      \t-o o --out <Save Dir>                                   #RootDir dei benchmark")
    exit(0)


def paramExtractor(subList):
    """
    Data la lista dei parametri ricevuti (partendo da argv[1]), prende il primo, lo elabora
    e ritorna quello che non ha ancora elaborato

    :param subList: #list   sotto lista degli argv da elaborare
    :return:
    """
    global IP, apachePort, badAlphaPort, outDir, badAlphaParam

    op = subList[0]

    if op == "-h":
        help()

    if op == "-u" or op == "--url":
        if len(subList) > 1:
            IP = subList[1]
            return subList[2:]
        else:
            print("<UrlServer> Param have error!!!")
            help()

    if op == "-p" or op == "--port":
        if len(subList) > 1:
            indexCut = subList[1].find(";")
            apachePortStr = subList[0:indexCut]
            badAlphaPortStr = subList[indexCut + 1:]
            if apachePortStr.isnumeric() and badAlphaPortStr.isnumeric():
                apachePort = int(apachePortStr)
                badAlphaPort = int(badAlphaPortStr)
                return subList[2:]
            print("<Port> Param have error!!!")
            help()

    if op == "-o" or op == "--out":
        if len(subList) > 1:
            outDir = subList[1]
            return subList[2:]
        else:
            print("<Save Dir> Param have error!!!")
            help()

    if op == "-bas" or op == "--badAlphaServer":
        if len(subList) > 2:
            badAlphaParam = [subList[1], subList[2]]
            return subList[3:]
        else:
            print("<PathBadAlpha> or <webHomePath> Param have error!!!")
            help()

    help()


def argvExtractor():
    subList = sys.argv[1:]
    while (len(subList) > 0):
        subList = paramExtractor(subList)

    if len(badAlphaParam) == 0:
        print("-bas è un parametro obbligatorio!!!")
        help()


def main():
    global IP, apachePort, badAlphaPort, outDir, badAlphaParam

    argvExtractor()

    # Inizio dell'esperimento

    savePath = initExpDir(outDir)
    nameOutList = savePath + "matlabList.txt"
    errorTestList = savePath + "ErrorTestList.txt"

    matlabPaths = open(nameOutList, "w")
    errorTest = open(errorTestList, "w")
    outFD = [matlabPaths, errorTest]
    rscList = ["index.html", "web/img/caffe.jpg", "web/img/computerAi.webp", "web/img/enterprise.png",
               "web/img/car.jpg"]

    errorTest.write("badAlpha error command:\n")
    errorTest.flush()
    for rsc in rscList:
        runBadAlphaExperiment(savePath, IP, badAlphaPort, rsc, badAlphaParam, outFD)

    time.sleep(1)
    # input("Press Enter to continue...")
    errorTest.write("\n#######################\n\napache2 error command:\n")
    errorTest.flush()
    for rsc in rscList:
        runApache2experiment(savePath, IP, apachePort, rsc, outFD)

    matlabPaths.close()
    errorTest.close()
    #todo: Comando matlab per l'elaborazioni delle immagini

    return 0


if __name__ == '__main__':
    main()
