#pragma once

#include <spdlog/logger.h>
#include <string>
#include <memory>

class LoggerManager
{
private:
	std::shared_ptr<spdlog::logger> TradeLogger;
	std::shared_ptr<spdlog::logger> TechnicalLogger;

public:
	explicit LoggerManager(const std::string& tradeFilePath, bool truncateTradeFile, const std::string& technicalFilePath, bool truncateTechnicalFile);

	[[nodiscard]] std::shared_ptr<spdlog::logger> GetTradeLogger() const { return TradeLogger; }
	[[nodiscard]] std::shared_ptr<spdlog::logger> GetTechnicalLogger() const { return TechnicalLogger; }
};
