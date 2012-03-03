#define Type float
#include "piglit-util-opencl-scan.h"
#include <math.h>

void cpu_reference(Type* in, Type* out, int size)
{
	int i;
	
	for (i = 0; i < size; i++)
	{
		float v = in[i];
		
		if (v < 0)
		{
			v = -v;
		}
		
		out[i] = sqrt(v);
	}
}
