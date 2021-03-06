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

/** @file glsl-vs-mvp-statechange.c
 *
 * Tests that changing just gl_ModelViewProjectionMatrix state is
 * reflected in program execution.
 */

#include "piglit-util.h"

int piglit_width = 100, piglit_height = 100;
int piglit_window_mode = GLUT_RGB | GLUT_DOUBLE | GLUT_ALPHA;

enum piglit_result
piglit_display(void)
{
	GLboolean pass = GL_TRUE;
	static const float green[] = {0.0, 1.0, 0.0, 0.0};

	glClearColor(1.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glMatrixMode(GL_PROJECTION);

        glLoadIdentity();
        glOrtho(0, 1, 0, 1, -1, 1);
	piglit_draw_rect(0.0, 0.0, 0.5, 1.0);

        glLoadIdentity();
        glOrtho(0, 2, 0, 2, -1, 1);
	piglit_draw_rect(1.0, 0.0, 1.0, 2.0);

	pass &= piglit_probe_rect_rgba(0, 0, piglit_width, piglit_height,
				       green);

	glutSwapBuffers();

	return pass ? PIGLIT_PASS : PIGLIT_FAIL;
}

void
piglit_init(int argc, char **argv)
{
	GLint vs, fs, prog;
	const char *fs_source =
		"void main()\n"
		"{\n"
		"	gl_FragColor = vec4(0.0, 1.0, 0.0, 0.0);\n"
		"}\n";

	if (!GLEW_VERSION_2_0) {
		printf("Requires OpenGL 2.0\n");
		piglit_report_result(PIGLIT_SKIP);
	}

	piglit_ortho_projection(piglit_width, piglit_height, GL_FALSE);

	vs = piglit_compile_shader(GL_VERTEX_SHADER,
				   "shaders/glsl-mvp.vert");
	fs = piglit_compile_shader_text(GL_FRAGMENT_SHADER, fs_source);

	prog = piglit_link_simple_program(vs, fs);

	glUseProgram(prog);
}
