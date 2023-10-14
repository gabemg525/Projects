// Filename: wordrange.cpp

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

    BST myBST; // initializing the linked list
        

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
            myBST.insert(val);
        }

        if(strcmp(op,"r") == 0)
        {
            output << myBST.range(val, val2) << endl; // printing to console
            continue; // move on to next command
        }

     }

     myBST.deleteBST(); // delete the tree from memory
     input.close();
     output.close();


}
