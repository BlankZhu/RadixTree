/*****************************************************************
** File : RadixTreeNode.hpp
** Description : Implementation of radix-tree's node
** Author : BlankZhu
*******************************************************************/
#ifndef RADIXTREENODE_H_
#define RADIXTREENODE_H_

#include <string>
#include <vector>
#include <memory>
#include <utility>

template <typename T>
class RadixTreeNode
{
    using NodePtr = std::shared_ptr<RadixTreeNode<T>>;
    using Pair = std::pair<char, NodePtr>;

public:
    RadixTreeNode() : is_stop_(false), value_(T{}){};
    ~RadixTreeNode() = default;
    RadixTreeNode &operator=(const RadixTreeNode &) = default;

    std::vector<Pair> search_path_; // for route searching
    std::string path_content_;      // for path compression
    bool is_stop_;                  // if current node is a stop, if so, value_ is meaningful
    T value_;                       // the value it store
};

#endif
