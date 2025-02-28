#pragma once

#include <iostream>
#include <fstream>
#include <optional>
#include <queue>
#include <functional>
#include <memory>
#include <concepts>
#include <type_traits>
#include <stack>


namespace tree
{

enum class Degree : int
{
    none = 0,
    only_left = 1,
    only_right = 2,
    both = 3,
};

template<typename T>
class Node;

template<typename T>
class InOrderIterator
{
private:

    /*
        Forward iterator

        Operators that seem to be necessary:
        pre-increment operator operator++() -> moves iterator to next value.
        dereference operator operator*()    -> Gets value
        and equality operators operator==() -> What are these supposed to do?
                           and operator!=()
    */

    std::stack<Node<T>*> stack_;

    void push_left_(Node<T>* node)
    {
        while(node)
        {
            stack_.push(node);
            node = node->left().get();
        }
    }

public:

    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

    InOrderIterator(Node<T>* root)
    {
        push_left_(root);
    }

    value_type operator*() const
    {
        return stack_.top()->data;
    }

    InOrderIterator<T>& operator++()    // Pre-increment
    {
        auto node { stack_.top() };
        stack_.pop();
        push_left_(node->right().get());
        return *this;
    }
    InOrderIterator<T> operator++(int)  // Post-increment
    {
        InOrderIterator temp { *this };
        ++(*this);
        return temp;
    }

    bool operator!=(const InOrderIterator& other) const
    {
        return !(*this == other);
    }

    bool operator==(const InOrderIterator& other) const
    {
        return stack_.empty() && other.stack_.empty();  // TO DO: Check the logic behind this.

    }

};

template<typename T>
class Node
{
private:

    std::unique_ptr<Node<T>> right_ { nullptr };
    std::unique_ptr<Node<T>> left_  { nullptr };
    size_t height_ { 1 };

public:

    T data;

    /*
        Constructors
    */
    explicit Node(T data_) : data {data_} {}

    Node()
    requires (std::is_default_constructible_v<T> &&
              !std::is_trivially_default_constructible_v<T>)
    : data {} {}

    template<typename... Args>
    explicit Node(Args&&... args)
        : data {std::forward<Args>(args)...}
    {
        if constexpr ( std::is_class_v<T> )
            data = T(std::forward<Args>(args)...);
    }

    Node(const Node& other) = delete;
    Node& operator=(const Node& other) = delete;

    Node(Node&& other)
    {
        right_ = std::move(other.right_);
        left_ = std::move(other.left_);
        data = std::move(other.data);
    }

    Node& operator= (Node&& other)
    {
        right_ = std::move(other.right_);
        left_ = std::move(other.left_);
        data = std::move(other.data);
        return *this;
    }

    /*
        Public member functions
    */

    void right(Node* child) { right_ = std::unique_ptr<Node<T>>(child); }
    void right(std::unique_ptr<Node<T>> child)
    {
        right_ = std::move(child);
    }
    void right(Node<T>&& child)
    {
        right_ = std::make_unique<Node<T>>(std::forward<Node<T>&&>(child));
    }
    std::unique_ptr<Node<T>>& right(T new_data)
    {
        right_ = std::make_unique<Node<T>>(new_data);
        return right_;
    }
    template<typename... Args>
    requires (sizeof...(Args) > 0)
    std::unique_ptr<Node<T>>& right(Args&&... args)
    {
        right_ = std::make_unique<Node<T>>(args...);
        return right_;
    }
    std::unique_ptr<Node<T>>& right() { return right_; }
    std::unique_ptr<Node<T>> release_right() { return std::move(right_); }

    void left(Node* child) { left_ = std::unique_ptr<Node<T>>(child); }
    void left(std::unique_ptr<Node<T>> child)
    {
        left_ = std::move(child);
    }
    void left(Node<T>&& child)
    {
        left_ = std::make_unique<Node<T>>(std::forward<Node<T>&&>(child));
    }
    std::unique_ptr<Node<T>>& left(T new_data)
    {
        left_ = std::make_unique<Node<T>>(new_data);
        return left_;
    }
    template<typename... Args>
    requires (
        (sizeof...(Args) > 0) &&
        !(sizeof...(Args) == 1 && std::same_as<std::tuple_element_t<0, std::tuple<Args...>>, std::unique_ptr<Node<T>>&>)
        )
    std::unique_ptr<Node<T>>& left(Args&&... args)
    {
        left_ = std::make_unique<Node<T>>(args...);
        return left_;
    }
    std::unique_ptr<Node<T>>& left() { return left_; }
    std::unique_ptr<Node<T>> release_left() { return std::move(left_); }

    // Operators
    auto operator<=>(const Node<T>& other) const
    {
        return data <=> other.data;
    }
    bool operator ==(const Node<T>& other) const
    {
        return data == other.data;
    }

    inline Degree degree() const
    {
        int value { ( right_ ? 2 : 0 ) + ( left_  ? 1 : 0 ) };
        return Degree(value);
    }

    template<typename K> friend size_t height(const std::unique_ptr<Node<K>>&);   // Return node's height
    template<typename K> friend void update_height(std::unique_ptr<Node<K>>&);    // Update node's height
    template<typename K> friend long long skew(const std::unique_ptr<Node<K>>&);
    template<typename K> friend size_t depth(const std::unique_ptr<Node<K>>&);    // Return tree's depth

    template<typename K> friend size_t count_nodes(const std::unique_ptr<Node<K>>&);

    // Traversals

    template<typename K> friend void print(const std::unique_ptr<Node<K>>&, int);

    template<typename K, typename F> friend void in_order(const std::unique_ptr<Node<K>>&, F);
    template<typename K, typename F> friend void pre_order(const std::unique_ptr<Node<K>>&, F);
    template<typename K, typename F> friend void post_order(const std::unique_ptr<Node<K>>&, F);
    template<typename K, typename F> friend void level_order(const std::unique_ptr<Node<K>>&, F);

    // Tree Type

    template<typename K> friend bool is_full(const std::unique_ptr<Node<K>>&);
    template<typename K> friend bool is_complete(const std::unique_ptr<Node<K>>&, size_t);
    template<typename K> friend bool is_perfect(const std::unique_ptr<Node<K>>&, size_t);
    template<typename K> friend bool is_balanced(const std::unique_ptr<Node<K>>&);

    template<typename K> friend std::optional<K> search(const std::unique_ptr<Node<K>>&, K);

    // Complete comparison
    template<typename K> friend bool compare(const std::unique_ptr<Node<K>>& lhs, const std::unique_ptr<Node<K>>& rhs);

    // Serialization
    template<typename K> friend void serialize(const std::unique_ptr<Node<K>>&, std::ostream&);

    // Iteration

    InOrderIterator<T> begin()
    {
        return InOrderIterator<T>(this);
    }
    InOrderIterator<T> end()
    {
        return InOrderIterator<T>(nullptr);
    }

};

// Serialization

template<typename T>
void serialize(const std::unique_ptr<Node<T>>& root, std::ostream& out)
{
    if ( root == nullptr ) {
        out << "# ";  // Use "#" to denote a null node
        return;
    }
    out << root->data << " "; // Serialize current node's data
    serialize(root->left_, out);      // Serialize left subtree
    serialize(root->right_, out);     // Serialize right subtree
}

template<typename T>
std::unique_ptr<Node<T>> deserialize(std::ifstream& in)
{
    std::string val;
    in >> val;

    if (val == "#") {
        return nullptr;  // If the value is "#", it represents a null node
    }

    T data = std::stoi(val);
    auto node { std::make_unique<Node<T>>(data) };
    node->left(deserialize<T>(in));  // Rebuild left subtree
    node->right(deserialize<T>(in)); // Rebuild right subtree
    return std::move(node);
}

// Height of a node.
template<typename T>
inline size_t height(const std::unique_ptr<Node<T>>& node)
{
    return node ? node->height_ : 0;
}

// Update the height of a node.
template<typename T>
inline void update_height(std::unique_ptr<Node<T>>& node)
{
    node->height_ = std::max(height(node->left()), height(node->right())) + 1;
}

// Calculate skew of a node.
template<typename T>
inline long long skew(const std::unique_ptr<Node<T>>& node)
{
    if ( !node ) return 0;
    return height(node->right_) - height(node->left_);
}

// Depth of the tree.
template<typename T>
size_t depth(const std::unique_ptr<Node<T>>& root)
{
    if ( root == nullptr ) return 0;
    size_t height_left  { depth(root->left_)  };
    size_t height_right { depth(root->right_) };
    return std::max(height_left, height_right) + 1;
}

template<typename T>
size_t count_nodes(const std::unique_ptr<Node<T>>& node)
{
    if ( node == nullptr ) return 0;
    return (count_nodes(node->left_) + count_nodes(node->right_) + 1);
}

// Recursive function to display the tree sideways
template<typename T>
void print(const std::unique_ptr<Node<T>>& node, int depth = 0)
{
    if (node == nullptr) return;
    print(node->right_, depth + 1);
    std::cout << std::string(4 * depth, ' ');
    std::cout << node->data << " " << node->height_;
    std::cout << std::endl;
    print(node->left_, depth + 1);
}




// Traversals

template<typename T, typename F>
void in_order(const std::unique_ptr<Node<T>>& root, F fnc)
{
    if ( root == nullptr ) return;
    in_order(root->left_, fnc);
    fnc(root->data);
    in_order(root->right_, fnc);
}

template<typename T, typename F>
void pre_order(const std::unique_ptr<Node<T>>& root, F fnc)
{
    if ( root == nullptr ) return;
    fnc(root->data);
    pre_order(root->left_, fnc);
    pre_order(root->right_, fnc);
}

template<typename T, typename F>
void post_order(const std::unique_ptr<Node<T>>& root, F fnc)
{
    if ( root == nullptr ) return;
    post_order(root->left_, fnc);
    post_order(root->right_, fnc);
    fnc(root->data);
}

template<typename T, typename F>
void level_order(const std::unique_ptr<Node<T>>& root, F fnc)
{
    if ( root == nullptr ) return;
    std::queue<const Node<T>*> q;
    q.push(root.get());
    while ( !q.empty() )
    {
        const Node<T>* it { q.front() };
        q.pop();
        fnc(it->data);
        if ( it->left_ != nullptr )  q.push(it->left_.get());
        if ( it->right_ != nullptr ) q.push(it->right_.get());
    }
}

// Tree Type

/*
    Every node in a full binary tree has either 0 or 2 children.

    Also known as a proper binary tree.
*/
template<typename T>
bool is_full(const std::unique_ptr<Node<T>>& node)
{
  if ( !node ) return true;
  if ( node->left_ == nullptr && node->right_ == nullptr ) return true;   // Base case

  if ( node->left_ && node->right_ )
    return (is_full(node->left_) && is_full(node->right_));
  return false;
}

/*
    In a complete binary tree every level, except possibly the last, is completely
    filled and all nodes at the last level are as far left as possible.
*/
template<typename T>
bool is_complete(const std::unique_ptr<Node<T>>& node, size_t index = 0)
{
    if ( !node ) return true;
    static size_t node_count { count_nodes(node) };
    if ( index >= node_count ) return false;
    return (is_complete(node->left_, 2 * index + 1) && is_complete(node->right_, 2 * index + 2));
}

/*
    All parents in a perfect binary tree have exactly 2 children, and
    all leaves are on the same level.

    Every perfect binary tree is full and perfect.
*/
template<typename T>
bool is_perfect(const std::unique_ptr<Node<T>>& node, size_t level = 0)
{
    if ( !node ) return true;
    static size_t tree_depth { depth(node) };
    if ( node->left_ == nullptr && node->right_ == nullptr)  // TO DO: Use Degree enum
        return (tree_depth == level + 1);  // We got a leaf. Check if it is at the last level.
    if ( node->left_ == nullptr || node->right_ == nullptr ) // TO DO: Use Degree enum
        return false;                       // Internal node with only one child.
                                            // Such nodes cannot exist in a perfect tree.
    // If we have a internal node with both children, we must check both its subtrees.
    return is_perfect(node->left_, level + 1) &&
           is_perfect(node->right_, level + 1);
}

/*
    The height of both subtrees of every node differs by at most 1.
*/
template<typename T>
bool is_balanced(const std::unique_ptr<Node<T>>& node)
{
    if ( !node ) return true;
    std::queue<const Node<T>*> q;
    q.push(node.get());
    while ( !q.empty() )
    {
        const Node<T>* it { q.front() };
        q.pop();

        size_t height_left  { depth(it->left_)  };
        size_t height_right { depth(it->right_) };
        if ( (height_left >= (height_right + 2)) || (height_right >= (height_left + 2)) ) return false;

        if ( it->left_ != nullptr )  q.push(it->left_.get());
        if ( it->right_ != nullptr ) q.push(it->right_.get());
    }
    return true;
}

template<typename T>
std::optional<T> search(const std::unique_ptr<Node<T>>& node, T key)
{
    if ( node == nullptr ) return std::nullopt;
    std::queue<Node<T>*> q;
    q.push(node.get());
    while ( !q.empty() )
    {
        Node<T>* it { q.front() };
        q.pop();
        if ( it->data == key ) return it->data;
        if ( it->left_ != nullptr ) q.push(it->left_.get());
        if ( it->right_ != nullptr ) q.push(it->right_.get());
    }
    return std::nullopt;
}

template<typename T>
bool compare(const std::unique_ptr<Node<T>>& lhs, const std::unique_ptr<Node<T>>& rhs)
{
    if ( !lhs && !rhs ) return true;
    if ( (*lhs != *rhs) || (lhs->degree() != rhs->degree()) ) return false;
    bool result { true };
    result = compare(lhs->left(), rhs->left());
    if ( !result ) return false;
    result = compare(lhs->right(), rhs->right());
    return result;
}

}  // namespace tree
