// Filename: bst.cpp
// 
// Contains the class BST that represents a binary search tree. This contains some basic operations, such as insert, delete, find, and printing in various traversal orders
// 
// C. Seshadhri, Jan 2020

#include "bst.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector> 
#include <algorithm>
#include <string>
#include <stack>

using namespace std;

// Default constructor sets head and tail to null
BST :: BST()
{
	root = NULL;
}

// Insert(string val): Inserts the string val into tree, at the head of the list. Note that there may be multiple copies of val in the list. Just calls the recursive function
// Input: string to insert into the BST
// Output: Void, just inserts new Node
void BST :: insert(char* val)
{
    if(find(val)!=NULL){
       return;
    }
    Node *to_insert = new Node(val); // create a new Node with the value val
    if (root == NULL) // tree is currently empty
        root = to_insert; // make new node the root
    else
        sink(root,to_insert); // make call to recursive insert, starting from root
    Node* temp = to_insert;
    while(temp!=root){
        if(temp->parent->st_size<= temp->st_size){
            temp->parent->st_size = temp->st_size + 1;
        }
        if(temp->parent->height <= temp->height){
            temp->parent->height = temp->height + 1;
            
        }
        temp=temp->parent;
    }
    temp = to_insert;
    // cout<<to_insert->key<< to_insert->height <<endl;
    int b_factor;
    while(temp!=root){
        temp=temp->parent;
        
        b_factor = balance(temp);
        adjust_height(temp);
        adjust_st_size(temp);
        // cout<<"balance of "<<temp->key<<": "<<b_factor<<endl;

        if(b_factor>1){
            if(balance(temp->left)>=0){
                // cout<<"case 1"<<endl;
                // cout<<printLevelOrder()<<endl;
                rotateRight(temp->left);
                // cout<<printLevelOrder()<<endl;

            }else{
                // cout<<"case 2"<<endl;
                // cout<<printLevelOrder()<<endl;
                rotateLeft(temp->left->right);
                rotateRight(temp->left);
                // cout<<printLevelOrder()<<endl;
            }
        }else if(b_factor<-1){
            if(balance(temp->right)<=0){
                // cout<<"case 3"<<endl;
                // cout<<printLevelOrder()<<endl;
                rotateLeft(temp->right);
                // cout<<printLevelOrder()<<endl;
            }else{
                // cout<<"case 4"<<endl;
                // cout<<printLevelOrder()<<endl;
                rotateRight(temp->right->left);
                rotateLeft(temp->right);
                // cout<<printLevelOrder()<<endl;
            }
        }

    }
}

void BST :: rotateLeft(Node* N){
    // cout<<"rotating "<<N->key<<" to left parent"<<endl;
    if(N->parent!=root){
        if(N->parent->parent->left==N->parent){
            N->parent->parent->left=N;
        }else{
            N->parent->parent->right=N;
        }
    }else{
        root = N;
    }
    N->parent->right = N->left;

    if(N->left) N->left->parent = N->parent;
    N->left = N->parent;
    Node* temp = N->parent->parent;
    N->parent->parent=N;
    N->parent=temp;
    adjust_height(N->left);  
    adjust_height(N);
    adjust_st_size(N->left);
    adjust_st_size(N);
    
    return;
}
void BST :: rotateRight(Node* N){
    // cout<<"rotating "<<N->key<<" to right parent"<<endl;
    if(N->parent!=root){
        if(N->parent->parent->left==N->parent){
            N->parent->parent->left=N;
        }else{
            N->parent->parent->right=N;
        }
    }else{
        root = N;
    }
    N->parent->left = N->right;
    if(N->right) N->right->parent = N->parent;
    N->right = N->parent;
    Node* temp = N->parent->parent;
    N->parent->parent=N;
    N->parent=temp;
    adjust_height(N->right); 
    adjust_height(N);
    adjust_st_size(N->right);
    adjust_st_size(N);
    return;
}

int BST :: balance(Node* N){
    int left_H, right_H;
    
    if(N->left==NULL){ left_H=0;
    }else{ left_H=N->left->height; }

    if(N->right==NULL){ right_H=0;
    }else{ right_H=N->right->height; }

    return left_H-right_H;   
}

void BST :: adjust_height(Node* N){
    int right_H, left_H;
    if(N->right==NULL){
        right_H = 0;
    }else{
        right_H = N->right->height;
    }
    if(N->left==NULL){
        left_H = 0;
    }else{
        left_H = N->left->height;
    }
        
    if(left_H>right_H){
        N->height = left_H + 1;
    }else{
        N->height = right_H + 1;
    }
    // cout<<"height works"<<endl;
}

void BST :: adjust_st_size(Node* N){
    int right_st, left_st;
    if(N->right==NULL){
        right_st = 0;
    }else{
        right_st = N->right->st_size;
    }
    if(N->left==NULL){
        left_st = 0;
    }else{
        left_st = N->left->st_size;
    }     
    N->st_size=1+left_st+right_st;
}

// insert(Node* start, Node* to_insert): Inserts the Node to_insert into tree rooted at start. We will always call with start being non-null. Note that there may be multiple copies of val in the list. 
// Input: Int to insert into the subtree
// Output: Void, just inserts new Node
void BST :: sink(Node* start, Node* to_insert)
{
    if (start == NULL) // in general, this should not happen. We never call insert from a null tree
        return;
    if (strcmp(to_insert->key, start->key)<=0) // inserted node has smaller (or equal) key, so go left
    {
        if(start->left == NULL)
        {
            start->left = to_insert; // make this node the left child
            to_insert->parent = start; // set the parent pointer
            return;
        }
        else // need to make recursive call
        {
            sink(start->left, to_insert);
            return;
        }
    }
    else // inserted node has larger key, so go right
    {
        if(start->right == NULL)
        {
            start->right = to_insert; // make this node the right child
            to_insert->parent = start; // set the parent pointer
            return;
        }
        else // need to make recursive call
        {
            sink(start->right, to_insert);
            return;
        }
    }
}

int BST :: range(char* string1, char* string2){
    if(root == NULL){
        return 0;
    }
    if(strcmp(string1, string2)==0){
        if(string1!=NULL||string2!=NULL){
            return 1;
        }else{
            return 0;
        }
    }
    char* string_low, *string_high;

    if(strcmp(string1,string2)<=0){
        string_low = string1;
        string_high = string2;
    }else{
        string_low = string2;
        string_high = string1;
    }

    if(find(string_low) == NULL){
        //cout<<string_low;
        string_low = string_substitute(string_low, false);
        //cout<<" substituted with "<<string_low<<endl;
    }
    if(find(string_high) == NULL){
      //cout<<string_high;
        string_high = string_substitute(string_high, true);
        //cout<<" substituted with "<<string_high<<endl;
    }
    Node* start = root;
    int range_size = 0;
    range_size = range(start, string_low, string_high);

    return range_size;
}

char* BST :: string_substitute(char* s_to_sub, bool high){
    Node* temp = root;
    char* s_to_return;
    char* max = getMax();
    char* min = getMin();
    if(high==true){
        s_to_return = min;
        if(strcmp(temp->key,s_to_sub)<0&&strcmp(temp->key,s_to_return)>0){ // if the current key is greater than the string to substitute and lower than the current string to return, sets string to return to the current key.
            s_to_return=temp->key;
        }
        while(!(temp->left==NULL&&temp->right==NULL)){
            if(strcmp(temp->key, s_to_sub) < 0 ){
                if(temp->right!=NULL){
                  //cout<<"go right"<<endl;
                  temp = temp->right;
                }else break;
            }else if(strcmp(temp->key, s_to_sub) > 0){
                if(temp->left!=NULL){
                  //cout<<"go left"<<endl;
                  temp = temp->left;
                }else break;
            }
            if(strcmp(temp->key,s_to_sub)<0&&strcmp(temp->key,s_to_return)>0){ // if the current key is greater than the string to substitute and lower than the current string to return, sets string to return to the current key.
                s_to_return=temp->key;
            }
        }
    }else{
        s_to_return = max;
        if(strcmp(temp->key,s_to_sub)>0&&strcmp(temp->key,s_to_return)<0){ // if the current key is greater than the string to substitute and lower than the current string to return, sets string to return to the current key.
                s_to_return=temp->key;
        }
        while(!(temp->left==NULL&&temp->right==NULL)){
          
            if(strcmp(temp->key, s_to_sub) < 0 ){
                if(temp->right!=NULL){
                  //cout<<"go right"<<endl;
                  temp = temp->right;
                }else break;
            }else if(strcmp(temp->key, s_to_sub) > 0){
                if(temp->left!=NULL){
                  temp = temp->left;
                  //cout<<"go left"<<endl;
                }else break;
            }
            if(strcmp(temp->key,s_to_sub)>0&&strcmp(temp->key,s_to_return)<0){ // if the current key is greater than the string to substitute and lower than the current string to return, sets string to return to the current key.
                s_to_return=temp->key;
            }
        }
    }
    return s_to_return;
}

char* BST :: getMax(){
    Node* temp = root;
    while(temp->right!=NULL){
        temp=temp->right;
    }
    return temp->key;
}

char* BST :: getMin(){
    Node* temp = root;
    while(temp->left!=NULL){
        temp=temp->left;
    }
    return temp->key;
}


int BST :: range(Node* start, char* string_low, char* string_high){
    
    if(strcmp(start->key,string_low)<0 && strcmp(start->key,string_high)<0){

        return range(start->right, string_low, string_high);  
    }else if (strcmp(start->key,string_low)>0&&strcmp(start->key,string_high)>0){
    
        return range(start->left, string_low, string_high);
    }else if (strcmp(start->key,string_low)>=0 && strcmp(start->key,string_high)<=0){
        int low, high;
        if(start->left==NULL){
            low = 0;
        }else if(strcmp(start->key, string_low)==0){
            low = 0;
        }else{
            
            //cout<<start->left->key<<endl;
            low = range_only_low(start->left, 0, string_low);
            
        }
        if(start->right==NULL){
          
            high = 0;
        }else if(strcmp(start->key, string_high)==0){
            high = 0;
        }else{
            //cout<<start->right->key<<endl;
            high = range_only_high(start->right, 0, string_high);
        }
        //cout<< low << " " << high <<endl;
        return low + high + 1;
    }else{
      return 0;
    }
}

int BST :: range_only_low(Node* start, int total, char* string_low) {
    if(strcmp(start->key,string_low)<0){
        return range_only_low(start->right, total, string_low);
    }
    if(strcmp(start->key,string_low)>0){
        int add_on = total+1;
        if(start->right!=NULL){
            add_on = add_on+start->right->st_size;
        }
        //cout<<"+"<<add_on<<endl;
        return range_only_low(start->left, add_on, string_low);
    }
    if(strcmp(start->key,string_low)==0){
        int right_st_size;
        if(start->right==NULL){
            right_st_size = 0;
        }else{
            right_st_size = start->right->st_size;
        }
        //cout<<1+right_st_size+total<<endl;
        return 1+right_st_size+total;
    }
    return 0;
}

int BST :: range_only_high(Node* start, int total, char* string_high) {
    if(strcmp(start->key,string_high)>0){
        return range_only_high(start->left, total, string_high);
    }
    if(strcmp(start->key,string_high)<0){
        int add_on = total+1;
        if(start->left!=NULL){
            add_on = add_on+start->left->st_size;
        }
        return range_only_high(start->right, add_on, string_high);
    }
    if(strcmp(start->key,string_high)==0){
        int left_st_size;
        if(start->left==NULL){
            left_st_size = 0;
        }else{
            left_st_size = start->left->st_size;
        }
        return 1+left_st_size+total;
    }
    return 0;
}

// find(int val): Finds a Node with key "val"
// Input: int to be found
// Output: a pointer to a Node containing val, if it exists. Otherwise, it returns NULL
// Technically, it finds the first Node with val, at it traverses down the tree
Node* BST :: find(char* val)
{
    return find(root, val); // call the recursive function starting at root
}

// find(Node* start, int val): Recursively tries to find a Node with key "val", in subtree rooted at val
// Input: int to be found
// Output: a pointer to a Node containing val, if it exists. Otherwise, it returns NULL
// Technically, it finds the first Node with val, at it traverses down the tree
Node* BST :: find(Node* start, char* val)
{
    if (start == NULL || strcmp(start->key, val)==0) // tree is empty or we found val
        return start;
    if(strcmp(val, start->key)<0) // val is smaller, so go left
        return find(start->left, val);
    else // val is larger, so go right
        return find(start->right, val);
}

// minNode(Node* start): gets the minimum Node in subtree rooted at start
// Input: Pointer to subtree root
// Output: pointer to the minimum node in the subtree
Node* BST :: minNode(Node* start)
{
    if(start == NULL) // typically, this should not happen. But let's return the safe thing
        return NULL;
    if(start->left == NULL) // Base case: we have found the minimum
        return start;
    else
        return minNode(start->left); // recursive call in left subtree
}

// deleteNode(int val): Delete a Node with key val, if it exists. Otherwise, do nothing.
// Input: int to be removed
// Output: pointer to Node that was deleted. If no Node is deleted, return NULL. If there are multiple Nodes with val, only the first Node in the list is deleted.
Node* BST :: deleteKey(char* val)
{
    return deleteNode(find(val)); // get a node with the value and delete that node
}

// deleteNode(Node* to_delete): Delete the input node, and return pointer to the deleted node. The node will be isolated from the tree, to prevent memory leaks
// Input: Node to be removed
// Output: pointer to Node that was deleted. If no Node is deleted, return NULL. 
Node* BST :: deleteNode(Node* to_delete)
{
    if(to_delete == NULL) // val not present in tree, so return NULL
        return NULL;

    bool isRoot = (to_delete == root) ? true : false; // determine if node to delete is root
    bool isLeftChild = false;
    if (!isRoot) // if this is not the root
        isLeftChild = (to_delete->parent->left == to_delete) ? true : false; // determine if node is left child of parent. Note that line throws error iff to_delete is root

    bool isDeleted = false; // convenient flag for writing code
    
    // if to_delete's left child is NULL, then we can splice this node off. We set the appropriate
    // pointer of the parent to the right child of to_delete
    if(to_delete->left == NULL)
    {
        if(isRoot) // if deleting root, then we reset root
        {
            root = to_delete->right;
            if(root != NULL)
                root->parent = NULL; // set parent to be NULL
        }
        else
        {
            if(isLeftChild) // node is left child of parent
                to_delete->parent->left = to_delete->right; // setting left child of parent to be right child of node
            else // node is right child of parent
                to_delete->parent->right = to_delete->right; // setting right child of parent to be right child of node
            if(to_delete->right != NULL) // to_delete is not a leaf
                to_delete->right->parent = to_delete->parent; // update parent of the child of the deleted node, to be parent of deleted node
        }
        isDeleted = true; // delete is done
    }
    // suppose node is not deleted yet, and it's right child is NULL. We splice off as before, by setting parent's child pointer to to_delete->left
    if(!isDeleted && to_delete->right == NULL) 
    {
        if(isRoot) // if deleting root, then we reset root
        {
            root = to_delete->left;
            if(root != NULL)
                root->parent = NULL; // set parent to be NULL
        }
        else
        {
            if(isLeftChild) // node is left child of parent
                to_delete->parent->left = to_delete->left; // setting left child of parent to be left child of node
            else // node is right child of parent
                to_delete->parent->right = to_delete->left; // setting right child of parent to be left child of node
            if(to_delete->left != NULL) // to delete is not a leaf
                to_delete->left->parent = to_delete->parent; // update parent of the child of deleted node, to be parent of deleted node
        }
        isDeleted = true; // delete is done
    }
    if(isDeleted) // so node has been deleted
    {
        to_delete->left = to_delete->right = NULL;
        return to_delete;
    }
    
    // phew. The splicing case is done, so now for the recursive case. Both children of to_delete are not null, so we replace the data in to_delete by the successor. Then we delete the successor node
    // first, get the minimum node of right subtree
    Node* succ = minNode(to_delete->right);
    to_delete->key = succ->key;
//     cout << "Replacing with "+to_string(succ->key) << endl;
    return deleteNode(succ); // make recursive call on succ. Note that succ has one null child, so this recursive call will terminate without any other recursive calls
}


// Deletes every Node to prevent memory leaks.
// Input: None
// Output: Void, just deletes every Node of the list
void BST :: deleteBST()
{
    deleteBST(root);
}

// Deletes every Node in subtree rooted at startto prevent memory leaks.
// Input: Node* start
// Output: Void, just deletes every Node of the list
void BST :: deleteBST(Node* start)
{
    if(start == NULL) // tree is already empty
        return; 
    deleteBST(start->left); // delete left subtree
    deleteBST(start->right); // delete right subtree
    delete(start); // delete node itself
}





string BST :: printLevelOrder()

{
    if(!root) return 0;
    string output = "";
    Node* temp = root;
    stack<Node*> recurStack, tempStack;
    recurStack.push(temp);
    int level = 1;
    while(!recurStack.empty()&&level<=6){
        while(!recurStack.empty()){
            temp = recurStack.top();
            cout<<temp->key<<" ("<<to_string(temp->height)<<","<<to_string(temp->st_size)<<") (";
            // output = output + temp->key + " " + to_string(temp->height) + " " + to_string(temp->st_size)+" ";

            if(temp->left){ tempStack.push(temp->left); cout<<temp->left->key;}
            cout<<",";
            if(temp->right){ tempStack.push(temp->right); cout<<temp->right->key;}
            cout<<") ";
            recurStack.pop();
            // output = output +")";
        }
        cout<<endl;
        // output = output +"\n";
        while(!tempStack.empty()){   
            recurStack.push(tempStack.top());
            tempStack.pop();
        }
        level++;
        
    }
    return output;
}



