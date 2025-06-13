#include "CmdLineParser.h"

CCmdLineParser::CCmdLineParser(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.rfind('-', 0) == 0) { // Check if it starts with '-'
            if (i + 1 < argc) {
                std::string next_arg = argv[i + 1];
                if (next_arg.rfind('-', 0) != 0) { // Next argument is not an option, so it's a value
                    m_args[arg] = next_arg;
                    i++; // Skip next argument as it's consumed as a value
                } else {
                    m_args[arg] = ""; // Option without a value (boolean flag)
                }
            } else {
                m_args[arg] = ""; // Option without a value (boolean flag) at the end
            }
        }
    }
}

std::string CCmdLineParser::GetArgument(const std::string& option) const {
    auto it = m_args.find(option);
    if (it != m_args.end()) {
        return it->second;
    }
    return "";
}

bool CCmdLineParser::HasArgument(const std::string& option) const {
    return m_args.count(option) > 0;
}
