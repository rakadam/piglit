#include "piglit-util-opencl.h"

enum piglit_result opencl_run_test()
{
  int size;
  cl_int err;
  cl_mem buf;
  int i = 0;
  size = 1024;
  
  buf = clCreateBuffer(opencl_context, CL_MEM_WRITE_ONLY, size, NULL, &err); 
  OCL_CHECK(err);
  assert(buf);
  
  char* cpu_buffer = malloc(size);
  
  for (i = 0; i < size; i++)
  {
    cpu_buffer[i] = (char)(i % 256);
  }
  
  OCL_CHECK(clEnqueueWriteBuffer(opencl_command_queue, buf, CL_TRUE,  0, size, cpu_buffer, 0, NULL, NULL));
  
  for (i = 0; i < size; i++)
  {
    cpu_buffer[i] = 0;
  }
  
  OCL_CHECK(clEnqueueReadBuffer(opencl_command_queue, buf, CL_TRUE,  0, size, cpu_buffer, 0, NULL, NULL));
  
  for (i = 0; i < size; i++)
  {
    assert(cpu_buffer[i] == (char)(i % 256));
  }
  
  OCL_CHECK(clReleaseMemObject(buf));
  
  return PIGLIT_PASS;
}

void piglit_opencl_preinit(int argc, char **argv)
{
}

void piglit_opencl_test_init(int argc, char **argv)
{
}
