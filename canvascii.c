#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
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
#define ERROR_MESSAGE_NON_RECTANGULAR "Error: canvas should be rectangular\n"

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

void print_canvas_in_color(struct canvas canvas, int colorCode) {
    double height = 1;
    double width = 0.3;
    for (int i = 0; i < canvas.height; i++) {
        printf("\033[0;4%dm", colorCode);
        for (int j = 0; j < canvas.width; j++) {
//            printf("%c", canvas.pixels[i][j]);
            printf(" ");
        }
        printf("\033[0m\n");
    }



//    for (i = 0; i < height; i++) {
//        printf("\033[0;4%dm", colorCode);
//        for (j = 0; j < width; j++) {
//            printf(" ");
//        }
//        printf("\033[0m\n");
//    }
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

void count_chars_and_lines(int *line_count, int *char_count, enum error *err) {
    char line[4000];

    *char_count = 0;
    *line_count = 0;
    while (fgets(line, sizeof(line), stdin) != NULL) {
        (*line_count)++;
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
//    (*canvas).pen = '.';
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

struct canvas plotLine(int x0, int y0, int x1, int y1, struct canvas *canvas) {
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int error = dx + dy;

    while (1) {
        if ((x0 >= 0 && x0 <= (*canvas).width) || (y0 >= 0 && y0 <= (*canvas).height)) {
            (*canvas).pixels[x0][y0] = canvas->pen;
        }
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * error;
        if (e2 >= dy) {
            if (x0 == x1) break;
            error += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            if (y0 == y1) break;
            error += dx;
            y0 += sy;
        }
    }
    return(*canvas);
}
//struct image {
//    int width;
//    int height;
//    char pixels[][];
//};
//
//typedef struct color_component color_component;
//
//void raster_circle(
//        struct image img,
//        unsigned int x0,
//        unsigned int y0,
//        unsigned int radius,
//        color_component r,
//        color_component g,
//        color_component b );
//
//void put_pixel_clip(struct image img, int x, int y, color_component r, color_component g, color_component b)
//{
//    if (x >= 0 && x < img.width && y >= 0 && y < img.height)
//    {
//        img.pixels[y][x].r = r;
//        img.pixels[y][x].g = g;
//        img.pixels[y][x].b = b;
//    }
//}
//
//
//#define plot(x, y) put_pixel_clip(img, x, y, r, g, b)
//
//void raster_circle(
//        struct image img,
//        unsigned int x0,
//        unsigned int y0,
//        unsigned int radius,
//        struct color_component r,
//        struct color_component g,
//        struct color_component b )
//{
//    int f = 1 - radius;
//    int ddF_x = 0;
//    int ddF_y = -2 * radius;
//    int x = 0;
//    int y = radius;
//
//    plot(x0, y0 + radius);
//    plot(x0, y0 - radius);
//    plot(x0 + radius, y0);
//    plot(x0 - radius, y0);
//
//    while(x < y)
//    {
//        if(f >= 0)
//        {
//            y--;
//            ddF_y += 2;
//            f += ddF_y;
//        }
//        x++;
//        ddF_x += 2;
//        f += ddF_x + 1;
//        plot(x0 + x, y0 + y);
//        plot(x0 - x, y0 + y);
//        plot(x0 + x, y0 - y);
//        plot(x0 - x, y0 - y);
//        plot(x0 + y, y0 + x);
//        plot(x0 - y, y0 + x);
//        plot(x0 + y, y0 - x);
//        plot(x0 - y, y0 - x);
//    }
//}
//#undef plot

int main(int argc, char* argv[]) {
    enum error err = OK;
    if (argc == 1) { //imprime manuel
        puts(USAGE);
        err = OK;
    }else{ //arguments
        struct canvas canvas;
        canvas.pen = DEFAULT_PEN;
//        char pen = DEFAULT_PEN;
        int width, height;
        bool can_print_canvas = false;
        bool isColor = false;

        if(strcmp(argv[1], "-s") == 0){
            if (!isatty(STDIN_FILENO)){
                canvas = isSTDIN(&canvas, &err);

                if(err == OK){
                    can_print_canvas = true;
                }
                if(err == ERR_CANVAS_TOO_HIGH){
                    fprintf(stderr, ERROR_MESSAGE_S_TOO_HIGH);
                    fprintf(stderr, USAGE);
                }
                if(err == ERR_CANVAS_TOO_WIDE){
                    fprintf(stderr, ERROR_MESSAGE_S_TOO_WIDE);
                    fprintf(stderr, USAGE);
                }
                if(err == ERR_CANVAS_NON_RECTANGULAR){
                    fprintf(stderr, ERROR_MESSAGE_NON_RECTANGULAR);
                    fprintf(stderr, USAGE);
                }
            }
        } else {
            for (int i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-n") == 0 && argc == 2){
                    fprintf(stderr, ERROR_MISSING_N);
                    fprintf(stderr, USAGE);
                    err = ERR_MISSING_VALUE;
                } else if (strcmp(argv[i], "-n") == 0) {
                    if(sscanf(argv[i + 1], "%d,%d", &height, &width) == 2){
                        if (width > MAX_WIDTH || height > MAX_HEIGHT) {
                            fprintf(stderr, ERROR_MESSAGE_N);
                            fprintf(stderr, USAGE);
                            err = ERR_WITH_VALUE;
                        } else {
                            canvas.pen = '.';
                            canvas.height = height;
                            canvas.width = width;
                            if(canvas.height > 40){
                                canvas.height = 40;
                                err = ERR_CANVAS_TOO_HIGH;
                            }
                            if(canvas.width > 80){
                                canvas.width = 80;
                                err = ERR_CANVAS_TOO_WIDE;
                            }
                            for (int a = 0; a < canvas.height; a++) {
                                for (int b = 0; b < canvas.width ; b++) {
                                    canvas.pixels[a][b] = canvas.pen;
                                }
                            }
                            err = OK;
                            can_print_canvas = true;
                            canvas.pen = DEFAULT_PEN;
                        }
                    } else {
                        fprintf(stderr, ERROR_MESSAGE_N);
                        fprintf(stderr, USAGE);
                        err = ERR_WITH_VALUE;
                    }
                } else if (strcmp(argv[i], "-h") == 0) {
                    int row;

                    if (!isatty(STDIN_FILENO)){
                        canvas = isSTDIN(&canvas, &err);
                        if(err == OK){
                            can_print_canvas = true;
                        }
                    }

                    if (sscanf(argv[i + 1], "%d", &row) == 1 && can_print_canvas) {
                        if (row < 0 || row >= canvas.width) {
                            fprintf(stderr, ERROR_MESSAGE_H);
                            fprintf(stderr, USAGE);
                            can_print_canvas = false;
                            err = ERR_WITH_VALUE;
                            return err;
                        }
                        can_print_canvas = true;
                        canvas = draw_horizontal_line(canvas.pen, &canvas, row);
                    }

                } else if (strcmp(argv[i], "-v") == 0){
                    int col;
                    if (!isatty(STDIN_FILENO)){
                        canvas = isSTDIN(&canvas, &err);
                        if(err == OK){
                            can_print_canvas = true;
                        }
                    }

                    if (sscanf(argv[i + 1], "%d", &col) == 1) {
                        if (col < 0 || col >= canvas.height) {
                            fprintf(stderr, ERROR_MESSAGE_V);
                            fprintf(stderr, USAGE);
                            can_print_canvas = false;
                            err = ERR_WITH_VALUE;
                            return err;
                        }
                        can_print_canvas = true;
                        canvas = draw_vertical_line(canvas.pen, &canvas, col);

                    }
                } else if (strcmp(argv[i], "-r") == 0){
                    // parse command line arguments
                    if (!isatty(STDIN_FILENO)){
                        canvas = isSTDIN(&canvas, &err);
                        if(err == OK){
                            can_print_canvas = true;
                        }
                    }

                    int x1, y1, x2, y2;
                    err = OK;
                    if (sscanf(argv[i + 1], "%d,%d,%d,%d", &x1, &y1, &x2, &y2) != 4) {
                        fprintf(stderr, ERROR_MESSAGE_R);
                        fprintf(stderr, USAGE);
                        err = ERR_WITH_VALUE;
                        can_print_canvas = false;
                    }
                    if (x2 < x1 || y2 < y1 || x2 < 0 || y2 < 0) {
                        fprintf(stderr, ERROR_MESSAGE_R);
                        fprintf(stderr, USAGE);
                        err = ERR_WITH_VALUE;
                        can_print_canvas = false;
                    }

                    canvas = drawRectangle(x1, y1, x2, y2, &canvas);

                } else if (strcmp(argv[i], "-l") == 0) {
                    if (!isatty(STDIN_FILENO)){
                        canvas = isSTDIN(&canvas, &err);
                        if(err == OK){
                            can_print_canvas = true;
                        }
                    }

                    int x0, y0, x1, y1;
                    err = OK;

                    if (sscanf(argv[i + 1], "%d,%d,%d,%d", &x0, &y0, &x1, &y1) != 4) {
                        fprintf(stderr, ERROR_MESSAGE_L);
                        fprintf(stderr, USAGE);
                        err = ERR_WITH_VALUE;
                        can_print_canvas = false;
                    }

                    canvas = plotLine(x0, y0, x1, y1, &canvas);

                } else if (strcmp(argv[i], "-c") == 0) {

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
                    }
                } else if (strcmp(argv[i], "-k") == 0){
//                int colorCode;
//                isColor = true;
//                sscanf(&canvas.pen, "%d", &colorCode);
//                print_canvas_in_color(canvas, colorCode);
//
//
//                int i, j;
//                int colorCode = 1;
//                double height = 1;
//                double width = 0.3;
//
//                for (i = 0; i < height; i++) {
//                    printf("\033[0;4%dm", colorCode);
//                    for (j = 0; j < width; j++) {
//                        printf(" ");
//                    }
//                    printf("\033[0m\n");
//                }
                }
            }
        }

        if(can_print_canvas && err == OK && !isColor){
            print_canvas(canvas);
        }
    }
    return err;
}