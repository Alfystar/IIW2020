import subprocess
import time

from systemUtility import *

# Combinazioni dei test del server BadAlpha
connList = [1, 10, 25, 50, 75, 100, 150, 200, 300, 400, 500, 600, 700,
            800, 900, 1000, 1200, 1400, 1800, 2000, 3000]
nWorkerList = [1, int(os.cpu_count() / 4), int(os.cpu_count() / 2), os.cpu_count(), os.cpu_count() * 2]


def paramNumExtract(stdOut, paramFind, postParamFind, idStart=0):
    """
    Permette di estrarre il valore numerico del benchmark dallo stdOut
    :param stdOut:
    :param paramFind:
    :param postParamFind:
    :param idStart:
    :return:
    """
    if idStart == -1:
        return -1, -1
    startIndex = stdOut.find(paramFind, idStart)
    endIndex = stdOut.find(postParamFind, startIndex)
    if startIndex == -1 or endIndex == -1:
        return -1, -1
    param = stdOut[startIndex + len(paramFind):endIndex]
    return float(param), endIndex


def runTestApache(parallelCon, IP, Port, rsc, errorTes):
    return runTest(1, parallelCon, IP, Port, rsc, errorTes)


def runTest(Worker, parallelCon, IP, Port, rsc, errorFd):
    """
     ./gb -k -n 10000000 -G 4 -t 10 -c <ParallelCon> http://<IP>:<Port>/<rsc>

    :param Worker:
    :param parallelCon: @int    Numero Connessioni aperte e usate in parallelo durante il test
    :param IP: @string
    :param Port: @int
    :param rsc: @string         Path della Risorsa web SENZA "/"
    :param errorFd: @FD         File descriptor in caso di errore, per segnare il comando fallito
    :return : @string           Linea da appendere al file .Dat con i risultati di questo esperimento
    """
    sep = "\t"
    command = "./gb -r -k -n 1000000 -t 2 -c " + str(parallelCon) + " http://" + IP + ":" + str(Port) + "/" + rsc
    print(command)

    # Esegue il comando e ritorna lo
    stdOut = subprocess.check_output(command, shell=True, text=True)

    startIndex = 0
    nReqSec, startIndex = paramNumExtract(stdOut, "Requests per second:    ", "[#/sec] (mean)", startIndex)
    reqTime, startIndex = paramNumExtract(stdOut, "(mean)\nTime per request:       ", "[ms]", startIndex)
    speedTransfer, startIndex = paramNumExtract(stdOut, "HTML Transfer rate:     ", "[Kbytes/sec] received", startIndex)
    minTime, startIndex = paramNumExtract(stdOut, "Total:", "\t", startIndex)
    pec50, startIndex = paramNumExtract(stdOut, "50%\t", "\n", startIndex)
    pec66, startIndex = paramNumExtract(stdOut, "66%\t", "\n", startIndex)
    pec75, startIndex = paramNumExtract(stdOut, "75%\t", "\n", startIndex)
    pec80, startIndex = paramNumExtract(stdOut, "80%\t", "\n", startIndex)
    pec90, startIndex = paramNumExtract(stdOut, "90%\t", "\n", startIndex)
    pec95, startIndex = paramNumExtract(stdOut, "95%\t", "\n", startIndex)
    pec98, startIndex = paramNumExtract(stdOut, "98%\t", "\n", startIndex)
    pec99, startIndex = paramNumExtract(stdOut, "99%\t", "\n", startIndex)
    pec100, startIndex = paramNumExtract(stdOut, "100%\t", " (longest request)", startIndex)

    line = str(Worker) + sep + str(parallelCon) + sep + str(nReqSec) + sep + str(reqTime) + sep + \
           str(speedTransfer) + sep + str(minTime) + sep + str(pec50) + sep + str(pec66) + sep + str(pec75) + sep + \
           str(pec80) + sep + str(pec90) + sep + str(pec95) + sep + str(pec98) + sep + str(pec99) + sep + \
           str(pec100) + "\n"

    if line.find("-1") > 0:  # un parametro ha dato un errore
        errorFd.write(command + "\n")  # Segno per usi futuri il parametro problematico
        errorFd.flush()

    return line


def parallelRun(cmd):
    """
    Permette di eseguire in parallelo il comando, e ritorna il pid del figlio
    :param cmd:
    :return:
    """
    pid = os.fork()
    if (pid == 0):  # son
        os.system(cmd)
        exit(0)
    else:  # dad
        time.sleep(5)
        return pid


def serverKill():
    """
    Funzione da terminale che ottiene i processi server per ogni variante escludendo questo programma python
    :return:
    """
    os.system("ps -ef | grep 'badAlpha' | grep -v '.py' | grep -v grep | awk '{print $2}' | xargs -r kill -9")
    print("\tKilling All badAlpha Server Instance")


def runBadAlphaExperiment(saveDir, IP, Port, rsc, badAlphaParam, outFD):
    """
    Tutto il necessario per avviare 1 esperimento sulla porta richiesta
    :param saveDir: @string                                 Directory Home dell'esperimento SENZA "/" finale
    :param IP: @string
    :param Port: @int
    :param rsc: @string                                     Path della risorsa partendo SENZA "/" (es "index.html o page/web/...")
    :param badAlphaParam: @list [programPath, webHome]
    :param outFD: @FDList= [matlabPaths, errorTest]         File Descriptor dei file di salvataggio
    :return:
    """
    matlabPaths = outFD[0]
    errorTest = outFD[1]
    badAlphaCmd = badAlphaParam[0] + " " + str(Port) + " " + badAlphaParam[1] + " -w "

    pathExperiment = saveDir + "badAlpha"
    name = rsc[rsc.rfind("/") + 1: rsc.rfind(".")]
    f = initFileExperiment(pathExperiment, name, matlabPaths)

    print("\nExperiment Start:")
    # Cancello il vecchio server per sicurezza
    serverKill()

    global nWorkerList, connList
    for nWorker in nWorkerList:
        # Avviare il server con i worker Parametrici
        print("Creating badAlphaServer with " + str(nWorker) + " Workers")
        errorTest.write("\tnWorkers = " + str(nWorker) + "\n")
        errorTest.flush()
        parallelRun(badAlphaCmd + str(nWorker))
        for pCon in connList:
            line = runTest(nWorker, pCon, IP, Port, rsc, errorTest)
            # line = str(nWorker) + " " + str(pCon) + "\n"
            f.write(line)
            f.flush()
            print("\tSession Ended")
            time.sleep(1)
        # Stoppare il server
        serverKill()

    f.close()

    return


def runApache2experiment(saveDir, IP, Port, rsc, outFD):
    """
    Tutto il necessario per avviare 1 esperimento sulla porta richiesta
    :param saveDir: @string             Directory Home dell'esperimento SENZA "/" finale
    :param IP: @string
    :param Port: @int
    :param rsc: @string                 Path della risorsa partendo SENZA "/" (es "index.html o page/web/...")
    :param outFD: @FDList= [matlabPaths, errorTest]         File Descriptor dei file di salvataggio
    :return:
    """
    matlabPaths = outFD[0]
    errorTes = outFD[1]

    pathExperiment = saveDir + "apache2"
    name = rsc[rsc.rfind("/") + 1: rsc.rfind(".")]
    f = initFileExperiment(pathExperiment, name, matlabPaths)

    global connList
    for pCon in connList:
        line = runTestApache(pCon, IP, Port, rsc, errorTes)
        # line = "1 " + str(pCon) + "\n"
        f.write(line)
        f.flush()
        print("\tSession Ended")
        time.sleep(1)
    f.close()

    return
