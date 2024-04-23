#ifndef INFLUXDB_CPP_CLIENT_FIELD_H
#define INFLUXDB_CPP_CLIENT_FIELD_H

//Purpose: the mapping to InfluxDB data types in field

#include <string>
#include <string_view>
#include <variant>
#include <format>

template<class... Ts>
struct overloaded : Ts ... {
    using Ts::operator()...;
};
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

namespace nc{

    class Field {
    public:
        explicit Field(std::string_view k) : key_(k) {}
        Field(std::string_view k, int v) : key_(k) { AddValue(v); }
        Field(std::string_view k, long long int v) : key_(k) { AddValue(v); }
        Field(std::string_view k, std::string_view v) : key_(k) { AddValue(v); }
        Field(std::string_view k, double v) : key_(k) { AddValue(v); }
        void AddValue(int v) noexcept { value_ = v; }
        void AddValue(long long int v) noexcept { value_ = v; }
        void AddValue(std::string_view v) noexcept { value_ = std::string(v); }
        void AddValue(double v) noexcept { value_ = v; }
        std::string GetKey() const noexcept { return key_; }
        std::string GetValue() const noexcept {
            std::string result;
            auto visitor = overloaded{
                    [&result](int value) { result = std::format("{}i", value); },
                    [&result](long long int value) { result = std::format("{}i", value); },
                    [&result](double value) { result = std::format("{}", value); },
                    [&result](const std::string &value) { result = std::format("\"{}\"", value); },
            };
            std::visit(visitor, value_);
            return result;
        }

    private:
        std::string key_;
        std::variant<int, long long int, std::string, double> value_;
    };
}

#endif
