#include <string>
#include <vector>
#include <sstream>
#include <cstdint>

class RESPSerializer
{
public:
    static std::string serializeSimpleString(const std::string &value)
    {
        return "+" + value + "\r\n";
    }

    static std::string serializeError(const std::string &error)
    {
        return "-" + error + "\r\n";
    }

    static std::string serializeInteger(int64_t value)
    {
        return ":" + std::to_string(value) + "\r\n";
    }

    static std::string serializeBulkString(const std::string &value)
    {
        std::ostringstream out;
        out << "$" << value.size() << "\r\n"
            << value << "\r\n";
        return out.str();
    }

    static std::string serializeNullBulkString()
    {
        return "$-1\r\n";
    }

    static std::string serializeArray(const std::vector<std::string> &elements)
    {
        std::ostringstream out;
        out << "*" << elements.size() << "\r\n";
        for (const auto &elem : elements)
        {
            out << serializeBulkString(elem);
        }
        return out.str();
    }

    static std::string serializeNullArray()
    {
        return "*-1\r\n";
    }
};
