#pragma once
#include <iostream>
#include <stdio.h> 
#include <stdarg.h>

#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KWHT  "\x1B[37m"

enum LogLevel {
    WARNING,
    ERROR,
    INFO
};

class Lang {
    private:
    static void set_color(const char* colorPrefix) {
        std::cout << colorPrefix;
    };

    static void end_color() {
        std::cout << "\033[0m";
    };

    public:
        static void Log(LogLevel lvl, const char* format, ...) {
            switch (lvl)
            {
            case LogLevel::WARNING:
                set_color(KYEL);
                break;

            case LogLevel::ERROR:
                set_color(KRED);
                break;

            case LogLevel::INFO:
                set_color(KWHT);
                break;
            
            default:
                break;
            }

            va_list args;
            va_start(args, format);
            vprintf(format, args);
            va_end(args);

            end_color();
        };
        
};