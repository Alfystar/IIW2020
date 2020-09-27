import os
from datetime import datetime


def initExpDir(dirOut):
    """
    La funzione crea dentro la directory <dirOut> una cartella con la data corrente per salvare l'esperimento
    e ritorna il path dell'uscita

    :param dirOut: @strint      Path della directory su cui salvare i risultati SENZA '/' FINALE
    :return: @strint            Ritorno il path della direcory su cui salvare "./<direcotries>/"
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


def initFileExperiment(pathExperiment, name, matlabPaths):
    """
    La funzione prende il path della directory che conterrà l'esperimento e ritorna un FD di questo file e il suo path
    :param pathExperiment: @string      Path della directory su cui salvare i risultati SENZA '/' FINALE
    :param name: @string                Nome del file
    :param matlabPaths: @FD             File su cui salvare il path del file corrente
    :return: @FD, @string       File descriptro del file, e path dello stesso
    """

    try:
        os.makedirs(pathExperiment)
    except OSError:
        print("Creation of the directory %s failed" % pathExperiment)
    else:
        print("Successfully created the directory %s" % pathExperiment)


    fileExp = pathExperiment + "/" + name + "_BenchMark.dat"
    f = open(fileExp, "w")
    title = "Prove sperimentali del file <" + name + "> i dati sono raccolti nelle colonne:\n"
    title += "N°Worker\tN°Parallel Connection\tRequests [#ps]\tRequestTime [ms]\tHttpSpeedRate [KBps]\tMin Response [ms]\t" \
             "50% Response [ms]\t66% Response [ms]\t75% Response [ms]\t80% Response [ms]\t90% Response [ms]\t" \
             "95% Response [ms]\t98% Response [ms]\t99% Response [ms]\t100% Response [ms]\t\n"
    f.write(title)
    f.flush()

    matlabPaths.write(fileExp + "\n")
    matlabPaths.flush()

    return f
