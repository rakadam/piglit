#define Type float
#define TEST_ARRAY_SIZE (16*18*20)
#define LOCAL_DIMENSIONS {2, 2, 2}
#define GLOBAL_DIMENSIONS {16, 18, 20}
#include "piglit-util-opencl-scan.h"
#include <math.h>

void cpu_reference(Type* in, Type* out, int size)
{
	int dims[] = GLOBAL_DIMENSIONS;
	int x,y,z;
	
	for (x = 0; x < dims[0]; x++)
	for (y = 0; y < dims[1]; y++)
	for (z = 0; z < dims[2]; z++)
	{
		out[x+y*16+z*16*18] = x*20*18+y*20+z;
	}
}
