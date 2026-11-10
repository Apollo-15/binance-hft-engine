#pragma once

#include <memory>
#include <optional>
#include <spdlog/logger.h>

#include "models/account_info.hpp"

namespace JsonParser
{
	std::optional<AccountInfo> ParseAccount(const std::string& json, const std::shared_ptr<spdlog::logger>& technicalLogger);
}
