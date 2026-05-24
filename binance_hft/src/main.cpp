#include <iostream>
#include "network/websocket_client.hpp"

int main()
{
    Net::io_context ioContext;
    Net::ssl::context sslContext{boost::asio::ssl::context::tlsv12_client };

    auto client = std::make_shared<Binance::WebSocketClient>(
        "stream.binance.com",
        "9443",
        ioContext,
        sslContext
        );

    client->Connect();

    ioContext.run();

    return 0;
}
