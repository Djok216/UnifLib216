#define LOG_CPP__
#include "log.h"
#include <iostream>
#include <string>
#include <cassert>

int debug_level;
bool debug_sat;

using namespace std;

std::string levelToString(const int level)
{
  if (level == ERROR) {
    return "ERR ";
  } else if (level == WARNING) {
    return "WRN ";
  } else if (level == INFO) {
    return "INF ";
  } else if (level == DEBUG3) {
    return "DB3 ";
  } else if (level == DEBUG4) {
    return "DB4 ";
  } else if (level == DEBUG5) {
    return "DB5 ";
  } else if (level == DEBUG6) {
    return "DB6 ";
  } else if (level == DEBUG7) {
    return "DB7 ";
  } else if (level == DEBUG8) {
    return "DB8 ";
  } else if (level == DEBUG9) {
    return "DB9 ";
  } else {
    return "OTH ";
  }
}

const std::string codeANSI_Black = "\u001b[30m";
const std::string codeANSI_Red = "\u001b[31m";
const std::string codeANSI_Green = "\u001b[32m";
const std::string codeANSI_Yellow = "\u001b[33m";
const std::string codeANSI_Blue = "\u001b[34m";
const std::string codeANSI_Magenta = "\u001b[35m";
const std::string codeANSI_Cyan = "\u001b[36m";
const std::string codeANSI_White = "\u001b[37m";
const std::string codeANSI_Reset = "\u001b[0m";

const std::string codeANSI_BRBlack = "\u001b[30;1m";
const std::string codeANSI_BRRed = "\u001b[31;1m";
const std::string codeANSI_BRGreen = "\u001b[32;1m";
const std::string codeANSI_BRYellow = "\u001b[33;1m";
const std::string codeANSI_BRBlue = "\u001b[34;1m";
const std::string codeANSI_BRMagenta = "\u001b[35;1m";
const std::string codeANSI_BRCyan = "\u001b[36;1m";
const std::string codeANSI_BRWhite = "\u001b[37;1m";

const std::string codeANSI_BGBlack = "\u001b[40m";
const std::string codeANSI_BGRed = "\u001b[41m";
const std::string codeANSI_BGGreen = "\u001b[42m";
const std::string codeANSI_BGYellow = "\u001b[43m";
const std::string codeANSI_BGBlue = "\u001b[44m";
const std::string codeANSI_BGMagenta = "\u001b[45m";
const std::string codeANSI_BGCyan = "\u001b[46m";
const std::string codeANSI_BGWhite = "\u001b[47m";
const std::string codeANSI_BGBRBlack = "\u001b[40;1m";
const std::string codeANSI_BGBRRed = "\u001b[41;1m";
const std::string codeANSI_BGBRGreen = "\u001b[42;1m";
const std::string codeANSI_BGBRYellow = "\u001b[43;1m";
const std::string codeANSI_BGBRBlue = "\u001b[44;1m";
const std::string codeANSI_BGBRMagenta = "\u001b[45;1m";
const std::string codeANSI_BGBRCyan = "\u001b[46;1m";
const std::string codeANSI_BGBRWhite = "\u001b[47;1m";

#define USE_ANSI isatty(fileno(stderr))

const std::string ANSI_Black()
{
  return USE_ANSI ? codeANSI_Black : "";
}

const std::string ANSI_Red()
{
  return USE_ANSI ? codeANSI_Red : "";
}

const std::string ANSI_Green()
{
  return USE_ANSI ? codeANSI_Green : "";
}

const std::string ANSI_Yellow()
{
  return USE_ANSI ? codeANSI_Yellow : "";
}

const std::string ANSI_Blue()
{
  return USE_ANSI ? codeANSI_Blue : "";
}

const std::string ANSI_Magenta()
{
  return USE_ANSI ? codeANSI_Magenta : "";
}

const std::string ANSI_Cyan()
{
  return USE_ANSI ? codeANSI_Cyan : "";
}

const std::string ANSI_White()
{
  return USE_ANSI ? codeANSI_White : "";
}

const std::string ANSI_Reset()
{
  return USE_ANSI ? codeANSI_Reset : "";
}

const std::string ANSI_BRBlack()
{
  return USE_ANSI ? codeANSI_BRBlack : "";
}

const std::string ANSI_BRRed()
{
  return USE_ANSI ? codeANSI_BRRed : "";
}

const std::string ANSI_BRGreen()
{
  return USE_ANSI ? codeANSI_BRGreen : "";
}

const std::string ANSI_BRYellow()
{
  return USE_ANSI ? codeANSI_BRYellow : "";
}

const std::string ANSI_BRBlue()
{
  return USE_ANSI ? codeANSI_BRBlue : "";
}

const std::string ANSI_BRMagenta()
{
  return USE_ANSI ? codeANSI_BRMagenta : "";
}

const std::string ANSI_BRCyan()
{
  return USE_ANSI ? codeANSI_BRCyan : "";
}

const std::string ANSI_BRWhite()
{
  return USE_ANSI ? codeANSI_BRWhite : "";
}

const std::string ANSI_BGBlack()
{
  return USE_ANSI ? codeANSI_BGBlack : "";
}

const std::string ANSI_BGRed()
{
  return USE_ANSI ? codeANSI_BGRed : "";
}

const std::string ANSI_BGGreen()
{
  return USE_ANSI ? codeANSI_BGGreen : "";
}

const std::string ANSI_BGYellow()
{
  return USE_ANSI ? codeANSI_BGYellow : "";
}

const std::string ANSI_BGBlue()
{
  return USE_ANSI ? codeANSI_BGBlue : "";
}

const std::string ANSI_BGMagenta()
{
  return USE_ANSI ? codeANSI_BGMagenta : "";
}

const std::string ANSI_BGCyan()
{
  return USE_ANSI ? codeANSI_BGCyan : "";
}

const std::string ANSI_BGWhite()
{
  return USE_ANSI ? codeANSI_BGWhite : "";
}

const std::string ANSI_BGBRBlack()
{
  return USE_ANSI ? codeANSI_BGBRBlack : "";
}

const std::string ANSI_BGBRRed()
{
  return USE_ANSI ? codeANSI_BGBRRed : "";
}

const std::string ANSI_BGBRGreen()
{
  return USE_ANSI ? codeANSI_BGBRGreen : "";
}

const std::string ANSI_BGBRYellow()
{
  return USE_ANSI ? codeANSI_BGBRYellow : "";
}

const std::string ANSI_BGBRBlue()
{
  return USE_ANSI ? codeANSI_BGBRBlue : "";
}

const std::string ANSI_BGBRMagenta()
{
  return USE_ANSI ? codeANSI_BGBRMagenta : "";
}

const std::string ANSI_BGBRCyan()
{
  return USE_ANSI ? codeANSI_BGBRCyan : "";
}

const std::string ANSI_BGBRWhite()
{
  return USE_ANSI ? codeANSI_BGBRWhite : "";
}
