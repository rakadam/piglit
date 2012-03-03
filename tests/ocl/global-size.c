#define Type float
#define TEST_ARRAY_SIZE 1024
#define LOCAL_DIMENSIONS {8, 4, 2}
#define GLOBAL_DIMENSIONS {32, 36, 40}
#include "piglit-util-opencl-scan.h"
#include <math.h>

void cpu_reference(Type* in, Type* out, int size)
{
	int dims[] = GLOBAL_DIMENSIONS;
	
	out[0] = dims[0];
	out[1] = dims[1];
	out[2] = dims[2];
}
