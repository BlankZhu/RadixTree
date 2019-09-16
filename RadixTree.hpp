/*****************************************************************
** File : RadixTree.hpp
** Description : An implementation of radix-tree
** Author : BlankZhu
*******************************************************************/
#ifndef RADIXTREE_H_
#define RADIXTREE_H_

#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <deque>
#include <iostream>

#include "RadixTreeNode.hpp"

template <typename T>
class RadixTree
{
public:
	using NodePtr = std::shared_ptr<RadixTreeNode<T>>;
	using Pair = std::pair<char, NodePtr>;

	RadixTree() = default;
	~RadixTree() = default;
	RadixTree(const RadixTree&) = delete;
	RadixTree& operator=(const RadixTree&) = delete;
	RadixTree(RadixTree&&) = delete;            // 'cause I'm lazy
	RadixTree& operator=(RadixTree&&) = delete; // 'cause I'm lazy

	/*********************************
	@Method: operator[]
	@Description: use SearchByKey to access value by key in []
	@param key in string
	@result throw error if key doesn't exist, otherwise return a copy of value
	@see
	@author BlankZhu
	@version 1.0
	@date: 2019/09/16
	**********************************/
	T operator[](const std::string& key) const
	{
		return SearchByKey(key);
	}

	/*********************************
	@Method: UpsertString
	@Description: used to insert new value by given key, if key already exists, do update
	@param key in string and a value
	@result update or insert
	@see
	@author BlankZhu
	@version 1.0
	@date: 2019/09/16
	**********************************/
	void UpsertString(const std::string& key, T value)
	{
		if (key.size() == 0)
		{
			throw std::runtime_error("Invalid key");
		}

		// first init
		if (root_ == nullptr)
		{
			NodePtr new_node(new RadixTreeNode<T>);

			root_ = std::make_shared<RadixTreeNode<T>>();
			root_->is_stop_ = false;
			(root_->search_path_).push_back(std::make_pair(key[0], new_node));

			new_node->path_content_ = key;
			new_node->is_stop_ = true;
			new_node->value_ = value;
		}
		else
		{
			// start searching the tree
			NodePtr curr_node = root_;
			std::deque<char> key_queue(key.begin(), key.end());

			// check in found in the first layer (after root)
			bool in_first_layer = false;
			for (auto& p : root_->search_path_)
			{
				if (p.first == key_queue.front())
				{
					curr_node = p.second;
					in_first_layer = true;
					break;
				}
			}
			if (!in_first_layer)
			{
				// do insertion in first layer
				NodePtr new_node_ptr(new RadixTreeNode<T>);

				(root_->search_path_).push_back(std::make_pair(key_queue.front(), new_node_ptr));
				new_node_ptr->is_stop_ = true;
				new_node_ptr->value_ = value;
				new_node_ptr->path_content_ = key;

				return;
			}

			// get longest common prefix in the tree
			while (curr_node != nullptr)
			{
				auto path_content_begin = (curr_node->path_content_).begin();
				auto path_content_end = (curr_node->path_content_).end();
				auto iter = path_content_begin;

				while (!(key_queue.empty() || iter == path_content_end))
				{
					if (key_queue.front() == *iter)
					{
						key_queue.pop_front();
						++iter;
					}
					else
					{
						break;
					}
				}

				// both content and key are all consumed
				if (key_queue.empty() && iter == path_content_end)
				{
					// update value
					curr_node->is_stop_ = true;
					curr_node->value_ = value;
					break;
				}
				// content remains, current key is a common prefix
				else if (key_queue.empty())
				{
					NodePtr new_node_ptr(new RadixTreeNode<T>);
					// split a new tree from curr node
                    auto new_search_char = *iter;
					auto new_parent_content = (curr_node->path_content_).substr(0, iter - path_content_begin);
					auto new_child_content = (curr_node->path_content_).substr(iter - path_content_begin);
					curr_node->path_content_ = std::move(new_parent_content);
					new_node_ptr->path_content_ = std::move(new_child_content);
					new_node_ptr->is_stop_ = (curr_node->is_stop_);
					curr_node->is_stop_ = true;

					new_node_ptr->value_ = curr_node->value_;
					curr_node->value_ = value;
					// make up search path
					new_node_ptr->search_path_ = curr_node->search_path_;
					(curr_node->search_path_).clear();

					(curr_node->search_path_).push_back(std::make_pair(new_search_char, new_node_ptr));

					break;
				}
				// key remains, keep searching the next
				else if (iter == path_content_end)
				{
					// chek if next node with specific char exist
					// if do, keep searching the path
					bool search_next = false;
					for (const auto& p : curr_node->search_path_)
					{
						// such path exists, search next;
						if (p.first == key_queue.front())
						{
							curr_node = p.second;
							search_next = true;
							break;
						}
					}
					// otherwise, no such relevant value, do insertion
					if (!search_next)
					{
						NodePtr new_node(new RadixTreeNode<T>);
						new_node->is_stop_ = true;
						new_node->path_content_ = std::string(key_queue.begin(), key_queue.end());
						new_node->value_ = value;
						(curr_node->search_path_).push_back(std::make_pair(key_queue.front(), new_node));

						break;
					}
				}
				// key remains, iter doesn't meet the end, key and content differ in the middle
				else
				{
					NodePtr new_node_a(new RadixTreeNode<T>);
					NodePtr new_node_b(new RadixTreeNode<T>);

					// make up a new node for new key
					new_node_a->path_content_ = std::string(key_queue.begin(), key_queue.end());
					new_node_a->is_stop_ = true;
					new_node_a->value_ = value;

					// make up a new node for old key
					new_node_b->path_content_ = std::string(iter, path_content_end);
					new_node_b->is_stop_ = curr_node->is_stop_;
					new_node_b->value_ = curr_node->value_;
					new_node_b->search_path_ = curr_node->search_path_;

					// remake curr node
					// curr_node->path_content_ = std::string(path_content_begin, iter);
					curr_node->path_content_ = (curr_node->path_content_).substr(0, iter - path_content_begin);
					(curr_node->search_path_).clear();
					(curr_node->search_path_).push_back(std::make_pair(new_node_a->path_content_[0], new_node_a));
					(curr_node->search_path_).push_back(std::make_pair(new_node_b->path_content_[0], new_node_b));
					curr_node->is_stop_ = false;
					//curr_node->value_.clear();
					curr_node->value_ = T{};
					break;
				}
			}
		}
	}

	/*********************************
	@Method: DeleteString
	@Description: delete given key-value
	@param key in string
	@result delete given key and value, throw error if key doesn't exist
	@see
	@author BlankZhu
	@version 0.0
	@date: 2019/09/16
	@NOTE: too lazy to implement this...
	**********************************/
	// bool DeleteString(const std::string& key);

	/*********************************
	@Method: SearchByKey
	@Description: search and get value by key
	@param key in string
	@result return a value of relevant key, throw error if no such key
	@see
	@author BlankZhu
	@version 1.0
	@date: 2019/09/16
	**********************************/
	T SearchByKey(const std::string& key) const
	{
		char err_msg[] = "No such key";
		// param check
		if (key.size() == 0)
		{
			throw std::runtime_error(err_msg);
		}

		auto curr_node = root_;
		// first step search
		for (auto& p : root_->search_path_)
		{
			if (p.first == key[0])
			{
				curr_node = p.second;
				break;
			}
		}
		// otherwise, no such string
		if (curr_node == root_)
		{
			throw std::runtime_error(err_msg);
		}

		std::deque<char> key_queue(key.begin(), key.end());
		while (curr_node != nullptr)
		{
			auto path_content_begin = (curr_node->path_content_).begin();
			auto path_content_end = (curr_node->path_content_).end();
			auto iter = path_content_begin;

			while (!(key_queue.empty() || iter == path_content_end))
			{
				if (key_queue.front() == *iter)
				{
					key_queue.pop_front();
					++iter;
				}
				else
				{
					break;
				}
			}

			// both content and key are all consumed
			if (key_queue.empty() && iter == path_content_end)
			{
				// return value
				if (curr_node->is_stop_ == true)
				{
					return curr_node->value_;
				}
				throw std::runtime_error("This tree has a bad initialization, check your insertion!");
			}
			// content remains, current key is a common prefix
			else if (key_queue.empty())
			{
				throw std::runtime_error("No such key!");
			}
			// key remains, keep searching the next
			else if (iter == path_content_end)
			{
				// check if next node with specific char exist
				bool search_next = false;
				// if do, keep searching the path
				for (const auto& p : curr_node->search_path_)
				{
					// such path exists, search next;
					if (p.first == key_queue.front())
					{
						curr_node = p.second;
						search_next = true;
						break;
					}
				}
				// otherwise, no such relevant value, do insertion
				if (!search_next)
				{
					throw std::runtime_error("No such key!");
				}
			}
			else
			{
				throw std::runtime_error("No such key!");
			}
		}
		return T{};
	}

	/*********************************
	@Method: DebugPrint
	@Description: print the radix tree
	@param (none)
	@result print to std::cout
	@see
	@author BlankZhu
	@version 1.0
	@date: 2019/09/16
	@NOTE: only for debugging
	**********************************/
	void DebugPrint()
	{
		DebugPrintHelper(root_);
	}

protected:
	NodePtr root_;

private:
	/*********************************
	@Method: DebugPrintHelper
	@Description: help to print the radix tree
	@param (none)
	@result
	@see
	@author BlankZhu
	@version 1.0
	@date: 2019/09/16
	@NOTE: only for debugging
	**********************************/
	void DebugPrintHelper(NodePtr ptr, int depth = 0)
	{
		if (ptr == root_)
		{
			std::cout << "[root]"
				<< " content_path_: " << ptr->path_content_;
			if (root_->search_path_.size() != 0)
			{
				for (auto p : ptr->search_path_)
				{
					std::cout << "[" << p.first << " - " << p.second << "] ";
				}
			}
			std::cout << std::endl;
			for (auto p : ptr->search_path_)
			{
				DebugPrintHelper(p.second, depth + 1);
			}
		}
		else
		{
			for (int i = 0; i < depth; ++i)
				std::cout << " ";
			std::cout << "[" << ptr << "] ";
			if (ptr->is_stop_)
			{
				std::cout << "value_: [" << ptr->value_ << "]";
			}

			if ((ptr->search_path_).size() == 0)
			{
				std::cout << " content_path_: [" << ptr->path_content_ << "]  EOT" << std::endl;
				return;
			}
			else
			{
				std::cout << "  search_path_:";
				for (auto p : ptr->search_path_)
				{
					std::cout << "[" << p.first << " - " << p.second << "] ";
				}
				std::cout << " content_path_: [" << ptr->path_content_ << "]\n";
				for (auto p : (ptr->search_path_))
				{
					DebugPrintHelper(p.second, depth + 1);
				}
			}
		}
	}
};

#endif
