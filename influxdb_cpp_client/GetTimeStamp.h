#ifndef NC_TIMESTAMP_H
#define NC_TIMESTAMP_H

//Purpose: return an UTC timestamp

#include <chrono>

namespace nc {
    long long GetTimeStamp() {
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
        return nanoseconds;
    }
}
#endif
