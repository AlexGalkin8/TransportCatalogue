#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>

namespace json
{
    class Node;
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;

    using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;
    const std::string CORRECT_NUM_SYMVOL("-1234567890");

    // Эта ошибка должна выбрасываться при ошибках парсинга JSON
    class ParsingError : public std::runtime_error
    {
    public:
        using runtime_error::runtime_error;
    };

    class Node
    {
    public:
        Node() = default;
        Node(std::nullptr_t value);
        Node(Array value);
        Node(Dict value);
        Node(bool value);
        Node(int value);
        Node(double value);
        Node(std::string value);


        const Array& AsArray() const;
        const Dict& AsDict() const;
        bool AsBool() const;
        int AsInt() const;
        double AsDouble() const;
        const std::string& AsString() const;


        bool IsNull() const;
        bool IsArray() const;
        bool IsMap() const;
        bool IsBool() const;
        bool IsInt() const;
        bool IsDouble() const;
        bool IsPureDouble() const;
        bool IsString() const;

        friend std::ostream& operator<< (std::ostream& out, const Node& node);
        friend inline bool operator==(const Node& lhs, const Node& rhs);
        friend inline bool operator!=(const Node& lhs, const Node& rhs);
    private:
        Value node_value_ = nullptr;
    };


    inline bool operator==(const Node& lhs, const Node& rhs)
    {
        return lhs.node_value_ == rhs.node_value_;
    }

    inline bool operator!=(const Node& lhs, const Node& rhs)
    {
        return lhs.node_value_ != rhs.node_value_;
    }




    class Document
    {
    public:
        explicit Document(Node root);
        const Node& GetRoot() const;

        friend inline bool operator==(const Document& lhs, const Document& rhs);
        friend inline bool operator!=(const Document& lhs, const Document& rhs);

    private:
        Node root_;
    };

    inline bool operator==(const Document& lhs, const Document& rhs)
    {
        return lhs.root_ == rhs.root_;
    }

    inline bool operator!=(const Document& lhs, const Document& rhs)
    {
        return lhs.root_ != rhs.root_;
    }


    Document Load(std::istream& reader);

    void Print(const Document& doc, std::ostream& output);



    struct NodeValue
    {
        std::string operator()(const std::nullptr_t) const;
        std::string operator()(const Array& value) const;
        std::string operator()(const Dict& value) const;
        std::string operator()(bool value) const;
        std::string operator()(int value) const;
        std::string operator()(double value) const;
        std::string operator()(const std::string& value) const;
    };


    std::ostream& operator<< (std::ostream& out, const Node& node);
    std::ostream& operator<< (std::ostream& out, const Array& node_array);
    std::ostream& operator<< (std::ostream& out, const Dict& node_dict);

}  // namespace json