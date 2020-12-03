#include <stdio.h>
#include <functional>

#include "Server.h"

int main(int argc, char **argv) {
    Port port(8080);
    int thr = 2;
    
    Address addr(Ipv4::any(), port);
    StatsEndpoint stats(addr);
    stats.init(thr);
    stats.start();

    stats.shutdown();
    return 0;
}
