#include "stringbuffer.hpp"

void StringBuffer::init() {
    lineNo = 0;
    inPosition = 0;
    eofChar = 0x7f;
    current = input[lineNo];
    readChar = current[inPosition];
}

StringBuffer::StringBuffer() {
            
}
void StringBuffer::load(vector<string>& text) {
    input = text;
    init();
}

void StringBuffer::load(string text) {
    input = { text };
    init();
}

void StringBuffer::loadFile(string filename) {
    ifstream ifile(filename);
    if (ifile.is_open()) {
        while (getline(ifile, current)) {
            input.push_back(current);
        }
        ifile.close();
    } else {
        cout<<"Couldnt open: "<<filename<<endl;
    }
    init();
}
char StringBuffer::nextChar() {
    if (inPosition+1 == current.size()) {
        inPosition = 0;
        if (lineNo+1 == input.size()) {
            readChar = eofChar;
            return eofChar;
        }
        else current = input[++lineNo];
    } else {
        inPosition++;
    }
    readChar = current[inPosition];
    return readChar;
}
int StringBuffer::lineNumber() {
    return lineNo;
}
char StringBuffer::unGet() {
    if (inPosition-1 < 0) {
        if (lineNo == 0) {
            readChar = eofChar;
            return eofChar;
        }
        else current = input[--lineNo];
        inPosition = current.size() - 1;
    } else {
        --inPosition;
    }
    readChar = current[inPosition];
    return readChar;
}
char StringBuffer::getChar() const {
    return readChar;
}
string StringBuffer::asString() {
    string ret;
    ret.push_back(getChar());
    return ret;
}
char StringBuffer::EOFMark() {
    return eofChar;
}