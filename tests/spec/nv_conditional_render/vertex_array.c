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
 */

#include "piglit-util.h"

/**
 * @file vertex_array.c
 *
 * Tests that conditional rendering appropriately affects vertex array
 * commands.
 *
 * From the NV_conditional_render spec:
 *
 *     "If the result (SAMPLES_PASSED) of the query is zero, all
 *      rendering commands between BeginConditionalRenderNV and the
 *      corresponding EndConditionalRenderNV are discarded.  In this
 *      case, Begin, End, all vertex array commands performing an
 *      implicit Begin and End, DrawPixels (section 3.6), Bitmap
 *      (section 3.7), Clear (section 4.2.3), Accum (section 4.2.4),
 *      CopyPixels (section 4.3.3), EvalMesh1, and EvalMesh2 (section
 *      5.1) have no effect."
 */

int piglit_width = 32;
int piglit_height = 32;
int piglit_window_mode = GLUT_DOUBLE | GLUT_RGB | GLUT_ALPHA;

enum piglit_result
piglit_display(void)
{
	bool pass = true;
	float green[4] = {0.0, 1.0, 0.0, 0.0};
	GLuint q;

	glClearColor(0.5, 0.5, 0.5, 0.5);
	glClear(GL_COLOR_BUFFER_BIT);

	glGenQueries(1, &q);

	/* Generate query pass: draw bottom half of screen. */
	glColor4f(0.0, 1.0, 0.0, 0.0);
	glBeginQuery(GL_SAMPLES_PASSED, q);
	piglit_draw_rect(-1, -1, 2, 1);
	glEndQuery(GL_SAMPLES_PASSED);

	/* Conditional render that should draw top half of screen. */
	glBeginConditionalRenderNV(q, GL_QUERY_WAIT_NV);
	piglit_draw_rect(-1, 0, 2, 1);
	glEndConditionalRenderNV();

	/* Generate query fail */
	glBeginQuery(GL_SAMPLES_PASSED, q);
	glEndQuery(GL_SAMPLES_PASSED);

	/* Conditional render that should not draw full screen. */
	glBeginConditionalRenderNV(q, GL_QUERY_WAIT_NV);
	glColor4f(1.0, 0.0, 0.0, 0.0);
	piglit_draw_rect(-1, -1, 2, 2);
	glEndConditionalRenderNV();

	pass = piglit_probe_rect_rgba(0, 0, piglit_width, piglit_height,
				      green);

	glutSwapBuffers();

	glDeleteQueries(1, &q);

	return pass ? PIGLIT_PASS : PIGLIT_FAIL;
}

void
piglit_init(int argc, char **argv)
{
	if (!GLEW_VERSION_2_0) {
		printf("Requires OpenGL 2.0\n");
		piglit_report_result(PIGLIT_SKIP);
	}

	piglit_require_extension("GL_NV_conditional_render");
}
