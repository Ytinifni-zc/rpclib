#include "gtest/gtest.h"

#include "rpc/client.h"
#include "rpc/server.h"
#include "rpc/rpc_error.h"
#include "testutils.h"

#include <sstream>
#include <chrono>
#include <thread>

using namespace rpc::testutils;

class client_test : public testing::Test {
public:
    client_test() : s(host, test_port), is_running_(false) {
        s.bind("dummy_void_zeroarg", [this]() { md.dummy_void_zeroarg(); });
        s.bind("dummy_void_singlearg",
               [this](int x) { md.dummy_void_singlearg(x); });
        s.bind("dummy_void_multiarg",
               [this](int x, int y) { md.dummy_void_multiarg(x, y); });
        s.bind("large_return", [](std::size_t bytes){ get_blob(bytes); });
        s.bind("sleep", [](uint64_t ms) {
                std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        });
        s.async_run();
    }

protected:
    static RPCLIB_CONSTEXPR uint16_t test_port = rpc::constants::DEFAULT_PORT;
    const std::string host{"20.0.1.121"};
    MockDummy md;
    rpc::server s;
    std::atomic_bool is_running_;
};

TEST_F(client_test, instantiation) {
    rpc::client client(host, test_port);
}

TEST_F(client_test, call) {
    EXPECT_CALL(md, dummy_void_zeroarg());
    EXPECT_CALL(md, dummy_void_singlearg(5));
    EXPECT_CALL(md, dummy_void_multiarg(5, 6));
    rpc::client client(host, test_port);
    client.call("dummy_void_zeroarg");
    client.call("dummy_void_singlearg", 5);
    client.call("dummy_void_multiarg", 5, 6);
}

TEST_F(client_test, notification) {
    EXPECT_CALL(md, dummy_void_zeroarg());
    rpc::client client(host, test_port);
    client.send("dummy_void_zeroarg");
    client.wait_all_responses();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

TEST_F(client_test, large_return) {
    rpc::client client(host, test_port);
    std::size_t blob_size = 2 << 10 << 10;
    for (int i = 0; i < 4; ++i) {
        client.call("large_return", blob_size);
        blob_size *= 2;
    }
    // 0: 1MB
    // 1: 2MB
    // 2: 4MB
    // 3: 8MB
    // no crash is enough
}

TEST_F(client_test, timeout_setting_works) {
    rpc::client client(host, test_port);
    EXPECT_FALSE(client.get_timeout());

    const uint64_t short_timeout = 50;
    client.set_timeout(short_timeout);

    EXPECT_EQ(*client.get_timeout(), short_timeout);
    EXPECT_THROW(client.call("sleep", short_timeout + 10), rpc::timeout);

    client.set_timeout(short_timeout * 2);
    EXPECT_EQ(*client.get_timeout(), short_timeout * 2);
    EXPECT_NO_THROW(client.call("sleep", short_timeout + 1));
}

TEST_F(client_test, timeout_right_msg) {
    rpc::client client(host, test_port);
    const uint64_t short_timeout = 50;
    try {
        client.set_timeout(short_timeout);
        client.call("sleep", short_timeout + 10);
        FAIL() << "There was no exception thrown.";
    } catch (rpc::timeout &t) {
        std::stringstream ss;
        ss 
          << "rpc::timeout: Timeout of " 
          << *client.get_timeout()
          << "ms while calling RPC function 'sleep'";
        EXPECT_TRUE(str_match(t.what(), ss.str()));
    }
}

TEST_F(client_test, timeout_clear) {
    rpc::client client(host, test_port);
    EXPECT_FALSE(client.get_timeout());
    client.set_timeout(50);
    EXPECT_EQ(50, *client.get_timeout());
    client.clear_timeout();
    EXPECT_FALSE(client.get_timeout());
}

TEST(client_test2, timeout_while_connection) {
    rpc::client client("localhost", rpc::constants::DEFAULT_PORT);
    client.set_timeout(50);
    // this client never connects, so this tests the timout in wait_conn()
    EXPECT_THROW(client.call("whatev"), rpc::timeout);
}
