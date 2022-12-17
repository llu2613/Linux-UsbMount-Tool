#include "IniParse.h"
#include <cstring>
using namespace std;

IniParse::IniParse(const std::string& path):fileName(path+COMMAND_FILENAME)
{
    file.open(fileName, ios::app|ios::out|ios::in);
    if(file.is_open())
    {
        if(getFileSize(fileName) > 0)
        {
            parseFile();
        }
        else
        {
            file.write(EXAMPLE_COMMAND,strlen(EXAMPLE_COMMAND));
        }
        if(file.is_open())
        {
            file.close();
        }
    }
}

IniParse::~IniParse()
{
    if(file.is_open())
    {
        file.close();
    }
}
bool IniParse::parseFile()
{
    if(!file.is_open())
    {
        return false;
    }
    while(!file.eof())
    {
        size_t pos = 0;
        string line(1024,'\0');
        file.getline(&*line.begin(), line.size());
        pos = line.find_first_not_of(' ');
        if(line[pos] == '#' || line[pos] == '\r' || line[pos] == '\0')
        {
            continue;
        }
        line.erase(0,pos);
        line.erase(line.find_first_of('\0'));
        pos = line.find_first_of('#');
        if(pos != string::npos)
        {
            line.erase(pos);
        }
        pos = line.find_last_not_of(' ');
        if(line[pos] == '\r')
        {
            line.erase(pos);
        }
        else
        {
            line.erase(pos + 1);
        }
        if(line.compare(SECTION_COMMAND) == 0)
        {
            continue;
        }
        command.push_back(std::move(line));
    }
    return true;
}