#include <iostream>
#include <cstring>
#include <vector>
#include <map>
#include "compile/compiler.hpp"
#include "vm/vm.hpp"
using namespace std;

#define MAJOR_VER     0
#define MINOR_VER    88
#define RELEASE_VER  'a'

void printVersion() {
    printf("Owlscript v%d.%d%c, (c) 2026 MaxGCoding.com\n", MAJOR_VER, MINOR_VER, RELEASE_VER);
}

void initStdLib(Compiler& compiler, VM& vm) {
    FileStringBuffer* fb = new FileStringBuffer();
    fb->readFile("/usr/local/bin/vm/stdlib.owl");
    auto code = compiler.compile(fb);
    vm.setConstPool(compiler.getConstPool());
    vm.run(code, 0);
}

void compileAndRun(CharBuffer* buff, int verbosity) {
    VM vm;
    Compiler compiler(verbosity);
    //initStdLib(compiler, vm);
    vector<Instruction> code = compiler.compile(buff);
    vm.setConstPool(compiler.getConstPool());
    vm.run(code, verbosity);
}

void runScript(string filename, int verbosity) {
    FileStringBuffer* fb = new FileStringBuffer();
    fb->readFile(filename);
    compileAndRun(fb, verbosity);
}

void runCommand(string cmd, int verbosity) {
    cout<< "Running: "<<cmd<<endl;
    StringBuffer* sb = new StringBuffer();
    sb->init(cmd);
    compileAndRun(sb, verbosity);
}

void repl(int vb) {
    bool looping = true;
    StringBuffer* sb = new StringBuffer();
    Compiler compiler(vb);
    VM vm;
    //initStdLib(compiler, vm);
    unsigned int lno = 0;
    printVersion();
    while (looping) {
        string input;
        cout<<"Owlscript("<<lno++<<")> ";
        getline(cin, input);
        sb->init(input);
        vector<Instruction> code = compiler.compile(sb);
        vm.setConstPool(compiler.getConstPool());
        vm.run(code, vb);
    }
}

int verbosityLevel(char *str) {
    int vlev = 0;
    for (char *x = str; *x; x++)
        if (*x == 'v')
            vlev++;
    return vlev;
}

int main(int argc, char* argv[]) {
    srand(time(0));
    switch (argc) {
        case 1: repl(0); break;
        case 2: repl(verbosityLevel(argv[1]));
        default:
            if (argc == 3 && argv[1][0] == '-') {
                switch (argv[1][1]) {
                    case 'e': runCommand(argv[2], verbosityLevel(argv[1])); break;
                    case 'f': runScript(argv[2], verbosityLevel(argv[1])); break;
                    default: break;
                }
            }
    }
    return 0;
}