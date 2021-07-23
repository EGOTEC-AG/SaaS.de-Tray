#include "logger.h"

#include <time.h>
#include <fstream>
#include <QDir>
#include <QApplication>

Logger::Logger(std::string action) {
    std::string loggingDir = (qApp->applicationDirPath() + "/Logs/").toStdString();
    QDir dir(loggingDir.c_str());
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    std::ofstream out((loggingDir + "log.txt").c_str(), std::fstream::app|std::fstream::out);
    out  << currentDateTime().data() << ": " << action.data() << std::endl;
    out.close();
}

const std::string Logger::currentDateTime() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    return buf;
}
