#define _CRT_SECURE_NO_WARNINGS
#ifndef NO_SYSTEM_HEADERS
#include <cstdio>
#endif

#define VERSION ("0.02a")

#include <typeinfo>
#include "tokenizer/Tokenizer.h"
#include "parser/Parser.h"

void changeOut(const char *file, bool inFile) {
    if (inFile) {
        string outFile = file;
        auto sep = outFile.find_last_of('.');
        outFile = outFile.substr(0, sep) + ".out";
        freopen(outFile.c_str(), "w", stdout);
    }
    else
        freopen("CON", "w", stdout);
}

void startTokenizer(char *file) {
    ifstream *stream = new ifstream();
    stream->open(file, std::ifstream::binary);
    try {
        Tokenizer tokenizer(stream);
        Token tok = tokenizer.Get();
        if (tokenizer.Eof())
            throw TokenException(0, 0, "", "Can't read file or file empty");
        do {
            tok.print();
            tok = tokenizer.Next();
        } while (!tokenizer.Eof());
    } catch(TokenException e) {
        e.print();
    }
    stream->close();
}

void startParser(char *file) {
    ifstream *stream = new ifstream();
    stream->open(file, std::ifstream::binary);
    try {
        Parser parser(stream);
        SymProcedure *main = parser.Parse();
        main->print();
    } catch (TokenException e) {
        e.print();
    } catch (SyntaxException e) {
        e.print();
    }
    stream->close();
}

void printInfo() {
    printf("Pascal compiler by TheLocky version: %s\n", VERSION);
    printf("Usage: pasc.exe -la|sa [-nologo] input.pas\n");
    printf("Parameters:\n");
    printf("-la - Lexical analysis\n");
    printf("-sa - Syntax analysis\n");
    printf("-nologo - Do not print compiler info\n");
    printf("-out - Print to file\n");
}

int main(int argc, char *argv[]) {
    int module = -1;
    bool toFile = false;
    bool printLogo = true;
    char *file = 0;
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (string(argv[i]).compare("-la") == 0) {
                if (module != -1) {
                    printInfo();
                    return 0;
                }
                module = 0;
            } else if (string(argv[i]).compare("-sa") == 0) {
                if (module != -1) {
                    printInfo();
                    return 0;
                }
                module = 1;
            } else if (string(argv[i]).compare("-nologo") == 0) {
                printLogo = false;
            } else if (string(argv[i]).compare("-test") == 0) {
                return 0;
            } else if (string(argv[i]).compare("-out") == 0) {
                toFile = true;
            } else {
                file = argv[i];
            }
        }
    } else {
        printInfo();
        return 0;
    }

    if (file != 0) {
        changeOut(file, toFile);
        switch (module) {
            case 0:
                if (printLogo)
                    printf("Pascal compiler by TheLocky version: %s\n", VERSION);
                startTokenizer(file);
                break;
            case 1:
                if (printLogo)
                    printf("Pascal compiler by TheLocky version: %s\n", VERSION);
                startParser(file);
                break;
            default:
                changeOut("", false);
                printInfo();
        }
    } else {
        printInfo();
    }
    return 0;
}