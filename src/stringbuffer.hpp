#ifndef stringbuffer_hpp
#define stringbuffer_hpp
#include <iostream>
#include <fstream>
#include "stream_iface.hpp"
using namespace std;


class StringBuffer : public Stream<char> {
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
            return spos >= buff.size();
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
        char& get() {
            if (spos >= buff.length() && lpos >= lines.size()) {
                    return eosChar; 
            }
            return buff[spos];
        }
        void skipLine() {
            lpos++;
            spos = 0;
            buff = lines[lpos];
        }
        char& advance() {
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
        bool reverse() {
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
};


class FileBuffer {
    private:
        
    public:
        FileBuffer() {

        }
        vector<string> readFile(string filename) {
            vector<string> lines;
            fstream ifile(filename);
            if (!ifile.is_open()) {
                cout<<"Error: couldn't open '"<<filename<<"' for reading."<<endl;
                return lines;
            }
            string input;
            while (ifile.good()) {
                getline(ifile, input);
                lines.push_back(input);
            }
            return lines;
        }
};

#endif