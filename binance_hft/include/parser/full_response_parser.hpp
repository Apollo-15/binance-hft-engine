#pragma once

#include <optional>

#include "models/full_response.hpp"

namespace JsonParser
{
	std::optional<FullResponse> ParseFullResponse(const std::string& json);
}
