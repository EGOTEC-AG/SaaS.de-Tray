#ifndef LOGGER_H
#define LOGGER_H

#include <string>

class Logger {
    public:
        Logger(std::string action);

    private:
        const std::string currentDateTime();
};

#endif // LOGGER_H
