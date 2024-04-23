#ifndef INFLUXDB_CPP_CLIENT_POINTTOLINEPROTOCOL_H
#define INFLUXDB_CPP_CLIENT_POINTTOLINEPROTOCOL_H

//Purpose: convert Point to Line protocol of InfluxDB

#include <algorithm>
#include "Point.h"

namespace nc {

    class PointToLineProtocol {
    public:
        PointToLineProtocol() { line_.reserve(1024); }
        ~PointToLineProtocol() = default;
        PointToLineProtocol(const PointToLineProtocol &) = delete;
        PointToLineProtocol &operator=(PointToLineProtocol &) = delete;
        PointToLineProtocol(PointToLineProtocol &&) = delete;
        PointToLineProtocol &operator=(PointToLineProtocol &&) = delete;
        std::string ProcessMeasurement(const Point *p) const noexcept { return p->GetMeasurement(); }
        std::string ProcessTimestamp(const Point *p) const noexcept { return std::to_string(p->GetTimeStamp()); }

        std::string Convert(const Point *p) noexcept {
            line_.clear();
            return Process(p);
        }

        std::string Process(const Point *p) noexcept {
            std::string measurement = ProcessMeasurement(p);
            std::string tags = ProcessTag(p);
            std::string fields = ProcessField(p);

            line_ += measurement;
            line_ += ",";
            line_ += tags;
            line_ += " ";
            line_ += fields;

            if (p->TimestampSet()) {
                std::string ts = ProcessTimestamp(p);
                line_ += " ";
                line_ += ts;
            }
            return line_;
        }

        std::string ProcessTag(const Point *p) const noexcept {
            std::string result;
            auto &v = p->GetVectTag();
            std::ranges::for_each(v, [&](const auto &tag) {
                  if (result.length() > 0) {
                      result += ",";
                  }
                  std::string s = tag.GetKey();
                  s += "=";
                  s += tag.GetValue();
                  result += s;
              }
            );
            return result;
        }

        std::string ProcessField(const Point *p) const noexcept {
            std::string result;
            const auto &v = p->GetVectField();

            std::ranges::for_each(v, [&](const auto &field) {
                  if (result.length() > 0) {
                      result += ",";
                  }
                  std::string s = field.GetKey();
                  s += "=";
                  s += field.GetValue();
                  result += s;
              }
            );
            return result;
        }

    private:
        std::string line_;
    };
}


#endif
