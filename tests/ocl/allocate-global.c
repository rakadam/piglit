#include "piglit-util-opencl.h"

enum piglit_result opencl_run_test()
{
  int size;
  cl_int err;
  cl_mem buf;
  
  size = 1024;
  
  buf = clCreateBuffer(opencl_context, CL_MEM_WRITE_ONLY, size, NULL, &err); 
  OCL_CHECK(err);
  assert(buf);
  OCL_CHECK(clReleaseMemObject(buf));
  
  buf = clCreateBuffer(opencl_context, CL_MEM_READ_ONLY, size, NULL, &err); 
  OCL_CHECK(err);
  assert(buf);
  OCL_CHECK(clReleaseMemObject(buf));

  buf = clCreateBuffer(opencl_context, CL_MEM_READ_WRITE, size, NULL, &err); 
  OCL_CHECK(err);
  assert(buf);
  OCL_CHECK(clReleaseMemObject(buf));

  return PIGLIT_PASS;
}


void piglit_opencl_preinit(int argc, char **argv)
{
}

void piglit_opencl_test_init(int argc, char **argv)
{
}
