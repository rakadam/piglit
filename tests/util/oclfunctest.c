#include <stdio.h>
#include <string.h>
#include <assert.h>

int main(int argc, char* argv[])
{
	char *name;
	char *c_code;
	char *cl_code;
	char *type;
	FILE *f;
	char buf[1000];
	
	assert(argc == 4 || argc == 5);
	
	name = argv[1];
	type = argv[2];
	c_code = argv[3];
	
	if (argc == 5)
	{
		cl_code = argv[4];
	}
	else
	{
		cl_code = c_code;
	}
	
	sprintf(buf, "%s.c", name);
	f = fopen(buf, "w");
	assert(f);
	
	fprintf(f, "const char* opencl_kernel_file = \n");
	
	if (strstr(type, "double"))
	{
		fprintf(f, "\"#pragma OPENCL EXTENSION cl_khr_fp64 : enable\\n\"\n");
	}
	
	fprintf(f, "\"#define Type %s\\n\"\n", type);
	fprintf(f, "\"kernel void functest(global Type* in, global Type* out, int size) {for (int i = get_global_id(0); i < size; i += get_global_size(0)) { Type x = in[i]; out[i] = %s;}}\";\n", cl_code);
	fprintf(f, "#define Type %s\n", type);
	fprintf(f, "#include \"piglit-util-opencl-scan.h\"\n");
	fprintf(f, "#include <math.h>\n");
	fprintf(f, "void cpu_reference(Type* in, Type* out, int size) { int i; for (i = 0; i < size; i++) { Type x = in[i]; out[i] = %s; }}\n", c_code);

	fclose(f);
	
	return 0;
}

