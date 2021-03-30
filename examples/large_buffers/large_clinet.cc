#include "rpc/client.h"
#include "utils/StopWatch.h"

#include <cstdio>
#include <iostream>
#include <algorithm>

int main(int argc, char *argv[]) {
    std::string host = "127.0.0.1";
    uint16_t port = rpc::constants::DEFAULT_PORT;
    std::size_t blob_size = 1ull << 20;
    if (argc > 1) {
        blob_size = (1ull << atoi(argv[1]));
        if (argc > 2) {
            host = std::string(argv[2]);
            if (argc > 3) {
                port = std::atoi(argv[3]);
            }
        }
    }
    rpc::client c(host, port);
    // The upper bound of the available container size is 0xffffffff.
    blob_size = std::min(blob_size, (size_t)0xffffffff);
    printf("---- Blob size: 0x%lx\n", blob_size);
    Stopwatch w;
    c.call("large_return", blob_size);
    printf("++++ Finish Blob size: 0x%lx\n", blob_size);
    printf("Time: [%lums]\n", w.elapsedMilliseconds());
}
