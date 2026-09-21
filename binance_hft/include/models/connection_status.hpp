#pragma once

#include <cstdint>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>

enum class ConnectionStatus
	: std::uint8_t
{
	Disconnected,
	Connecting,
	Connected,
	Closed,
	Error,
	Reconnecting
};

inline std::string ConnectionStatusToDisplayString(const ConnectionStatus connectionStatus, const std::string& errorMessage)
{
	switch (connectionStatus)
	{
		case ConnectionStatus::Disconnected:
			return "Disconnected.";
		case ConnectionStatus::Connecting:
			return "Connecting in progress...";
		case ConnectionStatus::Connected:
			return "Connected.";
		case ConnectionStatus::Closed:
			if (errorMessage.empty())
			{
				return "Connection closed.";
			}

			return "Connection has been closed with one or more errors: " + errorMessage;
		case ConnectionStatus::Error:
			return "An error appeared: " + errorMessage;
		case ConnectionStatus::Reconnecting:
			return "Reconnecting in progress...";
	}

	__assume(false);
}

struct ConnectionData
{
	std::string ClientName;
	ConnectionStatus ClientStatus;
	std::string LastErrorMessage;
};

inline ftxui::Color ConnectionStatusToColor(const ConnectionStatus connectionStatus)
{
	switch (connectionStatus)
	{
	case ConnectionStatus::Disconnected:
			return ftxui::Color::Red;
	case ConnectionStatus::Connecting:
			return ftxui::Color::Yellow;
		case ConnectionStatus::Connected:
			return ftxui::Color::Green;
		case ConnectionStatus::Closed:
			return ftxui::Color::Red;
		case ConnectionStatus::Error:
			return ftxui::Color::Red;
		case ConnectionStatus::Reconnecting:
			return ftxui::Color::Orange1;
	}

	__assume(false);
}