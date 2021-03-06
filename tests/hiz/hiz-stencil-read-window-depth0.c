/*
 * Copyright © 2011 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 * Authors:
 *    Chad Versace <chad.versace@intel.com>
 */

/**
 * \file hiz-stencil-read-window-depth0.c
 *
 * Check that 1) correct values are written to and that 2) expected values are
 * read back from the stencil buffer when using a window framebuffer for which
 * no depth buffer was requested.
 *
 * \author Chad Versace <chad.versace@intel.com>
 */

#include "piglit-util.h"
#include "hiz/hiz-util.h"

int piglit_width = 400;
int piglit_height = 400;
int piglit_window_mode = GLUT_RGB | GLUT_ALPHA | GLUT_STENCIL;

void
piglit_init(int argc, char **argv)
{
	/* empty */
}

enum piglit_result
piglit_display()
{
	if (hiz_run_test_stencil_read_window())
		return PIGLIT_PASS;
	else
		return PIGLIT_FAIL;
}
