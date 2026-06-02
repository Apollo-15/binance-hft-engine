#include <csignal>
#include <memory>

#include "config/binance_config.hpp"
#include "network/websocket_client.hpp"
#include "network/reconnect_manager.hpp"
#include "portfolio/portfolio.hpp"
#include "strategy/trading_strategy.hpp"
#include "database/database.hpp"

boost::asio::executor_work_guard<boost::asio::io_context::executor_type>* workGuard;

void SignalHandler(int signal)
{
    if (workGuard != nullptr)
    {
        workGuard->reset();
    }
}

int main()
{
    void(std::signal(SIGINT, SignalHandler));

    Net::io_context ioContext;
    Net::ssl::context sslContext{ Net::ssl::context::tlsv12_client };
    TradeQueue<TradeEvent> queue;
    TradeQueue<TradeEvent> dbTradeQueue;
    TradeQueue<TradeBatch> dbBatchQueue;
	std::atomic<bool> bIsRunning = true;
    DataBase db;
    Binance::BinanceConfig binanceConfig{
        "stream.binance.com",
        "9443",
        std::chrono::seconds(16),
        std::chrono::seconds(1),
        std::chrono::seconds(5)
    };

    auto client = std::make_shared<Binance::WebSocketClient>(
        binanceConfig.Host,
        binanceConfig.Port,
        ioContext,
        sslContext,
        queue
        );

    auto reconnectManager = std::make_shared<Binance::ReconnectManager>(client, ioContext, binanceConfig);

    client->SetManager(reconnectManager);
    client->Connect();

    db.OpenConnection("portfolio.db");
    db.CreatePortfolioTable();
    db.CreateBatchesTable();
    db.CreateTradesTable();

    auto localGuard = Net::make_work_guard(ioContext);

    workGuard = &localGuard;

    std::thread ioThread([](Net::io_context& ioContext)
    {
		ioContext.run();
    }, std::ref(ioContext));


    std::thread portfolioThread([](TradeQueue<TradeEvent>& queue, std::atomic<bool>& bIsRunning, 
        TradeQueue<TradeEvent>& dbTradeQueue, TradeQueue<TradeBatch>& dbBatchQueue)
    {
        TradeEvent event;
        auto& portfolio = Portfolio::Instance(50000.0);
        int64_t batchCounter = 0;
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

                    bool isNewBatch = strategy.ProcessTrades(event);
                    
                    if (isNewBatch)
                    {
                        strategy.GetBack().BatchId = batchCounter++;
                        dbBatchQueue.Push(strategy.GetBack());
                    }

	                if (std::optional<TradeBatch> savedBatch = strategy.ShouldSell(event.Price))
	                {
                        const auto finalBalance = portfolio.GetBalance() + event.Price * event.Quantity * (1 - 0.001);
                        portfolio.UpdateBalance(finalBalance);

                        const auto finalPosition = portfolio.GetPosition() - event.Quantity;
                        portfolio.UpdatePosition(finalPosition);

                        savedBatch->PnL = (finalBalance - newBalance) / newBalance * 100;
                        savedBatch->BatchId = batchCounter++;

                        dbBatchQueue.Push(*savedBatch);
	                }
                    dbTradeQueue.Push(event);
                }
		    }
            else
            {
                std::this_thread::yield();
            }
	    }
    }, std::ref(queue), std::ref(bIsRunning), std::ref(dbTradeQueue), std::ref(dbBatchQueue));

    std::thread dbThread([](std::atomic<bool>& bIsRunning, TradeQueue<TradeEvent>& dbTradeQueue, 
        TradeQueue<TradeBatch>& dbBatchQueue, DataBase& db)
    {
        TradeEvent event;
        TradeBatch batch;

        while (bIsRunning)
        {
	        if (dbTradeQueue.Pop(event))
	        {
                db.SaveTrade(event);
                db.UpdatePortfolio();
	        }

            if (dbBatchQueue.Pop(batch))
            {
                db.SaveBatch(batch);
            }
        }
    }, std::ref(bIsRunning), std::ref(dbTradeQueue), std::ref(dbBatchQueue), std::ref(db));

    ioThread.join();
    bIsRunning = false;
    portfolioThread.join();
    dbThread.join();

    return 0;
}
