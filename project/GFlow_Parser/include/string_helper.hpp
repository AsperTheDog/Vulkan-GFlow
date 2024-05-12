#pragma once
#include <string>
#include <vector>

namespace gflow::string
{

    inline std::string trim(const std::string& str, const std::string& token = " ")
    {
        const size_t first = str.find_first_not_of(token);
        if (std::string::npos == first)
            return str;
        const size_t last = str.find_last_not_of(token);
        return str.substr(first, last - first + 1);
    }

    inline std::string toLower(const std::string& str)
    {
        std::string result = str;
        for (char& c : result)
            c = std::tolower(c);
        return result;
    }

    inline std::string toUpper(const std::string& str)
    {
        std::string result = str;
        for (char& c : result)
            c = std::toupper(c);
        return result;
    }

    inline std::vector<std::string> split(const std::string& str, const std::string& delimiter)
    {
        std::vector<std::string> tokens;
        size_t pos;
        size_t prev = 0;
        while ((pos = str.find(delimiter, prev)) != std::string::npos)
        {
            if (pos - prev != 0)
            {
                tokens.push_back(str.substr(prev, pos - prev));
                prev = pos + delimiter.size();
            }
        }
        if (prev != str.size()) tokens.push_back(str.substr(prev));
        return tokens;
    }

    inline std::vector<std::string> splitAndTrim(const std::string& str, const std::string& delimiter)
    {
        std::vector<std::string> tokens;
        size_t pos;
        size_t prev = 0;
        while ((pos = str.find(delimiter, prev)) != std::string::npos)
        {
            tokens.push_back(trim(str.substr(prev, pos - prev)));
            prev = pos + delimiter.size();
        }
        tokens.push_back(trim(str.substr(prev)));
        return tokens;
    }

    inline std::string join(const std::vector<std::string>& strings, const std::string& delimiter, const size_t begin = 0, size_t end = 0)
    {
        std::string result;
        if (begin >= strings.size())
            return result;
        if (end == 0 || end > strings.size())
            end = strings.size();
        for (size_t i = begin; i < end; ++i)
        {
            result += strings[i];
            if (i < strings.size() - 1)
                result += delimiter;
        }
        return result;
    }

    inline std::pair<std::string, std::string> tokenize(const std::string& str, const std::string& delimiter)
    {
        const size_t pos = str.find_first_of(delimiter);
        if (pos == std::string::npos)
            return { str, "" };
        return { trim(str.substr(0, pos)), trim(str.substr(pos + delimiter.size())) };
    }

    inline std::string reduce(const std::string& str, const uint32_t left = 1, const uint32_t right = 1)
    {
        if (left + right >= str.size())
            return "";
        return str.substr(left, str.size() - left - right);
    }

    inline std::string getPathExtension(const std::string& path)
    {
        const size_t pos = path.find_last_of('.');
        if (pos == std::string::npos)
            return "";
        return path.substr(pos + 1);
    }

    inline std::string getPathFilename(const std::string& path)
    {
        std::string truePath = path;
        if (path.back() == '/') truePath.pop_back();
        const size_t pos = truePath.find_last_of('/');
        if (pos == std::string::npos)
            return truePath;
        return truePath.substr(pos + 1);
    }

    inline std::string getPathDirectory(const std::string& path)
    {
        std::string truePath = path;
        if (path.back() == '/') truePath.pop_back();
        const size_t pos = truePath.find_last_of('/');
        if (pos == std::string::npos)
            return "";
        return truePath.substr(0, pos);
    }

    inline bool contains(const std::string_view variable, const std::string_view str)
    {
        return variable.find(str) != std::string::npos;
    }
}
