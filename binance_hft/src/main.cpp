#include "network/websocket_client.hpp"

int main()
{
    Net::io_context ioContext;
    Net::ssl::context sslContext{ Net::ssl::context::tlsv12_client };

    auto client = std::make_shared<Binance::WebSocketClient>(
        "stream.binance.com",
        "9443",
        ioContext,
        sslContext
        );

    client->Connect();


    std::thread ioThread([](boost::asio::io_context& ioContext)
    {
		ioContext.run();
	    
    }, std::ref(ioContext));

    ioThread.join();

    return 0;
}
