/*
	Copyright (c) 2022 ByteBit/xtreme8000

	This file is part of CavEX.

	CavEX is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	CavEX is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with CavEX.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <assert.h>
#include <malloc.h>
#include <math.h>
#include <string.h>

#include "../../../game/game_state.h"
#include "../../../lodepng/lodepng.h"
#include "../../../util.h"
#include "../../input.h"
#include "../gfx.h"

static GLuint textures[8];

static GLuint load_tex(const char* filename, bool nearest) {
	unsigned width, height;
	unsigned char* img;
	if(lodepng_decode32_file(&img, &width, &height, filename))
		printf("error loading texture %s\n", filename);

	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
				 GL_UNSIGNED_BYTE, img);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
					nearest ? GL_NEAREST : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
					nearest ? GL_NEAREST : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	free(img);

	return tex;
}

static void gfx_load_textures() {
	textures[0] = load_tex("terrain.png", true);
	textures[1] = load_tex("default.png", true);
	textures[2] = load_tex("anim.png", true);
	textures[3] = load_tex("gui.png", true);
	textures[4] = load_tex("gui_2.png", true);
	textures[5] = load_tex("items.png", true);
}

static void testtest(GLuint shader) {
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if(isCompiled == GL_FALSE) {
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		char log[maxLength];
		glGetShaderInfoLog(shader, maxLength, &maxLength, log);
		printf("%s\n", log);

		// Provide the infolog in whatever manor you deem best.
		// Exit with failure.
		glDeleteShader(shader); // Don't leak the shader.
	}
}

static GLuint create_shader(const char* vertex, const char* fragment) {
	GLuint v = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(v, 1, (const GLchar* const*)&vertex, NULL);
	glCompileShader(v);
	testtest(v);

	GLuint f = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(f, 1, (const GLchar* const*)&fragment, NULL);
	glCompileShader(f);
	testtest(f);

	GLuint program = glCreateProgram();
	glAttachShader(program, v);
	glAttachShader(program, f);

	glBindAttribLocation(program, 0, "a_pos");
	glBindAttribLocation(program, 1, "a_color");
	glBindAttribLocation(program, 2, "a_texcoord");
	glBindAttribLocation(program, 3, "a_light");

	glLinkProgram(program);
	return program;
}

static int window_width = 854;
static int window_height = 480;
GLFWwindow* window;

int gfx_width() {
	return window_width;
}

int gfx_height() {
	return window_height;
}

static enum input_button glfw_key_translate(int key) {
	switch(key) {
		case GLFW_KEY_W: return IB_FORWARD;
		case GLFW_KEY_S: return IB_BACKWARD;
		case GLFW_KEY_A: return IB_LEFT;
		case GLFW_KEY_D: return IB_RIGHT;
		case GLFW_KEY_SPACE: return IB_JUMP;
		case GLFW_KEY_LEFT_SHIFT: return IB_INVENTORY;
		case GLFW_KEY_ENTER: return IB_HOME;
		default: return IB_MAX;
	}
}

static enum input_button glfw_button_translate(int key) {
	switch(key) {
		case GLFW_MOUSE_BUTTON_LEFT: return IB_ACTION1;
		case GLFW_MOUSE_BUTTON_RIGHT: return IB_ACTION2;
		default: return IB_MAX;
	}
}

static double last_mouse_x = 0, last_mouse_y = 0;

static void framebuffer_size_callback(GLFWwindow* window, int width,
									  int height) {
	window_width = width;
	window_height = height;
	glViewport(0, 0, gfx_width(), gfx_height());
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action,
						 int mods) {
	if(glfw_key_translate(key) != IB_MAX)
		input_set_status(glfw_key_translate(key), action != GLFW_RELEASE);

	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		if(glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		} else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}
}

static void scroll_callback(GLFWwindow* window, double xoffset,
							double yoffset) {
	// TODO: buttons are not released
	if(glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
		if(yoffset > 0) {
			input_set_status(IB_SCROLL_LEFT, GLFW_PRESS);
		} else if(yoffset < 0) {
			input_set_status(IB_SCROLL_RIGHT, GLFW_PRESS);
		}
	}
}

static void mouse_button_callback(GLFWwindow* window, int button, int action,
								  int mods) {
	if(glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED
	   && glfw_button_translate(button) != IB_MAX)
		input_set_status(glfw_button_translate(button), action != GLFW_RELEASE);
}

static GLuint shader_prog;

void gfx_setup() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	window
		= glfwCreateWindow(window_width, window_height, GAME_NAME, NULL, NULL);
	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	if(glfwRawMouseMotionSupported())
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	if(glewInit())
		printf("Could not load extended OpenGL functions!\n");

	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));

	void* vertex = file_read("vertex.shader");
	assert(vertex);
	void* fragment = file_read("fragment.shader");
	assert(fragment);

	shader_prog = create_shader(vertex, fragment);
	free(vertex);
	free(fragment);
	glUseProgram(shader_prog);

	gfx_clear_buffers(255, 255, 255);
	gfx_bind_texture(TEXTURE_TERRAIN);
	gfx_texture(true);
	gfx_alpha_test(true);

	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	gfx_culling(true);

	glDepthFunc(GL_LEQUAL);

	glViewport(0, 0, gfx_width(), gfx_height());

	gfx_load_textures();

	glUniform1i(glGetUniformLocation(shader_prog, "tex"), 0);
}

void gfx_update_light(float daytime, const float* light_lookup) {
	assert(daytime > -GLM_FLT_EPSILON && daytime < 1.0F + GLM_FLT_EPSILON
		   && light_lookup);

	float colors[256];

	for(int sky = 0; sky < 16; sky++) {
		for(int torch = 0; torch < 16; torch++) {
			colors[torch * 16 + sky]
				= fmaxf(light_lookup[torch], light_lookup[sky] * daytime);
		}
	}

	glUniform1fv(glGetUniformLocation(shader_prog, "lighting"), 256, colors);
}

void gfx_clear_buffers(uint8_t r, uint8_t g, uint8_t b) {
	glClearColor(r / 255.0F, g / 255.0F, b / 255.0F, 1.0F);
}

void gfx_finish(bool vsync) {
	glfwSwapBuffers(window);
	gfx_write_buffers(true, true, true);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glfwPollEvents();

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	if(glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
		input_set_joystick((xpos - last_mouse_x) * 0.001F,
						   -(ypos - last_mouse_y) * 0.001F);

	} else {
		input_set_joystick(0, 0);
	}

	last_mouse_x = xpos;
	last_mouse_y = ypos;
}

void gfx_flip_buffers(float* gpu_wait, float* vsync_wait) {
	*gpu_wait = 0;
	*vsync_wait = 0;
}

void gfx_bind_texture(enum gfx_texture tex) {
	switch(tex) {
		case TEXTURE_TERRAIN: glBindTexture(GL_TEXTURE_2D, textures[0]); break;
		case TEXTURE_FONT: glBindTexture(GL_TEXTURE_2D, textures[1]); break;
		case TEXTURE_ANIM: glBindTexture(GL_TEXTURE_2D, textures[2]); break;
		case TEXTURE_GUI: glBindTexture(GL_TEXTURE_2D, textures[3]); break;
		case TEXTURE_GUI2: glBindTexture(GL_TEXTURE_2D, textures[4]); break;
		case TEXTURE_ITEMS: glBindTexture(GL_TEXTURE_2D, textures[5]); break;
	}
}

void gfx_mode_world() {
	gfx_write_buffers(true, true, true);
	gfx_matrix_texture(false, NULL);
}

void gfx_mode_gui() {
	gfx_fog(false);

	mat4 proj;
	glm_ortho(0, gfx_width(), gfx_height(), 0, -256, 256, proj);
	gfx_matrix_projection(proj, false);
	gfx_matrix_modelview(GLM_MAT4_IDENTITY);

	gfx_lighting(false);
	gfx_blending(MODE_BLEND);
	gfx_alpha_test(true);
	gfx_write_buffers(true, false, false);
}

void gfx_matrix_projection(mat4 proj, bool is_perspective) {
	assert(proj);
	glUniformMatrix4fv(glGetUniformLocation(shader_prog, "proj"), 1, GL_FALSE,
					   (float*)proj);
}

void gfx_matrix_modelview(mat4 mv) {
	assert(mv);
	glUniformMatrix4fv(glGetUniformLocation(shader_prog, "mv"), 1, GL_FALSE,
					   (float*)mv);
}

void gfx_matrix_texture(bool enable, mat4 tex) {
	if(enable) {
		assert(tex);
		glUniformMatrix4fv(glGetUniformLocation(shader_prog, "texm"), 1,
						   GL_FALSE, (float*)tex);
	} else {
		glUniformMatrix4fv(glGetUniformLocation(shader_prog, "texm"), 1,
						   GL_FALSE, (float*)GLM_MAT4_IDENTITY);
	}
}

void gfx_fog_color(uint8_t r, uint8_t g, uint8_t b) {
	glUniform3f(glGetUniformLocation(shader_prog, "fog_color"), r / 255.0F,
				g / 255.0F, b / 255.0F);
}

void gfx_fog_pos(float dx, float dz, float distance) {
	assert(distance > 0);

	glUniform2f(glGetUniformLocation(shader_prog, "fog_delta"), dx, dz);
	glUniform1f(glGetUniformLocation(shader_prog, "fog_distance"), distance);
}

void gfx_fog(bool enable) {
	glUniform1i(glGetUniformLocation(shader_prog, "enable_fog"), enable);
}

void gfx_blending(enum gfx_blend mode) {
	switch(mode) {
		case MODE_BLEND:
			glDisable(GL_COLOR_LOGIC_OP);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;
		case MODE_BLEND2:
			glDisable(GL_COLOR_LOGIC_OP);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			break;
		case MODE_INVERT:
			glDisable(GL_BLEND);
			glEnable(GL_COLOR_LOGIC_OP);
			glLogicOp(GL_INVERT);
			break;
		case MODE_OFF:
			glDisable(GL_COLOR_LOGIC_OP);
			glDisable(GL_BLEND);
			break;
	}
}

void gfx_alpha_test(bool enable) {
	glUniform1i(glGetUniformLocation(shader_prog, "enable_alpha"), enable);
}

void gfx_write_buffers(bool color, bool depth, bool depth_test) {
	glColorMask(color, color, color, color);
	glDepthMask(depth);

	if(depth_test) {
		glEnable(GL_DEPTH_TEST);
	} else {
		glDisable(GL_DEPTH_TEST);
	}
}

void gfx_depth_range(float near, float far) {
	glDepthRange(near, far);
}

void gfx_texture(bool enable) {
	glUniform1i(glGetUniformLocation(shader_prog, "enable_texture"), enable);
}

void gfx_lighting(bool enable) {
	glUniform1i(glGetUniformLocation(shader_prog, "enable_lighting"), enable);
}

void gfx_culling(bool enable) {
	if(enable) {
		glEnable(GL_CULL_FACE);
	} else {
		glDisable(GL_CULL_FACE);
	}
}

void gfx_scissor(bool enable, uint32_t x, uint32_t y, uint32_t width,
				 uint32_t height) {
	if(enable) {
		glEnable(GL_SCISSOR_TEST);
		glScissor(x, y, width, height);
	} else {
		glDisable(GL_SCISSOR_TEST);
	}
}

void gfx_draw_lines(size_t vertex_count, const int16_t* vertices,
					const uint8_t* colors) {
	assert(vertices && colors);
	glLineWidth(2.0F);

	assert(vertex_count < 256);

	float tmp[vertex_count * 3];
	for(size_t k = 0; k < vertex_count * 3; k++)
		tmp[k] = vertices[k] / 256.0F;

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, tmp);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, colors);

	glDrawArrays(GL_LINES, 0, vertex_count);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void gfx_draw_quads(size_t vertex_count, const int16_t* vertices,
					const uint8_t* colors, const uint16_t* texcoords) {
	assert(vertices && colors && texcoords);

	assert(vertex_count < 256);

	float tmp[vertex_count * 3];
	for(size_t k = 0; k < vertex_count * 3; k++)
		tmp[k] = texcoords[k] / 256.0F;

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 3, GL_SHORT, GL_FALSE, 0, vertices);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, colors);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, tmp);

	glDrawArrays(GL_QUADS, 0, vertex_count);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}