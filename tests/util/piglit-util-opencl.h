#ifndef PIGLIT_UTIL_OPENCL_H
#define PIGLIT_UTIL_OPENCL_H
#include <CL/cl.h>
#include "piglit-util.h"

void piglit_opencl_test_init(int argc, char **argv);

//set to defaults
#ifndef OPENCL_UTIL_NODEF
#define OPENCL_UTIL_NODEF
#endif

extern const char* opencl_kernel_file;

OPENCL_UTIL_NODEF enum piglit_result (*opencl_display)();
OPENCL_UTIL_NODEF enum piglit_result (*opencl_run_test)();
OPENCL_UTIL_NODEF const char* platform_name;

OPENCL_UTIL_NODEF cl_context opencl_context;
OPENCL_UTIL_NODEF cl_command_queue opencl_command_queue;
OPENCL_UTIL_NODEF cl_program opencl_program;
OPENCL_UTIL_NODEF cl_kernel* opencl_kernels;
OPENCL_UTIL_NODEF cl_uint opencl_kernels_num;

void do_ocl_assert(long long cond, const char* name, int line, const char* file);
void ocl_check(cl_int err, int line, const char* file);
#define OCL_CHECK(err) ocl_check(err, __LINE__, __FILE__)

#ifdef assert
#undef assert
#endif

#define assert(condition) do_ocl_assert((long long)(condition), #condition, __LINE__, __FILE__)

#endif
