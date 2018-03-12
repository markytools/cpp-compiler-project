#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <regex>
#include "trieset.h"

using namespace std;

//This function checks for keywords for the Lexical Analyzer. It traverses a Trie wherein it is going to check the characters of every
//keyword if it is inside of it. If the characters are found, then the &keyword will store the values of these characters up until the Trie
//ends up in a node where the @isString boolean variable returns true. Once this happens, it first checks if the non-alphanumeric regex
//matches the next character, to avoid having an identifier. If it's not, the keyword then gets to be stored into the list of
//keywords which is a vector object @keywords.
//
//
//It returns the next node to be scanned.
Node *checkForKeyword(TrieSet *keywordTrie, Node *currentNode, vector<string> *keywords, string &keyword, char c, bool hasMaskwords) {
    if (hasMaskwords) {
        keyword = "";
    }
    else {
        string charStr(1, c);
        regex nonAlphaNumRegex("[^a-zA-Z0-9]");
        if (currentNode->isString && regex_match(charStr, nonAlphaNumRegex)) {
            keywords->push_back(string(keyword));
            keyword = "";
        }
        if (currentNode->children[c] != NULL) {
            keyword.append(charStr);
            return currentNode->children[c];
        }
        else {
            keyword = "";
            if (keywordTrie->getRoot()->children[c] != NULL) {
                keyword.append(charStr);
                return keywordTrie->getRoot()->children[c];
            }
        }
    }

    return keywordTrie->getRoot();
}

//This function checks if a character corresponds to an alphabet letter. If it is, it then checks if the next character is an alphanumeric
//character for it to be a valid identifier. Once it gets to a regex that does not match, the remaining @identifier are to be checked
//if it does not collide with a keyword. If it does, then it won't add it to list of identifiers, otherwise it's going to be added to the list.
string checkForIdentifier(vector<string> *identifiers, string &identifier, char c, bool &isKeyword, bool &isLiteral, bool hasMaskwords) {
    if (hasMaskwords) {
        identifier = "";
    }
    else {
        string charStr(1, c);
        if (identifier.length() == 0) {
            regex alphaRegex("[a-zA-Z]");
            if (regex_match(charStr, alphaRegex)) identifier.append(charStr);
        }
        else if (identifier.length() > 0) {
            regex alphaNumRegex("[a-zA-Z0-9]");
            if (regex_match(charStr, alphaNumRegex)) identifier.append(charStr);
            else {
                if (!isKeyword && !isLiteral) {
                    identifiers->push_back(string(identifier));
                }
                isKeyword = false;
                isLiteral = false;
                identifier = "";
            }
        }
    }
    return identifier;
}

//A trie is still going to be used to check if the characters of a literal manage to match the characters of an input file. A boolean variable
//called @singleQuoteLiteral or @doubleQuoteLiteral is activated when the current char is inside a single quotation or double quotation state,
//respectively.
Node *checkForLiteral(TrieSet *literalTrie, Node *currentNode, vector<string> *literals, string &literal, char c, char prevChar, bool &insideSingleQuotes,
                      bool &insideDoubleQuotes, bool &singleQuoteLiteral, bool &doubleQuoteLiteral, bool &isCheckingNumber,
                      bool firstCommentActivated, bool secondCommentActivated) {
    if (firstCommentActivated || secondCommentActivated) {
        literal = "";
    }
    else {
        string charStr(1, c);
        string prevCharStr(1, prevChar);
        regex numRegex("[0-9]");
        regex alphaRegex("[a-zA-Z]");
        if (singleQuoteLiteral) {
            if (!insideSingleQuotes) {
                singleQuoteLiteral = false;
                literals->push_back("\'" + literal);
                literal = "";
                if (literalTrie->getRoot()->children[c] != NULL) {
                    literal.append(charStr);
                    return literalTrie->getRoot()->children[c];
                }
            }
            else literal.append(charStr);
        }
        else if (doubleQuoteLiteral) {
            if (!insideDoubleQuotes) {
                doubleQuoteLiteral = false;
                literals->push_back("\"" + literal);
                literal = "";
                if (literalTrie->getRoot()->children[c] != NULL) {
                    literal.append(charStr);
                    return literalTrie->getRoot()->children[c];
                }
            }
            else literal.append(charStr);
        }
        else if (literal.empty() && !regex_match(prevCharStr, alphaRegex) && regex_match(charStr, numRegex)) {
            isCheckingNumber = true;
            literal.append(charStr);
        }
        else if (isCheckingNumber) {
            if (regex_match(charStr, numRegex)) literal.append(charStr);
            else {
                isCheckingNumber = false;
                literals->push_back(literal);
                literal = "";
                if (literalTrie->getRoot()->children[c] != NULL) {
                    literal.append(charStr);
                    return literalTrie->getRoot()->children[c];
                }
            }
        }
        else {
            regex nonAlphaNumRegex("[^a-zA-Z0-9]");
            if (currentNode->isString && regex_match(charStr, nonAlphaNumRegex)) {
                literals->push_back(string(literal));
                literal = "";
            }
            if (currentNode->children[c] != NULL) {
                literal.append(charStr);
                return currentNode->children[c];
            }
            else {
                literal = "";
                if (literalTrie->getRoot()->children[c] != NULL) {
                    literal.append(charStr);
                    return literalTrie->getRoot()->children[c];
                }
            }
        }
    }
    return literalTrie->getRoot();
}

//The separators are distinguished by the characters identified as such. They are compared using the 'or' condition, as there are only a few
//characters that are regarded as such
void checkForSeparator(vector<string> *separator, char c, bool hasMaskwords) {
    string charStr(1, c);
    if (!hasMaskwords)
        if (c == '(' || c == ')' || c == '{' || c == '}' || c == ';' || c == '[' || c == ']') separator->push_back(charStr);
}


void pushOperatorCharacters(vector<string> *operators, char c, string &operatorStr, string charStr) {
    if (c == '%' || c == '*' || c == '/' || c == '*') {
        operatorStr += charStr;
        operators->push_back(operatorStr);
    }
    else if (c == '+' || c == '-' || c == '<' || c == '>' || c == '=' || c == '!' || c == '&' || c == '|') {
        operatorStr += charStr;
    }
}

bool performMultipleOperator(string firstChar, char secondChar, vector<string> *operators, string &operatorStr, char c, string charStr, string prevCharStr) {
    if (operatorStr == firstChar) {
        if (c == secondChar) {
            operatorStr += charStr;
            operators->push_back(string(operatorStr));
            operatorStr = "";
            return true;
        }
    }
    return false;
}

void checkForOperator(vector<string> *operators, string &operatorStr, char c, char prevChar, bool hasMaskwords) {
    if (hasMaskwords) {
        operatorStr = "";
    }
    else {
        string charStr(1, c);
        string prevCharStr(1, prevChar);
        if (operatorStr == "") {
            pushOperatorCharacters(operators, c, operatorStr, charStr);
        }
        else {
            if (performMultipleOperator("+", '+', operators, operatorStr, c, charStr, prevCharStr)) return;
            if (performMultipleOperator("-", '-', operators, operatorStr, c, charStr, prevCharStr)) return;
            if (performMultipleOperator("<", '<', operators, operatorStr, c, charStr, prevCharStr)) return;
            if (performMultipleOperator(">", '>', operators, operatorStr, c, charStr, prevCharStr)) return;
            if (performMultipleOperator("=", '=', operators, operatorStr, c, charStr, prevCharStr)) return;
            if (performMultipleOperator("!", '=', operators, operatorStr, c, charStr, prevCharStr)) return;
            if (performMultipleOperator("&", '&', operators, operatorStr, c, charStr, prevCharStr)) return;
            if (performMultipleOperator("|", '|', operators, operatorStr, c, charStr, prevCharStr)) return;
            if (performMultipleOperator("<", '=', operators, operatorStr, c, charStr, prevCharStr)) return;
            if (performMultipleOperator(">", '=', operators, operatorStr, c, charStr, prevCharStr)) return;

            operatorStr = "";
            operators->push_back(string(prevCharStr));
            pushOperatorCharacters(operators, c, operatorStr, charStr);
        }
    }
}

void checkForComments(bool &firstCommentActivated, bool &secondCommentActivated, vector<string> *comments, string &comment, char c,
                      char prevChar, bool &insideSingleQuotes, bool &insideDoubleQuotes) {
    if (insideSingleQuotes || insideDoubleQuotes) {
        comment = "";
    }
    else {
        string charStr(1, c);
        if (firstCommentActivated && c == 10) firstCommentActivated = false;
        if (secondCommentActivated && prevChar == '*' && c == '/') {
            secondCommentActivated = false;
            comment += "/";
        }
        if (firstCommentActivated || secondCommentActivated) comment += charStr;
        else {
            if (comment != "") {
                comments->push_back(string(comment));
                comment = "";
            }
        }
        if (prevChar == '/' && c == '/') {
            comment += "//";
            firstCommentActivated = true;
        }
        if (prevChar == '/' && c == '*') {
            comment += "/*";
            secondCommentActivated = true;
        }
    }
}

int main(int argc, char *argv[])
{
    vector<string> *keywords = new vector<string>();
    vector<string> *identifiers = new vector<string>();
    vector<string> *literals = new vector<string>();
    vector<string> *operators = new vector<string>();
    vector<string> *separator = new vector<string>();
    vector<string> *comments = new vector<string>();

    TrieSet *keywordTrie = new TrieSet();
    keywordTrie->insert("#include");
    keywordTrie->insert("namespace");
    keywordTrie->insert("using");
    keywordTrie->insert("unsigned");
    keywordTrie->insert("double");
    keywordTrie->insert("float");
    keywordTrie->insert("int");
    keywordTrie->insert("if");
    keywordTrie->insert("std");
    keywordTrie->insert("string");
    keywordTrie->insert("bool");
    keywordTrie->insert("char");
    keywordTrie->insert("while");
    keywordTrie->insert("return");
    keywordTrie->insert("void");
    keywordTrie->insert("else");
    keywordTrie->insert("auto");
    keywordTrie->insert("break");
    keywordTrie->insert("case");
    keywordTrie->insert("const");
    keywordTrie->insert("continue");
    keywordTrie->insert("default");
    keywordTrie->insert("do");
    keywordTrie->insert("enum");
    keywordTrie->insert("extern");
    keywordTrie->insert("goto");
    keywordTrie->insert("register");
    keywordTrie->insert("short");
    keywordTrie->insert("signed");
    keywordTrie->insert("sizeof");
    keywordTrie->insert("static");
    keywordTrie->insert("struct");
    keywordTrie->insert("typedef");
    keywordTrie->insert("union");
    keywordTrie->insert("volatile");
    keywordTrie->insert("unsigned");

    TrieSet *literalTrie = new TrieSet();
    literalTrie->insert("false");
    literalTrie->insert("true");
    literalTrie->insert("NULL");

    char ch;
    Node *currentKeywordTrieNode = keywordTrie->getRoot();
    Node *currentLiteralTrieNode = literalTrie->getRoot();
    string keyword = "";
    string identifier = "";
    string literal = "";
    string operatorStr = "";
    string comment = "";

    char prevChar = 256;
    bool insideSingleQuotes = false;
    bool insideDoubleQuotes = false;
    bool singleQuoteLiteral = false;
    bool doubleQuoteLiteral = false;
    bool isCheckingNumber = false;
    bool isKeyword = false;
    bool isLiteral = false;
    bool firstCommentActivated = false;
    bool secondCommentActivated = false;
    fstream fin(string(argv[1]), fstream::in);
    while (fin >> noskipws >> ch) {
        cout << ch; // Or whatever
        bool hasMaskwords = (insideSingleQuotes || insideDoubleQuotes || firstCommentActivated || secondCommentActivated);
        currentKeywordTrieNode = checkForKeyword(keywordTrie, currentKeywordTrieNode, keywords, keyword, ch, hasMaskwords);
        identifier = checkForIdentifier(identifiers, identifier, ch, isKeyword, isLiteral, hasMaskwords);
        currentLiteralTrieNode = checkForLiteral(literalTrie, currentLiteralTrieNode, literals, literal, ch, prevChar, insideSingleQuotes,
                                                 insideDoubleQuotes, singleQuoteLiteral, doubleQuoteLiteral, isCheckingNumber,
                                                 firstCommentActivated, secondCommentActivated);
        if (currentKeywordTrieNode->isString) isKeyword = true;
        if (currentLiteralTrieNode->isString) isLiteral = true;
        if (!firstCommentActivated && !secondCommentActivated) {
            if (ch == '\'') {
                insideSingleQuotes = !insideSingleQuotes;
                if (insideSingleQuotes) singleQuoteLiteral = true;
            }
            if (ch == '"') {
                insideDoubleQuotes = !insideDoubleQuotes;
                if (insideDoubleQuotes) doubleQuoteLiteral = true;
            }
        }
        checkForSeparator(separator, ch, hasMaskwords);
        checkForOperator(operators, operatorStr, ch, prevChar, hasMaskwords);
        checkForComments(firstCommentActivated, secondCommentActivated, comments, comment, ch, prevChar, insideSingleQuotes, insideDoubleQuotes);
        prevChar = ch;
    }
    fin.close();
    cout << endl;


    cout << "Keywords: ";
    for (unsigned int i = 0; i < keywords->size(); i++) cout << keywords->at(i) << ", ";
    cout << endl;

    cout << "Identifiers: ";
    for (unsigned int i = 0; i < identifiers->size(); i++) cout << identifiers->at(i) << ", ";
    cout << endl;

    cout << "Literals: ";
    for (unsigned int i = 0; i < literals->size(); i++) cout << literals->at(i) << ", ";
    cout << endl;

    cout << "Operators: ";
    for (unsigned int i = 0; i < operators->size(); i++) cout << operators->at(i) << ", ";
    cout << endl;

    cout << "Separators: ";
    for (unsigned int i = 0; i < separator->size(); i++) cout << separator->at(i) << ", ";
    cout << endl;

    cout << "Comments: ";
    for (unsigned int i = 0; i < comments->size(); i++) cout << comments->at(i) << ", " << endl;
    cout << endl;
}
