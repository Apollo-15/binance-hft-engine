#pragma once

#include <string_view>

enum class Interval
{
	Sec1,
	Min1,
	Min3,
	Min5,
	Min15,
	Min30,
	Hr1,
	Hr2,
	Hr4,
	Hr6,
	Hr8,
	Hr12,
	Day1,
	Day3,
	Wk1,
	Mon1,
	FinalBorder
};

inline std::string_view IntervalToString(const Interval interval)
{
	switch (interval)
	{
		case Interval::Sec1:
			return "1s";

		case Interval::Min1:
			return "1m";
		case Interval::Min3:
			return "3m";
		case Interval::Min5:
			return "5m";
		case Interval::Min15:
			return "15m";
		case Interval::Min30:
			return "30m";

		case Interval::Hr1:
			return "1h";
		case Interval::Hr2:
			return "2h";
		case Interval::Hr4:
			return "4h";
		case Interval::Hr6:
			return "6h";
		case Interval::Hr8:
			return "8h";
		case Interval::Hr12:
			return "12h";

		case Interval::Day1:
			return "1d";
		case Interval::Day3:
			return "3d";

		case Interval::Wk1:
			return "1w";

		case Interval::Mon1:
			return "1M";

		case Interval::FinalBorder:
			return "";
	}
	__assume(false);
}

inline std::string_view IntervalToDisplayString(const Interval interval)
{
	switch (interval)
	{
	case Interval::Sec1:
		return "1 Sec";

	case Interval::Min1:
		return "1 Min";
	case Interval::Min3:
		return "3 Mins";
	case Interval::Min5:
		return "5 Mins";
	case Interval::Min15:
		return "15 Mins";
	case Interval::Min30:
		return "30 Mins";

	case Interval::Hr1:
		return "1 Hour";
	case Interval::Hr2:
		return "2 Hours";
	case Interval::Hr4:
		return "4 Hours";
	case Interval::Hr6:
		return "6 Hours";
	case Interval::Hr8:
		return "8 Hours";
	case Interval::Hr12:
		return "12 Hours";

	case Interval::Day1:
		return "1 Day";
	case Interval::Day3:
		return "3 Days";

	case Interval::Wk1:
		return "1 Week";

	case Interval::Mon1:
		return "1 Month";

	case Interval::FinalBorder:
		return "";
	}
	__assume(false);
}

inline Interval IntervalFromString(const std::string_view stringView)
{
	if (stringView == "1s")
	{
		return Interval::Sec1;
	}
	else if (stringView == "1m")
	{
		return Interval::Min1;
	}
	else if (stringView == "3m")
	{
		return Interval::Min3;
	}
	else if (stringView == "5m")
	{
		return Interval::Min5;
	}
	else if (stringView == "15m")
	{
		return Interval::Min15;
	}
	else if (stringView == "30m")
	{
		return Interval::Min30;
	}
	else if (stringView == "1h")
	{
		return Interval::Hr1;
	}
	else if (stringView == "2h")
	{
		return Interval::Hr2;
	}
	else if (stringView == "4h")
	{
		return Interval::Hr4;
	}
	else if (stringView == "6h")
	{
		return Interval::Hr6;
	}
	else if (stringView == "8h")
	{
		return Interval::Hr8;
	}
	else if (stringView == "12h")
	{
		return Interval::Hr12;
	}
	else if (stringView == "1d")
	{
		return Interval::Day1;
	}
	else if (stringView == "3d")
	{
		return Interval::Day3;
	}
	else if (stringView == "1w")
	{
		return Interval::Wk1;
	}
	else if (stringView == "1M")
	{
		return Interval::Mon1;
	}

	__assume(false);
}