#include "piglit-util-opencl.h"

static enum piglit_result run_test()
{
  return PIGLIT_PASS;
}

void piglit_opencl_test_init(int argc, char **argv)
{
  opencl_run_test = run_test;
}
