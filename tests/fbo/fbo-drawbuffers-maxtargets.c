/*
 * Copyright © 2010 Intel Corporation
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

/** @file fbo-drawbuffers-maxtargets.c
 *
 * Tests that drawing the same color to as many render targets as
 * possible with ARB_draw_buffers and fixed function fragment works.
 */

#include <string.h>
#include "piglit-util.h"

int piglit_width = 256;
int piglit_height = 256;
int piglit_window_mode = GLUT_RGB | GLUT_DOUBLE;

static GLint max_targets;

static char *vs_source =
	"void main()\n"
	"{\n"
	"	gl_Position = gl_Vertex;\n"
	"}\n";

static char *fs_source =
	"void main()\n"
	"{\n"
	"	for (int i = 0; i < %d; i++) {\n"
	"		gl_FragData[i] = vec4(0.0, 1.0, 0.0, 0.0);\n"
	"	}\n"
	"}\n";

static GLuint
attach_texture(int i)
{
	GLuint tex;

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
		     32, 32, 0,
		     GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
				  GL_COLOR_ATTACHMENT0_EXT + i,
				  GL_TEXTURE_2D,
				  tex,
				  0);
	assert(glGetError() == 0);

	return tex;
}

static void
generate_and_display_drawbuffers(int count)
{
	GLuint tex[16], fb, fs, vs, prog;
	GLenum attachments[16], status, error;
	char *fs_count_source;
	int i;

	glGenFramebuffersEXT(1, &fb);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);

	for (i = 0; i < count; i++) {
		tex[i] = attach_texture(i);
		attachments[i] = GL_COLOR_ATTACHMENT0 + i;
	}

	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if (status != GL_FRAMEBUFFER_COMPLETE_EXT) {
		fprintf(stderr, "fbo incomplete (status = 0x%04x)\n", status);
		piglit_report_result(PIGLIT_SKIP);
	}

	glDrawBuffersARB(count, attachments);

	/* Clear all to red so we see if the shader rendering happens. */
	glClearColor(1.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	/* Build the shader that spams green to all outputs. */
	vs = piglit_compile_shader_text(GL_VERTEX_SHADER, vs_source);

	fs_count_source = malloc(strlen(fs_source) + 5);
	sprintf(fs_count_source, fs_source, count);
	fs = piglit_compile_shader_text(GL_FRAGMENT_SHADER, fs_count_source);
	free(fs_count_source);

	prog = piglit_link_simple_program(vs, fs);
	glUseProgram(prog);

	error = glGetError();
	if (error) {
		fprintf(stderr, "glUseProgram error: 0x%x\n", error);
		piglit_report_result(PIGLIT_FAIL);
	}

	/* Now render to all the color buffers. */
	piglit_draw_rect(-1, -1, 2, 2);

	/* OK, now draw each of these textures to the winsys framebuffer. */
	glUseProgram(0);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	piglit_ortho_projection(piglit_width, piglit_height, GL_FALSE);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glEnable(GL_TEXTURE_2D);
	for (i = 0; i < count; i++) {
		glBindTexture(GL_TEXTURE_2D, tex[i]);
		piglit_draw_rect_tex(16 * i, 16 * (count - 1),
				     16, 16,
				     0, 0,
				     1, 1);
	}
	glDisable(GL_TEXTURE_2D);

	for (i = 0; i < count; i++) {
		glDeleteTextures(1, &tex[i]);
	}
	glDeleteFramebuffersEXT(1, &fb);
}

enum piglit_result
piglit_display(void)
{
	GLboolean pass = GL_TRUE;
	float green[] = {0, 1, 0, 0};
	int count, i;

	glClearColor(0.5, 0.5, 0.5, 0.5);
	glClear(GL_COLOR_BUFFER_BIT);

	for (count = 1; count <= max_targets; count++) {
		generate_and_display_drawbuffers(count);
	}

	for (count = 1; count <= max_targets; count++) {
		for (i = 0; i < count; i++) {
			pass = pass &&
				piglit_probe_pixel_rgb(16 * i + 8,
						       16 * (count - 1) + 8,
						       green);
		}
	}

	glutSwapBuffers();

	return pass ? PIGLIT_PASS : PIGLIT_FAIL;
}

void
piglit_init(int argc, char **argv)
{
	GLint max_attachments;

	printf("The result should be increasing lengths of rows of green\n"
	       "boxes as the test increases the number of drawbuffers \n"
	       "targets used.\n");

	piglit_ortho_projection(piglit_width, piglit_height, GL_FALSE);

	if (!GLEW_VERSION_2_0) {
		printf("Requires OpenGL 2.0\n");
		piglit_report_result(PIGLIT_SKIP);
	}

	piglit_require_extension("GL_EXT_framebuffer_object");
	piglit_require_extension("GL_ARB_draw_buffers");

	glGetIntegerv(GL_MAX_DRAW_BUFFERS_ARB, &max_targets);
	if (max_targets < 2)
		piglit_report_result(PIGLIT_SKIP);

	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &max_attachments);
	if (max_targets > max_attachments)
		max_targets = max_attachments;

	if (max_targets > 16)
		max_targets = 16;
}
