/*
MIT License

Copyright (c) 2021 Meng Rao <raomeng1@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "fmtlog.h"
#include <mutex>
#include <thread>
#include <limits>
#include <ios>

#ifdef _WIN32
/*
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <processthreadsapi.h>
*/
#else
#include <sys/syscall.h>
#include <unistd.h>
#endif

namespace {
void fmtlogEmptyFun(void*) {
}
} // namespace

template<int ___ = 0>
class fmtlogDetailT
{
public:
  // https://github.com/MengRao/str
  template<size_t SIZE>
  class Str
  {
  public:
    static const int Size = SIZE;
    FMTLOG_CHAR s[SIZE];

    Str() {}
    Str(const FMTLOG_CHAR* p) { *this = *(const Str<SIZE>*)p; }

    FMTLOG_CHAR& operator[](int i) { return s[i]; }
    FMTLOG_CHAR operator[](int i) const { return s[i]; }

    template<typename T>
    void fromi(T num) {
      if constexpr (Size & 1) {
        s[Size - 1] = '0' + (num % 10);
        num /= 10;
      }
      switch (Size & -2) {
        case 18: *(FMTLOG_DWORD*)(s + 16) = *(FMTLOG_DWORD*)(digit_pairs + ((num % 100) << 1)); num /= 100;
        case 16: *(FMTLOG_DWORD*)(s + 14) = *(FMTLOG_DWORD*)(digit_pairs + ((num % 100) << 1)); num /= 100;
        case 14: *(FMTLOG_DWORD*)(s + 12) = *(FMTLOG_DWORD*)(digit_pairs + ((num % 100) << 1)); num /= 100;
        case 12: *(FMTLOG_DWORD*)(s + 10) = *(FMTLOG_DWORD*)(digit_pairs + ((num % 100) << 1)); num /= 100;
        case 10: *(FMTLOG_DWORD*)(s + 8) = *(FMTLOG_DWORD*)(digit_pairs + ((num % 100) << 1)); num /= 100;
        case 8: *(FMTLOG_DWORD*)(s + 6) = *(FMTLOG_DWORD*)(digit_pairs + ((num % 100) << 1)); num /= 100;
        case 6: *(FMTLOG_DWORD*)(s + 4) = *(FMTLOG_DWORD*)(digit_pairs + ((num % 100) << 1)); num /= 100;
        case 4: *(FMTLOG_DWORD*)(s + 2) = *(FMTLOG_DWORD*)(digit_pairs + ((num % 100) << 1)); num /= 100;
        case 2: *(FMTLOG_DWORD*)(s + 0) = *(FMTLOG_DWORD*)(digit_pairs + ((num % 100) << 1)); num /= 100;
      }
    }

    static constexpr const FMTLOG_CHAR* digit_pairs = FMTLOG_T("00010203040506070809")
                                                      FMTLOG_T("10111213141516171819")
                                                      FMTLOG_T("20212223242526272829")
                                                      FMTLOG_T("30313233343536373839")
                                                      FMTLOG_T("40414243444546474849")
                                                      FMTLOG_T("50515253545556575859")
                                                      FMTLOG_T("60616263646566676869")
                                                      FMTLOG_T("70717273747576777879")
                                                      FMTLOG_T("80818283848586878889")
                                                      FMTLOG_T("90919293949596979899");
  };

  fmtlogDetailT()
    : flushDelay(3000000000) {
    args.reserve(4096);
    args.resize(parttenArgSize);

    fmtlogWrapper<>::impl.init();
    resetDate();
    fmtlog::setLogFile(stdout);
    setHeaderPattern(FMTLOG_T("{HMSf} {s:<10} {L}[{t:<5}] "));
    logInfos.reserve(32);
    bgLogInfos.reserve(128);
    bgLogInfos.emplace_back(nullptr, nullptr, fmtlog::DBG, fmt::basic_string_view<FMTLOG_CHAR>());
    bgLogInfos.emplace_back(nullptr, nullptr, fmtlog::INF, fmt::basic_string_view<FMTLOG_CHAR>());
    bgLogInfos.emplace_back(nullptr, nullptr, fmtlog::WRN, fmt::basic_string_view<FMTLOG_CHAR>());
    bgLogInfos.emplace_back(nullptr, nullptr, fmtlog::ERR, fmt::basic_string_view<FMTLOG_CHAR>());
    threadBuffers.reserve(8);
    bgThreadBuffers.reserve(8);
    memset(membuf.data(), 0, membuf.capacity() * sizeof(FMTLOG_CHAR));
  }

  ~fmtlogDetailT() {
    stopPollingThread();
    poll(true);
    closeLogFile();
  }

  void setHeaderPattern(const FMTLOG_CHAR* pattern) {
    if (shouldDeallocateHeader) delete[] headerPattern.data();
    using namespace fmt::literals;
    for (int i = 0; i < parttenArgSize; i++) {
      reorderIdx[i] = parttenArgSize - 1;
    }
#if FMTLOG_UNICODE_STRING == 1
    headerPattern = fmtlog::unNameFormat<true>(
      pattern, reorderIdx, L"a"_a = "", L"b"_a = "", L"C"_a = "", L"Y"_a = "", L"m"_a = "", L"d"_a = "",
      L"t"_a = L"thread name", L"F"_a = "", L"f"_a = "", L"e"_a = "", L"S"_a = "", L"M"_a = "", L"H"_a = "",
      L"l"_a = fmtlog::LogLevel(), L"s"_a = L"fmtlog.cc:123", L"g"_a = L"/home/raomeng/fmtlog/fmtlog.cc:123", L"Ymd"_a = "",
      L"HMS"_a = "", L"HMSe"_a = "", L"HMSf"_a = "", L"HMSF"_a = "", L"YmdHMS"_a = "", L"YmdHMSe"_a = "", L"YmdHMSf"_a = "",
      L"YmdHMSF"_a = "", L"L"_a = fmtlog::LogLevel());
#else
    headerPattern = fmtlog::unNameFormat<true>(
      pattern, reorderIdx, "a"_a = "", "b"_a = "", "C"_a = "", "Y"_a = "", "m"_a = "", "d"_a = "",
      "t"_a = "thread name", "F"_a = "", "f"_a = "", "e"_a = "", "S"_a = "", "M"_a = "", "H"_a = "",
      "l"_a = fmtlog::LogLevel(), "s"_a = "fmtlog.cc:123", "g"_a = "/home/raomeng/fmtlog/fmtlog.cc:123", "Ymd"_a = "",
      "HMS"_a = "", "HMSe"_a = "", "HMSf"_a = "", "HMSF"_a = "", "YmdHMS"_a = "", "YmdHMSe"_a = "", "YmdHMSf"_a = "",
      "YmdHMSF"_a = "", "L"_a = fmtlog::LogLevel());
#endif
    shouldDeallocateHeader = headerPattern.data() != pattern;

    setArg<0>(fmt::basic_string_view<FMTLOG_CHAR>(weekdayName.s, 3));
    setArg<1>(fmt::basic_string_view<FMTLOG_CHAR>(monthName.s, 3));
    setArg<2>(fmt::basic_string_view<FMTLOG_CHAR>(&year[2], 2));
    setArg<3>(fmt::basic_string_view<FMTLOG_CHAR>(year.s, 4));
    setArg<4>(fmt::basic_string_view<FMTLOG_CHAR>(month.s, 2));
    setArg<5>(fmt::basic_string_view<FMTLOG_CHAR>(day.s, 2));
    setArg<6>(fmt::basic_string_view<FMTLOG_CHAR>());
    setArg<7>(fmt::basic_string_view<FMTLOG_CHAR>(nanosecond.s, 9));
    setArg<8>(fmt::basic_string_view<FMTLOG_CHAR>(nanosecond.s, 6));
    setArg<9>(fmt::basic_string_view<FMTLOG_CHAR>(nanosecond.s, 3));
    setArg<10>(fmt::basic_string_view<FMTLOG_CHAR>(second.s, 2));
    setArg<11>(fmt::basic_string_view<FMTLOG_CHAR>(minute.s, 2));
    setArg<12>(fmt::basic_string_view<FMTLOG_CHAR>(hour.s, 2));
    setArg<13>(fmt::basic_string_view<FMTLOG_CHAR>(logLevel.s, 3));
    setArg<14>(fmt::basic_string_view<FMTLOG_CHAR>());
    setArg<15>(fmt::basic_string_view<FMTLOG_CHAR>());
    setArg<16>(fmt::basic_string_view<FMTLOG_CHAR>(year.s, 10)); // Ymd
    setArg<17>(fmt::basic_string_view<FMTLOG_CHAR>(hour.s, 8));  // HMS
    setArg<18>(fmt::basic_string_view<FMTLOG_CHAR>(hour.s, 12)); // HMSe
    setArg<19>(fmt::basic_string_view<FMTLOG_CHAR>(hour.s, 15)); // HMSf
    setArg<20>(fmt::basic_string_view<FMTLOG_CHAR>(hour.s, 18)); // HMSF
    setArg<21>(fmt::basic_string_view<FMTLOG_CHAR>(year.s, 19));   // YmdHMS
    setArg<22>(fmt::basic_string_view<FMTLOG_CHAR>(year.s, 23));   // YmdHMSe
    setArg<23>(fmt::basic_string_view<FMTLOG_CHAR>(year.s, 26));   // YmdHMSf
    setArg<24>(fmt::basic_string_view<FMTLOG_CHAR>(year.s, 29));   // YmdHMSF
#if FMTLOG_UNICODE_STRING == 1
    setArg<25>(fmt::basic_string_view<FMTLOG_CHAR>(logLevelCN.s, 2));
#else
    setArg<25>(fmt::basic_string_view<FMTLOG_CHAR>(logLevelCN.s, 6));
#endif
  }

  class ThreadBufferDestroyer
  {
  public:
    explicit ThreadBufferDestroyer() {}

    void threadBufferCreated() {}

    ~ThreadBufferDestroyer() {
      if (fmtlog::threadBuffer != nullptr) {
        fmtlog::threadBuffer->shouldDeallocate = true;
        fmtlog::threadBuffer = nullptr;
      }
    }
  };

  struct StaticLogInfo
  {
    // Constructor
    constexpr StaticLogInfo(fmtlog::FormatToFn fn, const FMTLOG_CHAR* loc, fmtlog::LogLevel level, fmt::basic_string_view<FMTLOG_CHAR> fmtString)
      : formatToFn(fn)
      , formatString(fmtString)
      , location(loc)
      , logLevel(level)
      , argIdx(-1) {}

    void processLocation() {
      size_t size = FMTLOG_tcslen(location);
      const FMTLOG_CHAR* p = location + size;
      if (size > 255) {
        location = p - 255;
      }
      endPos = static_cast<uint8_t>(p - location);
      const FMTLOG_CHAR* base = location;
      while (p > location) {
        FMTLOG_CHAR c = *--p;
        if (c == '/' || c == '\\') {
          base = p + 1;
          break;
        }
      }
      basePos = static_cast<uint8_t>(base - location);
    }

    inline fmt::basic_string_view<FMTLOG_CHAR> getBase() { return fmt::basic_string_view<FMTLOG_CHAR>(location + basePos, endPos - basePos); }

    inline fmt::basic_string_view<FMTLOG_CHAR> getLocation() { return fmt::basic_string_view<FMTLOG_CHAR>(location, endPos); }

    fmtlog::FormatToFn formatToFn;
    fmt::basic_string_view<FMTLOG_CHAR> formatString;
    const FMTLOG_CHAR* location;
    uint8_t basePos;
    uint8_t endPos;
    fmtlog::LogLevel logLevel;
    int argIdx;
  };

  static thread_local ThreadBufferDestroyer sbc;
  int64_t midnightNs;
  fmt::basic_string_view<FMTLOG_CHAR> headerPattern;
  bool shouldDeallocateHeader = false;
  FILE* outputFp = nullptr;
  bool manageFp = false;
  size_t fpos = 0; // file position of membuf, used only when manageFp == true
  int64_t flushDelay;
  int64_t nextFlushTime = (std::numeric_limits<int64_t>::max)();
  uint32_t flushBufSize = 8 * 1024;
  fmtlog::LogLevel flushLogLevel = fmtlog::OFF;
  std::mutex bufferMutex;
  std::vector<fmtlog::ThreadBuffer*> threadBuffers;
  struct HeapNode
  {
    HeapNode(fmtlog::ThreadBuffer* buffer)
      : tb(buffer) {}

    fmtlog::ThreadBuffer* tb;
    const fmtlog::SPSCVarQueueOPT::MsgHeader* header = nullptr;
  };
  std::vector<HeapNode> bgThreadBuffers;
  std::mutex logInfoMutex;
  std::vector<StaticLogInfo> logInfos;
  std::vector<StaticLogInfo> bgLogInfos;

  std::basic_string<FMTLOG_CHAR> dailyFileName;
  std::basic_string<FMTLOG_CHAR> dataFormat;
  time_t nextDailyTime = 0;
  int32_t nextDayDiffSecond = 0;

  fmtlog::LogCBFn logCB = nullptr;
  fmtlog::LogLevel minCBLogLevel;
  fmtlog::LogQFullCBFn logQFullCB = fmtlogEmptyFun;
  void* logQFullCBArg = nullptr;

  fmtlog::MemoryBuffer membuf;

  const static int parttenArgSize = 26;
  uint32_t reorderIdx[parttenArgSize];
  Str<3> weekdayName;
  Str<3> monthName;
  Str<4> year;
  FMTLOG_CHAR dash1 = '-';
  Str<2> month;
  FMTLOG_CHAR dash2 = '-';
  Str<2> day;
  FMTLOG_CHAR space = ' ';
  Str<2> hour;
  FMTLOG_CHAR colon1 = ':';
  Str<2> minute;
  FMTLOG_CHAR colon2 = ':';
  Str<2> second;
  FMTLOG_CHAR dot1 = '.';
  Str<9> nanosecond;
  Str<3> logLevel;
#if FMTLOG_UNICODE_STRING == 1
  Str<2> logLevelCN;
#else
  Str<6> logLevelCN;
#endif
  std::vector<fmt::basic_format_arg<fmtlog::Context>> args;

  volatile bool threadRunning = false;
  std::thread thr;

  void resetDate() {
    time_t rawtime = fmtlogWrapper<>::impl.tscns.rdns() / 1000000000;
    struct tm* timeinfo = localtime(&rawtime);
    timeinfo->tm_sec = timeinfo->tm_min = timeinfo->tm_hour = 0;
    midnightNs = mktime(timeinfo) * 1000000000;
    year.fromi(1900 + timeinfo->tm_year);
    month.fromi(1 + timeinfo->tm_mon);
    day.fromi(timeinfo->tm_mday);
    const FMTLOG_CHAR* weekdays[7] = {FMTLOG_T("Sun"), FMTLOG_T("Mon"), FMTLOG_T("Tue"), FMTLOG_T("Wed"), FMTLOG_T("Thu"), FMTLOG_T("Fri"), FMTLOG_T("Sat")};
    weekdayName = weekdays[timeinfo->tm_wday];
    const FMTLOG_CHAR* monthNames[12] = {FMTLOG_T("Jan"), FMTLOG_T("Feb"), FMTLOG_T("Mar"), FMTLOG_T("Apr"), FMTLOG_T("May"), FMTLOG_T("Jun"), FMTLOG_T("Jul"), FMTLOG_T("Aug"), FMTLOG_T("Sep"), FMTLOG_T("Oct"), FMTLOG_T("Nov"), FMTLOG_T("Dec")};
    monthName = monthNames[timeinfo->tm_mon];
  }

  void preallocate() {
    if (fmtlog::threadBuffer) return;
    fmtlog::threadBuffer = new fmtlog::ThreadBuffer();
#ifdef _WIN32
    uint32_t tid = static_cast<uint32_t>(::GetCurrentThreadId());
#else
    uint32_t tid = static_cast<uint32_t>(::syscall(SYS_gettid));
#endif
    fmtlog::threadBuffer->nameSize =
      fmt::format_to_n(fmtlog::threadBuffer->name, sizeof(fmtlog::threadBuffer->name), FMTLOG_T("{}"), tid).size;
    sbc.threadBufferCreated();

    std::unique_lock<std::mutex> guard(bufferMutex);
    threadBuffers.push_back(fmtlog::threadBuffer);
  }

  template<size_t I, typename T>
  inline void setArg(const T& arg) {
    args[reorderIdx[I]] = fmt::detail::make_arg<fmtlog::Context>(arg);
  }

  template<size_t I, typename T>
  inline void setArgVal(const T& arg) {
    fmt::detail::value<fmtlog::Context>& value_ = *(fmt::detail::value<fmtlog::Context>*)&args[reorderIdx[I]];
    value_ = fmt::detail::arg_mapper<fmtlog::Context>().map(arg);
  }

  void flushLogFile() {
    if (outputFp) {
      fwrite(membuf.data(), sizeof(FMTLOG_CHAR), membuf.size(), outputFp);
      if (!manageFp) fflush(outputFp);
      else
        fpos += membuf.size();
    }
    membuf.clear();
    memset(membuf.data(), 0, membuf.capacity() * sizeof(FMTLOG_CHAR));
    nextFlushTime = (std::numeric_limits<int64_t>::max)();
  }

  void closeLogFile() {
    if (membuf.size()) flushLogFile();
    if (manageFp) fclose(outputFp);
    outputFp = nullptr;
    manageFp = false;
  }

  void startPollingThread(int64_t pollInterval) {
    stopPollingThread();
    threadRunning = true;
    thr = std::thread([pollInterval, this]() {
      while (threadRunning) {
        int64_t before = fmtlogWrapper<>::impl.tscns.rdns();
        poll(false);
        int64_t delay = fmtlogWrapper<>::impl.tscns.rdns() - before;
        if (delay < pollInterval) {
          std::this_thread::sleep_for(std::chrono::nanoseconds(pollInterval - delay));
        }
      }
      poll(true);
    });
  }

  void stopPollingThread() {
    if (!threadRunning) return;
    threadRunning = false;
    if (thr.joinable()) thr.join();
  }

  void handleLog(fmt::basic_string_view<FMTLOG_CHAR> threadName, const fmtlog::SPSCVarQueueOPT::MsgHeader* header) {
    setArgVal<6>(threadName);
    StaticLogInfo& info = bgLogInfos[header->logId];
    const FMTLOG_CHAR* data = (const FMTLOG_CHAR*)(header + 1);
    const FMTLOG_CHAR* end;
    int64_t tsc = *(int64_t*)data;
    data += (8 / sizeof(FMTLOG_CHAR));
    if (!info.formatToFn) { // log once
      info.location = *(const FMTLOG_CHAR**)data;
      data += (8 / sizeof(FMTLOG_CHAR));
      info.processLocation();
      end = data + (header->size - 24) / sizeof(FMTLOG_CHAR);
    } else {
      end = data + (header->size - 16) / sizeof(FMTLOG_CHAR);
    }
    int64_t ts = fmtlogWrapper<>::impl.tscns.tsc2ns(tsc);
    // the date could go back when polling different threads
    uint64_t t = (ts > midnightNs) ? (ts - midnightNs) : 0;
    nanosecond.fromi(t % 1000000000);
    t /= 1000000000;
    second.fromi(t % 60);
    t /= 60;
    minute.fromi(t % 60);
    t /= 60;
    uint32_t h = static_cast<uint32_t>(t); // hour
    if (h > 23) {
      h %= 24;
      resetDate();
    }
    hour.fromi(h);
    setArgVal<14>(info.getBase());
    setArgVal<15>(info.getLocation());
    logLevel = (const FMTLOG_CHAR*)FMTLOG_T("DBG INF WRN ERR OFF") + (info.logLevel << 2);
    const FMTLOG_CHAR* logLevelCNs[5] = {FMTLOG_T("调试"), FMTLOG_T("信息"), FMTLOG_T("警告"), FMTLOG_T("错误"), FMTLOG_T("关闭")};
    logLevelCN = logLevelCNs[info.logLevel];
    size_t headerPos = membuf.size();
    fmtlog::vformat_to(membuf, headerPattern, fmt::basic_format_args(args.data(), parttenArgSize));
    size_t bodyPos = membuf.size();
    if (info.formatToFn) {
      info.formatToFn(info.formatString, data, membuf, info.argIdx, args);
    }
    else { // log once
      membuf.append(fmt::basic_string_view<FMTLOG_CHAR>(data, end - data));
    }

    if (logCB && info.logLevel >= minCBLogLevel) {
      logCB(ts, info.logLevel, info.getLocation(), info.basePos, threadName,
            fmt::basic_string_view<FMTLOG_CHAR>(membuf.data() + headerPos, membuf.size() - headerPos), bodyPos - headerPos,
            fpos + headerPos, logQFullCBArg);
    }
    membuf.append(fmt::basic_string_view<FMTLOG_CHAR>(FMTLOG_T("\r\n")));
    if (membuf.size() >= flushBufSize || info.logLevel >= flushLogLevel) {
      flushLogFile();
    }
  }

  void adjustHeap(size_t i) {
    while (true) {
      size_t min_i = i;
      for (size_t ch = i * 2 + 1, end = (std::min)(ch + 2, bgThreadBuffers.size()); ch < end; ch++) {
        auto h_ch = bgThreadBuffers[ch].header;
        auto h_min = bgThreadBuffers[min_i].header;
        if (h_ch && (!h_min || *(int64_t*)(h_ch + 1) < *(int64_t*)(h_min + 1))) min_i = ch;
      }
      if (min_i == i) break;
      std::swap(bgThreadBuffers[i], bgThreadBuffers[min_i]);
      i = min_i;
    }
  }

  void poll(bool forceFlush) {
    fmtlogWrapper<>::impl.tscns.calibrate();
    int64_t tsc = fmtlogWrapper<>::impl.tscns.rdtsc();
    if (logInfos.size()) {
      std::unique_lock<std::mutex> lock(logInfoMutex);
      for (auto& info : logInfos) {
        info.processLocation();
      }
      bgLogInfos.insert(bgLogInfos.end(), logInfos.begin(), logInfos.end());
      logInfos.clear();
    }
    if (threadBuffers.size()) {
      std::unique_lock<std::mutex> lock(bufferMutex);
      for (auto tb : threadBuffers) {
        bgThreadBuffers.emplace_back(tb);
      }
      threadBuffers.clear();
    }

    for (size_t i = 0; i < bgThreadBuffers.size(); i++) {
      auto& node = bgThreadBuffers[i];
      if (node.header) continue;
      node.header = node.tb->varq.front();
      if (!node.header && node.tb->shouldDeallocate) {
        delete node.tb;
        node = bgThreadBuffers.back();
        bgThreadBuffers.pop_back();
        i--;
      }
    }

    if (bgThreadBuffers.empty()) return;

    // build heap
    for (int i = static_cast<int>(bgThreadBuffers.size() / 2); i >= 0; i--) {
      adjustHeap(i);
    }

    while (true) {
      auto h = bgThreadBuffers[0].header;
      if (!h || h->logId >= bgLogInfos.size() || *(int64_t*)(h + 1) >= tsc) break;
      auto tb = bgThreadBuffers[0].tb;
      handleLog(fmt::basic_string_view<FMTLOG_CHAR>(tb->name, tb->nameSize), h);
      tb->varq.pop();
      bgThreadBuffers[0].header = tb->varq.front();
      adjustHeap(0);
    }

    if (membuf.size() == 0) return;
    if (!manageFp || forceFlush) {
      flushLogFile();
      return;
    }
    int64_t now = fmtlogWrapper<>::impl.tscns.tsc2ns(tsc);
    if (nextDailyTime != 0 && now > nextDailyTime) {
      size_t pos1 = dailyFileName.find_last_of('.');
      size_t pos2 = dailyFileName.find_last_of('\\');
      std::basic_string<FMTLOG_CHAR> logFile = dailyFileName;
      struct tm timeinfo;
      time_t rawtime = now / 1000000000;
      ::localtime_s(&timeinfo, &rawtime);
      FMTLOG_CHAR szTime[256] { 0 };
      FMTLOG_tcsftime(szTime, sizeof(szTime), dataFormat.c_str(), &timeinfo);
      if (pos1 > pos2)
        logFile.insert(pos1, szTime);
      else
        logFile.append(szTime);
      timeinfo.tm_sec = 0; timeinfo.tm_min = 0; timeinfo.tm_hour = 0;
      nextDailyTime = (mktime(&timeinfo) + nextDayDiffSecond) * 1000000000;
      fmtlog::setLogFile(logFile.c_str(), false);
    }
    else if (now > nextFlushTime) {
      flushLogFile();
    }
    else if (nextFlushTime == (std::numeric_limits<int64_t>::max)()) {
      nextFlushTime = now + flushDelay;
    }
  }
};

template<int _>
thread_local typename fmtlogDetailT<_>::ThreadBufferDestroyer fmtlogDetailT<_>::sbc;

template<int __ = 0>
struct fmtlogDetailWrapper
{ static fmtlogDetailT<> impl; };

template<int _>
fmtlogDetailT<> fmtlogDetailWrapper<_>::impl;

template<int _>
void fmtlogT<_>::registerLogInfo(uint32_t& logId, FormatToFn fn, const FMTLOG_CHAR* location,
                                 LogLevel level, fmt::basic_string_view<FMTLOG_CHAR> fmtString) noexcept {
  auto& d = fmtlogDetailWrapper<>::impl;
  std::lock_guard<std::mutex> lock(d.logInfoMutex);
  if (logId) return;
  logId = static_cast<uint32_t>(d.logInfos.size() + d.bgLogInfos.size());
  d.logInfos.emplace_back(fn, location, level, fmtString);
}

template<int _>
void fmtlogT<_>::vformat_to(fmtlog::MemoryBuffer& out, fmt::basic_string_view<FMTLOG_CHAR> fmt,
                            fmt::basic_format_args<Context> args) {
  fmt::detail::vformat_to(out, fmt, args);
}

template<int _>
size_t fmtlogT<_>::formatted_size(fmt::basic_string_view<FMTLOG_CHAR> fmt, fmt::basic_format_args<Context> args) {
  auto buf = fmt::detail::counting_buffer<FMTLOG_CHAR>();
  fmt::detail::vformat_to(buf, fmt, args);
  return buf.count();
}

template<int _>
void fmtlogT<_>::vformat_to(FMTLOG_CHAR* out, fmt::basic_string_view<FMTLOG_CHAR> fmt, fmt::basic_format_args<Context> args) {
  fmt::vformat_to(out, fmt, args);
}

template<int _>
typename fmtlogT<_>::SPSCVarQueueOPT::MsgHeader* fmtlogT<_>::allocMsg(uint32_t size,
                                                                      bool q_full_cb) noexcept {
  auto& d = fmtlogDetailWrapper<>::impl;
  if (threadBuffer == nullptr) preallocate();
  auto ret = threadBuffer->varq.alloc(size);
  if ((ret == nullptr) & q_full_cb) d.logQFullCB(d.logQFullCBArg);
  return ret;
}

template<int _>
typename fmtlogT<_>::SPSCVarQueueOPT::MsgHeader*
fmtlogT<_>::SPSCVarQueueOPT::allocMsg(uint32_t size) noexcept {
  return alloc(size);
}

template<int _>
void fmtlogT<_>::preallocate() noexcept {
  fmtlogDetailWrapper<>::impl.preallocate();
}

template<int _>
bool fmtlogT<_>::setLogFile(const FMTLOG_CHAR* filename, bool truncate) {
  auto& d = fmtlogDetailWrapper<>::impl;
  FILE* newFp = FMTLOG_tfopen(filename, truncate ? FMTLOG_T("wb") : FMTLOG_T("ab"));
  if (!newFp) {
    /*
    std::string err = fmt::format("Unable to open file: {}", strerror(errno));
    fmt::detail::throw_format_error(err.c_str());
    */
    return false;
  }
  setbuf(newFp, nullptr);
  fseek(newFp, 0, SEEK_END);
  d.fpos = ftell(newFp);
  if (d.fpos == 0) {
#if FMTLOG_UNICODE_STRING == 1
    const unsigned char utfBom[2]{0xFF, 0xFE};
#else
    const unsigned char utfBom[3]{0xEF, 0xBB, 0xBF};
#endif
    fwrite(&utfBom, 1, sizeof(utfBom), newFp);
  }

  closeLogFile();
  d.outputFp = newFp;
  d.manageFp = true;
  return true;
}

template<int _>
void fmtlogT<_>::setLogFile(FILE* fp, bool manageFp) {
  auto& d = fmtlogDetailWrapper<>::impl;
  closeLogFile();
  if (manageFp) {
    setbuf(fp, nullptr);
    d.fpos = ftell(fp);
  }
  else
    d.fpos = 0;
  d.outputFp = fp;
  d.manageFp = manageFp;
}

template<int _>
bool fmtlogT<_>::setDailyLogFile(const FMTLOG_CHAR* filename, bool truncate, int32_t hour, int32_t second, const FMTLOG_CHAR* DateFormat) noexcept {
  auto& d = fmtlogDetailWrapper<>::impl;
  d.dailyFileName = filename;
  size_t pos1 = d.dailyFileName.find_last_of('.');
  size_t pos2 = d.dailyFileName.find_last_of('\\');
  if (pos1 == std::basic_string<FMTLOG_CHAR>::npos || pos2 == std::basic_string<FMTLOG_CHAR>::npos) {
    return false;
  }
  std::basic_string<FMTLOG_CHAR> logFile = d.dailyFileName;
  d.nextDayDiffSecond = (((24 + hour) * 60) + second) * 60;
  time_t rawtime = fmtlogWrapper<>::impl.tscns.rdns() / 1000000000;
  struct tm timeinfo;
  ::localtime_s(&timeinfo, &rawtime);
  FMTLOG_CHAR szTime[256] { 0 };
  FMTLOG_tcsftime(szTime, sizeof(szTime), DateFormat, &timeinfo);
  if (pos1 > pos2)
    logFile.insert(pos1, szTime);
  else
    logFile.append(szTime);
  d.dataFormat = DateFormat;
  timeinfo.tm_sec = 0; timeinfo.tm_min = 0; timeinfo.tm_hour = 0;
  d.nextDailyTime = (mktime(&timeinfo) + d.nextDayDiffSecond) * 1000000000;
  return setLogFile(logFile.c_str(), truncate);
}

template<int _>
void fmtlogT<_>::closeDailyLogFile() noexcept {
  auto& d = fmtlogDetailWrapper<>::impl;
  d.nextDailyTime = 0;
  d.nextDayDiffSecond = 0;
  d.closeLogFile();
}

template<int _>
void fmtlogT<_>::setFlushDelay(int64_t ns) noexcept {
  fmtlogDetailWrapper<>::impl.flushDelay = ns;
}

template<int _>
void fmtlogT<_>::flushOn(LogLevel flushLogLevel) noexcept {
  fmtlogDetailWrapper<>::impl.flushLogLevel = flushLogLevel;
}

template<int _>
void fmtlogT<_>::setFlushBufSize(uint32_t bytes) noexcept {
  fmtlogDetailWrapper<>::impl.flushBufSize = bytes;
}

template<int _>
void fmtlogT<_>::closeLogFile() noexcept {
  fmtlogDetailWrapper<>::impl.closeLogFile();
}

template<int _>
void fmtlogT<_>::poll(bool forceFlush) {
  fmtlogDetailWrapper<>::impl.poll(forceFlush);
}

template<int _>
void fmtlogT<_>::setThreadName(const FMTLOG_CHAR* name) noexcept {
  preallocate();
  threadBuffer->nameSize = fmt::format_to_n(threadBuffer->name, sizeof(fmtlog::threadBuffer->name), FMTLOG_T("{}"), name).size;
}

template<int _>
void fmtlogT<_>::setLogCB(LogCBFn cb, LogLevel minCBLogLevel_) noexcept {
  auto& d = fmtlogDetailWrapper<>::impl;
  d.logCB = cb;
  d.minCBLogLevel = minCBLogLevel_;
}

template<int _>
void fmtlogT<_>::setLogQFullCB(LogQFullCBFn cb, void* userData) noexcept {
  auto& d = fmtlogDetailWrapper<>::impl;
  d.logQFullCB = cb;
  d.logQFullCBArg = userData;
}

template<int _>
void fmtlogT<_>::setHeaderPattern(const FMTLOG_CHAR* pattern) {
  fmtlogDetailWrapper<>::impl.setHeaderPattern(pattern);
}

template<int _>
void fmtlogT<_>::startPollingThread(int64_t pollInterval) noexcept {
  fmtlogDetailWrapper<>::impl.startPollingThread(pollInterval);
}

template<int _>
void fmtlogT<_>::stopPollingThread() noexcept {
  fmtlogDetailWrapper<>::impl.stopPollingThread();
}

template class fmtlogT<0>;

