#ifndef INFLUXDB_CPP_CLIENT_HTTPASYNCCLIENT_H
#define INFLUXDB_CPP_CLIENT_HTTPASYNCCLIENT_H

// Purpose: Async Http client for InfluxDB operation

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/strand.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

#include "QueuePoint.h"
#include "PointToLineProtocol.h"
#include "GetTimeStamp.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

namespace nc{
    class HttpAsyncClient : public std::enable_shared_from_this<HttpAsyncClient>
    {
    public:

        explicit HttpAsyncClient(net::io_context& ioc)
                : resolver_(net::make_strand(ioc)) , stream_(net::make_strand(ioc)){}

        ~HttpAsyncClient(){
            beast::error_code ec;
            stream_.socket().shutdown(tcp::socket::shutdown_both, ec);
        }

        HttpAsyncClient(const HttpAsyncClient &) = delete;
        HttpAsyncClient &operator=(HttpAsyncClient &) = delete;
        HttpAsyncClient(HttpAsyncClient &&) = delete;
        HttpAsyncClient &operator=(HttpAsyncClient &&) = delete;

        // Start the asynchronous operation
        void Run(std::string_view host, int port, std::string_view token, std::string_view org, const BucketPoint& bp) {

            host_ = host;
            port_ = port;
            token_ = token;
            org_ = org;
            bp_ = bp;

            // Look up the domain name
            resolver_.async_resolve(
                    host,
                    std::to_string(port),
                    beast::bind_front_handler(
                            &HttpAsyncClient::OnResolve,
                            shared_from_this()));
        }

        void OnResolve(beast::error_code ec, tcp::resolver::results_type results) {
            if(ec)
                return fail_(ec, "resolve");

            stream_.expires_never();
            // Make the connection on the IP address we get from a lookup
            stream_.async_connect(
                    results,beast::bind_front_handler(&HttpAsyncClient::OnConnect, shared_from_this()));
        }

        void OnConnect(beast::error_code ec, tcp::resolver::results_type::endpoint_type) {
            if(ec)
                return fail_(ec, "connect");
            connected_ = true;

            SetupRequest(); //setup request parameters
            SetupTarget(bp_.bucket); //setup target url

            //write the points by async write
            std::string records;
            std::ranges::for_each(bp_.points, [&](const auto &point) {
                std::string r = ptlp_.Convert(&point); //convert from Point to line protocol
                records += r;
                records += "\n";
            });

            SendRequest(records); //send the line protocol to influxdb
        }

        void OnWrite( beast::error_code ec, std::size_t bytes_transferred) {
            boost::ignore_unused(bytes_transferred);

            if(ec)
                return fail_(ec, "write");

            // Receive the HTTP response
            http::async_read(stream_, buffer_, res_,
                             beast::bind_front_handler(&HttpAsyncClient::OnRead, shared_from_this()));
        }

        void OnRead(beast::error_code ec, std::size_t bytes_transferred) {
            boost::ignore_unused(bytes_transferred);

            if(ec)
                return fail_(ec, "read");

            if(!res_.body().empty()){
                responseBody_ = res_.body();
                responseCode_ = res_.result_int();
            }
        }

        void SetupRequest() {
            if (IsConnected()) {
                // Set the request method, target, and version
                req_.method(http::verb::post);
                req_.version(11); // HTTP/1.1

                // Set the request headers
                req_.set(http::field::host, host_);
                req_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
                req_.set(http::field::content_type, "text/plain; charset=utf-8");
                req_.set(http::field::authorization, "Token " + token_);
                req_.set(http::field::accept, "application/json");
            }
        }

        void SetupTarget(std::string_view bucket) {
            std::string target = "/api/v2/write?precision=ns";
            target += "&";
            target += "org=" + std::string(org_);
            target += "&";
            target += "bucket=" + std::string(bucket);
            req_.target(target);
        }

        void SendRequest(std::string_view req) {
            try {
                // Set the request body
                req_.body() = std::string(req);
                req_.prepare_payload();

                http::async_write(stream_, req_,
                                  beast::bind_front_handler(&HttpAsyncClient::OnWrite, shared_from_this()));

            } catch (std::exception &e) {
                responseCode_ = -1;
                responseBody_ = e.what();
            }

        }

        [[nodiscard]] bool IsConnected() const noexcept {return connected_;}
        [[nodiscard]] auto GetResponseCode() const noexcept { return responseCode_; }
        [[nodiscard]] auto GetResponseBody() const noexcept { return responseBody_; }

    private:
        // Report a failure
        void fail_(beast::error_code ec, char const* what){
            responseCode_ = -1;
            responseBody_ = std::string(what) + " "  + ec.message();
            std::cerr << what << ": " << ec.message() << "\n";
        }

    private:
        PointToLineProtocol ptlp_; //Point to Line protocol
        BucketPoint bp_; //bucket point
        tcp::resolver resolver_; //resolver object
        beast::tcp_stream stream_; //tcp stream
        beast::flat_buffer buffer_;  //buffer between calls
        http::request<http::string_body> req_; //request
        http::response<http::string_body> res_; //response

        std::string host_; //influxdb host
        int port_; //influxdb port
        std::string token_; //token to influxdb
        std::string org_; //organisation id
        bool connected_{false}; //flag for connected
        unsigned int responseCode_{0}; //response code
        std::string responseBody_; //response body

    };
}


#endif
