#ifndef NODE_H
#define NODE_H

#include <iostream>

template <typename T>
class Node {
    public:
        Node();
        Node(const T &v, const double p, Node<T> *l = nullptr, Node<T> *r = nullptr);

        T value;
        double probability;
        Node<T>* right;
        Node<T>* left;

        bool operator ()(const Node<T>* lhs, const Node<T>* rhs) const
        {
            return lhs->probability > rhs->probability;
        }

};

template <typename T>
Node<T>::Node()
{
    value = T();
    probability = 0;
    right = nullptr;
    left = nullptr;
    
}

template <typename T>
Node<T>::Node(const T &v, const double p, Node<T> *l, Node<T> *r)
{
    value = v; 
    probability = p;
    right = r;
    left = l; 
}

template <typename T>
void inorderTraversal(Node<T>* root, std::string encoding, std::map<T, std::string>& map){

    if (root == nullptr) { 
        return;
    }; 

    inorderTraversal(root->left, encoding + "0", map);
    if(root->left == nullptr && root->right == nullptr) {
        // std::cout << std::hex << (unsigned int) root -> value << " " << encoding << std::endl;
        map[root->value] = encoding;
    }
    inorderTraversal(root->right, encoding + "1", map);
}

#endif
