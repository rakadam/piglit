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
 *    Ben Widawsky <ben@bwidawsk.net>
 *
 */

#include "piglit-util.h"

#define COPY_BUFFER_SIZE (4<<20)
uint8_t src_data[COPY_BUFFER_SIZE];
uint8_t dest_data[COPY_BUFFER_SIZE];

int piglit_width = 400, piglit_height = 300;
int piglit_window_mode = GLUT_RGB | GLUT_DOUBLE;

enum piglit_result
piglit_display(void)
{
	return PIGLIT_PASS;
}

static enum piglit_result
do_copy()
{
	GLuint buffer_handles[2];

	glGenBuffersARB(2, buffer_handles);

	glBindBufferARB(GL_COPY_READ_BUFFER, buffer_handles[0]);
	glBindBufferARB(GL_COPY_WRITE_BUFFER, buffer_handles[1]);
	glBufferData(GL_COPY_READ_BUFFER, COPY_BUFFER_SIZE, src_data,
		     GL_STREAM_COPY);
	glBufferData(GL_COPY_WRITE_BUFFER, COPY_BUFFER_SIZE, NULL,
		     GL_STREAM_READ);

	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0,
			    COPY_BUFFER_SIZE);

	glGetBufferSubDataARB(GL_COPY_WRITE_BUFFER, 0, COPY_BUFFER_SIZE,
			      dest_data);

	if (memcmp(src_data, dest_data, COPY_BUFFER_SIZE) != 0)
		return PIGLIT_FAIL;

	return PIGLIT_PASS;
}

void
piglit_init(int argc, char *argv[])
{
	if (!GLEW_VERSION_1_5) {
		printf("Requires OpenGL 1.5\n");
		piglit_report_result(PIGLIT_SKIP);
	}

	piglit_require_extension("GL_ARB_vertex_buffer_object");
	piglit_require_extension("GL_ARB_copy_buffer");

	memset(src_data, 0xff, COPY_BUFFER_SIZE);

	piglit_report_result(do_copy());
}
