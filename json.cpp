#include "json.h"

#include <variant>
#include <sstream>

using namespace std;

namespace json
{
    Node LoadNode(istream& reader);

    Node LoadNull(istream& reader)
    {
        try
        {
            std::string str(4, ' ');
            reader.read(str.data(), 4);

            if (str == "null")
                return Node(nullptr);
            else
                throw ParsingError("Failed to read null from stream"s);
        }
        catch (const std::exception&)
        {
            throw ParsingError("Failed to read null from stream"s);
        }


        string line(' ', 4);
        reader.read(line.data(), 4);
        return Node(nullptr);
    }

    Node LoadArray(istream& reader)
    {
        Array result;
        bool isCorrectArr = false;

        for (char c; reader >> c;)
        {
            if (c == ']')
            {
                isCorrectArr = true;
                break;
            }
            if (c != ',')
                reader.putback(c);
            result.push_back(LoadNode(reader));
        }

        if (isCorrectArr)
            return Node(move(result));
        else
            throw ParsingError("Failed to read array from stream");
    }

    Node LoadNumber(istream& reader)
    {
        string parsed_num;

        auto read_char = [&parsed_num, &reader]
        {
            parsed_num += static_cast<char>(reader.get());
            if (!reader)
                throw ParsingError("Failed to read number from stream"s);
        };

        auto read_digits = [&reader, read_char]
        {
            if (!isdigit(reader.peek()))
                throw ParsingError("A digit is expected"s);
            while (isdigit(reader.peek()))
                read_char();
        };

        if (reader.peek() == '-')
            read_char();

        if (reader.peek() == '0')
            read_char();
        else
            read_digits();

        bool is_int = true;
        if (reader.peek() == '.')
        {
            read_char();
            read_digits();
            is_int = false;
        }

        if (int ch = reader.peek(); ch == 'e' || ch == 'E')
        {
            read_char();
            if (ch = reader.peek(); ch == '+' || ch == '-')
                read_char();
            read_digits();
            is_int = false;
        }

        try
        {
            if (is_int)
            {
                try
                {
                    return stoi(parsed_num);
                }
                catch (...) {}
            }
            return stod(parsed_num);
        }
        catch (...)
        {
            throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
        }
    }

    Node LoadString(istream& reader)
    {
        string result;
        char c;
        bool isCorrectStr = false;

        while (reader.get(c))
        {
            if (c == '"' && result.back() != '\\')
            {
                isCorrectStr = true;
                break;
            }

            else if (c == '\\')
            {
                reader >> c;
                switch (c)
                {
                case '"': { result += '"'; break; }
                case '\\': { result += '\\'; break; }
                case 't': { result += '\t'; break; }
                case 'n': { result += '\n'; break; }
                case 'r': { result += '\r'; break; }
                default:
                    reader.putback(c);
                    result += c;
                }
            }
            else
            {
                result += c;
            }
        }

        if (isCorrectStr)
            return Node(move(result));
        else
            throw ParsingError("Failed to read string from stream");


        return Node(move(result));
    }

    Node LoadDict(istream& reader)
    {
        Dict result;
        bool isCorrectArr = false;
        for (char c; reader >> c;)
        {

            if (c == '}')
            {
                isCorrectArr = true;
                break;
            }
            if (c == ',')
                reader >> c;

            std::string key = LoadString(reader).AsString();
            reader >> c;
            result.insert({ std::move(key), LoadNode(reader) });
        }

        if (isCorrectArr)
            return Node(std::move(result));
        else
            throw ParsingError("Failed to read array from stream");
    }

    Node LoadBool(istream& reader)
    {
        try
        {
            std::string str(4, ' ');
            reader.read(str.data(), 4);

            if (str == "true")
                return Node(true);

            str += reader.get();

            if (str == "false")
                return Node(false);
            else
                throw ParsingError("Failed to read boolean from stream"s);
        }
        catch (const std::exception&)
        {
            throw ParsingError("Failed to read boolean from stream"s);
        }
    }

    Node LoadNode(istream& reader)
    {
        char c;

        if (!(reader >> c))
            throw ParsingError("Empty data"s);

        switch (c)
        {
        case '[':
            return LoadArray(reader);
        case '{':
            return LoadDict(reader);
        case '"':
            return LoadString(reader);
        case 't':
        case 'f':
            reader.putback(c);
            return LoadBool(reader);
        case 'n':
            reader.putback(c);
            return LoadNull(reader);
        default:
            reader.putback(c);
            return LoadNumber(reader);
        }
    }



    //---------------  Node  ---------------//

    Node::Node(std::nullptr_t value)
        : node_value_(value)
    {
    }

    Node::Node(Array value)
        : node_value_(move(value))
    {
    }

    Node::Node(Dict value)
        : node_value_(move(value))
    {
    }

    Node::Node(bool value)
        : node_value_(value)
    {
    }

    Node::Node(int value)
        : node_value_(value)
    {
    }

    Node::Node(double value)
        : node_value_(value)
    {
    }

    Node::Node(string value)
        : node_value_(move(value))
    {
    }



    const Array& Node::AsArray() const
    {
        if (IsArray())
            return std::get<Array>(node_value_);
        else
            throw std::logic_error("No requested value"s);
    }

    const Dict& Node::AsDict() const
    {
        if (IsMap())
            return std::get<Dict>(node_value_);
        else
            throw std::logic_error("No requested value"s);
    }

    bool Node::AsBool() const
    {
        if (IsBool())
            return std::get<bool>(node_value_);
        else
            throw std::logic_error("No requested value"s);
    }

    int Node::AsInt() const
    {
        if (IsInt())
            return std::get<int>(node_value_);
        else if (IsDouble() && !IsPureDouble())
            return static_cast<int>(std::get<double>(node_value_));
        else
            throw std::logic_error("No requested value"s);
    }

    double Node::AsDouble() const
    {
        if (std::holds_alternative<double>(node_value_))
            return std::get<double>(node_value_);
        else if (std::holds_alternative<int>(node_value_))
            return static_cast<double>(std::get<int>(node_value_));
        else
            throw std::logic_error("No requested value"s);
    }

    const std::string& Node::AsString() const
    {
        if (IsString())
            return std::get<std::string>(node_value_);
        else
            throw std::logic_error("No requested value"s);
    }



    bool Node::IsNull() const
    {
        return std::holds_alternative<std::nullptr_t>(node_value_);
    }

    bool Node::IsArray() const
    {
        return std::holds_alternative<Array>(node_value_);
    }

    bool Node::IsMap() const
    {
        return std::holds_alternative<Dict>(node_value_);
    }

    bool Node::IsBool() const
    {
        return std::holds_alternative<bool>(node_value_);
    }

    bool Node::IsInt() const
    {
        return std::holds_alternative<int>(node_value_);
    }

    bool Node::IsDouble() const
    {
        return std::holds_alternative<double>(node_value_) ||
            std::holds_alternative<int>(node_value_);
    }

    bool Node::IsPureDouble() const
    {
        return std::holds_alternative<double>(node_value_);
    }

    bool Node::IsString() const
    {
        return std::holds_alternative<std::string>(node_value_);
    }



    //---------------  Document  ---------------//

    Document::Document(Node root)
        : root_(move(root))
    {
    }

    const Node& Document::GetRoot() const
    {
        return root_;
    }

    Document Load(istream& reader)
    {
        return Document{ LoadNode(reader) };
    }



    //---------------  Print Func  ---------------//

    std::ostream& operator<< (std::ostream& out, const Node& node)
    {
        out << std::visit(NodeValue{}, node.node_value_);
        return out;
    }

    std::ostream& operator<< (std::ostream& out, const Array& node_array)
    {
        out << "[ ";
        bool b = true;
        for (const Node& node : node_array)
        {
            if (b)
            {
                out << node;;
                b = false;
            }
            else
                out << ", " << node;
        }
        out << "]";
        return out;
    }

    std::ostream& operator<< (std::ostream& out, const Dict& node_dict)
    {
        out << "{ ";
        bool b = true;
        for (const auto& [key, value] : node_dict)
        {
            if (b)
            {
                out << "\"" << key << "\":" << value;
                b = false;
            }
            else
                out << ", \"" << key << "\":" << value;
        }
        out << "}";
        return out;
    }


    std::string NodeValue::operator()(std::nullptr_t) const
    {
        return "null";
    }

    void Print(const Document& doc, std::ostream& output)
    {
        output << doc.GetRoot();
    }

    std::string NodeValue::operator()(const Array& value) const
    {
        std::stringstream str("");
        str << value;
        return str.str();
    }

    std::string NodeValue::operator()(const Dict& value) const
    {
        std::stringstream str("");
        str << value;
        return str.str();
    }

    std::string NodeValue::operator()(bool value) const
    {
        if (value)
            return "true";
        else
            return "false";
    }

    std::string NodeValue::operator()(int value) const
    {
        std::stringstream str("");
        str << value;
        return str.str();
    }

    std::string NodeValue::operator()(double value) const
    {
        std::stringstream str("");
        str << value;
        return str.str();
    }

    std::string NodeValue::operator()(const std::string& value) const
    {
        std::stringstream out("");

        out << "\"";
        for (const char ch : value)
        {
            switch (ch)
            {
            case '\\':
                out << '\\' << '\\';
                break;
            case '"':
                out << '\\' << '"';
                break;
            case '\t':
                out << '\\' << 't';
                break;
            case '\n':
                out << '\\' << 'n';
                break;
            case '\r':
                out << '\\' << 'r';
                break;
            default:
                out << static_cast<char>(ch);
            }
        }
        out << "\"";

        return out.str();
    }

}  // namespace json