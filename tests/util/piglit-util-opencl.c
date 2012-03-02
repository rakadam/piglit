#define OPENCL_UTIL_NODEF extern
#include "piglit-util-opencl.h"

int piglit_width = 400;
int piglit_height = 400;
int piglit_window_mode = GLUT_RGB;

enum piglit_result
piglit_display()
{
  if (opencl_display)
  {
    return opencl_display();
  }
  
  assert(opencl_run_test);
  
  return opencl_run_test();
}

static void opencl_destroy();

#define CASE_ERR(ec) case ec: return #ec;

const char * err_string(cl_int error)
{
   switch(error) {

   CASE_ERR(CL_BUILD_PROGRAM_FAILURE);
   CASE_ERR(CL_DEVICE_NOT_AVAILABLE);
   CASE_ERR(CL_DEVICE_NOT_FOUND);
   CASE_ERR(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST);
   CASE_ERR(CL_INVALID_ARG_INDEX);
   CASE_ERR(CL_INVALID_ARG_SIZE);
   CASE_ERR(CL_INVALID_ARG_VALUE);
   CASE_ERR(CL_INVALID_COMMAND_QUEUE);
   CASE_ERR(CL_INVALID_CONTEXT);
   CASE_ERR(CL_INVALID_DEVICE);
   CASE_ERR(CL_INVALID_DEVICE_TYPE);
   CASE_ERR(CL_INVALID_EVENT_WAIT_LIST);
   CASE_ERR(CL_INVALID_GLOBAL_OFFSET);
   CASE_ERR(CL_INVALID_GLOBAL_WORK_SIZE);
   CASE_ERR(CL_INVALID_IMAGE_SIZE);
   CASE_ERR(CL_INVALID_KERNEL);
   CASE_ERR(CL_INVALID_KERNEL_ARGS);
   CASE_ERR(CL_INVALID_KERNEL_DEFINITION);
   CASE_ERR(CL_INVALID_KERNEL_NAME);
   CASE_ERR(CL_INVALID_MEM_OBJECT);
   CASE_ERR(CL_INVALID_OPERATION);
   CASE_ERR(CL_INVALID_PLATFORM);
   CASE_ERR(CL_INVALID_PROGRAM);
   CASE_ERR(CL_INVALID_PROGRAM_EXECUTABLE);
   CASE_ERR(CL_INVALID_PROPERTY);
   CASE_ERR(CL_INVALID_SAMPLER);
   CASE_ERR(CL_INVALID_VALUE);
   CASE_ERR(CL_INVALID_WORK_DIMENSION);
   CASE_ERR(CL_INVALID_WORK_GROUP_SIZE);
   CASE_ERR(CL_INVALID_WORK_ITEM_SIZE);
   CASE_ERR(CL_MEM_OBJECT_ALLOCATION_FAILURE);
   CASE_ERR(CL_MISALIGNED_SUB_BUFFER_OFFSET);
   CASE_ERR(CL_OUT_OF_HOST_MEMORY);
   CASE_ERR(CL_OUT_OF_RESOURCES);
   CASE_ERR(CL_SUCCESS);

   }
   
   return "Unknown";
}

void ocl_check(cl_int err, int line, const char* file)
{
  if (err != CL_SUCCESS)
  {
    fprintf(stderr, "Error: %s in %s:%i\n", err_string(err), file, line);
    piglit_report_result(PIGLIT_FAIL);
  }
}

void do_ocl_assert(long long cond, const char* name, int line, const char* file)
{
  if (cond == 0)
  {
    fprintf(stderr, "Assertion failed: %s at %s:%i\n", name, file, line);
    piglit_report_result(PIGLIT_FAIL);
  }
}

void piglit_opencl_init();

void
piglit_init(int argc, char **argv)
{
  opencl_display = NULL;
  opencl_run_test = NULL;
  piglit_opencl_destroy = &opencl_destroy;
  platform_name = "any";
  piglit_opencl_mode = PIGLIT_PURE_OPENCL;
  piglit_opencl_init();
  piglit_opencl_test_init(argc, argv);
}

void piglit_opencl_init()
{
  const char** program_src = &opencl_kernel_file;
  cl_platform_id platform = NULL;
  cl_uint numPlatforms;
  cl_platform_id* platforms;
  int i;
  cl_int err = 0;
  cl_device_id device_id[1000];
  cl_uint total_gpu_devices = 0;
  char build_str[100000];
  size_t loglen;
  int match = 0;
  
  OCL_CHECK(clGetPlatformIDs(0, NULL, &numPlatforms));
  
  platforms = malloc(sizeof(cl_platform_id)*numPlatforms);
  OCL_CHECK(clGetPlatformIDs(numPlatforms, platforms, NULL));
  
  for (i = 0; i < numPlatforms; ++i) 
  {
    char pbuf[100]={0};
    
    OCL_CHECK(clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, sizeof(pbuf)-1, pbuf, NULL));
    
    if (strcmp(platform_name, "any") == 0)
    {
      platform = platforms[i];
      match = 1;
      break;
    }
    
    if (strcmp(platform_name, pbuf) == 0)
    {
      platform = platforms[i];
      match = 1;
      break;
    }    
  }
  
  if (!match)
  {
    fprintf(stderr, "Failed to match a platform to: %s\n", platform_name);    
    piglit_report_result(PIGLIT_SKIP);
  }
  
  {
    cl_context_properties cps[3] = 
    {
      CL_CONTEXT_PLATFORM, 
      (cl_context_properties)platform, 
      0
    };
    
    opencl_context = clCreateContextFromType(cps, CL_DEVICE_TYPE_ALL, NULL, NULL, &err);
    OCL_CHECK(err);
    assert(opencl_context);
  }
  
  
  OCL_CHECK(clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1000, device_id, &total_gpu_devices));
  
  opencl_command_queue = clCreateCommandQueue(opencl_context, device_id[0], 
                                              0, /* Command queue properties */
                                              &err); /* Error code */
  OCL_CHECK(err);
  assert(opencl_command_queue);
  
  opencl_program = clCreateProgramWithSource(opencl_context,
                                              1, /* Number of strings */
                                              program_src,
                                              NULL, /* String lengths, 0 means all the
                                                      * strings are NULL terminated. */
                                              &err);
  OCL_CHECK(err);
  assert(opencl_program);
  
  err = clBuildProgram(opencl_program,
                          1, /* Number of devices */
                          &device_id[0],
                          NULL, /* options */
                          NULL, /* callback function when compile is complete */
                          NULL);

  
  OCL_CHECK(clGetProgramBuildInfo(opencl_program,
                                device_id[0],
                                CL_PROGRAM_BUILD_LOG,
                                100000, /* Size of output string */
                                build_str, /* pointer to write the log to */
                                &loglen)); /* Number of bytes written to the log */

  build_str[loglen] = 0;
  
  if (strlen(build_str))
  {
//    printf("build log: %s\n", build_str);
  }
  
  OCL_CHECK(err);
  
  opencl_kernels = malloc(sizeof(cl_kernel)*10000);
  OCL_CHECK(clCreateKernelsInProgram(opencl_program, 10000, opencl_kernels, &opencl_kernels_num));
}

static void opencl_destroy()
{
  int i = 0;
  
  for (i = 0; i < opencl_kernels_num; i++)
  {
    OCL_CHECK(clReleaseKernel(opencl_kernels[i]));
  }
  
  OCL_CHECK(clReleaseProgram(opencl_program));
  OCL_CHECK(clReleaseCommandQueue(opencl_command_queue));
  OCL_CHECK(clReleaseContext(opencl_context));
}

