#include "util.h"
#include <stdarg.h>
#include <memory>
#include <algorithm>
#include <chrono>
#include <fcntl.h>

using namespace std;

namespace handy {
//格式化
string util::format(const char* fmt, ...) {
    char buffer[500];
    unique_ptr<char[]> release1;
    char* base;
    for (int iter = 0; iter < 2; iter++) {
        int bufsize;
        if (iter == 0) {
            bufsize = sizeof(buffer);
            base = buffer;
        } else {
            bufsize = 30000;
            base = new char[bufsize];
            release1.reset(base);
        }
        char* p = base;
        char* limit = base + bufsize;
        if (p < limit) {
            va_list ap;
            va_start(ap, fmt);
            p += vsnprintf(p, limit - p, fmt, ap);
            va_end(ap);
        }
        // Truncate to available space if necessary
        if (p >= limit) {
            if (iter == 0) {
                continue;       // Try again with larger buffer
            } else {
                p = limit - 1;
                *p = '\0';
            }
        }
        break;
    }
    return base;
}
/*
 * system_clock：从系统获取的时钟；
 * steady_clock：不能被修改的时钟；
 * high_resolution_clock：高精度时钟，实际上是system_clock或者steady_clock的别名。
 * 可以通过now()来获取当前时间点：
 *
 * */
//系统时间
int64_t util::timeMicro() {
    chrono::time_point<chrono::system_clock> p = chrono::system_clock::now();
    return chrono::duration_cast<chrono::microseconds>(p.time_since_epoch()).count();
}
//绝对时间
int64_t util::steadyMicro() {
    chrono::time_point<chrono::steady_clock> p = chrono::steady_clock::now();
    return chrono::duration_cast<chrono::microseconds>(p.time_since_epoch()).count();
}
//时间格式化
std::string util::readableTime(time_t t) {
    struct tm tm1;
    localtime_r(&t, &tm1);
    return format("%04d-%02d-%02d %02d:%02d:%02d",
        tm1.tm_year+1900, tm1.tm_mon, tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec);
}

int util::addFdFlag(int fd, int flag) {
    int ret = fcntl(fd, F_GETFD);
    return fcntl(fd, F_SETFD, ret | flag);
}

}