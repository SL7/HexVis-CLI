#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

/*
 * hexvis -v
 * hexvis -x 5 test.file
 * hexvis test.file
 * hexvis -o out.file test.file
 * hexvis -x 5 -o out.file test.file
 * hexvis -r test.file
 * hexvis -x 5 -o test.file -r input.file
 * hexvis -h
 */

#define WIDTH 16

FILE *f, *out;

uint8_t config = 0x00;

char *filepath = NULL;
char *out_file = NULL; 
char *column = NULL;
uint8_t output = 0x00;
uint8_t raw = 0;
uint8_t width = 8;

char getCommand(char *str);
int isPrintable(char c);
void printVersion(void);
void printUsage(void);
void printHelp(void);
int containsChar(char *str);
int isFile(char *str);

int main(int argc, char **argv)
{
    if (argc == 1) {
        printUsage();
        return 0;
    } 
    for (int i = 1; i < argc;) {
        char c = getCommand(argv[i]);
        switch (c) {
            case 'v':
                printf("HexVis v0.1pre\n");
                return 0;
            case 'h':
                printHelp();
                return 0;
            case 'r':
                raw = 1;
                i++;
                break;
            case 'x':
                i++;
                if (containsChar(argv[i])) {
                    printf("[-] -x command line argument wrong:\n"
                           "Usage: -x <NUMBER>\n");
                    return 0;
                }
                column = argv[i++];
                break;
            case 'o':
                i++;
                out_file = argv[i++];
                break;
            case -1:
                if (isFile(argv[i])) {
                    filepath = argv[i++];
                    break;
                }
                printUsage();
                return 0;
            default:
                printUsage();
                return 0;

        }
    }
    if (filepath == NULL) {
        printUsage();
        return 0;
    }
    f = fopen(filepath, "r");
    if (out_file != NULL) {
        out = fopen(out_file, "r+");
    }
    //printf("raw: %d\n", raw);
    //printf("columns: %s\n", column);
    //printf("out file: %s\n", out_file);
    if(column != NULL) {
        width = atoi(column); 
    } 
    int c;
    uint8_t counter = 0;
    char *hex_buf, *char_buf;
    hex_buf = calloc(255, sizeof(char));
    char_buf = calloc(255, sizeof(char));
    printf("┌"); 

    while (counter < WIDTH+1) {
        if ( counter == (WIDTH/2)) {
            printf("─┬─");
        } else {
            printf("───");
        }
        counter++;
    }
    printf("┬─");
    counter = 0;
    while (counter < WIDTH) {
        printf("─");
        counter++;
    }
    printf("─┐\n│ ");
    counter = 0;
    char *buf = calloc(255, sizeof(char));
    while ((c=fgetc(f)) != EOF) {
        if (counter == (WIDTH/2) - 1) {
            if (isprint(c)) {    
                char temp[1] = {c};
                strcat(buf, temp);
            } else {
                char temp[1] = {'_'};
                strcat(buf, temp);
            }
            if (c < 0x10) {
                printf("0%x ¦ ", c);
            } else {
                printf("%x ¦ ",c);
            }
            counter++; 
        } else if (counter == WIDTH-1) {
            if (c < 0x10) {
                printf("0%x │", c);
            } else {
                printf("%x │", c);
            }
            printf(" %s%c │\n│ ", buf, (!isprint(c))?'_':c);
            buf = calloc(255, sizeof(char));
            counter = 0;
        } else {
            if (isprint(c)) { 
                char temp[1] = {c};
                strcat(buf, temp);
            } else {
                char temp[1] = {'_'};
                strcat(buf, temp);
            }
            if (c < 0x10) {
                printf("0%x ", c);
            } else {
                printf("%x ",c);
            }
            counter++;
        }
    }
    
    while (counter < WIDTH) {
        printf("00 ");
        if (counter == (WIDTH/2) - 1) {
            printf("¦ ");
        }
        counter++;
    }
    int len = strlen(buf);
    printf("│ %s",  buf);
    while ( len < WIDTH ) { printf(" ");len++; }
    printf(" │\n└─");
    counter = 0;
    while (counter < WIDTH) {
        if ( counter == WIDTH/2) {
            printf("┴─");
        } else {
            printf("───");
        }
        counter++;
    }
    printf("───┴");
    counter = 0;

    while (counter < WIDTH) {
        printf("─");
        counter++;
    }
    printf("──┘\n");
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    counter = 0;
    fclose(f);
    printf("\n");
    if (out != NULL) {
        fclose(out);
    }
    return 0;
}

char getCommand(char *str) {
    if (str[0] == '-' && strlen(str) == 2) {
        return str[1];
    }
    return -1;
}

void printHelp(void) {
    printUsage();
}


void printUsage(void) { 
    fprintf(stderr, "HexVis v0.1\nUsage:  hexvis [ARGS] [FILE]\nArguments:\n\t"
                    "-v \t\t get HexVis version\n\t"
                    "-o [FILE]\t output to file called file.hex  (NOT IMPLEMENTED)\n\t"
                    "-r \t\t raw mode, don't print the output in a table (NOT IMPLEMENTED)\n");
}

int containsChar(char *str) {
    int len = strlen(str);
    for(int i = 0; i < len; i++) {
        if (str[i] >= 'a' && str[i] <= 'z') {
            return 1;
        } else if (str[i] >= 'A' && str[i] <= 'Z') {
            return 1;
        }
    }
    return 0;
}

int isFile(char *str) {
    FILE *temp;
    temp = fopen(str, "r");
    if (temp == NULL) {
        return 0;
    }
    fclose(temp);
    return 1;
}

