#include <cstdio>
#include <thread>

#include <pistache/endpoint.h>

#include "DataService.hpp"


int main(int argc, char *argv[])
{
    using namespace Pistache;
    Port port(9080);

    size_t thr = hardware_concurrency();

    if (argc >= 2) {
        port = static_cast<uint16_t>(std::stol(argv[1]));

        if (argc == 3)
            thr = std::stoi(argv[2]);
    }

    Address addr(Ipv4::any(), port);

    http::DataService srv(addr);

    srv.init(thr);
    srv.start();
    return 0;
}