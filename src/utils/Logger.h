#ifndef __UTILS__LOGGER_H__
#define __UTILS__LOGGER_H__

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdarg>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

namespace CoQuiAAS {

    class Logger {

    public:

        static Logger* getInstance() {
            if(instance == NULL) {
                instance = new Logger();
            }
            return instance;
        }

        void addFile(std::string file) {
            if(!this->enabled) {
                this->enabled = true;
                boost::log::register_simple_formatter_factory<boost::log::trivial::severity_level, char>("Severity");
                boost::log::add_common_attributes();
            }
            boost::log::add_file_log(
                boost::log::keywords::file_name = file,
                boost::log::keywords::format = "[%TimeStamp%] [%ThreadID%] [%Severity%] %Message%"
            );
        }

        void trace(const char *format, ...) {
            if(!this->enabled) return;
            va_list args;
            va_start(args, format);
            vsnprintf(this->buffer, this->BUF_SIZE, format, args);
            va_end(args);
            BOOST_LOG_TRIVIAL(trace) << this->buffer;
        }

        void debug(const char *format, ...) {
            if(!this->enabled) return;
            va_list args;
            va_start(args, format);
            vsnprintf(this->buffer, this->BUF_SIZE, format, args);
            va_end(args);
            BOOST_LOG_TRIVIAL(debug) << this->buffer;
        }

        void info(const char *format, ...) {
            if(!this->enabled) return;
            va_list args;
            va_start(args, format);
            vsnprintf(this->buffer, this->BUF_SIZE, format, args);
            va_end(args);
            BOOST_LOG_TRIVIAL(info) << this->buffer;
        }

        void warning(const char *format, ...) {
            if(!this->enabled) return;
            va_list args;
            va_start(args, format);
            vsnprintf(this->buffer, this->BUF_SIZE, format, args);
            va_end(args);
            BOOST_LOG_TRIVIAL(warning) << this->buffer;
        }

        void error(const char *format, ...) {
            if(!this->enabled) return;
            va_list args;
            va_start(args, format);
            vsnprintf(this->buffer, this->BUF_SIZE, format, args);
            va_end(args);
            BOOST_LOG_TRIVIAL(error) << this->buffer;
        }

        void fatal(const char *format, ...) {
            if(!this->enabled) return;
            va_list args;
            va_start(args, format);
            vsnprintf(this->buffer, this->BUF_SIZE, format, args);
            va_end(args);
            BOOST_LOG_TRIVIAL(fatal) << this->buffer;
        }

    private:

        static const unsigned int BUF_SIZE = 2048;

        char buffer[BUF_SIZE];

        static Logger* instance;

        bool enabled = false;

        Logger() {}

    };
}

#endif