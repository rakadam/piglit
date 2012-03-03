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
  
  cpu_buffer[0] = -1;
  cpu_buffer[1] = 0;
  cpu_buffer[2] = 1;
  
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
  
  assert(cpu_buffer[3] == 1);
  assert(cpu_buffer[4] == 0);
  assert(cpu_buffer[5] == 0);

  assert(cpu_buffer[6] == 0);
  assert(cpu_buffer[7] == 0);
  assert(cpu_buffer[8] == 1);

  assert(cpu_buffer[9] == 1);
  assert(cpu_buffer[10] == 1);
  assert(cpu_buffer[11] == 0);
  
  assert(cpu_buffer[12] == 0);
  assert(cpu_buffer[13] == 1);
  assert(cpu_buffer[14] == 1);
  
  assert(cpu_buffer[15] == 1);
  assert(cpu_buffer[16] == 0);
  assert(cpu_buffer[17] == 0);

  assert(cpu_buffer[18] == 0);
  assert(cpu_buffer[19] == 0);
  assert(cpu_buffer[20] == 1);

  assert(cpu_buffer[21] == 1);
  assert(cpu_buffer[22] == 1);
  assert(cpu_buffer[23] == 0);
  
  assert(cpu_buffer[24] == 0);
  assert(cpu_buffer[25] == 1);
  assert(cpu_buffer[26] == 1);
  
  OCL_CHECK(clReleaseMemObject(buf));
  
  return PIGLIT_PASS;
}

void piglit_opencl_preinit(int argc, char **argv)
{
}

void piglit_opencl_test_init(int argc, char **argv)
{
}
