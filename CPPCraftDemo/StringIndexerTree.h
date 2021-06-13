#pragma once

#include "Record.h"

#include <string>
#include <vector>
#include <memory>

struct Node : public std::enable_shared_from_this<Node>
{
    using Ptr = std::shared_ptr<Node>;
    using WeakPtr = std::weak_ptr<Node>;

    Ptr AddChild(char c, Record::Ptr record);

    //Inserts string to the whole tree and adds records to node's links
    static void InsertString(Node::Ptr root, const std::string& str, Record::Ptr record);

    char m_character = 0;
    WeakPtr m_parent;
    std::vector<Ptr> m_children;
    std::vector<Record::Ptr> m_links;
};

//Indexes given strings to have faster search
class StringIndexerTree
{
public:
    void Add(const std::string& str, Record::Ptr record);
    void Delete(const std::string& str, Record::IdType id);

    Node::Ptr Find(const std::string& str) const;

private:
    Node::Ptr m_root = std::make_shared<Node>();

};