#include <iostream>
#include <string>
#include <vector>
#include <sstream>

class RESPValue
{
public:
    enum class Type
    {
        SIMPLE_STRING,
        ERROR,
        INTEGER,
        BULK_STRING,
        ARRAY,
        NIL
    };

    Type type;
    std::string str_value;
    int int_value = 0;
    std::vector<RESPValue> array_value;

    void print(int indent = 0) const
    {
        std::string ind(indent, ' ');
        switch (type)
        {
        case Type::SIMPLE_STRING:
            std::cout << ind << "Simple String: " << str_value << "\n";
            break;

        case Type::ERROR:
            std::cout << ind << "Error: " << str_value << "\n";
            break;

        case Type::INTEGER:
            std::cout << ind << "Integer: " << int_value << "\n";
            break;

        case Type::BULK_STRING:
            std::cout << ind << "Bulk String: " << (str_value.empty() ? "(nil)" : str_value) << "\n";
            break;

        case Type::ARRAY:
            std::cout << ind << "Array:\n";
            for (const auto &elem : array_value)
                elem.print(indent + 2);
            break;

        case Type::NIL:
            std::cout << ind << "Nil\n";
            break;

        default:
            break;
        }
    }
};

class RESPParser
{
public:
    RESPValue parse(const std::string &input)
    {
        std::istringstream stream(input);
        return parseValue(stream);
    }

private:
    RESPValue parseValue(std::istream &stream)
    {
        char prefix;
        if (!stream.get(prefix))
        {
            throw std::runtime_error("Unexpected end of input");
        }

        switch (prefix)
        {
        case '+':
            return parseSimpleString(stream);
        case '-':
            return parseError(stream);
        case ':':
            return parseInteger(stream);
        case '$':
            return parseBulkString(stream);
        case '*':
            return parseArray(stream);
        default:
            throw std::runtime_error(std::string("Unknown RESP prefix: ") + prefix);
        }
    }

    RESPValue parseSimpleString(std::istream &stream)
    {
        RESPValue v;
        v.type = RESPValue::Type::SIMPLE_STRING;
        v.str_value = readLine(stream);
        return v;
    }

    RESPValue parseError(std::istream &stream)
    {
        RESPValue v;
        v.type = RESPValue::Type::ERROR;
        v.str_value = readLine(stream);
        return v;
    }

    RESPValue parseInteger(std::istream &stream)
    {
        RESPValue v;
        v.type = RESPValue::Type::INTEGER;
        v.int_value = std::stoll(readLine(stream));
        return v;
    }

    RESPValue parseBulkString(std::istream &stream)
    {
        int64_t len = std::stoll(readLine(stream));
        RESPValue v;
        v.type = (len == -1) ? RESPValue::Type::NIL : RESPValue::Type::BULK_STRING;

        if (len >= 0)
        {
            v.str_value = readFixed(stream, len);
            expectCRLF(stream);
        }
        return v;
    }

    RESPValue parseArray(std::istream &stream)
    {
        int count = std::stoll(readLine(stream));
        if (count == -1)
        {
            return RESPValue{RESPValue::Type::NIL};
        }

        RESPValue v;
        v.type = RESPValue::Type::ARRAY;
        v.array_value.reserve(count);
        for (int i = 0; i < count; ++i)
        {
            v.array_value.push_back(parseValue(stream));
        }
        return v;
    }

    std::string readLine(std::istream &stream)
    {
        std::string line;
        if (!std::getline(stream, line, '\r'))
        {
            throw std::runtime_error("Expected CR in line");
        }
        char lf;
        stream.get(lf);
        if (lf != '\n')
        {
            throw std::runtime_error("Expected LF after CR");
        }
        return line;
    }

    std::string readFixed(std::istream &stream, size_t len)
    {
        std::string s(len, '\0');
        stream.read(&s[0], len);
        if (stream.gcount() != static_cast<std::streamsize>(len))
        {
            throw std::runtime_error("Unexpected end of bulk string");
        }
        return s;
    }

    void expectCRLF(std::istream &stream)
    {
        char cr = stream.get();
        char lf = stream.get();
        if (cr != '\r' || lf != '\n')
        {
            throw std::runtime_error("Expected CRLF");
        }
    }
};