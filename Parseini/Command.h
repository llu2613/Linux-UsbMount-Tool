#pragma once
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#define RESULT_FILENAME "/COMMAND_RESULT.log"
class Command
{
public:
    Command(const std::string& path);
    Command(const Command& cmd) = delete;
    Command(Command&& cmd) = delete;
    Command& operator =(const Command&) = delete;
    Command& operator =(Command&&) = delete;
    ~Command();

    bool runCommand(const std::vector<std::string>& cmd);
private:
    bool runCommand(const std::string& cmd);

private:
    FILE *fp;
    std::string fileName;
    std::string result;
};