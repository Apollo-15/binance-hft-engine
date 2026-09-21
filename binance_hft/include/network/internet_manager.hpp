#pragma once

#include <atomic>
#include <string>
#include <thread>

class InternetManager
{
private:
	std::atomic<bool> CurrentStatus { false };
	std::atomic<bool> BIsRunning { false };
	std::thread InternetManagerThread;

	static bool SendPing(const std::string& ipAddress);
	bool CheckConnection();

public:
	InternetManager() = default;
	~InternetManager();

	InternetManager(const InternetManager& other) = delete;
	InternetManager(InternetManager&& other) noexcept = delete;
	InternetManager& operator=(const InternetManager& other) = delete;
	InternetManager& operator=(InternetManager&& other) noexcept = delete;

	bool IsConnectedToInternet() const;
	void Connect();
};
