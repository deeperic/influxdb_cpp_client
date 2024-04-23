#ifndef INFLUXDB_CPP_CLIENT_POINT_H
#define INFLUXDB_CPP_CLIENT_POINT_H

//Purpose: each Point is a record in InfluxDB bucket

#include <string>
#include <string_view>
#include <vector>

#include "Tag.h"
#include "Field.h"

namespace nc {
    constexpr int VECT_SIZE = 16;

    class Point {
    public:
        Point() {
            vectTag_.reserve(VECT_SIZE);
            vectField_.reserve(VECT_SIZE);
        }
        ~Point() = default;

        explicit Point(std::string_view m) : measurement_(m) {
            vectTag_.reserve(VECT_SIZE);
            vectField_.reserve(VECT_SIZE);
        }

        Point(const Point &t) = default;
        Point &operator=(const Point &t) = default;

        Point(Point &&other) noexcept : measurement_(other.GetMeasurement()),
                               vectTag_(std::move(other.GetVectTag())),
                               vectField_(std::move(other.GetVectField())),
                               timestamp_(other.GetTimeStamp()),
                               timestampSet_(other.TimestampSet()) {
            other.ResetTimestamp();
        }

        Point &operator=(Point &&other) noexcept {
            if (this != &other) {
                measurement_ = other.GetMeasurement();
                vectTag_ = std::move(other.GetVectTag());
                vectField_ = std::move(other.GetVectField());
                timestamp_ = other.GetTimeStamp();
                timestampSet_ = other.TimestampSet();
                other.ResetTimestamp();
            }
            return *this;
        }

        void SetTimeStamp(long long ts) noexcept {
            timestamp_ = ts;
            timestampSet_ = true;
        }

        void ResetTimestamp() noexcept {
            timestamp_ = 0;
            timestampSet_ = false;
        }

        void SetMeasurement(std::string_view m) noexcept { measurement_ = m; }
        std::string GetMeasurement() const noexcept { return measurement_; }
        long long GetTimeStamp() const noexcept { return timestamp_; }
        bool TimestampSet() const noexcept { return timestampSet_; }
        void AddTag(std::string_view k, std::string_view v) noexcept { vectTag_.emplace_back(k, v); }
        void AddField(std::string_view k, std::string_view v) noexcept { vectField_.emplace_back(k, v); }
        void AddField(std::string_view k, double v) noexcept { vectField_.emplace_back(k, v); }
        void AddField(std::string_view k, long long int v) noexcept { vectField_.emplace_back(k, v); }
        void AddField(std::string_view k, int v) noexcept { vectField_.emplace_back(k, v); }
        const std::vector<Tag> &GetVectTag() const noexcept { return vectTag_; }
        std::vector<Tag> &GetVectTag() noexcept { return vectTag_; }
        const std::vector<Field> &GetVectField() const noexcept { return vectField_; }
        std::vector<Field> &GetVectField() noexcept { return vectField_; }

    private:
        std::string measurement_{}; //measurement
        std::vector<Tag> vectTag_; //tags
        std::vector<Field> vectField_; //fields
        long long timestamp_{0}; //timestamp
        bool timestampSet_{false}; //is using my own timestamp?
    };

}

#endif
