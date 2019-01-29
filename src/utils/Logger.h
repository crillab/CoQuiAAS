#ifndef __UTILS__LOGGER_H__
#define __UTILS__LOGGER_H__

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdarg>

namespace CoQuiAAS {

    typedef enum {
        DISABLED,
        TRACE,
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        FATAL
    } LoggingLevel;

    class Logger {

    public:

        static Logger* getInstance() {
            if(instance == NULL) {
                instance = new Logger();
            }
            return instance;
        }

        void setFile(std::string file) {
            if(this->output) {
                this->output->close();
                delete this->output;
            }
            this->output = new std::ofstream(file);
            if(!this->output->good()) {
                std::cerr << "FATAL: wrong logging file provided";
                std::exit(1);
            }
        }

        void trace(const char *format, ...) {
            if(this->level < TRACE || !this->output) return;
            va_list args;
            va_start(args, format);
            vsnprintf(this->buffer, this->BUF_SIZE, format, args);
            va_end(args);
            logBuffer("TRACE");
        }

        void debug(const char *format, ...) {
            if(this->level < DEBUG || !this->output) return;
            va_list args;
            va_start(args, format);
            vsnprintf(this->buffer, this->BUF_SIZE, format, args);
            va_end(args);
            logBuffer("DEBUG");
        }

        void info(const char *format, ...) {
            if(this->level < INFO || !this->output) return;
            va_list args;
            va_start(args, format);
            vsnprintf(this->buffer, this->BUF_SIZE, format, args);
            va_end(args);
            logBuffer(" INFO");
        }

        void warning(const char *format, ...) {
            if(this->level < WARNING || !this->output) return;
            va_list args;
            va_start(args, format);
            vsnprintf(this->buffer, this->BUF_SIZE, format, args);
            va_end(args);
            logBuffer(" WARN");
        }

        void error(const char *format, ...) {
            if(this->level < ERROR || !this->output) return;
            va_list args;
            va_start(args, format);
            vsnprintf(this->buffer, this->BUF_SIZE, format, args);
            va_end(args);
            logBuffer("ERROR");
        }

        void fatal(const char *format, ...) {
            if(this->level < FATAL || !this->output) return;
            va_list args;
            va_start(args, format);
            vsnprintf(this->buffer, this->BUF_SIZE, format, args);
            va_end(args);
            logBuffer("FATAL");
        }

    private:

        static const unsigned int BUF_SIZE = 1 << 12;

        char timeBuffer[BUF_SIZE];

        char buffer[BUF_SIZE];

        static Logger* instance;

        LoggingLevel level = INFO;

        std::ofstream* output = NULL;

        inline void logBuffer(std::string strLvl) {
            time_t rawtime;
            struct tm * timeinfo;
            time(&rawtime);
            timeinfo = localtime(&rawtime);
            strftime(timeBuffer, BUF_SIZE, "%Y-%m-%d %H:%M:%S", timeinfo);
            (*this->output) << "[" << timeBuffer << "] [" << strLvl << "] " << buffer << std::endl;
        }

        Logger() {}

        ~Logger() {
            if(this->output) {
                this->output->close();
                delete this->output;
            }
        }

    };
}

#endif