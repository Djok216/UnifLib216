#ifndef LOG_H__
#define LOG_H__

#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>

#ifdef LOG_CPP__
#else
extern int debug_level;
extern bool debug_sat;
#endif

const int ERROR = 0;
const int WARNING = 1;
const int INFO = 2;
const int DEBUG3 = 3;
const int DEBUG4 = 4;
const int DEBUG5 = 5;
const int DEBUG6 = 6;
const int DEBUG7 = 7;
const int DEBUG8 = 8;
const int DEBUG9 = 9;
const int DEBUG10 = 10;
const int DEBUG11 = 11;
const int DEBUG12 = 12;
const int DEBUG13 = 13;
const int DEBUG14 = 14;
const int DEBUG15 = 15;

std::string levelToString(const int);

#ifndef NDEBUG

const std::string ANSI_Black();
const std::string ANSI_Red();
const std::string ANSI_Green();
const std::string ANSI_Yellow();
const std::string ANSI_Blue();
const std::string ANSI_Magenta();
const std::string ANSI_Cyan();
const std::string ANSI_White();
const std::string ANSI_Reset();
const std::string ANSI_BRBlack();
const std::string ANSI_BRRed();
const std::string ANSI_BRGreen();
const std::string ANSI_BRYellow();
const std::string ANSI_BRBlue();
const std::string ANSI_BRMagenta();
const std::string ANSI_BRCyan();
const std::string ANSI_BRWhite();
const std::string ANSI_BGBlack();
const std::string ANSI_BGRed();
const std::string ANSI_BGGreen();
const std::string ANSI_BGYellow();
const std::string ANSI_BGBlue();
const std::string ANSI_BGMagenta();
const std::string ANSI_BGCyan();
const std::string ANSI_BGWhite();
const std::string ANSI_BGBRBlack();
const std::string ANSI_BGBRRed();
const std::string ANSI_BGBRGreen();
const std::string ANSI_BGBRYellow();
const std::string ANSI_BGBRBlue();
const std::string ANSI_BGBRMagenta();
const std::string ANSI_BGBRCyan();
const std::string ANSI_BGBRWhite();

#define LOG(level, params) \
  do { \
    if (level <= debug_level) { \
      std::cerr << levelToString(level); \
      std::cerr << ANSI_Green(); \
      std::cerr << __FILE__ << ":"; \
      std::cerr << __LINE__ << ":"; \
      std::cerr << ANSI_Red(); \
      std::cerr << __func__ << " "; \
      std::cerr << ANSI_Reset(); \
      params; \
      std::cerr << std::endl; \
    } \
  } while (0)

#else

#define LOG(level, params)

#endif

#define LOGSAT(params) do { if (debug_sat) { std::cerr << "SAT "; params; std::cerr << std::endl; } } while (0)

#endif
