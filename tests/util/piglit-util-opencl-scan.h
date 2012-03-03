#include <stdlib.h>
#include "piglit-util-opencl.h"

static void cpu_reference(Type* in, Type* out, int size);

Type t_abs(Type x)
{
	if (x < 0)
	{
		x = -x;
	}
	
	return x;
}

enum piglit_result opencl_run_test()
{
	cl_int size;
	int i;
	cl_int err;
	cl_mem gpu_input_buffer;
	cl_mem gpu_output_buffer;
	Type* cpu_input_buffer;
	Type* cpu_output_buffer;
	Type* cpu_output_buffer_ref;
	int has_nonzero = 0;
	
#ifdef LOCAL_DIMENSIONS
	size_t global_work_size[] = GLOBAL_DIMENSIONS;
	size_t local_work_size[] = LOCAL_DIMENSIONS;
	int dimnum = sizeof(global_work_size) / sizeof(size_t);
	assert(sizeof(global_work_size) == sizeof(local_work_size));
#else
	size_t global_work_size[] = {1024};
	size_t local_work_size[] = {256};
	int dimnum = 1;
#endif
	
#ifdef TEST_ARRAY_SIZE
	size = TEST_ARRAY_SIZE;
#else
  size = 1024*1024;
#endif
	
	assert(opencl_kernels_num);

	gpu_input_buffer = clCreateBuffer(opencl_context, CL_MEM_READ_ONLY, size*sizeof(Type), NULL, &err); 
	gpu_output_buffer = clCreateBuffer(opencl_context, CL_MEM_WRITE_ONLY, size*sizeof(Type), NULL, &err); 
	OCL_CHECK(err);
	assert(gpu_input_buffer);
	assert(gpu_output_buffer);
  
	cpu_input_buffer = malloc(size*sizeof(Type));
	cpu_output_buffer = malloc(size*sizeof(Type));
	cpu_output_buffer_ref = malloc(size*sizeof(Type));
  
	for (i = 0; i < size; i++)
	{
		cpu_input_buffer[i] = (Type)(rand()%100000 - 50000) / ((Type)100);
		cpu_output_buffer[i] = 0;
		cpu_output_buffer_ref[i] = 0;
	}
	
	OCL_CHECK(clEnqueueWriteBuffer(opencl_command_queue, gpu_input_buffer, CL_TRUE,  0, size*sizeof(Type), cpu_input_buffer, 0, NULL, NULL));
	OCL_CHECK(clEnqueueWriteBuffer(opencl_command_queue, gpu_output_buffer, CL_TRUE,  0, size*sizeof(Type), cpu_output_buffer, 0, NULL, NULL));
  
	OCL_CHECK(clSetKernelArg(opencl_kernels[0],
                            0, /* Arg index */
                            sizeof(cl_mem),
                            &gpu_input_buffer));
	
	OCL_CHECK(clSetKernelArg(opencl_kernels[0],
                            1, /* Arg index */
                            sizeof(cl_mem),
                            &gpu_output_buffer));
	
	OCL_CHECK(clSetKernelArg(opencl_kernels[0],
                            2, /* Arg index */
                            sizeof(size),
                            &size));
  
	OCL_CHECK(clEnqueueNDRangeKernel(opencl_command_queue,
                                  opencl_kernels[0],
                                  dimnum, /* Number of dimensions */
                                  NULL, /* Global work offset */
                                  global_work_size,
                                  local_work_size, /* local work size */
                                  0, /* Events in wait list */
                                  NULL, /* Wait list */
                                  NULL)); /* Event object for this event */
  
  OCL_CHECK(clEnqueueReadBuffer(opencl_command_queue, gpu_output_buffer, CL_TRUE,  0, size*sizeof(Type), cpu_output_buffer, 0, NULL, NULL));
  
	cpu_reference(cpu_input_buffer, cpu_output_buffer_ref, size);

	
	for (i = 0; i < size; i++)
	{
		Type diff = t_abs(cpu_output_buffer_ref[i] - cpu_output_buffer[i]);
		Type mag = t_abs(cpu_output_buffer_ref[i]) + t_abs(cpu_output_buffer[i])/((Type)2);
		
		if (mag == (-mag)) ///means that it's zero
		{
			mag = 1;
		}
		
		if (cpu_output_buffer_ref[i] != (-cpu_output_buffer_ref[i]))
		{
			has_nonzero = 1;
		}
		
#if Type == float || Type == double
		if (!isfinite(cpu_output_buffer_ref[i]) && !isfinite(cpu_output_buffer[i]))
		{
			continue;
		}
		
		if (diff / mag > 1E-5)
		{
			fprintf(stderr, "index: %i expected: %lf got: %lf relative error: %lf\n", i, (double)cpu_output_buffer_ref[i], (double)cpu_output_buffer[i], (double)(diff/mag));
			return PIGLIT_FAIL;
		}
#else
		if (diff)
		{
			fprintf(stderr, "index: %i expected: %ll got: %ll diff: %ll\n", i, (long long)cpu_output_buffer_ref[i], (long long)cpu_output_buffer[i], (long long)diff);
			return PIGLIT_FAIL;
		}
#endif
	}
	
	for (i = 0; i < 30; i++)
	{
#if Type == float || Type == double
		printf("in: %lf out: %lf\n", (double)cpu_input_buffer[i], (double)cpu_output_buffer[i]);
#else
		printf("in: %ll out: %ll\n", (long long)cpu_input_buffer[i], (long long)cpu_output_buffer[i]);
#endif
	}
	
	assert(has_nonzero && "Test self sanity check");
	
	OCL_CHECK(clReleaseMemObject(gpu_input_buffer));
	OCL_CHECK(clReleaseMemObject(gpu_output_buffer));

	return PIGLIT_PASS;
}

void piglit_opencl_preinit(int argc, char **argv)
{
}

void piglit_opencl_test_init(int argc, char **argv)
{
}
