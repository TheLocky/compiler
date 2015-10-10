#ifndef NO_SYSTEM_HEADERS
#include <cstdio>
#endif

#define VERSION ("0.02a")
#define OUT_TO_FILE

#include "tokenizer/Tokenizer.h"
#include "parser/Parser.h"

void changeOut(const char *file, bool inFile) {
#ifdef OUT_TO_FILE
    if (inFile)
        freopen(file, "w", stdout);
    else
        freopen("CON", "w", stdout);
#endif
}

void startTokenizer(char *file) {

    string outFile = string("out_") + file;
    changeOut(outFile.c_str(), true);

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

    changeOut("", false);
}

void startParser(char *file) {
    string outFile = string("out_") + file;
    changeOut(outFile.c_str(), true);

    ifstream *stream = new ifstream();
    stream->open(file, std::ifstream::binary);
    try {
        Parser parser(stream);
        Expression *expr = parser.ParseExpr();
        expr->print("");
    } catch (TokenException e) {
        e.print();
    } catch (SyntaxException e) {
        e.print();
    }
    stream->close();

    changeOut("", false);
}

void printInfo() {
    printf("Pascal compiler by TheLocky version: %s\n", VERSION);
    printf("Usage: pasc.exe -la|sa [-nologo] input.pas\n");
    printf("Parameters:\n");
    printf("-la - Lexical analysis\n");
    printf("-sa - Syntax analysis\n");
    printf("-nologo - Do not print compiler info\n");
}

int main(int argc, char *argv[]) {
    int module = -1;
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
                ifstream f("test.txt");
                printf("%c\n",(char)f.get());
                printf(f.eof() ? "true\n" : "false\n");
                f.get();
                printf(f.eof() ? "true\n" : "false\n");
                f.unget();
                f.get();
                printf(f.eof() ? "true\n" : "false\n");
                return 0;
            } else {
                file = argv[i];
            }
        }
    } else {
        printInfo();
        return 0;
    }

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
            printInfo();
    }
    return 0;
}