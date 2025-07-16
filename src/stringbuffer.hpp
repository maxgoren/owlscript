#ifndef stringbuffer_hpp
#define stringbuffer_hpp
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

class StringBuffer {
private:
        vector<string> lines;
        char eosChar;
        string buff;
        int spos;
        int lpos;
    public:
        StringBuffer() {
            eosChar = 0x70;
        }
        bool done() {
            return lpos >= lines.size();
        }
        void init(vector<string> lns) {
            lines.clear();
            lines = lns;
            spos = 0; 
            lpos = 0;
            buff = lines[lpos];
        }
        void init(string line) {
            lines.clear();
            lines.push_back(line);
            spos = 0;
            lpos = 0;
            buff = lines[lpos];
        }
        int lineNo() {
            return lpos;
        }
        
        bool rewind() {
            if (spos-1 < 0 && lpos-1 < 0)
                return eosChar;
            spos--;
            if (spos < 0) {
                lpos--;
                buff = lines[lpos];
                spos = buff.length()-1;
            }
            return buff[spos];
        }
        void readFromFile(string filename) {
            lines.clear();
            ifstream ifile(filename, ios::in);
            if (!ifile.is_open()) {
                cout<<"Error: couldn't open "<<filename<<endl;
            }
            while (ifile.good()) {
                getline(ifile, buff);
                lines.push_back(buff);
            }
            ifile.close();
            spos = 0; 
            lpos = 0;
            buff = lines[lpos];
        }
        char get() {
            if (spos >= buff.length() && lpos >= lines.size()) {
                    return eosChar; 
            }
            return buff[spos];
        }
        char peek(int spaces) {
            if (spos+spaces >= buff.length() && lpos >= lines.size())
                return eosChar;
            char ret;
            for (int i = 0; i < spaces; i++)
                advance();
            ret = get();
            for (int i = 0; i < spaces; i++)
                rewind();
            return ret;
        }
        char advance() {
            spos++;
            if (spos >= buff.length()) {
                lpos++;
                if (lpos >= lines.size()) {
                    return eosChar;
                }
                spos = 0;
                buff = lines[lpos];
            }
            return buff[spos];
        }
        const char* currentLine() const {
            return lines[lpos].data();
        }
        int position() {
            return spos;
        }
        string substring(int start, int len) {
            return lines[lpos].substr(start, len);
        }
        void makeCurrentStart() {
            lines[lpos] = lines[lpos].substr(spos);
        }
        vector<string> getLines() {
            return lines;
        }
};

#endif