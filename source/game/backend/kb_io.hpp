#pragma once

#include <string>
#include <fstream>

std::string readFileToString(const std::string& pPath)
{
    std::ifstream infile{pPath};
    return std::string{std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>()};
}
