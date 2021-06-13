#include "stdafx.h"
#include "StringIndexerTree.h"
#include <iostream>
#include <algorithm>
#include <functional>

namespace
{

auto GetChildIt(std::vector<Node::Ptr>& children, char c)
{
    auto it = std::lower_bound(children.begin(), children.end(), c,
        [](const Node::Ptr& lhs, char c)
        {
            return lhs->m_character < c;
        });

    return it;
}

}//end of namespace


void StringIndexerTree::Add(const std::string& str, Record::Ptr record)
{
    if (str.empty())
    {
        return;//nothing to insert
    }
    Node::InsertString(m_root, str, record);
}

void StringIndexerTree::Delete(const std::string& str, Record::IdType id)
{
    Node::Ptr depthNode = m_root;
    for (size_t i = 0; i < str.size(); ++i)
    {
        {//Delete links to string in depth
            char c = str[i];
            auto it = GetChildIt(depthNode->m_children, c);
            if (it != depthNode->m_children.end())
            {
                depthNode = *it;
                Record::DeleteRecord(depthNode->m_links, id);
                //Erase all children left with no links
                depthNode->m_children.erase(std::remove_if(depthNode->m_children.begin(), depthNode->m_children.end(), [](Node::Ptr node)
                    {
                        return node->m_links.empty();
                    }), depthNode->m_children.end());
            }
        }//Delete links to string in depth

        {//Delete links to string in bredth
            Node::Ptr bredthNode = m_root;
            for (size_t j = i + 1; j < str.size(); ++j)
            {
                char b = str[j];
                auto it = GetChildIt(bredthNode->m_children, b);
                if (it != bredthNode->m_children.end())
                {
                    bredthNode = *it;
                    Record::DeleteRecord(bredthNode->m_links, id);
                    //Erase all children left with no links
                    bredthNode->m_children.erase(std::remove_if(bredthNode->m_children.begin(), bredthNode->m_children.end(), [](Node::Ptr node)
                        {
                            return node->m_links.empty();
                        }), bredthNode->m_children.end());
                }
            }
        }//Delete links to string in bredth
    }
}

Node::Ptr StringIndexerTree::Find(const std::string& str) const
{
    //searches for str in the tree and returns node if str exists in tree
    Node::Ptr node = m_root;
    for (const auto& c : str)
    {
        auto it = GetChildIt(node->m_children, c);
        if (it != node->m_children.end())
        {
            if ((*it)->m_character == c)
            {
                node = *it;
                continue;
            }
        }
        return nullptr;
    }

    return node;
}

void Node::InsertString(Node::Ptr root, const std::string& str, Record::Ptr record)
{
    if (nullptr == root)
    {
        return;
    }

    auto writeSuffix = [](Node::Ptr parent, char c, Record::Ptr record) -> Node::Ptr
    {
        auto it = GetChildIt(parent->m_children, c);
        if (it != parent->m_children.end())
        {
            if ((*it)->m_character == c)//check if we already have character added
            {
                auto& collection = (*it)->m_links;
                const bool hasElement = std::any_of(collection.begin(), collection.end(), [record](const Record::Ptr& r)
                    {
                        return record->uniqueId == r->uniqueId;
                    });

                //Avoid duplicated records - when we have repeated characters in a string
                if (!hasElement)
                {
                    Record::InsertSorted(collection, record);//insert if new link
                }
                return *it;
            }
        }

        return parent->AddChild(c, record);
    };

    Node::Ptr depthNode = root;
    for (size_t i = 0; i < str.size(); ++i)
    {
        {//Insert whole word in depth
            char c = str[i];
            auto child = writeSuffix(depthNode, c, record);
            depthNode = child;
        }//Insert whole word in depth

        {//Insert suffix in bredth
            Node::Ptr bredthNode = root;//Start from root
            for (size_t j = i + 1; j < str.size(); ++j)
            {
                char b = str[j];
                auto bChild = writeSuffix(bredthNode, b, record);
                bredthNode = bChild;
            }
        }//Insert suffix in bredth
    }
}

Node::Ptr Node::AddChild(char c, Record::Ptr record)
{
    Ptr child = std::make_shared<Node>();
    child->m_character = c;
    child->m_links.push_back(record);//don't need to InsertSorted as it's first element
    child->m_parent = shared_from_this();

    m_children.insert(std::lower_bound(m_children.begin(), m_children.end(), child,
                                       [](const Node::Ptr& lhs, const Node::Ptr& rhs)
                                       {
                                           return lhs->m_character < rhs->m_character;
                                       }),
                      child);

    return child;
}
