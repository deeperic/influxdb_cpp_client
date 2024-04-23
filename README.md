# InfluxDB CPP Client

## Introduction

**influxdb_cpp_client** is a header-only C++ library for InfluxDB.
It focuses on the functionality and performance of writing data, writing lots of data. 

## Getting Started

Writing one point:

```
#include "influxdb_cpp_client/InfluxDBClient.h"

//create the client
auto client = std::make_unique<InfluxDBClient>("localhost", 8086, "token", "org");

//add the Point
Point pt("measurement_name");
pt.AddTag("location", "Central");
pt.AddField("temperature", 29.4567);

//write to InfluxDB
client->Write("bucket_name", pt);

```


Writing multiple points:

```
#include "influxdb_cpp_client/InfluxDBClient.h"

//create the client
auto client = std::make_unique<InfluxDBClient>("localhost", 8086, "token", "org");

//add the Points
std::vector<Point> points;

Point pt1("measurement_name");
pt1.AddTag("location", "Central");
pt1.AddField("temperature", 29.4567);
points.emplace_back(pt1);

Point pt2("measurement_name");
pt2.AddTag("location", "WanChai");
pt2.AddField("temperature", 30.1);
points.emplace_back(pt2);

//write to InfluxDB
client->Write("bucket_name", points);

```

## Building the example

```
git clone git@github.com:deeperic/influxdb_cpp_client.git
cd influxdb_cpp_client
mkdir build
cd build
cmake ..
make
```

## Installation

influxdb_cpp_client is a header-only C++ library. Just copy the /influxdb_cpp_client folder to system or project's include path.

## Issues

Issues feedback is welcome. Please feedback any issues encountered. 

