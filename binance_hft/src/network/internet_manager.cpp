#include "network/internet_manager.hpp"
#include "network/candlestick_websocket_client.hpp"

#ifdef _WIN32
#include <IPExport.h>
#include <windows.h>
#include <icmpapi.h>
#else
#include <cstdlib>
#endif

#include <chrono>
#include <ftxui/component/screen_interactive.hpp>

InternetManager::~InternetManager()
{
	BIsRunning = false;
	if (InternetManagerThread.joinable())
	{
		InternetManagerThread.join();
	}
}

bool InternetManager::SendPing(const std::string& ipAddress)
{
#ifdef _WIN32
	IPAddr destinationAddress;
	const HANDLE icmpHandle = IcmpCreateFile();
	const int conversionResult = inet_pton(AF_INET, ipAddress.c_str(), &destinationAddress);

	char requestData[] = " ";
	char replyBuffer[sizeof(ICMP_ECHO_REPLY) + 8];

	if (icmpHandle == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	if (conversionResult != 1)
	{
		IcmpCloseHandle(icmpHandle);
		return false;
	}

	const DWORD pingResult = IcmpSendEcho(
		icmpHandle,
		destinationAddress,
		requestData,
		sizeof(requestData),
		NULL,
		replyBuffer,
		sizeof(replyBuffer),
		1000
	);

	IcmpCloseHandle(icmpHandle);
	return pingResult > 0;
#else
	const std::string  command = "ping -c 1 -w 1 " + ipAddress + " > /dev/null 2>&1";
	return std::system(command.c_str()) == 0;
#endif
}

bool InternetManager::CheckConnection()
{
	return SendPing("8.8.8.8") || SendPing("1.1.1.1");
}

void InternetManager::Connect()
{
	BIsRunning = true;

	InternetManagerThread = std::thread([this]()
		{
			while (BIsRunning)
			{
				CurrentStatus = CheckConnection();

				if (ftxui::ScreenInteractive::Active() != nullptr)
				{
					ftxui::ScreenInteractive::Active()->RequestAnimationFrame();
				}

				std::this_thread::sleep_for(std::chrono::seconds(2));
			}
		}
	);
}

bool InternetManager::IsConnectedToInternet() const
{
	return CurrentStatus;
}
