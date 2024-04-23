
#ifndef INFLUXDB_CPP_CLIENT_H
#define INFLUXDB_CPP_CLIENT_H

//Purpose: InfluxDB client with Boost.Beast websocket library

#include <string_view>
#include <string>
#include <vector>
#include <queue>
#include <thread>

#include "Point.h"
#include "PointToLineProtocol.h"
#include "QueuePoint.h"
#include "ThreadPool.h"
#include "GetTimeStamp.h"

using namespace std::chrono_literals;

namespace nc{

    class InfluxDBClient {
    public:
        InfluxDBClient(std::string_view host, int port, std::string_view token, std::string_view org)
                : threadpool_(&qp_, host, port, token, org){
            Initialise();
        }

        ~InfluxDBClient() = default;
        InfluxDBClient(const InfluxDBClient &) = delete;
        InfluxDBClient &operator=(InfluxDBClient &) = delete;

        void Write(std::string_view bucket, const Point &p) {
            std::vector<Point> v;
            v.push_back(p);
            BucketPoint bp{bucket, v, GetTimeStamp()};
            qp_.Enqueue(bp);
        }

        void Write(std::string_view bucket, const std::vector<Point> &v) {
            BucketPoint bp{bucket, v, GetTimeStamp()};
            qp_.Enqueue(bp);
        }

        void Write(std::string_view bucket, std::vector<Point> &&v) {
            BucketPoint bp{bucket, std::move(v), GetTimeStamp()};
            qp_.Enqueue(std::move(bp));
        }

        void Stop(){
            threadpool_.Stop();
            for(int i = 0; i < threadpool_.GetNumThreads(); ++i){
                std::vector<Point> v;
                v.emplace_back(Point());
                BucketPoint bp{std::string("bucket"), v};
                qp_.Enqueue(bp);
            }
        }

        void Initialise(){threadpool_.Initialise();}

    private:
        QueuePoint qp_; // the queue for Point
        ThreadPool threadpool_; //thread pool for handlers
    };

}



#endif
