
#ifndef INFLUXDB_CPP_CLIENT_QUEUEPOINT_H
#define INFLUXDB_CPP_CLIENT_QUEUEPOINT_H

#include <mutex>
#include "BucketPoint.h"

namespace nc{

    class QueuePoint{
    public:
        void Enqueue(const BucketPoint& pt){
            std::unique_lock lock(queue_mutex_);
            bp_queue_.emplace(pt);
            queue_cv_.notify_one();
        }

        void Enqueue(BucketPoint&& pt){
            std::unique_lock lock(queue_mutex_);
            bp_queue_.emplace(std::move(pt));
            queue_cv_.notify_one();
        }

        bool Dequeue(BucketPoint& pt){
            std::unique_lock lock(queue_mutex_);
            queue_cv_.wait(lock, [&]{ return !bp_queue_.empty(); });
            if (!bp_queue_.empty()) {
                pt = std::move(bp_queue_.front());
                bp_queue_.pop();
                return true;
            }
            return false;
        }

    private:

        std::mutex queue_mutex_;
        std::condition_variable queue_cv_;
        BUCKET_POINT_QUEUE bp_queue_;

    };

}


#endif
