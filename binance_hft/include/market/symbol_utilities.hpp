#pragma once

#include <cstdint>
#include <string_view>

enum class Symbol : std::uint8_t
{
	Btc,
	Eth,
	Sol,
	Xrp,
	Bnb,
	FinalBorder
};

inline std::string_view SymbolToString(const Symbol symbol)
{
	switch (symbol)
	{
		case Symbol::Btc:
			return "BTCUSDT";
		case Symbol::Eth:
			return "ETHUSDT";
		case Symbol::Sol:
			return "SOLUSDT";
		case Symbol::Xrp:
			return "XRPUSDT";
		case Symbol::Bnb:
			return "BNBUSDT";

		case Symbol::FinalBorder:
			return "";

	}

	__assume(false);
}

inline std::string_view SymbolToDisplayString(const Symbol symbol)
{
	switch (symbol)
	{
		case Symbol::Btc:
			return "Bitcoin";
		case Symbol::Eth:
			return "Ethereum";
		case Symbol::Sol:
			return "Solana";
		case Symbol::Xrp:
			return "XRP";
		case Symbol::Bnb:
			return "BNB";

		case Symbol::FinalBorder:
			return "";
	}

	__assume(false);
}

inline Symbol SymbolFromString(const std::string_view stringView)
{
	if (stringView == "BTCUSDT")
	{
		return Symbol::Btc;
	}
	else if (stringView == "ETHUSDT")
	{
		return Symbol::Eth;
	}
	else if (stringView == "SOLUSDT")
	{
		return Symbol::Sol;
	}
	else if (stringView == "XRPUSDT")
	{
		return Symbol::Xrp;
	}
	else if (stringView == "BNBUSDT")
	{
		return Symbol::Bnb;
	}

	__assume(false);
}