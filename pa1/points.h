#ifndef POINTS_H
#define POINTS_H

typedef struct {
    long long x, y, z;
    long long line_number;
    long long max_distance_squared;
} FarthestPoint;

/* Returns 0 on success, -1 on error. */
int find_farthest_point(const char *filename, FarthestPoint *out);

#endif /* POINTS_H */
