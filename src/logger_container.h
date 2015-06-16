/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Sat May 16 22:24:08 2015
 * Description: 
 ************************************************************************/

#ifndef __LOGGER_CONTAINER_H__
#define __LOGGER_CONTAINER_H__

#define LOG4CPLUS_HAVE_GNU_VARIADIC_MACROS 1
#undef LOG4CPLUS_HAVE_C99_VARIADIC_MACROS

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/loggingmacros.h>

#define LOG4CPLUS_HAVE_GNU_VARIADIC_MACROS 1
#undef LOG4CPLUS_HAVE_C99_VARIADIC_MACROS

class LoggerContainer
{
public:
    static void init() {
        logger = log4cplus::Logger::getRoot();
        log4cplus::PropertyConfigurator::doConfigure("log.properties");
    }
    static log4cplus::Logger logger;
};

// flag: INFO TRACE DEBUG WARN ERROR FATAL
#define LOG_F_INTERNAL(flag, logFmt, ...) \
    LOG4CPLUS_##flag##_FMT(LoggerContainer::logger, logFmt, __VA_ARGS__)

#define LOG(flag, logEvent) \
    LOG_F_INTERNAL(flag, "[%s] * "logEvent, __FUNCTION__)

#define LOG_F(flag, logFmt, ...) \
    LOG_F_INTERNAL(flag, "[%s] * "logFmt, __FUNCTION__, __VA_ARGS__)

#endif //__LOGGER_CONTAINER_H__
