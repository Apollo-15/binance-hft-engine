#include "network/websocket_client.hpp"
#include "portfolio/portfolio.hpp"
#include <strategy/trading_strategy.hpp>

int main()
{
    Net::io_context ioContext;
    Net::ssl::context sslContext{ Net::ssl::context::tlsv12_client };
    TradeQueue<TradeEvent> queue;
	std::atomic<bool> bIsRunning = true;

    auto client = std::make_shared<Binance::WebSocketClient>(
        "stream.binance.com",
        "9443",
        ioContext,
        sslContext,
        queue
        );

    client->Connect();


    std::thread ioThread([](boost::asio::io_context& ioContext)
    {
		ioContext.run();
	    
    }, std::ref(ioContext));

    std::thread portfolioThread([](TradeQueue<TradeEvent>& queue, std::atomic<bool>& bIsRunning)
    {
        TradeEvent event;
        auto& portfolio = Portfolio::Instance(50000.0);
        TradingStrategy strategy;

	    while (bIsRunning)
	    {
		    if (queue.Pop(event))
		    {
                const auto newBalance = portfolio.GetBalance() - event.Price * event.Quantity * 1.001;

                if (newBalance > 0)
                {
	                portfolio.UpdateBalance(newBalance);

	                const auto newPosition = portfolio.GetPosition() + event.Quantity;
	                portfolio.UpdatePosition(newPosition);

                    strategy.ProcessTrades(event);

	                if (strategy.ShouldSell(event.Price))
	                {
                        const auto finalBalance = portfolio.GetBalance() + event.Price * event.Quantity * (1 - 0.001);
                        portfolio.UpdateBalance(finalBalance);

                        const auto finalPosition = portfolio.GetPosition() - event.Quantity;
                        portfolio.UpdatePosition(finalPosition);
	                }
                }
		    }
            else
            {
                std::this_thread::yield();
            }
	    }
    }, std::ref(queue), std::ref(bIsRunning));

    ioThread.join();
    portfolioThread.join();

    return 0;
}
