#ifndef INFLUXDB_CPP_CLIENT_BUCKETPOINT_H
#define INFLUXDB_CPP_CLIENT_BUCKETPOINT_H

//Purpose: The BucketPoint struct contains the points and the name of bucket they belong to

#include <string>
#include <vector>
#include <queue>
#include "Point.h"

namespace nc{
    struct BucketPoint{
        BucketPoint() = default;
        ~BucketPoint() = default;

        BucketPoint(std::string_view b, const std::vector<Point>& p): bucket(b), points(p){}
        BucketPoint(std::string_view b, const std::vector<Point>& p, long long t): bucket(b), points(p), timestamp(t){}
        BucketPoint(std::string_view b, std::vector<Point>&& p): bucket(b), points(std::move(p)){}
        BucketPoint(std::string_view b, std::vector<Point>&& p, long long t):bucket(b), points(std::move(p)), timestamp(t){}
        BucketPoint(const BucketPoint &t) = default;
        BucketPoint &operator=(const BucketPoint &t) = default;
        BucketPoint(BucketPoint &&other) noexcept : bucket(std::move(other.bucket)), points(std::move(other.points)), timestamp(other.timestamp){}

        BucketPoint &operator=(BucketPoint &&other) noexcept {
            if (this != &other) {
                bucket = std::move(other.bucket);
                points = std::move(other.points);
                std::swap(timestamp, other.timestamp);
            }
            return *this;
        }

        std::string bucket;
        std::vector<Point> points;
        long long timestamp;
    };

    using BUCKET_POINT_QUEUE = std::queue<BucketPoint>;

}


#endif
