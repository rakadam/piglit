/*
 * Copyright © 2009 Intel Corporation
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
 *    Eric Anholt <eric@anholt.net>
 *
 */

/** @file scissor-stencil-clear.c
 *
 * Tests that glScissor properly affects glClear of the stencil buffer.
 */

#include "piglit-util.h"

int piglit_width = 100, piglit_height = 100;
int piglit_window_mode = GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL;

enum piglit_result
piglit_display(void)
{
	GLboolean pass = GL_TRUE;
	static float red[]   = {1.0, 0.0, 0.0, 0.0};
	static float green[] = {0.0, 1.0, 0.0, 0.0};
	static float blue[]  = {0.0, 0.0, 1.0, 0.0};

	/* whole window gray -- none should be visible */
	glClearColor(0.5, 0.5, 0.5, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	/* Clear stencil to 0, which will be drawn green */
	glClearStencil(0);
	glClear(GL_STENCIL_BUFFER_BIT);

	/* quad at 10, 10 that will be drawn blue. */
	glEnable(GL_SCISSOR_TEST);
	glScissor(10, 10, 10, 10);
	glClearStencil(1);
	glClear(GL_STENCIL_BUFFER_BIT);

	/* 0-sized quad at 10, 30 that shouldn't be drawn (red) */
	glScissor(10, 30, 0, 0);
	glClearStencil(2);
	glClear(GL_STENCIL_BUFFER_BIT);

	glDisable(GL_SCISSOR_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	/* First quad -- stencil == 0 gets green */
	glStencilFunc(GL_EQUAL, 0, ~0);
	glColor4fv(green);
	piglit_draw_rect(0, 0, piglit_width, piglit_height);

	/* Second quad -- stencil == 1 gets blue */
	glStencilFunc(GL_EQUAL, 1, ~0);
	glColor4fv(blue);
	piglit_draw_rect(0, 0, piglit_width, piglit_height);

	/* Last quad -- stencil == 2 gets red (shouldn't occur!) */
	glStencilFunc(GL_EQUAL, 2, ~0);
	glColor4fv(red);
	piglit_draw_rect(0, 0, piglit_width, piglit_height);

	pass &= piglit_probe_rect_rgb(0, 0, piglit_width, 10, green);

	pass &= piglit_probe_rect_rgb(0, 10, 10, 10, green);
	pass &= piglit_probe_rect_rgb(10, 10, 10, 10, blue);
	pass &= piglit_probe_rect_rgb(20, 10, piglit_width-20, 10, green);

	pass &= piglit_probe_rect_rgb(0, 20, piglit_width, piglit_height - 20,
				      green);

	glutSwapBuffers();

	return pass ? PIGLIT_PASS : PIGLIT_FAIL;
}


static void reshape(int width, int height)
{
	piglit_width = width;
	piglit_height = height;

	piglit_ortho_projection(width, height, GL_FALSE);
}

void
piglit_init(int argc, char **argv)
{
	reshape(piglit_width, piglit_height);
}
