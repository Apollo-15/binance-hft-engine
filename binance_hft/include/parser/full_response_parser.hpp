#pragma once

#include <memory>
#include <optional>
#include <spdlog/logger.h>

#include "models/full_response.hpp"

namespace JsonParser
{
	std::optional<FullResponse> ParseFullResponse(const std::string& json, const std::shared_ptr<spdlog::logger>& technicalLogger);
}
