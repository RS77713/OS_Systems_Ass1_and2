#define _GNU_SOURCE
#include "points.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <stddef.h>
#include <errno.h>

/*
 * find_farthest_point
 *
 * Opens the file, maps it into virtual memory using mmap(), then parses it
 * line by line. Each line is expected to contain three integers: x y z.
 * For each point, the function computes distance^2 = x*x + y*y + z*z and
 * keeps track of the maximum.
 *
 * The file is not read into a manual buffer with read()/malloc; instead,
 * mmap() lets the kernel bring parts of the file into memory on demand.
 */
int find_farthest_point(const char *filename, FarthestPoint *out) {
    if (!filename || !out) {
        fprintf(stderr, "find_farthest_point: invalid arguments\n");
        return -1;
    }

    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("fstat");
        close(fd);
        return -1;
    }

    size_t filesize = st.st_size;
    if (filesize == 0) {
        fprintf(stderr, "File is empty\n");
        close(fd);
        return -1;
    }

    /* Map entire file into memory. The kernel pages it in as needed. */
    char *data = mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, fd, 0);
    if (data == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return -1;
    }

    long long max_d2 = -1;
    long long line = 0;

    size_t i = 0;
    while (i < filesize) {
        line++;

        long long x = 0, y = 0, z = 0;
        int sign = 1;

        /* Read three integers from the current line (x, y, z). */
        for (int coord = 0; coord < 3; coord++) {
            /* Skip whitespace. */
            while (i < filesize && isspace((unsigned char)data[i])) {
                /* newline is also whitespace; that's fine */
                i++;
            }

            /* Handle optional sign. */
            sign = 1;
            if (i < filesize && (data[i] == '-' || data[i] == '+')) {
                if (data[i] == '-') sign = -1;
                i++;
            }

            /* Parse digits. */
            long long val = 0;
            int has_digit = 0;
            while (i < filesize && isdigit((unsigned char)data[i])) {
                val = val * 10 + (data[i] - '0');
                i++;
                has_digit = 1;
            }

            if (!has_digit) {
                /* malformed line; skip until end of line */
                while (i < filesize && data[i] != '\n') {
                    i++;
                }
                if (i < filesize && data[i] == '\n') i++;
                /* move to next line */
                goto next_line;
            }

            val *= sign;

            if (coord == 0) x = val;
            else if (coord == 1) y = val;
            else z = val;
        }

        /* Skip to end of line (if not already there). */
        while (i < filesize && data[i] != '\n') {
            i++;
        }
        if (i < filesize && data[i] == '\n') i++;

        {
            long long d2 = x * x + y * y + z * z;
            if (d2 > max_d2) {
                max_d2 = d2;
                out->x = x;
                out->y = y;
                out->z = z;
                out->line_number = line;
                out->max_distance_squared = d2;
            }
        }

    next_line:
        /* loop continues; i already at next line */
        ;
    }

    munmap(data, filesize);
    close(fd);

    if (max_d2 < 0) {
        fprintf(stderr, "No valid points found in file\n");
        return -1;
    }

    return 0;
}
