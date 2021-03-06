/*
 * Copyright © 2009 Marek Olšák (maraeo@gmail.com)
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
 *    Marek Olšák <mareao@gmail.com>
 *
 */

/** @file glsl-sin.c
 *
 * Tests "sin" in both vertex and fragment shaders
 */

#include "piglit-util.h"

int piglit_width = 100, piglit_height = 100;
int piglit_window_mode = GLUT_RGB | GLUT_DOUBLE;

static char vs_code[] =
    "uniform float a;\n"
    "varying float val;\n"

    "void main()\n"
    "{\n"
    "    gl_Position = ftransform();\n"
    "    val = sin(a);\n"
    "}\n";

static char fs_code[] =
    "uniform float a;\n"
    "varying float val;\n"

    "void main()\n"
    "{\n"
    "    gl_FragColor = vec4(val, sin(a), -1.0, 1.0) * 0.5 + 0.5;\n"
    "}\n";

static GLuint setup_shaders()
{
    GLuint vs, fs, prog;

    vs = piglit_compile_shader_text(GL_VERTEX_SHADER, vs_code);
    fs = piglit_compile_shader_text(GL_FRAGMENT_SHADER, fs_code);
    prog = piglit_link_simple_program(vs, fs);

    glUseProgram(prog);
    return prog;
}

static GLboolean test()
{
    GLint prog, location;
    GLboolean pass = GL_TRUE;
    int i;
    float color[4] = {0, 0, 0, 1};
    float DEGREES_30 = 0.523598776;

    prog = setup_shaders();
    location = glGetUniformLocation(prog, "a");

    for (i = 0; i <= 50; i++) {
        glUniform1f(location, (i - 25) * DEGREES_30);
        piglit_draw_rect((i % 10) * 10, (i / 10) * 10, 10, 10);
    }
    assert(glGetError() == 0);

    for (i = 0; i <= 50; i++) {
        color[0] = color[1] = sin((i - 25) * DEGREES_30) * 0.5 + 0.5;
        pass = piglit_probe_pixel_rgb((i % 10) * 10 + 5, (i / 10) * 10 + 5, color) && pass;
    }

    return pass;
}

enum piglit_result piglit_display(void)
{
    GLboolean pass;

    piglit_ortho_projection(piglit_width, piglit_height, GL_FALSE);

    glClearColor(0.5, 0.5, 0.5, 0.5);
    glClear(GL_COLOR_BUFFER_BIT);
    
    pass = test();

    glutSwapBuffers();

    return pass ? PIGLIT_PASS : PIGLIT_FAIL;
}

void piglit_init(int argc, char **argv)
{
    if (!GLEW_VERSION_2_0) {
        printf("Requires OpenGL 2.0\n");
        piglit_report_result(PIGLIT_SKIP);
    }
}

