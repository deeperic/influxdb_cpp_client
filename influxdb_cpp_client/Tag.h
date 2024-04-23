#ifndef INFLUXDB_CPP_CLIENT_TAG_H
#define INFLUXDB_CPP_CLIENT_TAG_H

//Purpose: the InfluxDB tag

#include <string>
#include <string_view>

class Tag {
public:
    Tag(std::string_view k, std::string_view v) : key_(k), value_(v) {}
    ~Tag() = default;

    std::string GetKey() const noexcept { return key_; }
    std::string GetValue() const noexcept { return value_; }

private:
    std::string key_;
    std::string value_;
};

#endif
