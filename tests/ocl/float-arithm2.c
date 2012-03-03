#define Type float
#include "piglit-util-opencl-scan.h"

void cpu_reference(Type* in, Type* out, int size)
{
	int i;
	
	for (i = 0; i < size; i++)
	{
		out[i] = in[i]*3.1425f + in[i]*in[i]*in[i]*in[i] - 1/(in[i]+0.000001f);
	}
}
