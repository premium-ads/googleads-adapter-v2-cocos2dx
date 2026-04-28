#ifndef ADS_LOG_BUF_H
#define ADS_LOG_BUF_H

#include <deque>
#include <mutex>
#include <string>
#include <vector>

namespace adslog {

inline std::deque<std::string>& buffer() { static std::deque<std::string> b; return b; }
inline std::mutex& mtx() { static std::mutex m; return m; }

inline void push(const std::string& s) {
    std::lock_guard<std::mutex> g(mtx());
    buffer().push_back(s);
}

inline std::vector<std::string> drain() {
    std::lock_guard<std::mutex> g(mtx());
    std::vector<std::string> v(buffer().begin(), buffer().end());
    buffer().clear();
    return v;
}

}  // namespace adslog

extern "C" void ads_demo_pushInitLog(const char* msg);

#endif
