#include <stdio.h>
#include <stdlib.h>

void quit(char * str) {
    puts(str);
    exit(EXIT_FAILURE);
}

enum {
    VD, VO, LD, LO, AB
} mode;

enum {
    A, O
} out;

char * parse_out[2] = {
    "A", "O"
};

#define WORD_SIZE (8)

struct word {
    char * word;
    int size;
    int max_size;
};

#define VEC_SIZE (8)

struct word_pack {
    struct word * word;
    int rep;
};

struct {
    struct word_pack * words;
    int size;
    int max_size;
} vector;

struct node {
    struct word * value;
    struct node * next;
};

struct node * head;

struct joint {
    struct word * value;
    struct joint * left;
    struct joint * right;
};

struct joint * root;

struct word * newWord() {
    struct word * t = malloc(sizeof(struct word));
    if(t == NULL)
        quit("Out of memory");
    t->word = malloc(sizeof(char) * WORD_SIZE);
    if(t->word == NULL)
        quit("Out of memory");
    t->size = 0;
    t->max_size = WORD_SIZE;
    return t;
}

struct word * copyWord(struct word * t) {
    struct word * p = malloc(sizeof(struct word));
    if(p == NULL)
        quit("Out of memory");
    p->word = malloc(sizeof(char) * t->max_size);
    if(p->word == NULL)
        quit("Out of memory");
    for(int i = 0; i < t->size; i++)
        p->word[i] = t->word[i];
    p->size = t->size;
    p->max_size = t->max_size;
    return p;
}

void pushChar(struct word * t, char c) {
    if(t->size >= t->max_size) {
        t->max_size = t->max_size * 2;
        t->word = realloc(t->word, sizeof(char) * t->max_size);
        if(t->word == NULL)
            quit("Out of memory");
    }
    t->word[t->size++] = c;
}

/*-1 -> a < b, 0 -> a = b, 1 -> a > b*/
int cmpWord(struct word * a, struct word * b) {
    int state = 0;
    int i;
    int max = (a->size < b->size) ? a->size : b->size; 
    for(i = 0; i < max; i++) {
        if(a->word[i] < b->word[i]) {
            state = -1;
            break;
        }
        if(a->word[i] > b->word[i]) {
            state = 1;
            break;
        }

    }
    if(state == 0) {
        if(a->size > b->size) state = 1;
        else if(a->size < b->size) state = -1;
    }
    return state;
}

void printWord(struct word * t) {
    for(int i = 0; i < t->size; i++)
        printf("%c", t->word[i]);
}

void freeWord(struct word * t) {
    free(t->word);
    free(t);
}

void initStruct() {
    switch(mode) {
        case VD:
        case VO:
            vector.words = malloc(sizeof(struct word_pack) * VEC_SIZE);
            vector.size = 0;
            vector.max_size = VEC_SIZE;
        break;
        case LD:
        case LO:
            head = NULL;
        break;
        case AB:
            root = NULL;
        break;
    }
}

void addWordToVec(struct word * t) {
    if(vector.size >= vector.max_size) {
    vector.max_size = vector.max_size * 2;
    vector.words = realloc(vector.words, sizeof(struct word_pack) * vector.max_size);
    if(vector.words == NULL)
        quit("Out of memory");
    }
    vector.words[vector.size].word = copyWord(t);
    vector.words[vector.size++].rep = 1;
}

void addWord(struct word * t) {
    int found = 0;
    int position = -1;
    switch(mode) {
        case VD:
            for(int i = 0; i < vector.size; i++) {
                if(cmpWord(t, vector.words[i].word) == 0) {
                    position = i;
                    found = 1;
                    break;
                }
            }
            if(found)
                vector.words[position].rep += 1;
            else
                addWordToVec(t);
        break;
        case VO:
        break;
        case LD:
        case LO:
            head = NULL;
        break;
        case AB:
            root = NULL;
        break;
    }
}

void logWords() {
    switch(mode) {
        case VD:
            for(int i = 0; i < vector.size; i++) {
                printWord(vector.words[i].word);
                printf("\t %d\n", vector.words[i].rep);
            }
        break;
        case VO:
        break;
        case LD:
        case LO:
            head = NULL;
        break;
        case AB:
            root = NULL;
        break;
    }
}

void freeStruct() {
    switch(mode) {
        case VD:
        case VO:
            for(int i = 0; i < vector.size; i++) {
                freeWord(vector.words[i].word);
            }
            free(vector.words);
        break;
        case LD:
        case LO:
            head = NULL;
        break;
        case AB:
            root = NULL;
        break;
    }
}

int main(int argc, char ** argv) {
    /*Input: $cmd $file $mode $out*/

    /*Parse input*/
    if(argc != 4) {
        puts("hash $file $mode $out\n$mode=VD,VO,LD,LO,AB\n$out=A,O");
        quit("Invalid format");
    }

    if(argv[2][0] == 'V') {
        if(argv[2][1] == 'D')
            mode = VD; 
        else if(argv[2][1] == 'O')
            mode = VO;
        else
            quit("Invalid mode");
    } else if(argv[2][0] == 'L') {
        if(argv[2][1] == 'D')
            mode = LD; 
        else if(argv[2][1] == 'O')
            mode = LO;
        else
            quit("Invalid mode");
    } else if(argv[2][1] == 'A') {
        mode = AB;
    } else {
        quit("Invalid mode");
    }

    if(argv[3][0] == 'A')
        out = A;
    else if(argv[3][0] == 'O')
        out = O;
    else
        quit("Invalid out");

    /*Read file*/
    FILE * file = fopen(argv[1], "r");
    if(file == NULL) quit("Could not open file");

    initStruct();

    int int_read;
    char char_read;
    int_read = fgetc(file);
    struct word * word = NULL;
    while(int_read != EOF) {
        char_read = (char) int_read;
        if(word == NULL)
            word = newWord();
        switch(char_read) {
            case '?': case '!': case '\'':
            case '\n': case '\r': case '\t':
            case '(': case ')': case '\\':
            case '[': case ']': case ';':
            case '{': case '}': case ':':
            case '*': case '@': case '_':
            case ',': case '.': case ' ':
            case '#': case '/': case '=':
            case '+': case '-': case '"':
                printWord(word);
                puts("");
                if(word->size > 0) addWord(word);
                freeWord(word);
                word = NULL;
            break;
            default:
                pushChar(word, char_read);
        }
        int_read = fgetc(file);
    }
    if(word != NULL) {
        printWord(word);
        if(word->size > 0) addWord(word);
        freeWord(word);
        word = NULL;
    }

    logWords();
    freeStruct();


    fclose(file);
    /*Out: word occurances
        tab word -> max word size;
        tab number left -> max number size;
'    */
}
