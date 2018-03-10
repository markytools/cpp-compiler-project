#ifndef TRIESET_H
#define TRIESET_H
#include <iostream>

using namespace std;

class Node
{
private:
    const static unsigned int NODE_SIZE = 256;

public:
    Node();
    Node *children[NODE_SIZE];
    bool isString = false;
    void print(int parentNodeStr);
};

class TrieSet
{
private:
    int size = 0;
    Node *root;
    void insertNode(Node *node, string word, int ind);

public:
    TrieSet();
    void insert(string word);   //Inserts the characters of a word into the trie
    void print();
    int getSize() const;
    Node *getChildNode(Node *node, char c);
    Node *getRoot() const;
};

#endif // TRIESET_H
