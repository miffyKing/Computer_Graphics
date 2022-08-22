//
//  DrawScene.cpp
//
//  Written for CSE4170
//  Department of Computer Science and Engineering
//  Copyright © 2022 Sogang University. All rights reserved.
//

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "LoadScene.h"

// Begin of shader setup
#include "Shaders/LoadShaders.h"
#include "ShadingInfo.h"

extern SCENE scene;

// for simple shaders
GLuint h_ShaderProgram_simple, h_ShaderProgram_TXPS; // handle to shader program
GLint loc_ModelViewProjectionMatrix, loc_primitive_color; // indices of uniform variables

// for Phong Shading (Textured) shaders
#define NUMBER_OF_LIGHT_SUPPORTED 13
GLint loc_global_ambient_color;
loc_light_Parameters loc_light[NUMBER_OF_LIGHT_SUPPORTED];
loc_Material_Parameters loc_material;
GLint loc_ModelViewProjectionMatrix_TXPS, loc_ModelViewMatrix_TXPS, loc_ModelViewMatrixInvTrans_TXPS;
GLint loc_texture;
GLint loc_flag_texture_mapping;
GLint loc_flag_fog;

/***********************내 전역변수 *************/

#define N_TIGER_FRAMES 12
GLuint tiger_VBO, tiger_VAO;
int tiger_n_triangles[N_TIGER_FRAMES];
int tiger_vertex_offset[N_TIGER_FRAMES];
GLfloat* tiger_vertices[N_TIGER_FRAMES];

#define N_WOLF_FRAMES 17
GLuint wolf_VBO, wolf_VAO;
int wolf_n_triangles[N_WOLF_FRAMES];
int wolf_vertex_offset[N_WOLF_FRAMES];
GLfloat* wolf_vertices[N_WOLF_FRAMES];
Material_Parameters material_wolf;

GLuint godzilla_VBO, godzilla_VAO;
int godzilla_n_triangles;
GLfloat* godzilla_vertices;
Material_Parameters material_godzilla;

GLuint tank_VBO, tank_VAO;
int tank_n_triangles;
GLfloat* tank_vertices;
Material_Parameters material_tank;

// optimus object
GLuint optimus_VBO, optimus_VAO;
int optimus_n_triangles;
GLfloat* optimus_vertices;
Material_Parameters material_optimus;

// bus object
GLuint bus_VBO, bus_VAO;
int bus_n_triangles;
GLfloat* bus_vertices;
Material_Parameters material_bus;


#define LOC_VERTEX 0
#define LOC_NORMAL 1
#define LOC_TEXCOORD 2
int cur_frame_tiger = 0;
int cur_frame_spider = 0;
int cur_frame_wolf = 0;
float rotation_angle_tiger = 0.0f;
float zoom_factor = 1.0f;

int movable_mode = 0;
int tiger_mode = 0;
int looktiger_mode = 0;
//float rotation_angle_cow;
/************************************************/

// include glm/*.hpp only if necessary
// #include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, lookAt, perspective, etc.
// ViewProjectionMatrix = ProjectionMatrix * ViewMatrix
glm::mat4 ViewProjectionMatrix, ViewMatrix, ProjectionMatrix;
// ModelViewProjectionMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix
glm::mat4 ModelViewProjectionMatrix; // This one is sent to vertex shader when ready.
glm::mat4 ModelViewMatrix;
glm::mat3 ModelViewMatrixInvTrans;

#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f

/*********************************  START: camera *********************************/
typedef enum {
	CAMERA_1,
	CAMERA_2,
	CAMERA_3,
	CAMERA_4,
	CAMERA_5,
	CAMERA_6,
	CAMERA_TIGER,
	CAMERA_LOOKTIGER,
	NUM_CAMERAS
} CAMERA_INDEX;

typedef struct _Camera {
	float pos[3];
	float uaxis[3], vaxis[3], naxis[3];
	float fovy, aspect_ratio, near_c, far_c;
	int move, rotation_axis;
} Camera;

Camera camera_info[NUM_CAMERAS];
Camera current_camera;

using glm::mat4;
void set_ViewMatrix_from_camera_frame(void) {
	ViewMatrix = glm::mat4(current_camera.uaxis[0], current_camera.vaxis[0], current_camera.naxis[0], 0.0f,
		current_camera.uaxis[1], current_camera.vaxis[1], current_camera.naxis[1], 0.0f,
		current_camera.uaxis[2], current_camera.vaxis[2], current_camera.naxis[2], 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	 
	ViewMatrix = glm::translate(ViewMatrix, glm::vec3(-current_camera.pos[0], -current_camera.pos[1], -current_camera.pos[2]));
}

void set_current_camera(int camera_num) {
	Camera* pCamera = &camera_info[camera_num];

	memcpy(&current_camera, pCamera, sizeof(Camera));
	set_ViewMatrix_from_camera_frame();
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
}

void initialize_camera(void) {
	//CAMERA_1 : original view
	Camera* pCamera = &camera_info[CAMERA_1];
	for (int k = 0; k < 3; k++)
	{
		pCamera->pos[k] = scene.camera.e[k];
		pCamera->uaxis[k] = scene.camera.u[k];
		pCamera->vaxis[k] = scene.camera.v[k];
		pCamera->naxis[k] = scene.camera.n[k];
	}

	pCamera->move = 0;
	pCamera->fovy = TO_RADIAN * scene.camera.fovy, pCamera->aspect_ratio = scene.camera.aspect, pCamera->near_c = 0.1f; pCamera->far_c = 30000.0f;

	//CAMERA_2 : bronze statue view        
	pCamera = &camera_info[CAMERA_2];
	pCamera->pos[0] = -593.047974f; pCamera->pos[1] = -3758.460938f; pCamera->pos[2] = 474.587830f;
	pCamera->uaxis[0] = 0.864306f; pCamera->uaxis[1] = -0.502877f; pCamera->uaxis[2] = 0.009328f;
	pCamera->vaxis[0] = 0.036087f; pCamera->vaxis[1] = 0.080500f; pCamera->vaxis[2] = 0.996094f;
	pCamera->naxis[0] = -0.501662f; pCamera->naxis[1] = -0.860599f; pCamera->naxis[2] = 0.087724f;
	pCamera->move = 0;
	pCamera->fovy = TO_RADIAN * scene.camera.fovy, pCamera->aspect_ratio = scene.camera.aspect, pCamera->near_c = 0.1f; pCamera->far_c = 30000.0f;

	//CAMERA_3 : bronze statue view
	pCamera = &camera_info[CAMERA_3];
	pCamera->pos[0] = -1.463161f; pCamera->pos[1] = 1720.545166f; pCamera->pos[2] = 683.703491f;
	pCamera->uaxis[0] = -0.999413f; pCamera->uaxis[1] = -0.032568f; pCamera->uaxis[2] = -0.010066f;
	pCamera->vaxis[0] = -0.011190f; pCamera->vaxis[1] = -0.034529f; pCamera->vaxis[2] = 0.999328f;
	pCamera->naxis[0] = -0.032200f; pCamera->naxis[1] = 0.998855f; pCamera->naxis[2] = -0.034872f;
	pCamera->move = 0;
	pCamera->fovy = TO_RADIAN * scene.camera.fovy, pCamera->aspect_ratio = scene.camera.aspect, pCamera->near_c = 0.1f; pCamera->far_c = 30000.0f;

	//CAMERA_4 : top view
	pCamera = &camera_info[CAMERA_4];
	pCamera->pos[0] = 0.0f; pCamera->pos[1] = 0.0f; pCamera->pos[2] = 18300.0f;
	pCamera->uaxis[0] = 1.0f; pCamera->uaxis[1] = 0.0f; pCamera->uaxis[2] = 0.0f;
	pCamera->vaxis[0] = 0.0f; pCamera->vaxis[1] = 1.0f; pCamera->vaxis[2] = 0.0f;
	pCamera->naxis[0] = 0.0f; pCamera->naxis[1] = 0.0f; pCamera->naxis[2] = 1.0f;
	pCamera->move = 0;
	pCamera->fovy = TO_RADIAN * scene.camera.fovy, pCamera->aspect_ratio = scene.camera.aspect, pCamera->near_c = 0.1f; pCamera->far_c = 30000.0f;

	//CAMERA_5 : front view
	pCamera = &camera_info[CAMERA_5];
	pCamera->pos[0] = 0.0f; pCamera->pos[1] = 11700.0f; pCamera->pos[2] = 0.0f;
	pCamera->uaxis[0] = 1.0f; pCamera->uaxis[1] = 0.0f; pCamera->uaxis[2] = 0.0f;
	pCamera->vaxis[0] = 0.0f; pCamera->vaxis[1] = 0.0f; pCamera->vaxis[2] = 1.0f;
	pCamera->naxis[0] = 0.0f; pCamera->naxis[1] = 1.0f; pCamera->naxis[2] = 0.0f;
	pCamera->move = 0;
	pCamera->fovy = TO_RADIAN * scene.camera.fovy, pCamera->aspect_ratio = scene.camera.aspect, pCamera->near_c = 0.1f; pCamera->far_c = 30000.0f;

	//CAMERA_6 : side view
	pCamera = &camera_info[CAMERA_6];
	pCamera->pos[0] = 14600.0f; pCamera->pos[1] = 0.0f; pCamera->pos[2] = 0.0f;
	pCamera->uaxis[0] = 0.0f; pCamera->uaxis[1] = 1.0f; pCamera->uaxis[2] = 0.0f;
	pCamera->vaxis[0] = 0.0f; pCamera->vaxis[1] = 0.0f; pCamera->vaxis[2] = 1.0f;
	pCamera->naxis[0] = 1.0f; pCamera->naxis[1] = 0.0f; pCamera->naxis[2] = 0.0f;
	pCamera->move = 0;
	pCamera->fovy = TO_RADIAN * scene.camera.fovy, pCamera->aspect_ratio = scene.camera.aspect, pCamera->near_c = 0.1f; pCamera->far_c = 30000.0f;

	set_current_camera(CAMERA_1);
	//set_ViewMatrix_from_camera_frame();
	//ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
}
/*********************************  END: camera *********************************/

/******************************  START: shader setup ****************************/
// Begin of Callback function definitions
void prepare_shader_program(void) {
	char string[256];

	ShaderInfo shader_info[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
		{ GL_NONE, NULL }
	};

	ShaderInfo shader_info_TXPS[3] = {
	{ GL_VERTEX_SHADER, "Shaders/Phong_Tx.vert" },
	{ GL_FRAGMENT_SHADER, "Shaders/Phong_Tx.frag" },
	{ GL_NONE, NULL }
	};

	h_ShaderProgram_simple = LoadShaders(shader_info);
	glUseProgram(h_ShaderProgram_simple);

	loc_ModelViewProjectionMatrix = glGetUniformLocation(h_ShaderProgram_simple, "u_ModelViewProjectionMatrix");
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram_simple, "u_primitive_color");

	h_ShaderProgram_TXPS = LoadShaders(shader_info_TXPS);
	loc_ModelViewProjectionMatrix_TXPS = glGetUniformLocation(h_ShaderProgram_TXPS, "u_ModelViewProjectionMatrix");
	loc_ModelViewMatrix_TXPS = glGetUniformLocation(h_ShaderProgram_TXPS, "u_ModelViewMatrix");
	loc_ModelViewMatrixInvTrans_TXPS = glGetUniformLocation(h_ShaderProgram_TXPS, "u_ModelViewMatrixInvTrans");

	loc_global_ambient_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_global_ambient_color");

	for (int i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		sprintf(string, "u_light[%d].light_on", i);
		loc_light[i].light_on = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].position", i);
		loc_light[i].position = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].ambient_color", i);
		loc_light[i].ambient_color = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].diffuse_color", i);
		loc_light[i].diffuse_color = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].specular_color", i);
		loc_light[i].specular_color = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].spot_direction", i);
		loc_light[i].spot_direction = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].spot_exponent", i);
		loc_light[i].spot_exponent = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].spot_cutoff_angle", i);
		loc_light[i].spot_cutoff_angle = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].light_attenuation_factors", i);
		loc_light[i].light_attenuation_factors = glGetUniformLocation(h_ShaderProgram_TXPS, string);
	}

	loc_material.ambient_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.ambient_color");
	loc_material.diffuse_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.diffuse_color");
	loc_material.specular_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.specular_color");
	loc_material.emissive_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.emissive_color");
	loc_material.specular_exponent = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.specular_exponent");

	loc_texture = glGetUniformLocation(h_ShaderProgram_TXPS, "u_base_texture");
	loc_flag_texture_mapping = glGetUniformLocation(h_ShaderProgram_TXPS, "u_flag_texture_mapping");
	loc_flag_fog = glGetUniformLocation(h_ShaderProgram_TXPS, "u_flag_fog");
}
/*******************************  END: shder setup ******************************/

/****************************  START: geometry setup ****************************/
#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))
#define INDEX_VERTEX_POSITION	0
#define INDEX_NORMAL			1
#define INDEX_TEX_COORD			2

bool b_draw_grid = false;

//axes
GLuint axes_VBO, axes_VAO;
GLfloat axes_vertices[6][3] = {
	{ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f },
	{ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }
};
GLfloat axes_color[3][3] = { { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } };

void prepare_axes(void) {
	// Initialize vertex buffer object.
	glGenBuffers(1, &axes_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, axes_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axes_vertices), &axes_vertices[0][0], GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &axes_VAO);
	glBindVertexArray(axes_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, axes_VBO);
	glVertexAttribPointer(INDEX_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(INDEX_VERTEX_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	fprintf(stdout, " * Loaded axes into graphics memory.\n");
}

void draw_axes(void) {
	if (!b_draw_grid)
		return;

	glUseProgram(h_ShaderProgram_simple);
	ModelViewMatrix = glm::scale(ViewMatrix, glm::vec3(8000.0f, 8000.0f, 8000.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glLineWidth(2.0f);
	glBindVertexArray(axes_VAO);
	glUniform3fv(loc_primitive_color, 1, axes_color[0]);
	glDrawArrays(GL_LINES, 0, 2);
	glUniform3fv(loc_primitive_color, 1, axes_color[1]);
	glDrawArrays(GL_LINES, 2, 2);
	glUniform3fv(loc_primitive_color, 1, axes_color[2]);
	glDrawArrays(GL_LINES, 4, 2);
	glBindVertexArray(0);
	glLineWidth(1.0f);
	glUseProgram(0);
}

//grid
#define GRID_LENGTH			(100)
#define NUM_GRID_VETICES	((2 * GRID_LENGTH + 1) * 4)
GLuint grid_VBO, grid_VAO;
GLfloat grid_vertices[NUM_GRID_VETICES][3];
GLfloat grid_color[3] = { 0.5f, 0.5f, 0.5f };

void prepare_grid(void) {

	//set grid vertices
	int vertex_idx = 0;
	for (int x_idx = -GRID_LENGTH; x_idx <= GRID_LENGTH; x_idx++)
	{
		grid_vertices[vertex_idx][0] = x_idx;
		grid_vertices[vertex_idx][1] = -GRID_LENGTH;
		grid_vertices[vertex_idx][2] = 0.0f;
		vertex_idx++;

		grid_vertices[vertex_idx][0] = x_idx;
		grid_vertices[vertex_idx][1] = GRID_LENGTH;
		grid_vertices[vertex_idx][2] = 0.0f;
		vertex_idx++;
	}

	for (int y_idx = -GRID_LENGTH; y_idx <= GRID_LENGTH; y_idx++)
	{
		grid_vertices[vertex_idx][0] = -GRID_LENGTH;
		grid_vertices[vertex_idx][1] = y_idx;
		grid_vertices[vertex_idx][2] = 0.0f;
		vertex_idx++;

		grid_vertices[vertex_idx][0] = GRID_LENGTH;
		grid_vertices[vertex_idx][1] = y_idx;
		grid_vertices[vertex_idx][2] = 0.0f;
		vertex_idx++;
	}

	// Initialize vertex buffer object.
	glGenBuffers(1, &grid_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, grid_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grid_vertices), &grid_vertices[0][0], GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &grid_VAO);
	glBindVertexArray(grid_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, grid_VAO);
	glVertexAttribPointer(INDEX_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(INDEX_VERTEX_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	fprintf(stdout, " * Loaded grid into graphics memory.\n");
}

void draw_grid(void) {
	if (!b_draw_grid)
		return;

	glUseProgram(h_ShaderProgram_simple);
	ModelViewMatrix = glm::scale(ViewMatrix, glm::vec3(100.0f, 100.0f, 100.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glLineWidth(1.0f);
	glBindVertexArray(grid_VAO);
	glUniform3fv(loc_primitive_color, 1, grid_color);
	glDrawArrays(GL_LINES, 0, NUM_GRID_VETICES);
	glBindVertexArray(0);
	glLineWidth(1.0f);
	glUseProgram(0);
}

//sun_temple
GLuint* sun_temple_VBO;
GLuint* sun_temple_VAO;
int* sun_temple_n_triangles;
int* sun_temple_vertex_offset;
GLfloat** sun_temple_vertices;
GLuint* sun_temple_texture_names;

int flag_fog;
bool* flag_texture_mapping;

void initialize_lights(void) { // follow OpenGL conventions for initialization
	int i;

	glUseProgram(h_ShaderProgram_TXPS);

	glUniform4f(loc_global_ambient_color, 1.0f, 1.0f, 1.0f, 1.0f);

	for (i = 0; i < scene.n_lights; i++) {
		glUniform1i(loc_light[i].light_on, 1);
		glUniform4f(loc_light[i].position,
			scene.light_list[i].pos[0],
			scene.light_list[i].pos[1],
			scene.light_list[i].pos[2],
			1.0f);

		glUniform4f(loc_light[i].ambient_color, 0.13f, 0.13f, 0.13f, 1.0f);
		glUniform4f(loc_light[i].diffuse_color, 0.5f, 0.5f, 0.5f, 1.0f);
		glUniform4f(loc_light[i].specular_color, 0.8f, 0.8f, 0.8f, 1.0f);
		glUniform3f(loc_light[i].spot_direction, 0.0f, 0.0f, -1.0f);
		glUniform1f(loc_light[i].spot_exponent, 0.0f); // [0.0, 128.0]
		glUniform1f(loc_light[i].spot_cutoff_angle, 180.0f); // [0.0, 90.0] or 180.0 (180.0 for no spot light effect)
		glUniform4f(loc_light[i].light_attenuation_factors, 20.0f, 0.0f, 0.0f, 1.0f); // .w != 0.0f for no ligth attenuation
	}

	glUseProgram(0);
}

void initialize_flags(void) {
	flag_fog = 0;

	glUseProgram(h_ShaderProgram_TXPS);
	glUniform1i(loc_flag_fog, flag_fog);
	glUseProgram(0);
}

bool readTexImage2D_from_file(char* filename) {
	FREE_IMAGE_FORMAT tx_file_format;
	int tx_bits_per_pixel;
	FIBITMAP* tx_pixmap, * tx_pixmap_32;

	int width, height;
	GLvoid* data;

	tx_file_format = FreeImage_GetFileType(filename, 0);
	// assume everything is fine with reading texture from file: no error checking
	tx_pixmap = FreeImage_Load(tx_file_format, filename);
	if (tx_pixmap == NULL)
		return false;
	tx_bits_per_pixel = FreeImage_GetBPP(tx_pixmap);

	if (tx_bits_per_pixel == 32)
		tx_pixmap_32 = tx_pixmap;
	else {
		tx_pixmap_32 = FreeImage_ConvertTo32Bits(tx_pixmap);
	}

	width = FreeImage_GetWidth(tx_pixmap_32);
	height = FreeImage_GetHeight(tx_pixmap_32);
	data = FreeImage_GetBits(tx_pixmap_32);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
	//fprintf(stdout, " * Loaded %dx%d RGBA texture into graphics memory.\n\n", width, height);

	FreeImage_Unload(tx_pixmap_32);
	if (tx_bits_per_pixel != 32)
		FreeImage_Unload(tx_pixmap);

	return true;
}

void prepare_sun_temple(void) {
	int n_bytes_per_vertex, n_bytes_per_triangle;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	// VBO, VAO malloc
	sun_temple_VBO = (GLuint*)malloc(sizeof(GLuint) * scene.n_materials);
	sun_temple_VAO = (GLuint*)malloc(sizeof(GLuint) * scene.n_materials);

	sun_temple_n_triangles = (int*)malloc(sizeof(int) * scene.n_materials);
	sun_temple_vertex_offset = (int*)malloc(sizeof(int) * scene.n_materials);

	flag_texture_mapping = (bool*)malloc(sizeof(bool) * scene.n_textures);

	// vertices
	sun_temple_vertices = (GLfloat**)malloc(sizeof(GLfloat*) * scene.n_materials);

	for (int materialIdx = 0; materialIdx < scene.n_materials; materialIdx++) {
		MATERIAL* pMaterial = &(scene.material_list[materialIdx]);
		GEOMETRY_TRIANGULAR_MESH* tm = &(pMaterial->geometry.tm);

		// vertex
		sun_temple_vertices[materialIdx] = (GLfloat*)malloc(sizeof(GLfloat) * 8 * tm->n_triangle * 3);

		int vertexIdx = 0;
		for (int triIdx = 0; triIdx < tm->n_triangle; triIdx++) {
			TRIANGLE tri = tm->triangle_list[triIdx];
			for (int triVertex = 0; triVertex < 3; triVertex++) {
				sun_temple_vertices[materialIdx][vertexIdx++] = tri.position[triVertex].x;
				sun_temple_vertices[materialIdx][vertexIdx++] = tri.position[triVertex].y;
				sun_temple_vertices[materialIdx][vertexIdx++] = tri.position[triVertex].z;

				sun_temple_vertices[materialIdx][vertexIdx++] = tri.normal_vetcor[triVertex].x;
				sun_temple_vertices[materialIdx][vertexIdx++] = tri.normal_vetcor[triVertex].y;
				sun_temple_vertices[materialIdx][vertexIdx++] = tri.normal_vetcor[triVertex].z;

				sun_temple_vertices[materialIdx][vertexIdx++] = tri.texture_list[triVertex][0].u;
				sun_temple_vertices[materialIdx][vertexIdx++] = tri.texture_list[triVertex][0].v;
			}
		}

		// # of triangles
		sun_temple_n_triangles[materialIdx] = tm->n_triangle;

		if (materialIdx == 0)
			sun_temple_vertex_offset[materialIdx] = 0;
		else
			sun_temple_vertex_offset[materialIdx] = sun_temple_vertex_offset[materialIdx - 1] + 3 * sun_temple_n_triangles[materialIdx - 1];

		glGenBuffers(1, &sun_temple_VBO[materialIdx]);

		glBindBuffer(GL_ARRAY_BUFFER, sun_temple_VBO[materialIdx]);
		glBufferData(GL_ARRAY_BUFFER, sun_temple_n_triangles[materialIdx] * 3 * n_bytes_per_vertex,
			sun_temple_vertices[materialIdx], GL_STATIC_DRAW);

		// As the geometry data exists now in graphics memory, ...
		free(sun_temple_vertices[materialIdx]);

		// Initialize vertex array object.
		glGenVertexArrays(1, &sun_temple_VAO[materialIdx]);
		glBindVertexArray(sun_temple_VAO[materialIdx]);

		glBindBuffer(GL_ARRAY_BUFFER, sun_temple_VBO[materialIdx]);
		glVertexAttribPointer(INDEX_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), BUFFER_OFFSET(0));
		glEnableVertexAttribArray(INDEX_VERTEX_POSITION);
		glVertexAttribPointer(INDEX_NORMAL, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), BUFFER_OFFSET(3 * sizeof(float)));
		glEnableVertexAttribArray(INDEX_NORMAL);
		glVertexAttribPointer(INDEX_TEX_COORD, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), BUFFER_OFFSET(6 * sizeof(float)));
		glEnableVertexAttribArray(INDEX_TEX_COORD);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		if ((materialIdx > 0) && (materialIdx % 100 == 0))
			fprintf(stdout, " * Loaded %d sun temple materials into graphics memory.\n", materialIdx / 100 * 100);
	}
	fprintf(stdout, " * Loaded %d sun temple materials into graphics memory.\n", scene.n_materials);

	// textures
	sun_temple_texture_names = (GLuint*)malloc(sizeof(GLuint) * scene.n_textures);
	glGenTextures(scene.n_textures, sun_temple_texture_names);

	for (int texId = 0; texId < scene.n_textures; texId++) {
		glActiveTexture(GL_TEXTURE0 + texId);
		glBindTexture(GL_TEXTURE_2D, sun_temple_texture_names[texId]);

		bool bReturn = readTexImage2D_from_file(scene.texture_file_name[texId]);

		if (bReturn) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			flag_texture_mapping[texId] = true;
		}
		else {
			flag_texture_mapping[texId] = false;
		}

		glBindTexture(GL_TEXTURE_2D, 0);
	}
	fprintf(stdout, " * Loaded sun temple textures into graphics memory.\n\n");
	
	free(sun_temple_vertices);
}

void draw_sun_temple(void) {
	glUseProgram(h_ShaderProgram_TXPS);
	ModelViewMatrix = ViewMatrix;
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::transpose(glm::inverse(glm::mat3(ModelViewMatrix)));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);

	for (int materialIdx = 0; materialIdx < scene.n_materials; materialIdx++) {
		// set material
		glUniform4fv(loc_material.ambient_color, 1, scene.material_list[materialIdx].shading.ph.ka);
		glUniform4fv(loc_material.diffuse_color, 1, scene.material_list[materialIdx].shading.ph.kd);
		glUniform4fv(loc_material.specular_color, 1, scene.material_list[materialIdx].shading.ph.ks);
		glUniform1f(loc_material.specular_exponent, scene.material_list[materialIdx].shading.ph.spec_exp);
		glUniform4f(loc_material.emissive_color, 0.0f, 0.0f, 0.0f, 1.0f);

		int texId = scene.material_list[materialIdx].diffuseTexId;
		glUniform1i(loc_texture, texId);
		glUniform1i(loc_flag_texture_mapping, flag_texture_mapping[texId]);

		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0 + texId);
		glBindTexture(GL_TEXTURE_2D, sun_temple_texture_names[texId]);

		glBindVertexArray(sun_temple_VAO[materialIdx]);
		glDrawArrays(GL_TRIANGLES, 0, 3 * sun_temple_n_triangles[materialIdx]);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
	}
	glUseProgram(0);
}

unsigned int leftbutton_pressed = 0, rotation_mode_cow = 1, timestamp_cow = 0, timestamp_wolf = 0;
int prevx, prevy;
bool tiger_stop = false;

void timer_scene(int value) {
	
	rotation_angle_tiger = (float)(timestamp_cow);
	timestamp_cow = (timestamp_cow + 1) % 720;
	cur_frame_wolf = value % N_WOLF_FRAMES;
	glutPostRedisplay();
	glutTimerFunc(3, timer_scene, (timestamp_cow + 1) % INT_MAX);
}

void mousepress(int button, int state, int x, int y)
{
	if ((button == GLUT_LEFT_BUTTON)) {
		if (state == GLUT_DOWN) {
			current_camera.move = 1;
			prevx = x; prevy = y;
		}
		else if (state == GLUT_UP) current_camera.move = 0;
	}
}

void MyMouseWheelFunc(int wheel, int direction, int x, int y)
{
	int mod = glutGetModifiers();
	if (mod == GLUT_ACTIVE_SHIFT)
	{
		if (direction == -1)		//zoom out
		{
			if (45.0f * TO_RADIAN * zoom_factor * 1.02f <= 90.0f * TO_RADIAN)
			{
				//		zoom_factor *= 1.02f;			//더하고 빼는거로 바꿔서 더 예뻐보이는거로 하기
				//fprintf(stdout, "zoom in cci bal");
				current_camera.fovy *= 1.02f;
				ProjectionMatrix = glm::perspective(current_camera.fovy, current_camera.aspect_ratio, current_camera.near_c, current_camera.far_c);
				//ProjectionMatrix = glm::perspective(zoom_factor * 45.0f * TO_RADIAN, 1.0f, 200.0f, 1000.0f);
				//여기를 손보면 될 것 같은데 어떻게 하는지 모르겠다.
				glutPostRedisplay();
			}
		}
		else						//zoom in
		{
			if (45.0f * TO_RADIAN * zoom_factor * 1.02f >= 10.0f * TO_RADIAN)
			{
				//			zoom_factor /= 1.02f;
				//fprintf(stdout, "zoom out cci bal");
				current_camera.fovy /= 1.02f;
				ProjectionMatrix = glm::perspective(current_camera.fovy, current_camera.aspect_ratio, current_camera.near_c, current_camera.far_c);
				//ProjectionMatrix = glm::perspective(zoom_factor * 45.0f * TO_RADIAN, 1.0f, 200.0f, 1000.0f);
				//여기를 손보면 될 것 같은데 어떻게 하는지 모르겠다.
				glutPostRedisplay();
			}
		}
	}
	else
	{
		ProjectionMatrix = glm::perspective(current_camera.fovy, current_camera.aspect_ratio, current_camera.near_c, current_camera.far_c);
		glutPostRedisplay();
	}
}

enum axes { X_AXIS, Y_AXIS, Z_AXIS };
static int flag_translation_axis;
#define CAM_TSPEED 0.05f

void renew_cam_position(int del) {
	switch (flag_translation_axis) {
	case X_AXIS:
		current_camera.pos[0] += CAM_TSPEED * del * current_camera.uaxis[0];
		current_camera.pos[1] += CAM_TSPEED * del * current_camera.uaxis[1];
		current_camera.pos[2] += CAM_TSPEED * del * current_camera.uaxis[2];
		//glm::vec3(current_camera.pos[0], current_camera.pos[1], current_camera.pos[2]) += CAM_TSPEED * del * glm::vec3(current_camera.uaxis[0], current_camera.uaxis[1], current_camera.uaxis[2]);
		break;
	case Y_AXIS:
		//camera.pos += CAM_TSPEED * del * camera.vaxis;
		current_camera.pos[0] += CAM_TSPEED * del * current_camera.vaxis[0];
		current_camera.pos[1] += CAM_TSPEED * del * current_camera.vaxis[1];
		current_camera.pos[2] += CAM_TSPEED * del * current_camera.vaxis[2];
		//glm::vec3(current_camera.pos[0], current_camera.pos[1], current_camera.pos[2]) += CAM_TSPEED * del * glm::vec3(current_camera.vaxis[0], current_camera.vaxis[1], current_camera.vaxis[2]);
		break;
	case Z_AXIS: 
		//camera.pos += CAM_TSPEED * del * (-camera.naxis);
		current_camera.pos[0] += CAM_TSPEED * del * -current_camera.naxis[0];
		current_camera.pos[1] += CAM_TSPEED * del * -current_camera.naxis[1];
		current_camera.pos[2] += CAM_TSPEED * del * -current_camera.naxis[2];
		//glm::vec3(current_camera.pos[0], current_camera.pos[1], current_camera.pos[2]) += CAM_TSPEED * del * -1 * glm::vec3(current_camera.naxis[0], current_camera.naxis[1], current_camera.naxis[2]);
		break;
	}
}

#define CAM_RSPEED 0.3f
void renew_cam_orientation_rotation_around_v_axis(int anglex, int angley) {
	// let's get a help from glm
	glm::mat3 RotationMatrix;
	glm::vec3 direction;

	RotationMatrix = glm::mat3(glm::rotate(glm::mat4(1.0), CAM_RSPEED * TO_RADIAN * anglex,
		glm::vec3(current_camera.vaxis[0], current_camera.vaxis[1], current_camera.vaxis[2])));

	direction = RotationMatrix * glm::vec3(current_camera.uaxis[0], current_camera.uaxis[1], current_camera.uaxis[2]);
	current_camera.uaxis[0] = direction[0];
	current_camera.uaxis[1] = direction[1];
	current_camera.uaxis[2] = direction[2];
	direction = RotationMatrix * glm::vec3(current_camera.naxis[0], current_camera.naxis[1], current_camera.naxis[2]);
	current_camera.naxis[0] = direction[0];
	current_camera.naxis[1] = direction[1];
	current_camera.naxis[2] = direction[2];

	RotationMatrix = glm::mat3(glm::rotate(glm::mat4(1.0), CAM_RSPEED * TO_RADIAN * angley,
		glm::vec3(current_camera.uaxis[0], current_camera.uaxis[1], current_camera.uaxis[2])));
	direction = RotationMatrix * glm::vec3(current_camera.vaxis[0], current_camera.vaxis[1], current_camera.vaxis[2]);
	current_camera.vaxis[0] = direction[0];
	current_camera.vaxis[1] = direction[1];
	current_camera.vaxis[2] = direction[2];
	direction = RotationMatrix * glm::vec3(current_camera.naxis[0], current_camera.naxis[1], current_camera.naxis[2]);
	current_camera.naxis[0] = direction[0];
	current_camera.naxis[1] = direction[1];
	current_camera.naxis[2] = direction[2];
// TODO: Correct uaxis, vaxis, and naxis so that they continue to be orthonormal!!!
}

void motion(int x, int y) {
	if (!current_camera.move) return;
	//renew_cam_position(prevy - y);
	renew_cam_orientation_rotation_around_v_axis(prevx - x, prevy-y); 
//	prevx = x; prevy = y;
//	flag_translation_axis = X_AXIS;
//	renew_cam_position(prevx - x);
//	renew_cam_orientation_rotation_around_v_axis(prevy - y);

	prevx = x; prevy = y;
	set_ViewMatrix_from_camera_frame();
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
	glutPostRedisplay();
}

#define CAM_ROT_SENSITIVITY 0.15f

//내가 추가한 함수
int read_geometry_20171693(GLfloat** object, int bytes_per_primitive, char* filename) {
	int n_triangles;
	FILE* fp;

	// fprintf(stdout, "Reading geometry from the geometry file %s...\n", filename);
	fp = fopen(filename, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Cannot open the object file %s ...", filename);
		return -1;
	}
	fread(&n_triangles, sizeof(int), 1, fp);
	*object = (float*)malloc(n_triangles * bytes_per_primitive);
	if (*object == NULL) {
		fprintf(stderr, "Cannot allocate memory for the geometry file %s ...", filename);
		return -1;
	}

	fread(*object, bytes_per_primitive, n_triangles, fp);
	// fprintf(stdout, "Read %d primitives successfully.\n\n", n_triangles);
	fclose(fp);

	return n_triangles;
}

//////////////SPIDER????????///////////////
#define N_SPIDER_FRAMES 16
GLuint spider_VBO, spider_VAO;
int spider_n_triangles[N_SPIDER_FRAMES];
int spider_vertex_offset[N_SPIDER_FRAMES];
GLfloat* spider_vertices[N_SPIDER_FRAMES];
Material_Parameters material_spider;

GLuint ironman_VBO, ironman_VAO;
int ironman_n_triangles;
GLfloat* ironman_vertices;
Material_Parameters material_ironman;

void draw_spider(void) {
	glFrontFace(GL_CW);
	glBindVertexArray(spider_VAO);
	glDrawArrays(GL_TRIANGLES, spider_vertex_offset[cur_frame_spider], 3 * spider_n_triangles[cur_frame_spider]);
	glBindVertexArray(0);
}

void draw_wolf(void) {
	glFrontFace(GL_CW);
	glBindVertexArray(wolf_VAO);
	glDrawArrays(GL_TRIANGLES, wolf_vertex_offset[cur_frame_wolf], 3 * wolf_n_triangles[cur_frame_wolf]);
	glBindVertexArray(0);
}

void draw_ironman(void) {
	glFrontFace(GL_CW);

	glBindVertexArray(ironman_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * ironman_n_triangles);
	glBindVertexArray(0);
}

void draw_godzilla(void) {
	glFrontFace(GL_CW);

	glBindVertexArray(godzilla_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * godzilla_n_triangles);
	glBindVertexArray(0);
}

void draw_tank(void) {
	glFrontFace(GL_CW);

	glBindVertexArray(tank_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * tank_n_triangles);
	glBindVertexArray(0);
}

void draw_optimus(void) {
	glFrontFace(GL_CW);

	glBindVertexArray(optimus_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * optimus_n_triangles);
	glBindVertexArray(0);
}

void draw_bus(void) {
	glFrontFace(GL_CW);

	glBindVertexArray(bus_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * bus_n_triangles);
	glBindVertexArray(0);
}


// TO DO
void prepare_objects_20171693(void) {
	/* fill your code here */
	/////////////tiger/////////////////////////////////
	int i, n_bytes_per_vertex, n_bytes_per_triangle, tiger_n_total_triangles = 0;
	char filename[512];
	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;
	for (i = 0; i < N_TIGER_FRAMES; i++) {
		sprintf(filename, "Data/dynamic_objects/tiger/Tiger_%d%d_triangles_vnt.geom", i / 10, i % 10);

		tiger_n_triangles[i] = read_geometry_20171693(&tiger_vertices[i], n_bytes_per_triangle, filename);
		// Assume all geometry files are effective.
		tiger_n_total_triangles += tiger_n_triangles[i];

		if (i == 0)
			tiger_vertex_offset[i] = 0;
		else
			tiger_vertex_offset[i] = tiger_vertex_offset[i - 1] + 3 * tiger_n_triangles[i - 1];
	}
	// Initialize vertex buffer object.
	glGenBuffers(1, &tiger_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, tiger_VBO);
	glBufferData(GL_ARRAY_BUFFER, tiger_n_total_triangles * n_bytes_per_triangle, NULL, GL_STATIC_DRAW);
	for (i = 0; i < N_TIGER_FRAMES; i++)
		glBufferSubData(GL_ARRAY_BUFFER, tiger_vertex_offset[i] * n_bytes_per_vertex,
			tiger_n_triangles[i] * n_bytes_per_triangle, tiger_vertices[i]);
	// As the geometry data exists now in graphics memory, ...
	for (i = 0; i < N_TIGER_FRAMES; i++)
		free(tiger_vertices[i]);
	// Initialize vertex array object.
	glGenVertexArrays(1, &tiger_VAO);
	glBindVertexArray(tiger_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, tiger_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////SPIDER////////////////////////////////////////////
	int  spider_n_total_triangles = 0;
	

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	for (i = 0; i < N_SPIDER_FRAMES; i++) {
		sprintf(filename, "Data/dynamic_objects/spider/spider_vnt_%d%d.geom", i / 10, i % 10);
		spider_n_triangles[i] = read_geometry_20171693(&spider_vertices[i], n_bytes_per_triangle, filename);
		// assume all geometry files are effective
		spider_n_total_triangles += spider_n_triangles[i];
		if (i == 0)
			spider_vertex_offset[i] = 0;
		else
			spider_vertex_offset[i] = spider_vertex_offset[i - 1] + 3 * spider_n_triangles[i - 1];
	}
	// initialize vertex buffer object
	glGenBuffers(1, &spider_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, spider_VBO);
	glBufferData(GL_ARRAY_BUFFER, spider_n_total_triangles * n_bytes_per_triangle, NULL, GL_STATIC_DRAW);

	for (i = 0; i < N_SPIDER_FRAMES; i++)
		glBufferSubData(GL_ARRAY_BUFFER, spider_vertex_offset[i] * n_bytes_per_vertex,
			spider_n_triangles[i] * n_bytes_per_triangle, spider_vertices[i]);
	// as the geometry data exists now in graphics memory, ...
	for (i = 0; i < N_SPIDER_FRAMES; i++)
		free(spider_vertices[i]);
	// initialize vertex array object
	glGenVertexArrays(1, &spider_VAO);
	glBindVertexArray(spider_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, spider_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	///////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////WOLF///////////////////////////////////////////////////
	int wolf_n_total_triangles = 0;

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	for (i = 0; i < N_WOLF_FRAMES; i++) {
		sprintf(filename, "Data/dynamic_objects/wolf/wolf_%d%d_vnt.geom", i / 10, i % 10);
		wolf_n_triangles[i] = read_geometry_20171693(&wolf_vertices[i], n_bytes_per_triangle, filename);
		// assume all geometry files are effective
		wolf_n_total_triangles += wolf_n_triangles[i];

		if (i == 0)
			wolf_vertex_offset[i] = 0;
		else
			wolf_vertex_offset[i] = wolf_vertex_offset[i - 1] + 3 * wolf_n_triangles[i - 1];
	}
	// initialize vertex buffer object
	glGenBuffers(1, &wolf_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, wolf_VBO);
	glBufferData(GL_ARRAY_BUFFER, wolf_n_total_triangles * n_bytes_per_triangle, NULL, GL_STATIC_DRAW);
	for (i = 0; i < N_WOLF_FRAMES; i++)
		glBufferSubData(GL_ARRAY_BUFFER, wolf_vertex_offset[i] * n_bytes_per_vertex,
			wolf_n_triangles[i] * n_bytes_per_triangle, wolf_vertices[i]);
	// as the geometry data exists now in graphics memory, ...
	for (i = 0; i < N_WOLF_FRAMES; i++)
		free(wolf_vertices[i]);
	// initialize vertex array object
	glGenVertexArrays(1, &wolf_VAO);
	glBindVertexArray(wolf_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, wolf_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//////////////////////////////////////iron man///////////////////////////////////////////////
	int  ironman_n_total_triangles = 0;

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	sprintf(filename, "Data/static_objects/ironman_vnt.geom");
	ironman_n_triangles = read_geometry_20171693(&ironman_vertices, n_bytes_per_triangle, filename);
	// assume all geometry files are effective
	ironman_n_total_triangles += ironman_n_triangles;

	// initialize vertex buffer object
	glGenBuffers(1, &ironman_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, ironman_VBO);
	glBufferData(GL_ARRAY_BUFFER, ironman_n_total_triangles * 3 * n_bytes_per_vertex, ironman_vertices, GL_STATIC_DRAW);

	// as the geometry data exists now in graphics memory, ...
	free(ironman_vertices);

	// initialize vertex array object
	glGenVertexArrays(1, &ironman_VAO);
	glBindVertexArray(ironman_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, ironman_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////godzilla

	int godzilla_n_total_triangles = 0;
	
	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	sprintf(filename, "Data/static_objects/godzilla_vnt.geom");
	godzilla_n_triangles = read_geometry_20171693(&godzilla_vertices, n_bytes_per_triangle, filename);
	// assume all geometry files are effective
	godzilla_n_total_triangles += godzilla_n_triangles;


	// initialize vertex buffer object
	glGenBuffers(1, &godzilla_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, godzilla_VBO);
	glBufferData(GL_ARRAY_BUFFER, godzilla_n_total_triangles * 3 * n_bytes_per_vertex, godzilla_vertices, GL_STATIC_DRAW);

	// as the geometry data exists now in graphics memory, ...
	free(godzilla_vertices);

	// initialize vertex array object
	glGenVertexArrays(1, &godzilla_VAO);
	glBindVertexArray(godzilla_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, godzilla_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	////////////////////////////////////////////////////////////
	// 
	// 
	//////////////////////////////tank//////////////////////////////

	int tank_n_total_triangles = 0;
	
	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	sprintf(filename, "Data/static_objects/tank_vnt.geom");
	tank_n_triangles = read_geometry_20171693(&tank_vertices, n_bytes_per_triangle, filename);
	// assume all geometry files are effective
	tank_n_total_triangles += tank_n_triangles;

	// initialize vertex buffer object
	glGenBuffers(1, &tank_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, tank_VBO);
	glBufferData(GL_ARRAY_BUFFER, tank_n_total_triangles * 3 * n_bytes_per_vertex, tank_vertices, GL_STATIC_DRAW);
	// as the geometry data exists now in graphics memory, ...
	free(tank_vertices);
	// initialize vertex array object
	glGenVertexArrays(1, &tank_VAO);
	glBindVertexArray(tank_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, tank_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	////////////////////////////////////////////////////////////
		/////////////////////////////////optimus///////////////////////////////////////////////////////////

	int optimus_n_total_triangles = 0;
	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	sprintf(filename, "Data/static_objects/optimus_vnt.geom");
	optimus_n_triangles = read_geometry_20171693(&optimus_vertices, n_bytes_per_triangle, filename);
	// assume all geometry files are effective
	optimus_n_total_triangles += optimus_n_triangles;
	// initialize vertex buffer object
	glGenBuffers(1, &optimus_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, optimus_VBO);
	glBufferData(GL_ARRAY_BUFFER, optimus_n_total_triangles * 3 * n_bytes_per_vertex, optimus_vertices, GL_STATIC_DRAW);
	// as the geometry data exists now in graphics memory, ...
	free(optimus_vertices);
	// initialize vertex array object
	glGenVertexArrays(1, &optimus_VAO);
	glBindVertexArray(optimus_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, optimus_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
		////////////////////////////////////////////////////////////
		///////////////////////////////BUS/////////////////////////////////////////////////////////////////////////////////////////
	int  bus_n_total_triangles = 0;
	
	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;
	sprintf(filename, "Data/static_objects/bus_vnt.geom");
	bus_n_triangles = read_geometry_20171693(&bus_vertices, n_bytes_per_triangle, filename);
	// assume all geometry files are effective
	bus_n_total_triangles += bus_n_triangles;
	// initialize vertex buffer object
	glGenBuffers(1, &bus_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, bus_VBO);
	glBufferData(GL_ARRAY_BUFFER, bus_n_total_triangles * 3 * n_bytes_per_vertex, bus_vertices, GL_STATIC_DRAW);
	// as the geometry data exists now in graphics memory, ...
	free(bus_vertices);
	// initialize vertex array object
	glGenVertexArrays(1, &bus_VAO);
	glBindVertexArray(bus_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, bus_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
int flag_cull_face = 0;

float tiger_coordinate[3] = {0.0f,-93.8f,62.0f};
float tiger_uaxis[3] = {-1.0f, 0.0f, 0.0f};
float tiger_vaxis[3] = {0.0f, 0.0f, 1.0f};
float tiger_naxis[3] = {0.0f, 1.0f, 0.0f};

float looktiger_coordinate[3] = { 0.0f, 208.8f,62.0f };
float looktiger_uaxis[3] = { -1.0f, 0.0f, 0.0f };
float looktiger_vaxis[3] = { 0.0f, 0.0f, 1.0f };
float looktiger_naxis[3] = { 0.0f, 1.0f, 0.0f };
int t_angle;
int index = 0;
void draw_tiger_20171693(void) {
	glFrontFace(GL_CCW);

	if (!tiger_stop)
	{
		t_angle = (t_angle + 1) % 360;
		cur_frame_tiger = t_angle % N_TIGER_FRAMES;
	}
	//여기서 tiger coordinate , tiger uaxis vaxis naxis 저장해주고 
	//printf("%f %f %f %f\n", ModelViewMatrix[0][0], ModelViewMatrix[0][1], ModelViewMatrix[0][2], ModelViewMatrix[0][3]);
	Camera* pCamera = &camera_info[CAMERA_TIGER];
	pCamera = &camera_info[CAMERA_TIGER];
	pCamera->pos[0] = tiger_coordinate[0]; pCamera->pos[1] = tiger_coordinate[1]; pCamera->pos[2] = tiger_coordinate[2];
	pCamera->uaxis[0] = tiger_uaxis[0]; pCamera->uaxis[1] = tiger_uaxis[1]; pCamera->uaxis[2] = tiger_uaxis[2];
	pCamera->vaxis[0] = tiger_vaxis[0]; pCamera->vaxis[1] = tiger_vaxis[1]; pCamera->vaxis[2] = tiger_vaxis[2];
	pCamera->naxis[0] = tiger_naxis[0]; pCamera->naxis[1] = tiger_naxis[1]; pCamera->naxis[2] = tiger_naxis[2];
	pCamera->move = 0;
	pCamera->fovy = TO_RADIAN * scene.camera.fovy, pCamera->aspect_ratio = scene.camera.aspect, pCamera->near_c = 0.1f; pCamera->far_c = 30000.0f;
	
	pCamera = &camera_info[CAMERA_LOOKTIGER];
	pCamera->pos[0] = looktiger_coordinate[0]; pCamera->pos[1] = looktiger_coordinate[1]; pCamera->pos[2] = looktiger_coordinate[2];
	pCamera->uaxis[0] = looktiger_uaxis[0]; pCamera->uaxis[1] = looktiger_uaxis[1]; pCamera->uaxis[2] = looktiger_uaxis[2];
	pCamera->vaxis[0] = looktiger_vaxis[0]; pCamera->vaxis[1] = looktiger_vaxis[1]; pCamera->vaxis[2] = looktiger_vaxis[2];
	pCamera->naxis[0] = looktiger_naxis[0]; pCamera->naxis[1] = looktiger_naxis[1]; pCamera->naxis[2] = looktiger_naxis[2];
	pCamera->move = 0; 
	pCamera->fovy = TO_RADIAN * scene.camera.fovy, pCamera->aspect_ratio = scene.camera.aspect, pCamera->near_c = 0.1f; pCamera->far_c = 30000.0f;

	glm::mat4 ModelMatrix;					
	ModelMatrix = glm::mat4(1.0f);
//	ModelMatrix = glm::rotate(ModelMatrix, -rotation_angle_tiger * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
//	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(400.0f, 0.0f, 250.0f));
	glm::mat3 RotationMatrix;
	glm::vec3 direction;

	
	int airplane_clock = timestamp_cow % 720;
	//ModelMatrix = glm::rotate(glm::mat4(1.0f), airplane_clock * TO_RADIAN * 2.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	//ModelMatrix = glm::rotate(ModelMatrix, -rotation_angle_tiger * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	if (airplane_clock <= 360)
	{
		ModelMatrix = glm::rotate(ModelMatrix, -t_angle * TO_RADIAN * 2.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(200.0f + t_angle * 1.5f, 100.0f, 300.0f + t_angle * 1.5f));
		ModelMatrix = glm::rotate(ModelMatrix, ((int)t_angle * TO_RADIAN * 3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	if (airplane_clock > 360)
	{
		ModelMatrix = glm::rotate(ModelMatrix, t_angle * TO_RADIAN * 2.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(200.0f - (t_angle - 720) * 1.5f, 100.0f, 300.0f - (t_angle - 720) * 1.5f));
		ModelMatrix = glm::rotate(ModelMatrix, ((int)t_angle * TO_RADIAN * 3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	ModelViewMatrix = ViewMatrix * ModelMatrix;
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;

	if (tiger_mode || looktiger_mode)
	{

		glm::vec4 curU, curV, curN, curcam_Pos;
		index += 1;
		index %= 5;
		curU = glm::vec4(current_camera.uaxis[0], current_camera.uaxis[1], current_camera.uaxis[2], 0.0f);
		curV = glm::vec4(current_camera.vaxis[0], current_camera.vaxis[1], current_camera.vaxis[2], 0.0f);
		curN = glm::vec4(current_camera.naxis[0], current_camera.naxis[1], current_camera.naxis[2], 0.0f);
		if(tiger_mode) 
			curcam_Pos = glm::vec4(current_camera.pos[0],  current_camera.pos[1], current_camera.pos[2] * sinf(t_angle*TO_DEGREE) , 1.0f);
		else if (looktiger_mode) 
			curcam_Pos = glm::vec4(current_camera.pos[0], current_camera.pos[1], current_camera.pos[2] , 1.0f);
		curU = ModelMatrix *curU;
		curV = ModelMatrix *curV;
		curN = ModelMatrix *curN;
		curcam_Pos = ModelMatrix * curcam_Pos;		//점은 그대로

		//glm::vec4 temp;
		//temp = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		glm::mat4 NewViewMatrix;	
		NewViewMatrix = glm::mat4(curU, curV, curN, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));						//[n'x, n'y, n'z, {0001}] //세로로//
		NewViewMatrix = glm::transpose(NewViewMatrix);

		glm::vec3 curcam_pos3;
		curcam_pos3 = glm::vec3(curcam_Pos[0], curcam_Pos[1], curcam_Pos[2]);	//3차원만
		ViewMatrix = glm::translate(NewViewMatrix, -curcam_pos3);				//currentcamrea.pos[0], [1], [2]로 바꿔보자
		ModelViewProjectionMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix; 
	}
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glBindVertexArray(tiger_VAO);
	glDrawArrays(GL_TRIANGLES, tiger_vertex_offset[cur_frame_tiger], 3 * tiger_n_triangles[cur_frame_tiger]);
	glBindVertexArray(0);
}


void draw_objects(void) {
	/* fill your code here */
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glUseProgram(h_ShaderProgram_simple);

	draw_tiger_20171693();

	int arr1[10] = { -4,-3,-2,-1,0,1,2,3,4 };
	int arr2[10] = { -4,-3,-2,-1,0,1,2,3,4 };
	for (int i = -4; i < 4; i++)
	{
		for (int j = -4; j < 4; j++)
		{
			ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(0.0f + (200 * i), 0.0f + (200 * j), 800 + rand()%100 + rotation_angle_tiger / 2.0f));
			ModelViewMatrix = glm::rotate(ModelViewMatrix, ((int)rotation_angle_tiger % 100 *(rand()%20) * TO_RADIAN), glm::vec3(0.0f, 0.0f, 1.0f));
			ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(100.0f, 100.0f, 100.0f));
			ModelViewMatrix = glm::rotate(ModelViewMatrix, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
			ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_spider();
		}
	}
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(400, 0.0f, 250.0 + 30 * sinf(timestamp_cow % 180 * TO_RADIAN)));
	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(0.0f, -5000 - (40*rotation_angle_tiger + 45) , 0.0 + 400 * sinf((int)rotation_angle_tiger % 720 * TO_RADIAN)));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(200.0f, 200.0f, 200.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_wolf();

	//ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(400, 0.0f, 250.0 + 30 * sinf(timestamp_cow % 180 * TO_RADIAN)));
	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-100.0f, -4000 - (30 * rotation_angle_tiger + 45), 0.0 + 200 * sinf((int)rotation_angle_tiger % 720 * TO_RADIAN)));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(200.0f + (10 * rotation_angle_tiger + 45), 200.0f + (10 * rotation_angle_tiger + 45), 200.0f + (10 * rotation_angle_tiger + 45)));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_wolf();

	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(0.0f, 0.0f, 2800.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(200.0f, 200.0f, 200.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 180.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_ironman();

	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(0.0f, 0.0f, 3480.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(10.0f, 10.0f, 10.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 180.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_godzilla();

	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(4000.0f, 0.0f, 3480.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(300.0f, 300.0f, 300.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 180.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_tank();

	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-3000.0f, 0.0f, 3480.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(3.0f, 3.0f, 3.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 180.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_optimus();

	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(3000.0f, 0.0f, 3480.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(100.0f, 100.0f, 100.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 180.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_bus();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
/*****************************  END: geometry setup *****************************/

/********************  START: callback function definitions *********************/
void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	draw_grid();
	draw_axes();
	draw_sun_temple();
	glLineWidth(1.0f);

	/*ModelViewMatrix = glm::rotate(ViewMatrix, -rotation_angle_tiger, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(200.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::scale(ViewMatrix, glm::
(10.0f, 10.0f, 10.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;*/
	///
	
	//draw_tiger_20171693();
	draw_objects();

	glutSwapBuffers();
}

//timer_scene 추가 // 

void keyboard(unsigned char key, int x, int y) {
	float prev[3];
	switch (key) {
	case 'f':
		b_draw_grid = b_draw_grid ? false : true;
		glutPostRedisplay();
		break;
	case '0':
		set_current_camera(CAMERA_1);
		glutPostRedisplay();
		break;
	case '1':
		set_current_camera(CAMERA_2);
		glutPostRedisplay();
		break;
	case '2':
		set_current_camera(CAMERA_3);
		glutPostRedisplay();
		break;
	case '3':
		set_current_camera(CAMERA_4);
		glutPostRedisplay();
		break;
	case '4':
		set_current_camera(CAMERA_5);
		glutPostRedisplay();
		break;
	case '5':
		set_current_camera(CAMERA_6);
		glutPostRedisplay();
		break;
	case 't':
		set_current_camera(CAMERA_TIGER);
		tiger_mode = 1 - tiger_mode;
		glutPostRedisplay();
		break;
	case 'g':
		set_current_camera(CAMERA_LOOKTIGER);
		looktiger_mode = 1 - looktiger_mode;
		glutPostRedisplay();
		break;
	case 'm':
		movable_mode = 1 - movable_mode;
		break;
	case 's':
		if (tiger_stop == false)
			tiger_stop = true;
		else
			tiger_stop = false;
		break;
	case 'd':
		if (movable_mode)
		{
			current_camera.pos[0] -= 10.0f * current_camera.vaxis[0];
			current_camera.pos[1] -= 10.0f * current_camera.vaxis[1];
			current_camera.pos[2] -= 10.0f * current_camera.vaxis[2];
			prev[0] = -10.0f * current_camera.vaxis[0];
			prev[1] = -10.0f * current_camera.vaxis[1];
			prev[2] = -10.0f * current_camera.vaxis[2];

			set_ViewMatrix_from_camera_frame();
			//ProjectionMatrix = glm::perspective(current_camera.fovy, current_camera.aspect_ratio, current_camera.near_c, current_camera.far_c);
			//ViewMatrix = glm::translate(ViewMatrix, glm::vec3(prev[0], prev[1], prev[2]));
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;			
		}
		break;
	case 'u':
		if (movable_mode)
		{
			current_camera.pos[0] += 10.0f * current_camera.vaxis[0];
			current_camera.pos[1] += 10.0f * current_camera.vaxis[1];
			current_camera.pos[2] += 10.0f * current_camera.vaxis[2];
			prev[0] = 10.0f * current_camera.vaxis[0];
			prev[1] = 10.0f * current_camera.vaxis[1];
			prev[2] = 10.0f * current_camera.vaxis[2];
			set_ViewMatrix_from_camera_frame();
			//ProjectionMatrix = glm::perspective(current_camera.fovy, current_camera.aspect_ratio, current_camera.near_c, current_camera.far_c);
			//ViewMatrix = glm::translate(ViewMatrix, glm::vec3(prev[0], prev[1], prev[2]));
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		}
		break;
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	}
}

/////////////////////////////
#define CAM_FAR_CLIP_DISTANCE_STEP	2.0f
#define CAM_MAX_FAR_CLIP_DISTANCE	100.0f
#define CAM_ZOOM_STEP			0.05f
#define CAM_MAX_ZOOM_IN_FACTOR	0.25f
#define CAM_MAX_ZOOM_OUT_FACTOR	2.50f

void special(int key, int x, int y) {
	float prev[3];
	if (movable_mode)
	{
		switch (key) {
		case GLUT_KEY_DOWN:
			
			current_camera.pos[0] -= 8.0f * -current_camera.naxis[0];
			current_camera.pos[1] -= 8.0f * -current_camera.naxis[1];
			current_camera.pos[2] -= 8.0f * -current_camera.naxis[2];
			prev[0] = -8.0f * -current_camera.naxis[0];
			prev[1] = -8.0f * -current_camera.naxis[1];
			prev[2] = -8.0f * -current_camera.naxis[2];
			break;
		case GLUT_KEY_UP:
			current_camera.pos[0] += 8.0f * -current_camera.naxis[0];
			current_camera.pos[1] += 8.0f * -current_camera.naxis[1];
			current_camera.pos[2] += 8.0f * -current_camera.naxis[2];
			prev[0] = 8.0f * -current_camera.naxis[0];
			prev[1] = 8.0f * -current_camera.naxis[1];
			prev[2] = 8.0f * -current_camera.naxis[2];
			break;
		case GLUT_KEY_RIGHT:
			current_camera.pos[0] += 8.0f * current_camera.uaxis[0];
			current_camera.pos[1] += 8.0f * current_camera.uaxis[1];
			current_camera.pos[2] += 8.0f * current_camera.uaxis[2];
			prev[0] = +8.0f * current_camera.uaxis[0];
			prev[1] = +8.0f * current_camera.uaxis[1];
			prev[2] = +8.0f * current_camera.uaxis[2];
			break;
		case GLUT_KEY_LEFT:
			current_camera.pos[0] -= 8.0f * current_camera.uaxis[0];
			current_camera.pos[1] -= 8.0f * current_camera.uaxis[1];
			current_camera.pos[2] -= 8.0f * current_camera.uaxis[2];
			prev[0] = -8.0f * current_camera.uaxis[0];
			prev[1] = -8.0f * current_camera.uaxis[1];
			prev[2] = -8.0f * current_camera.uaxis[2];

			break;
		}
		set_ViewMatrix_from_camera_frame();
		//ProjectionMatrix = glm::perspective(current_camera.fovy, current_camera.aspect_ratio, current_camera.near_c, current_camera.far_c);
		//ViewMatrix = glm::translate(ViewMatrix, glm::vec3(prev[0], prev[1], prev[2]));
		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		glutPostRedisplay();
	}
}

//////////////////////////////

void reshape(int width, int height) {
	float aspect_ratio;

	glViewport(0, 0, width, height);

	ProjectionMatrix = glm::perspective(current_camera.fovy, current_camera.aspect_ratio, current_camera.near_c, current_camera.far_c);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
	glutPostRedisplay();
}

void cleanup(void) {
	glDeleteVertexArrays(1, &axes_VAO);
	glDeleteBuffers(1, &axes_VBO);

	glDeleteVertexArrays(1, &grid_VAO);
	glDeleteBuffers(1, &grid_VBO);

	glDeleteVertexArrays(scene.n_materials, sun_temple_VAO);
	glDeleteBuffers(scene.n_materials, sun_temple_VBO);

	glDeleteTextures(scene.n_textures, sun_temple_texture_names);

	free(sun_temple_n_triangles);
	free(sun_temple_vertex_offset);

	free(sun_temple_VAO);
	free(sun_temple_VBO);

	free(sun_temple_texture_names);
	free(flag_texture_mapping);
}
/*********************  END: callback function definitions **********************/

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	/// 나의 추가////////////
	glutSpecialFunc(special);
	glutMouseFunc(mousepress);
	glutMouseWheelFunc(MyMouseWheelFunc);
	//glutMotionFunc(motion_1);
	glutMotionFunc(motion);
	//glutSpecialFunc(special);
	//////////////////
	glutReshapeFunc(reshape);
	glutTimerFunc(100, timer_scene, 0);
	glutCloseFunc(cleanup);
}

void initialize_OpenGL(void) {
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	//rotation_angle_tiger = 0.0f;				//ㄴㅐ 추가
	initialize_lights();
	initialize_flags();
	flag_translation_axis = Y_AXIS;
}

void prepare_scene(void) {
	prepare_axes();
	prepare_grid();
	prepare_sun_temple();
	prepare_objects_20171693();
}

void initialize_renderer(void) {
	register_callbacks();
	prepare_shader_program();
	initialize_OpenGL();
	prepare_scene();
	initialize_camera();
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = GL_TRUE;

	error = glewInit();
	if (error != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "********************************************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "********************************************************************************\n\n");
}

void print_message(const char* m) {
	fprintf(stdout, "%s\n\n", m);
}

void greetings(char* program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "********************************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n********************************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 9
void drawScene(int argc, char* argv[]) {
	char program_name[64] = "Sogang CSE4170 Sun Temple Scene";
	char messages[N_MESSAGE_LINES][256] = { 
		"    - Keys used:",
		"		'f' : draw x, y, z axes and grid",
		"		'1' : set the camera for original view",
		"		'2' : set the camera for bronze statue view",
		"		'3' : set the camera for bronze statue view",	
		"		'4' : set the camera for top view",
		"		'5' : set the camera for front view",
		"		'6' : set the camera for side view",
		"		'ESC' : program close",
	};

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(900, 600);
	glutInitWindowPosition(20, 20);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}
