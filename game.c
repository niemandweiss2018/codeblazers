#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXWORDS 200
#define WORDSIZE 20
#define MAXBOARDSIZE 26
#define TRUE 1
#define FALSE 0
#define MAXMOVES 2703 // 26*26*4-1

struct dictonary {
    struct word* words[MAXWORDS];
    int nElements;
};

struct word {
    char string[WORDSIZE];
    size_t length;
};

struct boardCollection {
    struct board* allBoards;
    int nElements;
};

struct board {
    char** elements;
    int rows;
    int cols;
};

// prototypes
void begin();
void readIn(char[]);
void insert(struct dictonary*, char[]);
void addBoard(struct boardCollection*, char**, int, int);
int isWord(const struct dictonary*, const char[], const size_t);
void findWords(const struct dictonary*, const struct board*);
void searchBoard(const struct dictonary*, const struct board*, int**, int, int, int, int, char[WORDSIZE], int*, char[]);

struct dictonary* init_dict() {
    struct dictonary* newDict = malloc(sizeof(struct dictonary));
    for (int i = 0; i < MAXWORDS; i++) {
        newDict->words[i] = NULL; // null_ptr
    }

    newDict->nElements = FALSE;
    return newDict;
}

struct word* init_word(char in[], size_t l) {
    struct word* newWord = malloc(sizeof(struct word));
    strncpy(newWord->string, in, strlen(in));
    newWord->length = l;
    return newWord;
}

struct boardCollection* init_collect() {
    struct boardCollection* newCollection = malloc(sizeof(struct boardCollection));
    newCollection->allBoards = malloc(sizeof(struct board));
    newCollection->nElements = FALSE;
    return newCollection;
}

struct board* init_board(char** e, int rowsIn, int colsIn) {
    struct board* newBoard = malloc(sizeof(struct board));
    newBoard->elements = malloc(sizeof(char*) * rowsIn);
    newBoard->elements = e;
    newBoard->rows = rowsIn;
    newBoard->cols = colsIn;
    return newBoard;
}

int main(int argc, const char* argv[]) {
    struct dictonary* currentDict = init_dict();
    begin(); // start routine

    // read in words
    while(TRUE) {
        char input[WORDSIZE];
        readIn(input);
        if (strncmp(input, "END", 3) == 0)
            break;
        else
            insert(currentDict, input);
    }

    // read in boards
    int rows = 0, cols = 0;
    struct boardCollection* collection = init_collect();
    while (TRUE) {
        // size of next board
        scanf("%d", &rows);
        // check for escape
        if(rows == -1)
            break;

        scanf("%d", &cols);
        if(rows > MAXBOARDSIZE || cols > MAXBOARDSIZE) {
            printf("Too Large! Max size is %dx%d\n", MAXBOARDSIZE, MAXBOARDSIZE);
        } else {
            char** board = malloc(sizeof(char*) * rows);
            for(int i = 0; i < rows; i++) {
                char line[cols];
                scanf("%s", line);
                board[i] = (char*)malloc(strlen(line));
                strncpy(board[i], line, strlen(line));
            }

            addBoard(collection, board, rows, cols);
        }
    }

    for(int i = 0; i < collection->nElements; i++) {
        printf("\nSolutions:\n");
        findWords(currentDict, &collection->allBoards[i]);
    }

    // release dynamic allocations
    free(collection);
    free(currentDict);

    printf("\n\nDone!\n\n");
}

void readIn(char in[]) {
    scanf("%s", in);
}

void insert(struct dictonary* cDict, char word[]) {
    size_t length = 0;
    if(cDict->nElements < MAXWORDS) {
        for (int i = 0; i < WORDSIZE; i++) {
            if(strncmp(&word[i], "", 1) == 0)
                break;
            else
                length++;
        }

        cDict->words[cDict->nElements] = init_word(word, length);
        cDict->nElements++;
    }
}

void addBoard(struct boardCollection* boards, char** board, int rows, int cols) {
    if(boards->nElements > 0)
        boards->allBoards = realloc(boards->allBoards, sizeof(struct board) * (boards->nElements + 1));

    struct board* newBoard = init_board(board, rows, cols);
    boards->allBoards[boards->nElements] = *newBoard;
    boards->nElements++;
}

// check to see if checkWord is in dictonary
// returns 1 if word was found, 0 otherwise
int isWord(const struct dictonary* dict, const char checkWord[], const size_t length) {
    int flag = FALSE;
    for(int i = 0; i < dict->nElements; i++) {
        const size_t wordLength = dict->words[i]->length;
        if(length == wordLength) {
            for(int j = 0; j < wordLength; j++)
                if(strncmp(&checkWord[j], &dict->words[i]->string[j], TRUE) != 0)
                    flag = TRUE; // issue detected
            
            if(!flag)
                return TRUE; // word found
        }

        flag = FALSE; // reset
    }

    return FALSE;
}

// check to see if path is good
// returns 1 if track is good, 0 otherwise
int isGood(const struct dictonary* dict, const char checkWord[], const size_t length) {

}

void findWords(const struct dictonary* currentDict, const struct board* currentBoard) {
    const int cols = currentBoard->cols;
    const int rows = currentBoard->rows;
    char findPath[MAXMOVES] = { 0 };

    // init checked memory structure    
    int** checked = malloc(sizeof(int*) * rows); // build Y
    for(int i = 0; i < rows; i++) {
        checked[i] = malloc(sizeof(int) * cols); // build X
        for(int j = 0; j < cols; j++)
            checked[i][j] = FALSE;
    }

    int nChars = 0;
    char wordString[WORDSIZE];
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            searchBoard(currentDict, currentBoard, checked, i, j, rows, cols, wordString, &nChars, "\0");
        }
    }
}

void searchBoard(const struct dictonary* currentDict, const struct board* currentBoard, int** checked, int i, int j, int rows, int cols, char wordString[WORDSIZE], int *nChars, char findPath[]) {
    char new[MAXMOVES];
    strcpy(new, findPath);
    if(new[0] == 0)
        sprintf(new, "%c%d", (65 + i), j);
    else
        sprintf(new, "%s,%c%d", new, (65 + i), j);

    checked[i][j] = TRUE;
    wordString[*nChars] = currentBoard->elements[i][j];
    (*nChars)++;
    if(isWord(currentDict, wordString, (size_t)*nChars))
        printf("%s\n", new);

    for(int row = i - 1; row <= i + 1 && row < rows; row++)
        for(int col = j - 1; col <= j + 1 && col < cols; col++)
            if(row >= 0 && col >= 0 && checked[row][col] == 0)
                searchBoard(currentDict, currentBoard, checked, row, col, rows, cols, wordString, nChars, new);

    wordString[*nChars - 1] = FALSE;
    checked[i][j] = FALSE;
    (*nChars)--;
    strcpy(new, "");
}

void begin() {
    printf("Welcome to the Boggle Board Solver!\n\nINSTRUCTIONS\n");
    printf("Enter the words to look for. Terminate with \"END\".\n");
    printf("Enter the board size: Example: 4 6 (a 4 row by 6 character board).\n");
    printf("Enter the board layout.\n");
}