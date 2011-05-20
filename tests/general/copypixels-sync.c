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

/**
 * @file copypixels-sync.c
 *
 * Test repeated readback of small glCopyPixels calls.  This stresses
 * blit ring IRQ handling on the i965 driver.
 */

#include "piglit-util.h"
#include "piglit-framework.h"

int piglit_width = 64;
int piglit_height = 64;
int piglit_window_mode = GLUT_RGB | GLUT_DOUBLE | GLUT_ALPHA;

enum piglit_result
piglit_display(void)
{
	int x, y;
	float green[4] = {0.0, 1.0, 0.0, 0.0};
	bool pass = true;

	piglit_ortho_projection(piglit_width, piglit_height, GL_FALSE);

	glClearColor(1.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor4fv(green);
	piglit_draw_rect(0, 0, 1, 1);

	for (y = 0; y < piglit_height; y++) {
		for (x = 0; x < piglit_width; x++) {
			if (x == 0 && y == 0)
				continue;

			glRasterPos2i(x, y);
			if (x != 0) {
				glCopyPixels(x - 1, y, 1, 1, GL_COLOR);
			} else {
				glCopyPixels(piglit_width - 1, y - 1, 1, 1,
					     GL_COLOR);
			}

			pass = pass && piglit_probe_pixel_rgba(x, y, green);
		}
	}

	glutSwapBuffers();

	return pass ? PIGLIT_PASS : PIGLIT_FAIL;
}


void
piglit_init(int argc, char **argv)
{
}