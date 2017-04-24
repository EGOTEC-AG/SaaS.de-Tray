#include "logger.h"

#include <time.h>
#include <fstream>
#include <QApplication>

Logger::Logger(std::string action) {
    std::ofstream out((qApp->applicationDirPath() + "/Logs/log.txt").toStdString().c_str(), std::fstream::app|std::fstream::out);
    out << "Function performed - " << currentDateTime().data() << " Method: " << action.data() << std::endl;
    if (action == "sendQueue (IMPORTANT)") {
        out << std::endl;
    }
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
