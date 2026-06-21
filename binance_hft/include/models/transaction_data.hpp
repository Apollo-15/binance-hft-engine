#pragma once

#include <string>

struct TransactionData
{
	std::string Symbol;
	std::string Side;
	std::string Type;
	double Quantity;
};
