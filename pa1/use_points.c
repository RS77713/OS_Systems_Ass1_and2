#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "points.h"

/*
 * use_points.c
 *
 * Console application that:
 *  - loads the shared library libpoints.so at runtime using dlopen()
 *  - looks up the function find_farthest_point() with dlsym()
 *  - calls the function to process a file with 3D points
 *  - prints the line number and coordinates of the farthest point
 */

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <points_file>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    const char *libname = "./libpoints.so";

    /* Load shared library at runtime. */
    void *handle = dlopen(libname, RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "dlopen error: %s\n", dlerror());
        return 1;
    }

    dlerror(); /* clear existing errors */

    typedef int (*find_fn_t)(const char *, FarthestPoint *);
    find_fn_t find_fn = (find_fn_t)dlsym(handle, "find_farthest_point");
    const char *err = dlerror();
    if (err) {
        fprintf(stderr, "dlsym error: %s\n", err);
        dlclose(handle);
        return 1;
    }

    FarthestPoint fp;
    printf("Processing file: %s\n", filename);

    int rc = find_fn(filename, &fp);
    if (rc == 0) {
        printf("Farthest point found:\n");
        printf("  Line number: %lld\n", fp.line_number);
        printf("  Coordinates: (%lld, %lld, %lld)\n",
               fp.x, fp.y, fp.z);
        printf("  Distance^2 : %lld\n", fp.max_distance_squared);
    } else {
        printf("Error while processing file.\n");
    }

    dlclose(handle);
    return 0;
}
