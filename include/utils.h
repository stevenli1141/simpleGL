#ifndef GUARD_UTILS_H
#define GUARD_UTILS_H

#include <math.h>
#include <stdlib.h>

#define MAX(x, y) ((x) < (y) ? (y) : (x))
#define MIN(x, y) ((x) > (y) ? (y) : (x))
#define RETURN_IF_ZERO(expr) if (!(expr)) { return; }

#ifndef M_PI
#define M_PI 3.1415926535897931159979635
#endif

#define RAND(n) (rand() % n)

#endif
