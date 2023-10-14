// Filename: bst.h
// 
// Header file for the class BST that represents a binary search tree
// 
// C. Seshadhri, Jan 2020

#ifndef BST_H
#define BST_H

#include <string>

using namespace std;

// node struct to hold data
class Node
{
	public:
        char* key;
	    Node *left, *right, *parent;
        int height, st_size;

        Node() // default constructor
        {
            left = right = parent = NULL; // setting everything to NULL
        }

        Node(char* val) // constructor that sets key to val
        {
            key = val;
            left = right = parent = NULL; // setting everything to NULL
            height = st_size = 1;
        }
};

class BST
{
	private:
		Node *root; // Stores root of tree
	public:
		BST(); // Default constructor sets root to null
        void insert(char*); // insert int into list 
        void rotateLeft(Node*);
        void rotateRight(Node*);
        int balance(Node*);
        void adjust_height(Node*);
        void adjust_st_size(Node*);
        void sink(Node*, Node*); // recursive version that inserts a node
        int range(char*, char*);
        char* string_substitute(char*, bool);
        char* getMax();
        char* getMin();
        int range(Node*, char*, char*);
        int range_only_low(Node*, int, char*);
        int range_only_high(Node*, int, char*);
        Node* find(char*); // find int in tree, and return pointer to node with that int. If there are multiple copies, this only finds one copy
        Node* find(Node*, char*); // recursive version that finds in a rooted subtree
        Node* minNode(Node*); // gets minimum node in rooted subtree
        Node* deleteKey(char*); // remove a node with int (if it exists), and return pointer to deleted node. This does not delete all nodes with the value.
        Node* deleteNode(Node*); // try to delete node pointed by argument. This also returns the node, isolated from the tree.
		void deleteBST(); // deletes every node to prevent memory leaks, and frees memory
        void deleteBST(Node* start); // deletes every Node in subtree rooted at startto prevent memory leaks.

        Node* lca(char*,char*); // getting the lca of two ints. Just a wrapper for real recursion function
        int width(int);
        string printLevelOrder();

};

#endif
