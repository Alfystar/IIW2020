import os
from datetime import datetime


def initExpDir(dirOut):
    """
    La funzione crea dentro la directory <dirOut> una cartella con la data corrente per salvare l'esperimento
    e ritorna il path dell'uscita

    :param dirOut: @strint      Path della directory su cui salvare i risultati SENZA '/' FINALE
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


def initFileExperiment(pathDir, name):
    """
    La funzione prende il path della directory che conterrà l'esperimento e ritorna un FD di questo file e il suo path
    :param pathDir: @string     Path della directory su cui salvare i risultati SENZA '/' FINALE
    :param name: @string        Nome del file
    :return: @FD, @string       File descriptro del file, e path dello stesso
    """
    name = pathDir + "/" + name + "_BenchMark.dat"
    f = open(name, "w")
    # line = str(cpu) + sep + str(parallelCon) + sep + str(nReqSec)+ sep + str(reqTime) + sep + str(speedTransf) + "\n"
    title = "Prove sperimentali del file <" + name + "> i dati sono raccolti nelle colonne:\n"
    title += "N°Worker\tN°Parallel Connection\tRequests [#ps]\tRequestTime [ms]\tHttpSpeedRate [KBps]\n"
    f.write(title)

    return f, name
