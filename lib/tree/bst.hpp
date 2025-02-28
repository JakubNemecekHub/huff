#pragma once

#include <memory>

#include "linked.hpp"

namespace tree
{


template<typename T>
class BST
{
protected:

    std::unique_ptr<Node<T>> root_ { nullptr };

    // Recursive helper member function for adding nodes.
    virtual void add_(T data, std::unique_ptr<Node<T>>& node)
    {
        if ( data <= node->data )
        {
            if ( !node->left() ) node->left(data);
            else                 add_(data, node->left());

        }
        else
        {
            if ( !node->right() ) node->right(data);
            else                  add_(data, node->right());
        }
    }

    // Recursive helper member function for search.
    static std::optional<T> search_(T key, const std::unique_ptr<Node<T>>& node)
    {
        if ( !node )             return std::nullopt;
        if ( key == node->data ) return node->data;
        if ( key < node->data  ) return search_(key, node->left());
        else                     return search_(key, node->right());
    }

    // Recursive helper member function for finding maximum value
    static T max_(const std::unique_ptr<Node<T>>& node)
    {
        if ( !node->right() ) return node->data;
        else return max_(node->right());
    }

    // Recursive helper member function for finding minimum value
    static T min_(const std::unique_ptr<Node<T>>& node)
    {
        if ( !node->left() ) return node->data;
        else return min_(node->left());
    }

    // Recursive helper member function for extracting node with maximum value
    virtual T extract_max_(std::unique_ptr<Node<T>>& it)
    {
        if ( it->right() ) return extract_max_(it->right());
        T result { it->data };
        it = std::move(it->release_left());
        return result;
    }

    // Recursive helper member function for extracting node with minimum value
    virtual T extract_min_(std::unique_ptr<Node<T>>& it)
    {
        if ( it->left() ) return extract_min_(it->left());
        T result { it->data };
        it = std::move(it->release_right());
        return result;
    }

    // Recursive helper member function to delete node by its key
    virtual bool remove_(T key, std::unique_ptr<Node<T>>& it)
    {
        if ( !it ) return false;  // Key not found in the tree.
        if ( key == it->data )    // Base case: do the deletion.
        {
            switch ( it->degree() )
            {
            case Degree::none:       it.reset();                           break;
            case Degree::only_right: it = std::move(it->release_right());  break;
            case Degree::only_left:  it = std::move(it->release_left());   break;
            case Degree::both:       it->data = extract_min_(it->right()); break;
            default: break;
            }
            return true;
        }
        else if ( key < it->data ) return remove_(key, it->left());
        else                       return remove_(key, it->right());
        return false;
    }


public:

    /*
        Constructors
    */
    BST() {}

    BST(T data)
    {
        root_ = std::make_unique<Node<T>>(data);
    }

    template<typename... Args>
    requires (sizeof...(Args) > 0)
    explicit BST(Args&&... args)
    {
        root_ = std::make_unique<Node<T>>(std::forward<Args>(args)...);
    }

    BST(std::vector<T> data)
    {
        if ( data.empty() ) BST();
        else
        {
            root_ = std::make_unique<Node<T>>(data.at(0));
            for ( int i {1}; i < data.size(); ++i )
            {
                add(data.at(i));
            }
        }
    }

    /*
        Public member functions
    */

    std::unique_ptr<Node<T>>&  root() { return root_; };

    void add(T data)
    {
        if ( !root_ ) root_ = std::make_unique<Node<T>>(data);
        else          add_(data, root_);
    }

    std::optional<T> search(T key)
    {
        return search_(key, root_);
    }

    bool remove(T key)
    {
        // Test for nullptr is done in the auxiliary member function remove_,
        // just like in the search member function.
        // This is because it needs to be done for every node to properly handle
        // non-existing keys.
        return remove_(key, root_);
    }

    std::optional<T> max()
    {
        if ( !root_ ) return std::nullopt;
        return max_(root_);
    }

    std::optional<T> min()
    {
        if ( !root_ ) return std::nullopt;
        return min_(root_);
    }

    std::optional<T> extract_max()
    {
        if ( !root_ ) return std::nullopt;
        return extract_max_(root_);
    }
    std::unique_ptr<Node<T>> extract_max_node()
    {
        if ( !root_ ) return nullptr;
        return std::make_unique<Node<T>>(extract_max_(root_));
    }

    std::optional<T> extract_min()
    {
        if ( !root_ ) return std::nullopt;
        return extract_min_(root_);
    }
    std::unique_ptr<Node<T>> extract_min_node()
    {
        if ( !root_ ) return nullptr;
        return std::make_unique<Node<T>>(extract_min_(root_));
    }

    // Friends

    template<typename K> friend void print(const BST<K>&);

    template<typename K, typename F> friend void in_order(const BST<K>& tree, F fnc);
    template<typename K, typename F> friend void pre_order(const BST<K>& tree, F fnc);
    template<typename K, typename F> friend void post_order(const BST<K>& tree, F fnc);
    template<typename K, typename F> friend void level_order(const BST<K>& tree, F fnc);

    // Iteration

    InOrderIterator<T> begin()
    {
        return InOrderIterator<T>(root_.get());
    }
    InOrderIterator<T> end()
    {
        return InOrderIterator<T>(nullptr);
    }

};

template<typename T>
void print(const BST<T>& tree) { print(tree.root_); }

template<typename T, typename F>
void in_order(const BST<T>& tree, F fnc) { in_order(tree.root_, fnc); }

template<typename T, typename F>
void pre_order(const BST<T>& tree, F fnc) { pre_order(tree.root_, fnc); }

template<typename T, typename F>
void post_order(const BST<T>& tree, F fnc) { post_order(tree.root_, fnc); }

template<typename T, typename F>
void level_order(const BST<T>& tree, F fnc) { level_order(tree.root_, fnc); }

}  // namespace tree
