#ifndef INFLUXDB_CPP_CLIENT_THREADPOOL_H
#define INFLUXDB_CPP_CLIENT_THREADPOOL_H

#include <thread>
#include <iostream>
#include "QueuePoint.h"
#include "HttpAsyncClient.h"

namespace nc{

    class ThreadPool{
    public:

        ThreadPool(QueuePoint* qp, std::string_view host, int port, std::string_view token, std::string_view org)
                : qp_(qp), host_(host), port_(port), token_(token), org_(org){}

        ~ThreadPool(){
            for (auto& thread : threads_) {thread.join();}
        }

        ThreadPool(const ThreadPool &) = delete;
        ThreadPool &operator=(ThreadPool &) = delete;
        ThreadPool(ThreadPool &&) = delete;
        ThreadPool &operator=(ThreadPool &&) = delete;

        void Initialise() noexcept {
            // Start worker threads
            const unsigned int hw_num_threads = std::thread::hardware_concurrency();
            num_threads_ = hw_num_threads/2;
            for (int i = 0; i < num_threads_; ++i) {
                threads_.emplace_back(&ThreadPool::threadFunction, this, i);
            }
        }

        void Stop() {stopRunning_ = true;}
        auto GetNumThreads() const noexcept{return num_threads_;}

    private:
        void threadFunction(int id){
            BucketPoint bp;
            while (qp_->Dequeue(bp)) {
                if(stopRunning_){
                    break;
                }

                //start client
                net::io_context ioc;
                std::make_shared<HttpAsyncClient>(ioc)->Run(host_, port_, token_, org_, bp);
                ioc.run();
                //writing is done
            }
        }


    private:
        std::vector<std::thread> threads_; //threads pool
        bool stopRunning_{false}; //stop running flat
        QueuePoint* qp_{nullptr}; //pointer to QueuePoint for dequeueing

        std::string host_; //inflxudb host
        int port_; //influxdb port
        std::string token_; //token
        std::string org_; //organisation id

        int num_threads_{0}; //number of thread running
    };
}



#endif
