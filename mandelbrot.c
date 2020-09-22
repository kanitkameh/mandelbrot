// https://en.wikipedia.org/wiki/Plotting_algorithms_for_the_Mandelbrot_set#Continuous_(smooth)_coloring

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define X 1920
#define Y 1080

#define R_MAX 1.5
#define R_MIN -2
#define I_MAX 1.0
#define I_MIN -I_MAX

#define MAX_ITER 8000

typedef struct {
    int r;
    int g;
    int b;
} rgb;

double lerp(double v0, double v1, double t) {
    return (1 - t) * v0 + t * v1;
}


rgb mandelbrot(int px, int py, rgb* palette){
    double x = 0; // complex (c)
    double y = 0;

    double x0 = R_MIN + (px * ((R_MAX - R_MIN)/(X*1.0))); // complex scale of Px
    double y0 = I_MIN + (py * ((I_MAX - I_MIN)/(Y*1.0))); // complex scale of Py

    double i = 0;
    double x2 = 0;
    double y2 = 0;
  
    while(x*x + y*y <= 20 && i < MAX_ITER){
        y = 2*x*y + y0;
        x = x2 - y2 + x0;
        x2 = x*x;
        y2 = y*y;
        i++;
    }
    if(i < MAX_ITER){
        double log_zn = log(x*x + y*y) / 2.0;
        double nu = log(log_zn / log(2.0))/log(2.0);
        i += 1.0 - nu;
    }
    rgb c1 = palette[(int)i];
    rgb c2;
    if((int)i + 1 > MAX_ITER){
        c2 = palette[(int)i];
    }else{
        c2 = palette[((int)i)+1];
    }

    double mod = i - ((int)i) ; // cant mod doubles
    return (rgb){
        .r = (int)lerp(c1.r, c2.r, mod),
        .g = (int)lerp(c1.g, c2.g, mod),
        .b = (int)lerp(c1.b, c2.b, mod),
    };

}

rgb* make_pallete(int size){
    rgb* palette = (rgb*)malloc(sizeof(rgb)*MAX_ITER+1);
    for(int i=0;i<size+1;i++){
        if (i >= size){
            palette[i] = (rgb){.r=0,.g=0,.b=0};
            continue;
        }
        double j;
        if(i == 0){
            j = 3.0;
        }else{
            j = 3.0 * (log(i)/log(size-1.0));
        }

        if (j<1){
            palette[i] = (rgb){
                    .r = 0,
                    .g = 255 * j,
                    .b = 0
            };
        }else if(j<2){
            palette[i] = (rgb){
                    .r = 255*(j-1),
                    .g = 255,
                    .b = 0,
            };
        }else{
            palette[i] = (rgb){
                    .r = 255 * (j-2),
                    .g = 255,
                    .b = 255,
            };
        }
    }
}


int main(){
    rgb** colors = (rgb**)malloc(sizeof(rgb*)*Y);
    for(int y = 0;y < Y;y++){
        colors[y] = (rgb*)malloc(sizeof(rgb)*X);
    }
    rgb* palette = make_pallete(MAX_ITER);

    printf("finished palette\n");
    for(int Py = 0; Py < Y; Py++){
        for(int Px = 0; Px < X; Px++){
            colors[Py][Px] = mandelbrot(Px, Py, palette);
        }
    }
    printf("finished calcs\n");
    FILE* fout;
    fout = fopen("output/ms.ppm", "w");
    fprintf(fout, "P3\n");
    fprintf(fout, "%d %d\n", X, Y);
    fprintf(fout, "255\n");
    for(int y = 0; y < Y; y++){
        for(int x = 0; x < X; x++){
            fprintf(fout, "%ld %ld %ld\n", (int)colors[y][x].r, (int)colors[y][x].g, (int)colors[y][x].b);
        }
    }
}
