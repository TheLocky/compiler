#include <cstdio>

#define VERSION ("0.01a")
#define OUTTOFILE

#include "tokenizer/StateController.h"
#include "tokenizer/Tokenizer.h"

int main(int argc, char *argv[]) {
    if (argc > 1) {
        StateController::Build();
        printf("Pascal compiler by TheLocky version: %s\n", VERSION);
        string out_file = "out_";
        out_file += argv[1];
        printf("Compile file %s\n", argv[1]);
#ifdef OUTTOFILE
        freopen(out_file.c_str(), "w", stdout);
#endif
        ifstream *file = new ifstream();
        file->open(argv[1], std::ifstream::binary);
        try {
            Tokenizer tokenizer(file);
            Token tok(tokenizer.Next());
            if (tokenizer.Eof() && tok.tokenType == TK_ERROR)
                throw TokenException(0, 0, "", "Can't read file or file empty");
            while (!tokenizer.Eof()) {
                tok.print();
                tok = tokenizer.Next();
            }
            tok.print();
        } catch(TokenException e) {
            e.print();
        }
    } else {
        printf("Pascal compiler by TheLocky version: %s\n", VERSION);
        printf("Usage: pasc.exe [parameters] input.pas\n");
        printf("Parameters:\n");
        printf("<none>\n");
        StateController::Build();
    }
}