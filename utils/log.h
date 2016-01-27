/**
 * Universal Logging Class
 */

#ifndef UNILOG_H
#define UNILOG_H

/** Cannot use stdint.h when running rootcint on WIN32 */
#if ((defined WIN32) && (defined __CINT__))
typedef unsigned int uint32_t;
typedef unsigned int DWORD;
#include <Windows4Root.h>
#else
#if (defined WIN32)
typedef unsigned int uint32_t;
#include <Windows.h>
#else
#include <sys/time.h>
#include <stdint.h>
#endif //WIN32
#endif //WIN32 && CINT

#ifdef WIN32
#define __func__ __FUNCTION__
#endif // WIN32

#include <sstream>
#include <iomanip>
#include <cstdio>
#include <string.h>


namespace unilog {

  enum TLogLevel {
    logCRITICAL,
    logERROR,
    logRESULT,
    logWARNING,
    logINFO,
    logDEBUG,
    logDEBUG2,
    logDEBUG3,
    logDEBUG4,
    logDEBUG5
  };

  template <typename T>
    class uniLog {
  public:
    uniLog();
    virtual ~uniLog();
    std::ostringstream& Get(TLogLevel level = logINFO, std::string file = "", std::string function = "", uint32_t line = 0);
  public:
    static TLogLevel& ReportingLevel();
    static std::string ToString(TLogLevel level);
    static TLogLevel FromString(const std::string& level);
  protected:
    std::ostringstream os;
  private:
    uniLog(const uniLog&);
    uniLog& operator =(const uniLog&);
    std::string NowTime();
  };

  template <typename T>
    uniLog<T>::uniLog() {}


#ifdef WIN32

  template <typename T>
    std::string uniLog<T>::NowTime(){
    const int MAX_LEN = 200;
    char buffer[MAX_LEN];
    if (GetTimeFormatA(LOCALE_USER_DEFAULT, 0, 0, 
            "HH':'mm':'ss", buffer, MAX_LEN) == 0)
        return "Error in NowTime()";

    char result[100] = {0};
    static DWORD first = GetTickCount();
    std::sprintf(result, "%s.%03ld", buffer, static_cast<long>(GetTickCount() - first) % 1000); 
    return result;
}

#else

  template <typename T>
    std::string uniLog<T>::NowTime() {
    char buffer[11];
    time_t t;
    time(&t);
    tm r = * localtime(&t);
    strftime(buffer, sizeof(buffer), "%X", localtime_r(&t, &r));
    struct timeval tv;
    gettimeofday(&tv, 0);
    char result[100] = {0};
    std::sprintf(result, "%s.%03ld", buffer, static_cast<long>(tv.tv_usec) / 1000); 
    return result;
  }

#endif //WIN32

  template <typename T>
    std::ostringstream& uniLog<T>::Get(TLogLevel level, std::string file, std::string function, uint32_t line) {
    os << "[" << NowTime() << "] ";
    os << std::setw(8) << ToString(level) << ": ";
    
    // For debug levels we want also function name and line number printed:
    if (level != logINFO && level != logRESULT && level != logWARNING)
      os << "<" << file << "/" << function << ":L" << line << "> ";

    return os;
  }

  template <typename T>
    uniLog<T>::~uniLog() {
    os << std::endl;
    T::Output(os.str());
  }

  template <typename T>
    TLogLevel& uniLog<T>::ReportingLevel() {
    static TLogLevel reportingLevel = logINFO;
    return reportingLevel;
  }

  template <typename T>
    std::string uniLog<T>::ToString(TLogLevel level) {
    static const char* const buffer[] = {"CRITICAL", "ERROR", "RESULT", "WARNING", "INFO", "DEBUG", "DEBUG2", "DEBUG3", "DEBUG4", "DEBUG5"};
    return buffer[level];
  }

  template <typename T>
    TLogLevel uniLog<T>::FromString(const std::string& level) {
    if( level == "DEBUG5")
      return logDEBUG5;
    if (level == "DEBUG4")
      return logDEBUG4;
    if (level == "DEBUG3")
      return logDEBUG3;
    if (level == "DEBUG2")
      return logDEBUG2;
    if (level == "DEBUG")
      return logDEBUG;
    if (level == "INFO")
      return logINFO;
    if (level == "WARNING")
      return logWARNING;
    if (level == "ERROR")
      return logERROR;
    if (level == "CRITICAL")
      return logCRITICAL;
    if (level == "RESULT")
      return logRESULT;
    uniLog<T>().Get(logWARNING) << "Unknown logging level '" << level << "'. Using WARNING level as default.";
    return logWARNING;
  }


  class SetLogOutput
  {
  public:
    static FILE*& Stream();
    static bool& Duplicate();
    static void Output(const std::string& msg);
  };

  inline bool& SetLogOutput::Duplicate()
  {
    static bool duplic = false;
    return duplic;
  }

  inline FILE*& SetLogOutput::Stream()
  {
    static FILE* pStream = stderr;
    return pStream;
  }

  inline void SetLogOutput::Output(const std::string& msg)
  {   
    FILE* pStream = Stream();
    if (!pStream)
      return;
    // Check if duplication to stderr is needed:
    if (Duplicate() && pStream != stderr)
      fprintf(stderr, "%s", msg.c_str());
    fprintf(pStream, "%s", msg.c_str());
    fflush(pStream);
  }

typedef uniLog<SetLogOutput> Log;

#define __FILE_NAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define LOG(level)				\
  if (level > unilog::Log::ReportingLevel() || !unilog::SetLogOutput::Stream()) ; \
  else unilog::Log().Get(level,__FILE_NAME__,__func__,__LINE__)

} //namespace unilog

#endif /* UNILOG_H */
