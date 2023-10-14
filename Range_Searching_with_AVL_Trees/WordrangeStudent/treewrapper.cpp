// Filename: treewrapper.cpp
//
// This is the main wrapper for the BST class.
// 
// After running make, the usage is:
//     ./treewrapper <INPUT_FILE> <OUTPUT_FILE>
// 
// The input file contains a list of operations performed on a BST. The operations are insert, delete, find, printing in/pre/post order 
// Each line is of the form:
// 
//         i <INT>: insert <INT> into BST
//         f <INT>: find if <INT> is in BST
//         d <INT>: delete <INT> from BST
//         pin: print tree in order
//         ppre: print tree pre-order
//         ppost: print tree post-order
// 
// The find and print operations generate output, which is either "found/didn't find <INT>" or the tree printed in the desired order.
// 
// The output file contains the output of all commands that lead to output.
// 
// In addition, the console output shows the operations performed, and also prints the tree (for every print command).
// 
// C. Seshadhri, Jan 2020

#include <iostream>
#include "bst.h"
#include <stack>
#include <stdexcept>
#include <fstream>
#include <array>
#include <vector> 
#include <algorithm>
#include <sstream>
#include <cstring>
using namespace std;


int main(int argc, char** argv)
{
    if (argc < 3) // must provide two arguments as input
    {
        throw std::invalid_argument("Usage: ./treewrapper <INPUT FILE> <OUTPUT FILE>"); // throw error
    }

    ifstream input; // stream for input file
    ofstream output; // stream for output file

    input.open(argv[1]); // open input file
    output.open(argv[2]); // open output file

    string command; // to store the next command and operation
    char *com, *op; // for using with strtok, strtol
    char* val; // the value from the command
    char* val2;

    BST myBST, myBST2; // initializing the linked list
        

    while(getline(input,command)) // get next line of input, store as repeat
    {
        if (command.length() == 0) // command is empty
            continue;
        com = strdup(command.c_str()); // annoying, first copy string into a "C-style" string
        op = strtok(com, " \t"); //tokenize command on whitespace, first token is operation

        val = strtok(NULL, " \t"); // next token is value, as string (check out documentation on strtok, to understand the parsing)
        val2 = strtok(NULL, " \t"); // next token is value, as string (check out documentation on strtok, to understand the parsing)

        if(strcmp(op,"i") == 0) // insert into list
        {
            cout << "Insert "<<val << endl;
            myBST.insert(val);
        }
        if(strcmp(op,"d") == 0) // delete from list
        {
            cout << "Delete "<<val << endl;
            free(myBST.deleteKey(val)); // delete the value, and free the memory
        }

        if(strcmp(op,"f") == 0) // delete from list
        {
            string message;
            if(myBST.find(val)) // yes, val is found in BST
                message = " found";
            else
                message = " not found";
            cout << val<< message << endl;
            output << val<< message << endl;
        }
        if(strcmp(op,"r") == 0)
        {
            //cout << "Words in range: "<< val << " to " << val2 << endl;
            cout << myBST.range(val, val2) << endl; // printing to console
            continue; // move on to next command
        }



        if(strcmp(op,"lca") == 0)
        {
            cout << "Least Common Ancestor "<< val << " " << val2 << endl;
            cout << myBST.lca(val, val2)->key << endl; // printing to console
            continue; // move on to next command
        }

        if(strcmp(op,"w") == 0)
        {
            cout << "Width at height "<< to_string(4) << endl;
            cout << to_string(myBST.width(4)) << endl; // printing to console
            continue; // move on to next command
        }


        if(strcmp(op,"plo") == 0) // print tree in order
        {
            output << myBST.printLevelOrder() << endl; // write printed tree to file, and add new line
            cout << "Print Level Order" << endl;
            cout << myBST.printLevelOrder() << endl; // printing to console
            continue; // move on to next command
        }

     }

     myBST.deleteBST(); // delete the tree from memory
     input.close();
     output.close();


}
