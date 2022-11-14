#pragma once
#include <string>
#include <sstream>

namespace RenderD7
{
    class StealConsole
    {
        public:
        StealConsole();
        ~StealConsole();
        std::string GetStdout();
        private:
        std::stringstream stolen_stdout;
    };
}