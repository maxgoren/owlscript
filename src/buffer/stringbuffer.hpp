#ifndef stringbuffer_hpp
#define stringbuffer_hpp
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
using namespace std;

class StringBuffer {
    private:
        string current;
        vector<string> input;
        int inPosition;
        int lineNo;
        char readChar;
        char eofChar;
        void init();
    public:
        StringBuffer();
        void load(vector<string>& text);
        void load(string text);
        void loadFile(string filename);
        char nextChar();
        int lineNumber();
        char unGet();
        char getChar() const;
        string asString();
        char EOFMark();
};

#endif