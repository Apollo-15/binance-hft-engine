#include "network/latency_manager.hpp"

#include <ftxui/component/screen_interactive.hpp>


LatencyManager::~LatencyManager()
{
	BIsRunning = false;

	if (LatencyManagerThread.joinable())
	{
		LatencyManagerThread.join();
	}
}

void LatencyManager::StartMeasuringLatency()
{
	BIsRunning = true;

	LatencyManagerThread = std::thread([this]()
		{
			while (BIsRunning)
			{
				auto [latency, resultCode] = MeasureLatency();
				Latency = latency;
				ResultCode = resultCode;

				if (resultCode == 429 || resultCode == 418)
				{

					auto candidateInterval = std::chrono::duration_cast<std::chrono::milliseconds>(SleepInterval * 1.5);

					if (candidateInterval > CapInterval)
					{
						SleepInterval = CapInterval;
					}
					else
					{
						SleepInterval = candidateInterval;
					}

				}
				else if (resultCode == 200)
				{
					auto candidateSleepInterval = SleepInterval - std::chrono::milliseconds(25);

					if (candidateSleepInterval > BaseInterval)
					{
						SleepInterval = candidateSleepInterval;
					}
					else
					{
						SleepInterval = BaseInterval;
					}
				}

				if (ftxui::ScreenInteractive::Active() != nullptr)
				{
					ftxui::ScreenInteractive::Active()->RequestAnimationFrame();
				}

				std::this_thread::sleep_for(SleepInterval);
			}
		}
	);
}

std::pair<std::chrono::milliseconds, unsigned> LatencyManager::MeasureLatency()
{
	const auto startTime = std::chrono::steady_clock::now();
	auto serverStatus = RestRequesterRef.CheckServerStatus(RestHost, LatencyIoContext);
	const auto endTime = std::chrono::steady_clock::now();
	auto delay = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

	return { delay, serverStatus };
}
