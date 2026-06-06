#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include "position.hpp"

struct AccountInfo
{
	uint64_t AccountId;
	std::string AccountName;
	std::vector<Position> Positions;
};
