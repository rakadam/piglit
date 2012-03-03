#include "piglit-util-opencl.h"

enum piglit_result opencl_run_test()
{
  int size;
  cl_int err;
  cl_mem buf;
  float* cpu_buffer;
  size_t global_work_size = 1;
  size_t local_work_size = 1;
  
  size = sizeof(float)*1024;
  assert(opencl_kernels_num);
  
  buf = clCreateBuffer(opencl_context, CL_MEM_WRITE_ONLY, size, NULL, &err); 
  OCL_CHECK(err);
  assert(buf);
  
  cpu_buffer = malloc(size);
  
  cpu_buffer[0] = 1;
  
  OCL_CHECK(clEnqueueWriteBuffer(opencl_command_queue, buf, CL_TRUE,  0, size, cpu_buffer, 0, NULL, NULL));
  
  OCL_CHECK(clSetKernelArg(opencl_kernels[0],
                            0, /* Arg index */
                            sizeof(cl_mem),
                            &buf));
  
  OCL_CHECK(clEnqueueNDRangeKernel(opencl_command_queue,
                                  opencl_kernels[0],
                                  1, /* Number of dimensions */
                                  NULL, /* Global work offset */
                                  &global_work_size,
                                  &local_work_size, /* local work size */
                                  0, /* Events in wait list */
                                  NULL, /* Wait list */
                                  NULL)); /* Event object for this event */
  
  OCL_CHECK(clEnqueueReadBuffer(opencl_command_queue, buf, CL_TRUE,  0, size, cpu_buffer, 0, NULL, NULL));
  
  assert(cpu_buffer[0] == 42);
  
  OCL_CHECK(clReleaseMemObject(buf));
  
  return PIGLIT_PASS;
}

void piglit_opencl_preinit(int argc, char **argv)
{
}

void piglit_opencl_test_init(int argc, char **argv)
{
}
