#include "logger/reader/logger_reader.hpp"

#include <filesystem>
#include <system_error>
#include <thread>
#include <fstream>
#include <iterator>
#include <sstream>
#include <vector>

LoggerReader::~LoggerReader()
{
    BIsRunning = false;

    if (LoggerReaderThread.joinable())
    {
        LoggerReaderThread.join();
    }
}

void LoggerReader::ReadFile()
{
    std::error_code ec;
    const auto fileSize = std::filesystem::file_size(LoggerFilePath, ec);

    if (ec)
    {
        return;
    }

    if (fileSize < LastPosition)
    {
        LastPosition = 0;

        std::scoped_lock lock(Mutex);
        Lines.clear();
    }

    std::ifstream logFileStream (LoggerFilePath, std::ios::binary);

    if (!logFileStream.is_open())
    {
        return;
    }

    logFileStream.seekg(LastPosition, std::ios::beg);

    if (logFileStream.fail())
    {
        return;
    }

    std::istreambuf_iterator<char> begin(logFileStream);
    std::istreambuf_iterator<char> end;

    std::string remainingBytes(begin, end);

    if (remainingBytes.size() == 0)
    {
        return;
    }

    std::size_t lastNewlinePosition = remainingBytes.rfind('\n');
    if (lastNewlinePosition == std::string::npos)
    {
        return;
    }

    const std::string completeBytes = remainingBytes.substr(0, lastNewlinePosition + 1);

    std::istringstream lineStream(completeBytes);
    std::string currentLine;

    std::vector<std::string> lines;

    while (std::getline(lineStream, currentLine))
    {
        if (currentLine.size() != 0 && currentLine.back() == '\r')
        {
            currentLine.pop_back();
        }

        lines.emplace_back(currentLine);
    }

    std::scoped_lock lock(Mutex);

    Lines.insert(Lines.end(), std::make_move_iterator(lines.begin()), std::make_move_iterator(lines.end()));

    while (Lines.size() > LinesLimit)
    {
        Lines.pop_front();
    }

    LastPosition += completeBytes.size();
}

bool LoggerReader::Start()
{
    if (BIsRunning == true)
    {
        return false;
    }
    else
    {
        BIsRunning = true;

        try
        {
            LoggerReaderThread = std::thread([this]()
                {
                    while (BIsRunning)
                    {
                        ReadFile();
                        std::this_thread::sleep_for(std::chrono::seconds(1));
                    }
                }
            );

            return true;
        }
        catch (std::system_error& ec)
        {
            BIsRunning = false;
            return false;
        }
    }
}