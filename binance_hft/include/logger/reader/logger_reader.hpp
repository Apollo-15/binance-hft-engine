#pragma once

#include <atomic>
#include <cstddef>
#include <deque>
#include <memory>
#include <mutex>
#include <spdlog/logger.h>
#include <string>
#include <thread>

class LoggerReader
{
private:
    std::thread LoggerReaderThread;
    std::string LoggerFilePath;
    std::atomic<bool> BIsRunning { false };
    std::size_t LastPosition { 0 };
    std::size_t LinesLimit;
    mutable std::mutex Mutex;
    std::deque<std::string> Lines;  
    std::shared_ptr<spdlog::logger> TechnicalLogger;

    void ReadFile();

public:
    explicit LoggerReader(const std::string &loggerFilePath, std::size_t linesLimit, std::shared_ptr<spdlog::logger> technicalLogger)
        : LoggerFilePath(loggerFilePath),
          LinesLimit(linesLimit),
          TechnicalLogger(std::move(technicalLogger))
    {
    }

    ~LoggerReader();

    LoggerReader(const LoggerReader &other) = delete;
    LoggerReader(LoggerReader &&other) noexcept = delete;
    LoggerReader & operator=(const LoggerReader &other) = delete;
    LoggerReader & operator=(LoggerReader &&other) noexcept = delete;

    bool Start();

    [[nodiscard]]
    std::deque<std::string> GetLines() const
    {
        std::scoped_lock lock(Mutex);

        return Lines;
    }
};