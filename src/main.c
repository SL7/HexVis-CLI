/**
 * HexVis - simple Hex visualizer for all kinds of files
 * @author: SL7
 * @version: v1.2pre
 * 
 * This is a simple Hex visualizer for all kinds of files
 * It was a 1 day project and development will continue soon.
 *
 * TODO:
 *   - [ ] Add 'bytes to read' argument
 *   - [ ] Make more efficient
 * Changelog:
 *   |__v1.0
 *   |    |__ Initial Version
 *   |__v1.1
 *   |    |__ borders added
 *   |    |__ colors added
 *   |__v1.2pre
 *   |    |__ added changelog
 *
 */
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
#define TABLE_WIDTH 70

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

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
    printf("┌"); 
    for(int i = 0; i < TABLE_WIDTH; i++) {
        printf("─");
    }
    printf("┐\n");
    fseek(f, 0, SEEK_END);
    long f_size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = calloc(255, sizeof(char));
    sprintf(buf, "path: %s", filepath);
    counter = strlen(buf);
    printf("│ %s", buf);
    while( counter < TABLE_WIDTH-1) {
        printf(" ");
        counter++;
    }
    printf("│\n│ ");
    sprintf(buf, "size: %ld bytes", f_size); 
    counter = strlen(buf);
    printf("%s", buf);
    while (counter < TABLE_WIDTH-1) {
        printf(" ");
        counter++;
    }
    counter = 0; 
    printf("│\n├"); 

    counter = 0;
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
    printf("─┤\n│ ");
    counter = 0;
    buf = calloc(255, sizeof(char));
    while ((c=fgetc(f)) != EOF) {
        if (counter == (WIDTH/2) - 1) {
            char temp[1] = {(isprint(c)) ? c : '•'};
            strcat(buf, (isprint(c)?ANSI_COLOR_GREEN:ANSI_COLOR_RED)); 
            strcat(buf, temp);
            strcat(buf, ANSI_COLOR_RESET);
            if (isprint(c)) {
                printf(ANSI_COLOR_RED);
                printf((c<0x10)?"0%x"ANSI_COLOR_RESET" ¦ ":"%x"ANSI_COLOR_RESET" ¦ ",c);
            } else {
                printf(ANSI_COLOR_GREEN);
                printf((c<0x10)?"0%x"ANSI_COLOR_RESET" ¦ ":"%x"ANSI_COLOR_RESET" ¦ ",c);
            }
            counter++; 
        } else if (counter == WIDTH-1) {
            if (isprint(c)) {
                printf(ANSI_COLOR_GREEN);
                printf((c<0x10)?"0%x" ANSI_COLOR_RESET " │":"%x"ANSI_COLOR_RESET" │", c);
            } else {
                printf(ANSI_COLOR_RED);
                printf((c<0x10)?"0%x"ANSI_COLOR_RESET" │":"%x"ANSI_COLOR_RESET" │", c);
            }
            printf(" %s", buf);
            printf(isprint(c) ? ANSI_COLOR_GREEN : ANSI_COLOR_RED);
            printf("%c"ANSI_COLOR_RESET" │\n│ ",(!isprint(c))?'_':c);
            buf = calloc(255, sizeof(char));
            counter = 0;
        } else {
            char temp[1] = {(isprint(c)) ? c : '_'};
            strcat(buf, (isprint(c)?ANSI_COLOR_GREEN:ANSI_COLOR_RED)); 
            strcat(buf, temp);
            strcat(buf, ANSI_COLOR_RESET);
            if(isprint(c)) {
                printf(ANSI_COLOR_GREEN);
                printf((c<0x10)?"0%x ":"%x ", c);
                printf(ANSI_COLOR_RESET);
            } else {
                printf(ANSI_COLOR_RED);
                printf((c<0x10)?"0%x ":"%x ", c);
                printf(ANSI_COLOR_RESET);
            }
            counter++;
        }
    }
    int zero_count = 0; 
    while (counter < WIDTH) {
        printf(ANSI_COLOR_YELLOW "00 " ANSI_COLOR_RESET);
        if (counter == (WIDTH/2) - 1) {
            printf("¦ ");
        }
        zero_count++;
        counter++;
    }
    int len = strlen(buf); 
    printf("│ %s",  buf);
    for (int i = 0; i < zero_count; i++) {printf(" ");}
    //while ( len < WIDTH ) { printf(" ");len++; }
    printf(" │\n└─");
    counter = 0;
    while (counter < WIDTH) {
        printf((counter==WIDTH/2)?"┴─":"───");
        counter++;
    }
    printf("───┴");
    counter = 0;

    while (counter < WIDTH) {
        printf("─");
        counter++;
    }
    printf("──┘\n");
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

