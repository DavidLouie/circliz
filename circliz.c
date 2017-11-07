// circliz.c: Takes in an image and creates a modified version out of circles
// Usage: 1 mandatory command-line arg, 3 optional
//  - First argument (mandatory) is the source image file
//  - Second argument is the filename of the modified image
//  - Third argument is the number of iterations
//  - Fourth argument is included if you want the radius of the circle to change during iterations


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

#define DEFAULT_ITERATIONS 500000
#define MAX_RADIUS 50
#define DEFAULT_RADIUS 10
#define DEFAULT_NAME "output.png"

struct Pixel {
    int r;
    int g;
    int b;
};

// EFFECTS: returns 1 if (x, y) is in bounds, 0 otherwise
int in_bounds(int x, int y, int width, int height) {
    if (x < 0 || x >= width || y < 0 || y >= height)
        return 0;
    return 1;
}


// REQUIRES: x and y inside bounds of the image
// EFFECTS: Writes the given pixel at (x, y) in the data
void put_pixel(int x, int y, int width, unsigned char* data, struct Pixel p) {
    int pos = 3*width*y + 3*x;
    data[pos] = p.r;
    data[pos + 1] = p.g;
    data[pos + 2] = p.b;
}

// EFFECTS: Draws a circle of pixels p centered at (x0, y0) with the given radius onto data
void draw_circle(int x0, int y0, int width, int height, int radius, unsigned char* data, struct Pixel p) {
    int x, y;
    for (y = -radius; y <= radius; y++) {
        for (x = -radius; x <= radius; x++) {
            if (x*x + y*y <= radius*radius) {
                if (!in_bounds(x0+x, y0+y, width, height))
                    break;
                put_pixel(x0+x, y0+y, width, data, p);
            }
        }
    }
}

// EFFECTS: returns the euclidian color distance between the same position in two images
int euclid_dist(int pos, unsigned char* image, unsigned char* source) {
    return (image[pos] - source[pos])*(image[pos]-source[pos])
        + (image[pos+1]-source[pos+1])*(image[pos+1]-source[pos+1])
        + (image[pos+2]-source[pos+2])*(image[pos+2]-source[pos+2]);
}

// EFFECTS: copies the pixels of the circle with the given radius centered at (x0, y0) from source to dest
void copy(int x0, int y0, int width, int height, int radius, unsigned char* dest, unsigned char* source) {
    int x, y;
    for (y = -radius; y <= radius; y++) {
        for (x = -radius; x <= radius; x++) {
            if (x*x + y*y <= radius*radius) {
                if (!in_bounds(x0+x, y0+y, width, height))
                    break;
                int pos = 3*width*(y0+y) + 3*(x0+x);                    // converting (x,y) coordinate to 0-indexed position in the image arrays
                dest[pos] = source[pos];
                dest[pos+1] = source[pos+1];
                dest[pos+2] = source[pos+2];
            }
        }
    }
}


// EFFECTS: if the circle at (x0, y0) in image1 is more similar to source, copies the circle to image2
//          otherwise copies the circle at the same position in image2 to image1
void compare(int x0, int y0, int width, int height, int radius, unsigned char* image1, unsigned char* image2, unsigned char* source) {
    int x, y;
    int sum_dist_src= 0;
    int sum_dist_i2= 0;
    for (y = -radius; y <= radius; y++) {
        for (x = -radius; x <= radius; x++) {
            if (x*x + y*y <= radius*radius) {
                if (!in_bounds(x0+x, y0+y, width, height))
                    break;
                int pos = 3*width*(y0+y) + 3*(x0+x);
                sum_dist_i2 += euclid_dist(pos, image1, image2);
                sum_dist_src += euclid_dist(pos, image1, source);
            }
        }
    }
    if (sum_dist_src < sum_dist_i2) {                           // if image1 is more similar to source
        copy(x0, y0, width, height, radius, image2, image1);
    } else {
        copy(x0, y0, width, height, radius, image1, image2);
    }
}


int main(int argc, char **argv) {

    char *filename = DEFAULT_NAME;
    int iterations = DEFAULT_ITERATIONS;
    int radius = DEFAULT_RADIUS;
    int changing_rad = 0;

    if (argc == 1) {
        printf("Pass an image file as the first command line argument.\n");
        return 0;
    }
    if (argc > 2) {
        filename = argv[2];
    }
    if (argc > 3) {
        iterations = atoi(argv[3]);
        if (iterations < 100000) {
            printf("Number of iterations must be at least 100,000.\n");
            return 0;
        }
    }
    if (argc > 4) {
        changing_rad = 1;
    }

    // loading image into data
    // 3 array elements for each pixel: for example, pixel at (0,0) stored as:
    // data[0] = red component of pixel, data[1] = green component, data[2] = blue component
    int width, height, n;
    unsigned char *data = stbi_load(argv[1], &width, &height, &n, 3); 
    unsigned char *image1 = malloc(3*width*height+2);
    unsigned char *image2 = malloc(3*width*height+2);

    time_t start = time(NULL);              // used to determine runtime
    srand(start);

    
    int i, j;
    // If user specified for a changing rad, use a radius of MAX_RADIUS for half the iterations
    // Then iterate with progressively smaller circles for the last half of the iterations
    if (changing_rad) {
        int radius = MAX_RADIUS;
        j = iterations / 2;
    }

    for (i = 0; i < iterations; i++) {
        if (iterations / 2 == i) {
            printf("Half done: %d seconds estimated remaining.\n", time(NULL) - start);
        }
        if (changing_rad) {                     // change the radius at certain points
            if (i == iterations - 45000) {
                radius = 32;
            }
            if (i == iterations - 15000) {
                radius = 16;
            }
            if ( i == iterations - 5000) {
                radius = 8;
            }
            if (i == j) {
                radius = 40;
            }
        }

        // get a random color from the source image
        int r = rand() % (width*height);
        struct Pixel p;                     
        p.r = data[3*r];
        p.g = data[3*r+1];
        p.b = data[3*r+2];

        // use r to get the coordinates of the centre of the circle to be drawn
        int x0, y0;
        x0 = r % width;         
        y0 = r / width;

        draw_circle(x0, y0, width, height, radius, image1, p);
        compare(x0, y0, width, height, radius, image1, image2, data);
    }

    stbi_write_png(filename, width, height, n, image2, width*3);
    stbi_image_free(data);
    free(image1);
    free(image2);

    printf("Iterations completed in %d seconds.\n", time(NULL) - start);
    return 0;
}
