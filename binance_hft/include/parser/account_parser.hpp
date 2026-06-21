#pragma once

#include <optional>

#include "models/account_info.hpp"

namespace JsonParser
{
	std::optional<AccountInfo> ParseAccount(const std::string& json);
}
