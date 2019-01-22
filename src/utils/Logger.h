#ifndef __UTILS__LOGGER_H__
#define __UTILS__LOGGER_H__

#include <iostream>
#include <fstream>

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

        void trace(std::string msg) {
            if(!this->enabled) return;
            BOOST_LOG_TRIVIAL(trace) << msg;
        }

        void debug(std::string msg) {
            if(!this->enabled) return;
            BOOST_LOG_TRIVIAL(debug) << msg;
        }

        void info(std::string msg) {
            if(!this->enabled) return;
            BOOST_LOG_TRIVIAL(info) << msg;
        }

        void warning(std::string msg) {
            if(!this->enabled) return;
            BOOST_LOG_TRIVIAL(warning) << msg;
        }

        void error(std::string msg) {
            if(!this->enabled) return;
            BOOST_LOG_TRIVIAL(error) << msg;
        }

        void fatal(std::string msg) {
            if(!this->enabled) return;
            BOOST_LOG_TRIVIAL(fatal) << msg;
        }

    private:

        static Logger* instance;

        bool enabled = false;

        Logger() {}

    };
}

#endif