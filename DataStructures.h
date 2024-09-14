#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

#include <iostream>
using namespace std;

// Nodo de lista enlazada
template<typename T>
class Node {
public:
    T data;
    Node* next;
    Node(T val) : data(val), next(nullptr) {}
};

// Lista enlazada
template<typename T>
class LinkedList {
public:
    Node<T>* head;

    LinkedList() : head(nullptr) {}

    void insert(T val) {
        Node<T>* newNode = new Node<T>(val);
        if (head == nullptr) {
            head = newNode;
        } else {
            Node<T>* temp = head;
            while (temp->next != nullptr) {
                temp = temp->next;
            }
            temp->next = newNode;
        }
    }

    void search(T val) {
        Node<T>* temp = head;
        while (temp != nullptr) {
            if (temp->data == val) return;
            temp = temp->next;
        }
    }
};

// Nodo de Árbol Binario de Búsqueda (BST)
template<typename T>
class TreeNode {
public:
    T data;
    TreeNode* left;
    TreeNode* right;
    TreeNode(T val) : data(val), left(nullptr), right(nullptr) {}
};

// Árbol Binario de Búsqueda (BST)
template<typename T>
class BST {
public:
    TreeNode<T>* root;

    BST() : root(nullptr) {}

    void insert(T val) {
        root = insertRec(root, val);
    }

    void search(T val) {
        searchRec(root, val);
    }

private:
    TreeNode<T>* insertRec(TreeNode<T>* node, T val) {
        if (node == nullptr) return new TreeNode<T>(val);
        if (val < node->data) node->left = insertRec(node->left, val);
        else if (val > node->data) node->right = insertRec(node->right, val);
        return node;
    }

    TreeNode<T>* searchRec(TreeNode<T>* node, T val) {
        if (node == nullptr || node->data == val) return node;
        if (val < node->data) return searchRec(node->left, val);
        return searchRec(node->right, val);
    }
};

#endif
