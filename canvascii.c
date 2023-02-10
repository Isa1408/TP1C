#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_HEIGHT 40
#define MAX_WIDTH 80
#define DEFAULT_PEN '7'
#define USAGE "\
Usage: ./canvascii [-n HEIGHT,WIDTH] [-s] [-k] [-p CHAR]\n\
          [-h ROW] [-v COL] [-r ROW,COL,HEIGHT,WIDTH]\n\
          [-l ROW1,COL1,ROW2,COL2] [-c ROW,COL,RADIUS]\n\
Draws on an ASCII canvas. The canvas is provided on stdin and\n\
the result is printed on stdout. The dimensions of the canvas\n\
are limited to at most 40 rows and at most 80 columns.\n\
\n\
If no argument is provided, the program prints this help and exit.\n\
\n\
Canvas options:\n\
  -n HEIGHT,WIDTH           Creates a new empty canvas of HEIGHT rows and\n\
                            WIDTH columns. Should be used as first option,\n\
                            otherwise, the behavior is undefined.\n\
                            Ignores stdin.\n\
  -s                        Shows the canvas and exit.\n\
  -k                        Enables colored output. Replaces characters\n\
                            between 0 and 9 by their corresponding ANSI\n\
                            colors:\n\
                              0: black  1: red      2: green  3: yellow\n\
                              4: blue   5: magenta  6: cyan   7: white\n\
\n\
Drawing options:\n\
  -p CHAR                   Sets the pen to CHAR. Allowed pens are\n\
                            0, 1, 2, 3, 4, 5, 6 or 7. Default pen\n\
                            is 7.\n\
  -h ROW                    Draws an horizontal line on row ROW.\n\
  -v COL                    Draws a vertical line on column COL.\n\
  -r ROW,COL,HEIGHT,WIDTH   Draws a rectangle of dimension HEIGHTxWIDTH\n\
                            with top left corner at (ROW,COL).\n\
  -l ROW1,COL1,ROW2,COL2    Draws a discrete segment from (ROW1,COL1) to\n\
                            (ROW2,COL2) with Bresenham's algorithm.\n\
  -c ROW,COL,RADIUS         Draws a circle centered at (ROW,COL) of\n\
                            radius RADIUS with the midpoint algorithm.\n\
"
#define ERROR_MISSING_N "Error: missing value with option -n\n"
#define ERROR_MESSAGE_N "Error: incorrect value with option -n\n"
#define ERROR_MESSAGE_H "Error: incorrect value with option -h\n"
#define ERROR_MESSAGE_V "Error: incorrect value with option -v\n"
#define ERROR_MESSAGE_S_TOO_HIGH "Error: canvas is too high (max height: 40)\n"
#define ERROR_MESSAGE_S_TOO_WIDE "Error: canvas is too wide (max width: 80)\n"
#define ERROR_MESSAGE_R "Error: incorrect value with option -r\n"
#define ERROR_MESSAGE_L "Error: incorrect value with option -l\n"
#define ERROR_MESSAGE_P "Error: incorrect value with option -p\n"
#define ERROR_MESSAGE_C "Error: incorrect value with option -c\n"
#define ERROR_MESSAGE_NON_RECTANGULAR "Error: canvas should be rectangular\n"
#define ERROR_MESSAGE_WRONG_PIXEL "Error: wrong pixel value #\n"
#define ERROR_MESSAGE_UNRECOGNIZED_OPTION "Error: unrecognized option "

struct canvas {
    char pixels[MAX_HEIGHT][MAX_WIDTH]; // A matrix of pixels
    unsigned int width;                 // Its width
    unsigned int height;                // Its height
    char pen;                           // The character we are drawing with
};

enum error {
    OK                         = 0, // Everything is ok
    ERR_WRONG_PIXEL            = 1, // Wrong pixel value in canvas
    ERR_CANVAS_TOO_HIGH        = 2, // Canvas is too high
    ERR_CANVAS_TOO_WIDE        = 3, // Canvas is too wide
    ERR_CANVAS_NON_RECTANGULAR = 4, // Canvas is non rectangular
    ERR_UNRECOGNIZED_OPTION    = 5, // Unrecognized option
    ERR_MISSING_VALUE          = 6, // Option with missing value
    ERR_WITH_VALUE             = 7  // Problem with value
};

struct canvas draw_horizontal_line(char pen, struct canvas *canvas, int row) {
    (*canvas).pen = pen;
    for (int i = 0; i < (*canvas).width; i++) {
        (*canvas).pixels[row][i] = (*canvas).pen;
    }
    return (*canvas);
}

struct canvas draw_vertical_line(char pen, struct canvas *canvas, int col){
    (*canvas).pen = pen;
    for (int i = 0; i < (*canvas).height; i++) {
        (*canvas).pixels[i][col] = (*canvas).pen;
    }
    return (*canvas);
}

void print_canvas(struct canvas canvas) {
    for (int i = 0; i < canvas.height; i++) {
        for (int j = 0; j < canvas.width; j++) {
            printf("%c", canvas.pixels[i][j]);
        }
        printf("\n");
    }
}

void print_canvas_in_color(struct canvas canvas) {
    for (int i = 0; i < canvas.height; i++) {
        for (int j = 0; j < canvas.width; j++) {
            printf("\x1B[4%cm" " " "\x1B[0m", canvas.pixels[i][j]);
        }
        printf("\n");
    }
}

struct canvas drawRectangle(int x1, int y1, int x2, int y2, struct canvas *canvas) {
    for (int x = x1; x < x1 + x2; x++) {
        for (int y = y1; y < y1 + y2; y++) {
            if (x >= 0 && x <= (*canvas).width && y >= 0 && y <= (*canvas).height) {
                if (y == y1 || y == y1 + y2 - 1 || x == x1 || x == x1 + x2 - 1) {
                    (*canvas).pixels[x][y] = canvas->pen;
                }
            }
        }
    }
    return (*canvas);
}

struct canvas isSTDIN(struct canvas *canvas, enum error *err);

void verify_N(enum error *err, bool isN, struct canvas *canvas, bool *can_print_canvas) {
    if (!isN){
        (*canvas) = isSTDIN(canvas, err);
        if((*err) == OK){
            (*can_print_canvas) = true;
        }
    }
}

void count_chars_and_lines(int *line_count, int *char_count, enum error *err) {
    char line[4000];

    *char_count = 0;
    *line_count = 0;

    while (fgets(line, sizeof(line), stdin) != NULL) {
        if (ferror(stdin)) {
            printf("Error reading from stdin\n");
            break;
        }
        (*line_count)++;
        if (strstr(line, "#") != NULL) {
            *err = ERR_WRONG_PIXEL;
        }
        int len = strcspn(line, "\n");
        if(*char_count != len && *char_count != 0){
            *err = ERR_CANVAS_NON_RECTANGULAR;
        }
        *char_count = len;
    }
}

struct canvas isSTDIN(struct canvas *canvas, enum error *err) {
    int line_count, char_count;
    count_chars_and_lines(&line_count, &char_count, err);
    (*canvas).height = line_count;
    (*canvas).width = char_count;

    if((*canvas).height > 40){
        (*canvas).height = 40;
        *err = ERR_CANVAS_TOO_HIGH;
    }
    if((*canvas).width > 80){
        (*canvas).width = 80;
        *err = ERR_CANVAS_TOO_WIDE;
    }

    for (int i = 0; i < (*canvas).height; i++) {
        for (int j = 0; j < (*canvas).width; j++) {
            (*canvas).pixels[i][j] = '.';
        }
    }

    return (*canvas);
}

bool getErrValue(enum error *err, bool *can_print_canvas) {
    if((*err) == OK){
        *can_print_canvas = true;
    }
    if((*err) == ERR_CANVAS_TOO_HIGH){
        fprintf(stderr, ERROR_MESSAGE_S_TOO_HIGH);
        fprintf(stderr, USAGE);
    }
    if((*err) == ERR_CANVAS_TOO_WIDE){
        fprintf(stderr, ERROR_MESSAGE_S_TOO_WIDE);
        fprintf(stderr, USAGE);
    }
    if((*err) == ERR_CANVAS_NON_RECTANGULAR){
        fprintf(stderr, ERROR_MESSAGE_NON_RECTANGULAR);
        fprintf(stderr, USAGE);
    }
    if((*err) == ERR_WRONG_PIXEL){
        fprintf(stderr, ERROR_MESSAGE_WRONG_PIXEL);
        fprintf(stderr, USAGE);
    }
    return can_print_canvas;
}

void draw_empty_canvas(int width, int height, enum error *err, struct canvas *canvas, bool *can_print_canvas) {
    (*canvas).pen = '.';
    (*canvas).height = height;
    (*canvas).width = width;
    if((*canvas).height > 40){
        (*canvas).height = 40;
        (*err) = ERR_CANVAS_TOO_HIGH;
    }
    if((*canvas).width > 80){
        (*canvas).width = 80;
        (*err) = ERR_CANVAS_TOO_WIDE;
    }
    for (int a = 0; a < (*canvas).height; a++) {
        for (int b = 0; b < (*canvas).width ; b++) {
            (*canvas).pixels[a][b] = (*canvas).pen;
        }
    }
    (*err) = OK;
    (*can_print_canvas) = true;
    (*canvas).pen = DEFAULT_PEN;
}

struct canvas draw_segment(struct canvas *canvas, int x0, int y0, int x1, int y1) {
    int ax = abs(x1 - x0);
    int bx = x0 < x1 ? 1 : -1;
    int ay = -abs(y1 - y0);
    int by = y0 < y1 ? 1 : -1;
    int error = ax + ay;

    while (1) {
        if ((x0 >= 0 && x0 <= (*canvas).width) || (y0 >= 0 && y0 <= (*canvas).height)) {
            (*canvas).pixels[x0][y0] = canvas->pen;
        }
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * error;
        if (e2 >= ay) {
            if (x0 == x1) break;
            error += ay;
            x0 += bx;
        }
        if (e2 <= ax) {
            if (y0 == y1) break;
            error += ax;
            y0 += by;
        }
    }
    return(*canvas);
}

void delimit_canvas(struct canvas *canvas, int row, int col) {
    if (row < canvas->height && col < canvas->width && row >= 0 && col >= 0) {
        canvas->pixels[row][col] = canvas->pen;
    }
}

struct canvas draw_circle(struct canvas *canvas, int row, int col, int radius) {
    int f = 1 - radius;
    int ddF_x = 0;
    int ddF_y = -2 * radius;
    int x = 0;
    int y = radius;

    delimit_canvas(canvas, row, col + radius);
    delimit_canvas(canvas, row, col - radius);
    delimit_canvas(canvas, row + radius, col);
    delimit_canvas(canvas, row - radius, col);

    while(x < y)
    {
        if(f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x + 1;
        delimit_canvas(canvas, row + x, col + y);
        delimit_canvas(canvas, row - x, col + y);
        delimit_canvas(canvas, row + x, col - y);
        delimit_canvas(canvas, row - x, col - y);
        delimit_canvas(canvas, row + y, col + x);
        delimit_canvas(canvas, row - y, col + x);
        delimit_canvas(canvas, row + y, col - x);
        delimit_canvas(canvas, row - y, col - x);
    }
    return(*canvas);
}


int main(int argc, char* argv[]) {
    enum error err = OK;
    if (argc == 1) {
        puts(USAGE);
        err = OK;
    }else{
        struct canvas canvas;
        canvas.pen = DEFAULT_PEN;
        int width, height;
        bool can_print_canvas = false;
        bool isColor = false;
        bool isN = false;

        if(strcmp(argv[1], "-s") == 0){
            canvas = isSTDIN(&canvas, &err);
            can_print_canvas = getErrValue(&err, &can_print_canvas);
        } else {
            for (int i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-n") == 0 && argc == 2){
                    fprintf(stderr, ERROR_MISSING_N);
                    fprintf(stderr, USAGE);
                    err = ERR_MISSING_VALUE;
                    break;
                } else if (strcmp(argv[i], "-n") == 0) {
                    if(sscanf(argv[i + 1], "%d,%d", &height, &width) == 2){
                        isN = true;
                        if (width > MAX_WIDTH || height > MAX_HEIGHT) {
                            fprintf(stderr, ERROR_MESSAGE_N);
                            fprintf(stderr, USAGE);
                            err = ERR_WITH_VALUE;
                            break;
                        } else {
                            draw_empty_canvas(width, height, &err, &canvas, &can_print_canvas);
                        }
                    } else {
                        fprintf(stderr, ERROR_MESSAGE_N);
                        fprintf(stderr, USAGE);
                        err = ERR_WITH_VALUE;
                        break;
                    }
                } else if (strcmp(argv[i], "-h") == 0) {
                    int row;
                    verify_N(&err, isN, &canvas, &can_print_canvas);

                    if (sscanf(argv[i + 1], "%d", &row) == 1 && can_print_canvas) {
                        if (row < 0 || row >= canvas.width) {
                            fprintf(stderr, ERROR_MESSAGE_H);
                            fprintf(stderr, USAGE);
                            can_print_canvas = false;
                            err = ERR_WITH_VALUE;
                            break;
                        }
                        can_print_canvas = true;
                        canvas = draw_horizontal_line(canvas.pen, &canvas, row);
                    }

                } else if (strcmp(argv[i], "-v") == 0){
                    int col;
                    verify_N(&err, isN, &canvas, &can_print_canvas);

                    if (sscanf(argv[i + 1], "%d", &col) == 1) {
                        if (col < 0 || col >= canvas.height) {
                            fprintf(stderr, ERROR_MESSAGE_V);
                            fprintf(stderr, USAGE);
                            can_print_canvas = false;
                            err = ERR_WITH_VALUE;
                            break;
                        }
                        can_print_canvas = true;
                        canvas = draw_vertical_line(canvas.pen, &canvas, col);

                    }
                } else if (strcmp(argv[i], "-r") == 0){
                    verify_N(&err, isN, &canvas, &can_print_canvas);

                    int x1, y1, x2, y2;
                    err = OK;
                    if (sscanf(argv[i + 1], "%d,%d,%d,%d", &x1, &y1, &x2, &y2) != 4) {
                        fprintf(stderr, ERROR_MESSAGE_R);
                        fprintf(stderr, USAGE);
                        err = ERR_WITH_VALUE;
                        can_print_canvas = false;
                        break;
                    }
                    if (x2 < x1 || y2 < y1 || x2 < 0 || y2 < 0) {
                        fprintf(stderr, ERROR_MESSAGE_R);
                        fprintf(stderr, USAGE);
                        err = ERR_WITH_VALUE;
                        can_print_canvas = false;
                        break;
                    }

                    canvas = drawRectangle(x1, y1, x2, y2, &canvas);

                } else if (strcmp(argv[i], "-l") == 0) {
                    verify_N(&err, isN, &canvas, &can_print_canvas);

                    int x0, y0, x1, y1;
                    err = OK;

                    if (sscanf(argv[i + 1], "%d,%d,%d,%d", &x0, &y0, &x1, &y1) != 4) {
                        fprintf(stderr, ERROR_MESSAGE_L);
                        fprintf(stderr, USAGE);
                        err = ERR_WITH_VALUE;
                        can_print_canvas = false;
                        break;
                    }

                    canvas = draw_segment(&canvas, x0, y0, x1, y1);

                } else if (strcmp(argv[i], "-c") == 0) {
                    verify_N(&err, isN, &canvas, &can_print_canvas);

                    int row, col, radius;
                    err = OK;

                    if (sscanf(argv[i + 1], "%d,%d,%d", &row, &col, &radius) != 3 || radius < 0) {
                        fprintf(stderr, ERROR_MESSAGE_C);
                        fprintf(stderr, USAGE);
                        err = ERR_WITH_VALUE;
                        can_print_canvas = false;
                        break;
                    }

                    canvas = draw_circle(&canvas, row, col, radius);

                } else if (strcmp(argv[i], "-p") == 0) {
                    char pen;
                    if (strlen(argv[i + 1]) == 1
                        && sscanf(argv[i + 1], "%c", &pen) == 1
                        && pen >= '0' && pen <= '7') {
                        canvas.pen = pen;
                        err = OK;
                        can_print_canvas = true;
                    } else {
                        fprintf(stderr, ERROR_MESSAGE_P);
                        fprintf(stderr, USAGE);
                        err = ERR_WITH_VALUE;
                        can_print_canvas = false;
                        break;
                    }
                } else if (strcmp(argv[i], "-k" ) == 0){
                    isColor = true;
                } else {
                    if (!((strchr("-", argv[i - 1][0]) != NULL)
                    && strchr("nskphvrlc", argv[i - 1][1]) != NULL)) {
                        fprintf(stderr, ERROR_MESSAGE_UNRECOGNIZED_OPTION "%s\n", argv[i]);
                        fprintf(stderr, USAGE);
                        err = ERR_UNRECOGNIZED_OPTION;
                        can_print_canvas = false;
                        break;
                    }
                }
            }
        }

        if(can_print_canvas && err == OK && !isColor){
            print_canvas(canvas);
        }
        if(isColor && err == OK && can_print_canvas){
            print_canvas_in_color(canvas);
        }
    }
    return err;
}






