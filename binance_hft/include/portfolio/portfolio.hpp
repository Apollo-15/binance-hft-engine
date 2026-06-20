#pragma once

#include <mutex>

class Portfolio
{
private:
	mutable std::mutex Mutex;
	double Balance = 50000.0;
	double PnL = 0.0;
	double Position = 0.0;

	explicit Portfolio(const double balance)
		: Balance(balance)
	{
	}
public:

	~Portfolio() = default;

	static Portfolio& Instance(const double balance)
	{
		static Portfolio single(balance);
		return single;
	}

	Portfolio(const Portfolio&) = delete;
	Portfolio(Portfolio&&) = delete;
	Portfolio& operator=(const Portfolio&) = delete;
	Portfolio& operator=(Portfolio&&) = delete;

	[[nodiscard]]
	double GetBalance() const
	{
		std::scoped_lock<std::mutex> lock(Mutex);

		return Balance;
	}
	[[nodiscard]]
	double GetPnL() const
	{
		std::scoped_lock<std::mutex> lock(Mutex);

		return PnL;
	}
	[[nodiscard]]
	double GetPosition() const
	{
		std::scoped_lock<std::mutex> lock(Mutex);

		return Position;
	}

	void UpdateBalance(const double balance)
	{
		std::scoped_lock<std::mutex> lock(Mutex);

		Balance = balance;
	}

	void UpdatePnL(const double pnl)
	{
		std::scoped_lock<std::mutex> lock(Mutex);

		PnL = pnl;
	}

	void UpdatePosition(const double position)
	{
		std::scoped_lock<std::mutex> lock(Mutex);

		Position = position;
	}
};