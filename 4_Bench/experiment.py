import subprocess
import os

from systemUtility import *


def paramNumExtract(stdOut, paramFind, postParamFind, idStart=0):
    """
    Permette di estrarre il valore numerico del benchmark dallo stdOut
    :param stdOut:
    :param paramFind:
    :param postParamFind:
    :param idStart:
    :return:
    """
    startIndex = stdOut.find(paramFind, idStart)
    endIndex = stdOut.find(postParamFind, startIndex)
    param = stdOut[startIndex + len(paramFind):endIndex]
    return float(param), endIndex


def runTestApache(parallelCon, IP, Port, rsc):
    return runTest(1, parallelCon, IP, Port, rsc)


def runTest(Worker, parallelCon, IP, Port, rsc):
    """
     ./gb -k -n 10000000 -G 4 -t 10 -c <ParallelCon> http://<IP>:<Port>/<rsc>

    :param Worker:
    :param parallelCon: @int    Numero Connessioni aperte e usate in parallelo durante il test
    :param IP: @string
    :param Port: @int
    :param rsc: @string         Path della Risorsa web SENZA "/"
    :return : @string           Linea da appendere al file .Dat con i risultati di questo esperimento
    """
    sep = "\t"
    command = "./gb -k -n 10000000 -G 4 -t 10 -c " + str(parallelCon) + " http://" + IP + ":" + str(Port) + "/" + rsc
    print(command)

    # Esegue il comando e ritorna lo
    stdOut = subprocess.check_output(command, shell=True, text=True)

    startIndex = 0
    nReqSec, startIndex = paramNumExtract(stdOut, "Requests per second:    ", "[#/sec] (mean)", startIndex)
    reqTime, startIndex = paramNumExtract(stdOut, "(mean)\nTime per request:       ", "[ms]", startIndex)
    speedTransf, startIndex = paramNumExtract(stdOut, "HTML Transfer rate:     ", "[Kbytes/sec] received", startIndex)

    # todo: prendere gli altri parametri percentili
    # print("Connessioni parallele: " + str(parallelCon) )
    # print("CPU usate: " + str(cpu))
    # print("Requests per second: " + str(nReqSec) + "[#/sec]")
    # print("Time per request (every connection): " + str(reqTime) + "[ms]")
    # print("HTML Transfer rate: " + str(speedTransf) + "[Kbytes/sec]")

    line = str(Worker) + sep + str(parallelCon) + sep + str(nReqSec) + sep + str(reqTime) + sep + str(
        speedTransf) + "\n"

    return line


def runBadAlphaExperiment(saveDir, IP, Port, rsc, ElencoOutputDir):
    """
    Tutto il necessario per avviare 1 esperimento sulla porta richiesta
    :param saveDir: @string             Directory Home dell'esperimento SENZA "/" finale
    :param IP: @string
    :param Port: @int
    :param rsc: @string                 Path della risorsa partendo SENZA "/" (es "index.html o page/web/...")
    :param ElencoOutputDir: @FD         File Descriptor del file che contiene tutti i path degli esperimenti
    :return:
    """
    pathExperiment = saveDir + "/badAlpha"
    try:
        os.makedirs(pathExperiment)
    except OSError:
        print("Creation of the directory %s failed" % pathExperiment)
    else:
        print("Successfully created the directory %s" % pathExperiment)


    name = rsc[rsc.rfind("/") + 1: rsc.rfind(".")]

    f, fileName = initFileExperiment(pathExperiment, name)

    for nWorker in range(1, os.cpu_count()):
        #todo: Avviare e stoppare il server con i worker Parametrici
            for pCon in range(500, 4000, 500):
                line = runTest(nWorker, pCon, IP, Port, rsc)
                f.write(line)

    f.close()
    ElencoOutputDir.write(fileName+"\n")

    return


def runApache2experiment(saveDir, IP, Port, rsc):
    """
    Tutto il necessario per avviare 1 esperimento sulla porta richiesta
    :param saveDir: @string             Directory Home dell'esperimento SENZA "/" finale
    :param IP: @string
    :param Port: @int
    :param rsc: @string                 Path della risorsa partendo SENZA "/" (es "index.html o page/web/...")
    :return:
    """
    pathExperiment = saveDir + "/apache2"
    try:
        os.makedirs(pathExperiment)
    except OSError:
        print("Creation of the directory %s failed" % pathExperiment)
    else:
        print("Successfully created the directory %s" % pathExperiment)

    name = rsc[rsc.rfind("/") + 1: rsc.rfind(".")]

    f, fileName = initFileExperiment(pathExperiment, name)

    for pCon in range(500, 4000, 500):
        line = runTestApache(pCon, IP, Port, rsc)
        f.write(line)

    f.close()

    return