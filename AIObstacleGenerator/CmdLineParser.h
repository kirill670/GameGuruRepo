#pragma once
#include <string>
#include <vector>
#include <map>

class CCmdLineParser {
public:
    CCmdLineParser(int argc, char* argv[]);
    std::string GetArgument(const std::string& option) const;
    bool HasArgument(const std::string& option) const;

private:
    std::map<std::string, std::string> m_args;
};
