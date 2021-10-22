#pragma once

#include <memory>
#include <stack>
#include <string>

#include "json.h"

namespace json
{
    class BuildConstructor;
    class BuildContextFirst;
    class BuildContextSecond;
    class KeyContext;
    class ValueKeyContext;
    class ValueArrayContext;
    class DictContext;
    class ArrayContext;
    class Builder;



    //------------ BuildConstructor ---------------

    class BuildConstructor
    {
    public:
        explicit BuildConstructor(Builder& builder);

    protected:
        Builder& builder_;
    };



    //------------ BuildContextFirst ---------------

    class BuildContextFirst : public BuildConstructor
    {
    public:
        explicit BuildContextFirst(Builder& builder);

        DictContext& StartDict();

        ArrayContext& StartArray();
    };



    //------------ BuildContextSecond ---------------

    class BuildContextSecond : public BuildConstructor
    {
    public:
        explicit BuildContextSecond(Builder& builder);

        KeyContext& Key(std::string key);

        Builder& EndDict();
    };



    //------------ KeyContext ---------------

    class KeyContext : public BuildContextFirst
    {
    public:
        explicit KeyContext(Builder& builder);

        ValueKeyContext& Value(Node::Value value);
    };



    //------------ ValueKeyContext ---------------

    class ValueKeyContext : public BuildContextSecond
    {
    public:
        explicit ValueKeyContext(Builder& builder);
    };



    //------------ ValueArrayContext ---------------

    class ValueArrayContext : public BuildContextFirst
    {
    public:
        explicit ValueArrayContext(Builder& builder);

        ValueArrayContext& Value(Node::Value value);

        Builder& EndArray();
    };



    //------------ DictContext ---------------

    class DictContext : public BuildContextSecond
    {
    public:
        explicit DictContext(Builder& builder);
    };



    //------------ ArrayContext ---------------

    class ArrayContext : public ValueArrayContext
    {
    public:
        explicit ArrayContext(Builder& builder);
    };



    //------------ Builder ---------------

    class Builder final : virtual public KeyContext, virtual public ValueKeyContext,
        virtual public DictContext, virtual public ArrayContext
    {
    public:
        Builder();

        KeyContext& Key(std::string key);

        Builder& Value(Node::Value value);

        DictContext& StartDict();

        Builder& EndDict();

        ArrayContext& StartArray();

        Builder& EndArray();

        Node Build() const;

    private:
        Builder& AddNode(const Node& node);

        void PushNode(Node::Value value);

        bool UnableAdd() const;
        bool IsMakeObj() const;
        bool UnableUseKey() const;
        bool UnableUseValue() const;
        bool UnableUseStartDict() const;
        bool UnableUseEndDict() const;
        bool UnableUseStartArray() const;
        bool UnableUseEndArray() const;
        bool UnableUseBuild() const;

    private:
        Node root_ = nullptr;
        std::stack<std::unique_ptr<Node>> nodes_;
    };
}
