#include "rpc/server.h"

int main(int argc, char *argv[]) {
    std::string host = "20.0.1.121";
    uint16_t port = rpc::constants::DEFAULT_PORT;
    if (argc > 2) {
        host = std::string(argv[1]);
        if (argc > 3) {
            port = atoi(argv[2]);
        }
    }
    rpc::server srv(host, port);

    srv.bind("echo", [](std::string const &s) { return s; });
    auto get_blob = [](std::size_t size) {
        std::string s;
        s.resize(size);
        // for (auto &c : s) {
        //     c = static_cast<unsigned char>(rand() % 256);
        // }
        return s;
    };
    srv.bind("large_return",
             [&get_blob](std::size_t bytes) { return get_blob(bytes); });

    srv.run();
    return 0;
}
