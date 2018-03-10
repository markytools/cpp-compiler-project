#include "trieset.h"

int TrieSet::getSize() const
{
    return size;
}

Node *TrieSet::getRoot() const
{
    return root;
}

void TrieSet::insertNode(Node *node, string word, int ind)
{
    if (ind > word.length() - 1) {
        return;
    }
    if (node->children[word[ind]] == NULL)
    {
        Node *newNode = new Node();
        node->children[word[ind]] = newNode;
        if (ind == word.length() - 1) {
            newNode->isString = true;
            size++;
        }
        insertNode(newNode, word, ind+1);
    }
    else {
        if (ind == word.length() - 1) {
            if (!node->children[word[ind]]->isString) size++;
            node->children[word[ind]]->isString = true;
            return;
        }
        Node *childNode = node->children[word[ind]];
        insertNode(childNode, word, ind+1);
    }
}

TrieSet::TrieSet()
{
    root = new Node();
}

void TrieSet::insert(string word)
{
    insertNode(root, word, 0);
}

void TrieSet::print()
{
    root->print(200);
}

Node *TrieSet::getChildNode(Node *node, char c)
{
    return node->children[c];
}

Node::Node()
{
    for (int i = 0; i < NODE_SIZE; i++)
    {
        children[i] = NULL;
    }
}

void Node::print(int parentNodeStr)
{
    for (int i = 0; i < NODE_SIZE; i++)
    {
        if (children[i] != NULL) {
            cout << "Child Node:" << i << endl;
        }
    }
    cout << endl;
//    for (int i = 0; i < NODE_SIZE; i++)
//    {
//        if (children[i] != NULL) {
//            children[i]->print(i);
//        }
//    }
}
