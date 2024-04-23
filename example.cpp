#include <iostream>

#include "influxdb_cpp_client/InfluxDBClient.h"

using namespace nc;

std::unique_ptr<InfluxDBClient> CreateInfluxDBClient(){
    std::string host = "localhost";
    int port = 8086;
    std::string token = "your_token";
    std::string org = "your_organisation";
    auto client = std::make_unique<InfluxDBClient>(host, port, token, org);
    return std::move(client);
}

std::vector<Point> CreatePoints(){
    std::vector<Point> v;

    Point pt1("measurement2");
    pt1.AddTag("location", "Central");
    pt1.AddField("temperature", 29.4567);
    pt1.AddField("num_of_buses", 217);
    pt1.AddField("testing", "yes");
    pt1.SetTimeStamp(GetTimeStamp());

    Point pt2("measurement2");
    pt2.AddTag("location", "WanChai");
    pt2.AddField("temperature", 30.1234);
    pt2.AddField("num_of_buses", 81);
    pt2.AddField("testing", "no");
    pt2.SetTimeStamp(GetTimeStamp());

    v.emplace_back(pt1);
    v.emplace_back(pt2);
    return v;
}

int main() {
    std::cout << "InfluxDB CPP Client test" << std::endl;

    auto client = CreateInfluxDBClient();
    auto points = CreatePoints();
    client->Write("bucket1", std::move(points));
    std::this_thread::sleep_for(1s);
    client->Stop();

    return 0;
}
