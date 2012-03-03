#ifndef PIGLIT_UTIL_OPENCL_H
#define PIGLIT_UTIL_OPENCL_H
#include <CL/cl.h>
#include "piglit-util.h"

void piglit_opencl_test_init(int argc, char **argv);


extern const char* opencl_kernel_file;

/**
 * Specifies the OpenCL test mode, should be set in piglit_init
 */
extern enum piglit_opencl_mode_ {PIGLIT_NO_OPENCL, PIGLIT_PURE_OPENCL, PIGLIT_OPENCL_OPENGL} piglit_opencl_mode;
enum piglit_result opencl_run_test();
void piglit_opencl_destroy();
void piglit_opencl_preinit(int argc, char **argv);
void piglit_preinit(int argc, char **argv);
enum piglit_result opencl_run_test();

extern const char* platform_name;
extern cl_context opencl_context;
extern cl_command_queue opencl_command_queue;
extern cl_program opencl_program;
extern cl_kernel* opencl_kernels;
extern cl_uint opencl_kernels_num;

void do_ocl_assert(long long cond, const char* name, int line, const char* file);
void ocl_check(cl_int err, int line, const char* file);
#define OCL_CHECK(err) ocl_check(err, __LINE__, __FILE__)

#ifdef assert
#undef assert
#endif

#define assert(condition) do_ocl_assert((long long)(condition), #condition, __LINE__, __FILE__)

#endif
