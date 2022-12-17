#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <sys/stat.h>
#include <vector>
#define SECTION_COMMAND "[COMMAND]"
#define EXAMPLE_COMMAND SECTION_COMMAND"\r\nifconfig\r\n"

class IniParse
{
public:
    IniParse(const std::string& fileName);
    IniParse(const IniParse&) = delete;
    IniParse(IniParse&&) = delete;
    IniParse& operator =(const IniParse&) = delete;
    IniParse& operator =(IniParse&&) = delete;
    ~IniParse();
    bool fileExits(const std::string& file)
    {
        struct stat mstat;
        return (stat(file.c_str(), &mstat) == 0);
    }
    int64_t getFileSize(const std::string& file)
    {
        struct stat mstat;
        stat(file.c_str(), &mstat);
        return mstat.st_size;
    }
    const std::vector<std::string>& getCommand()
    {
        return command;
    }
private:
    bool parseFile();
private:
    std::string fileName;
    std::fstream file;
    std::vector<std::string> command;
};