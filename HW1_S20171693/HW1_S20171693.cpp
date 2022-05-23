#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stack>
#include <math.h>
#include <algorithm>
#include <vector>

int window_width = 0, window_height = 0;

int rightbuttonpressed = 0;
//float r = 0.0f, g = 0.0f, b = 0.0f; // Backgroud color = Black
float r = 250.0f / 255.0f, g = 128.0f / 255.0f, b = 114.0f / 255.0f; // Background color = Salmon
float x_cor = 0.0, y_cor = 0.0;
int window_garo = 500;				//일단은 페이지 처음 시작 사이즈
int window_sero = 500;
bool p_clicked = false;				//p가 눌렸으면 true 가 된다. p로 다각형 만들기 실패했을경우 다시 false로 바꿔주자.
bool polygon_made = false;			//p를 눌러서 다각형을 만드는데 성공하면 true가 된다. 이게 true면 p를 다시 눌러줘도 다각형이 더 생성되면 안된다.
bool keyboard_clicked = false;		//방향키 4개중 하나가 눌리면 true 로 설정되고, 좌표 옮긴거만큼 그대로 다시 찍어주자. 이건 다각형이 완성되어있을때에만 돌아가게 하자.
float coordinate_stack[129][2];		//좌표들을 저장하자.
int index_coor = 0;					//좌표들이 차곡차곡 배열에 쌓일 때 그 인덱스
float center_x = 0;
float center_y = 0;
bool mousescroll = false;


int animation_mode = 0;				//r키 눌리고있으면 애니매이션 모드로 회전하게...
unsigned int timestamp = 0;
int zRotate = 0;
bool animate_flag = false;
bool first_move = false;

void timer(int value) {
	timestamp = (timestamp + 1) % UINT_MAX;
	//fprintf(stdout, "$$$ animation mode is %d", animation_mode);
	glutPostRedisplay();
	if (animation_mode)
		glutTimerFunc(500, timer, 0);
}

void calculate_weight_center()
{
	float x_sum = 0; float y_sum = 0;
	for (int i = 0; i < index_coor; i++)
	{
		x_sum += coordinate_stack[i][0];
		y_sum += coordinate_stack[i][1];
	}
	center_x = x_sum / (float)index_coor;
	center_y = y_sum / (float)index_coor;

	//new_coordinate_func();
}

void print_center_func()
{
	calculate_weight_center();
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POINTS);
	float x_for_win, y_for_win;
	x_for_win = (center_x - (500.0 / 2.0)) * (1.0 / (500.0 / 2.0));
	y_for_win = -1 * (center_y - (500.0 / 2.0)) * (1.0 / (500.0 / 2.0));
	glVertex3f(x_for_win, y_for_win, 0.0f);
	glEnd();
}

void calculate_rotated()
{
	float rotate_angle = 0.3;
	float rotated_x, rotated_y;
	for (int i = 0; i < index_coor; i++)
	{
		coordinate_stack[i][0] -= center_x;
		coordinate_stack[i][1] -= center_y;

		rotated_x = coordinate_stack[i][0] * cos(rotate_angle) + coordinate_stack[i][1] * sin(rotate_angle);
		rotated_y = coordinate_stack[i][0] * -1*sin(rotate_angle) + coordinate_stack[i][1] * cos(rotate_angle);
		coordinate_stack[i][0] = rotated_x;
		coordinate_stack[i][1] = rotated_y;

		coordinate_stack[i][0] += center_x;
		coordinate_stack[i][1] += center_y;
	}
}

void draw_func(int animated)
{
	
	if (animated == 1)				//회전중일때는 무게중심을 찍어.
	{
		print_center_func();
		calculate_rotated();
		draw_func(0);
	}
	else
	{
		glColor3f(1.0f, 1.0f, 0.0f);
		glBegin(GL_POINTS);						//점을 직어주기
		for (int i = 0; i < index_coor; i++)
		{
			float x_for_win, y_for_win;
			float pre_x, pre_y;
			pre_x = coordinate_stack[i][0]; pre_y = coordinate_stack[i][1];

			x_for_win = (pre_x - (500.0 / 2.0)) * (1.0 / (500.0 / 2.0));
			y_for_win = -1 * (pre_y - (500.0 / 2.0)) * (1.0 / (500.0 / 2.0));

			glVertex3f(x_for_win, y_for_win, 0.0f);
		}
		glEnd();

		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_LINE_STRIP);					//점들 이어주기

		if (p_clicked == true)
		{
			if (index_coor > 2)
			{
				glColor3f(1.0f, 1.0f, 1.0f);

				glBegin(GL_LINE_LOOP);

				for (int i = 0; i < index_coor; i++)
				{
					float x_for_win, y_for_win;
					float pre_x, pre_y;
					pre_x = coordinate_stack[i][0]; pre_y = coordinate_stack[i][1];

					x_for_win = (pre_x - (500.0 / 2.0)) * (1.0 / (500.0 / 2.0));
					y_for_win = -1 * (pre_y - (500.0 / 2.0)) * (1.0 / (500.0 / 2.0));

					glVertex3f(x_for_win, y_for_win, 0.0f);
				}
				polygon_made = true;
				glEnd();
			}
			else
			{
				fprintf(stdout, "$$$ you need to choose more points to make a polygon\n");
				p_clicked = false;					//2개일때 p누르면, 에러메시지 출력하는데, 이후에 점 하나를 더 찍으면 p변수가 아까눌렸던거 그대로 남아있어서 바로 다각형 생성..
			}
		}
	}
}

void display(void) {
	glClearColor(r, g, b, 1.0f); 
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0f, 1.0f, 1.0f);	
	if (polygon_made == false)					//대전제로, 만약 이미 다각형이 만들어졌다면 c가 입력되기전까지 아무런 일도 하면 안되는거다.
	{
		glColor3f(1.0f, 1.0f, 0.0f);
		glBegin(GL_POINTS);						//점을 직어주기
		for (int i = 0; i < index_coor; i++)
		{
			float x_for_win, y_for_win;
			float pre_x, pre_y;
			pre_x = coordinate_stack[i][0]; pre_y = coordinate_stack[i][1];

			x_for_win = (pre_x - (500.0 / 2.0)) * (1.0 / (500.0 / 2.0));
			y_for_win = -1 * (pre_y - (500.0 / 2.0)) * (1.0 / (500.0 / 2.0));

			glVertex3f(x_for_win, y_for_win, 0.0f);
		}
		glEnd();
		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_LINE_STRIP);					//점들 이어주기
	
		/// 좌표 변환해주는 코드
		/// <param name=""></param>
		for (int i = 0; i < index_coor; i++)
		{
			float x_for_win, y_for_win;
			float pre_x, pre_y;
			pre_x = coordinate_stack[i][0]; pre_y = coordinate_stack[i][1];

			x_for_win = (pre_x - (500.0 / 2.0)) * (1.0 / (500.0 / 2.0));
			y_for_win = -1 * (pre_y - (500.0 / 2.0)) * (1.0 / (500.0 / 2.0));

			glVertex3f(x_for_win, y_for_win, 0.0f);
		}
	
		glEnd();
	
		if (p_clicked == true)
		{
			if (index_coor > 2)
			{
				glColor3f(1.0f, 1.0f, 1.0f);
				glBegin(GL_LINE_LOOP);

				for (int i = 0; i < index_coor; i++)
				{
					float x_for_win, y_for_win;
					float pre_x, pre_y;
					pre_x = coordinate_stack[i][0]; pre_y = coordinate_stack[i][1];

					x_for_win = (pre_x - (500.0 / 2.0)) * (1.0 / (500.0 / 2.0));
					y_for_win = -1 * (pre_y - (500.0 / 2.0)) * (1.0 / (500.0 / 2.0));

					glVertex3f(x_for_win, y_for_win, 0.0f);
				}

				polygon_made = true;
				glEnd();
			}
			else
			{
				fprintf(stdout, "$$$ you need to choose more points to make a polygon\n");
				p_clicked = false;					//2개일때 p누르면, 에러메시지 출력하는데, 이후에 점 하나를 더 찍으면 p변수가 아까눌렸던거 그대로 남아있어서 바로 다각형 생성..

			}
		}
	}
	//다각형이 만들어지기전 선이을때
	else
	{
		if (animation_mode == 1 )
		{
			print_center_func();
			animate_flag = true;
			draw_func(1);

		}
		else
		{
			if(animate_flag == false)
				draw_func(0);

		}
	}

	glFlush();
	glPointSize(5.0f);
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 's':
		r = 250.0f / 255.0f, g = 128.0f / 255.0f, b = 114.0f / 255.0f;
		fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		glutPostRedisplay();
		break;
	case 'f':
		fprintf(stdout, "$$$ Thank you for using this program. \n");
		glutLeaveMainLoop(); 
		break;
	case 'p':
		p_clicked = true;
		glutPostRedisplay();
		break;
	case 'c':
		if (animation_mode != 1)
		{
			p_clicked = false;
			polygon_made = false;
			for (int i = 0; i < index_coor; i++)
			{
				coordinate_stack[i][0] = 0;
				coordinate_stack[i][1] = 0;
			}
			index_coor = 0;
			animation_mode = 0;
			glutPostRedisplay();
		}
		break;
	case 'r':							//rotate 해야지
		if (polygon_made == true)
		{
			//fprintf(stdout, " INSINDE R KEY TAPPED, :  %d\n", animation_mode);
			animation_mode = 1 - animation_mode;
			if (animation_mode)
			{
				animate_flag == true;
				glutTimerFunc(10, timer, 0);
			}
			else
			{
				animate_flag = false;
			}
		}
		break;
	}

	
}

void special(int key, int x, int y) {
	if (animation_mode != 1)
	{
		switch (key) {
		case GLUT_KEY_LEFT:
			if (polygon_made == true)
			{
				for (int i = 0; i < index_coor; i++)
				{
					coordinate_stack[i][0] -= 10;
				}
			}
			keyboard_clicked = true;
			calculate_weight_center();
			glutPostRedisplay();
			break;
		case GLUT_KEY_RIGHT:
			if (polygon_made == true)
			{
				for (int i = 0; i < index_coor; i++)
				{
					coordinate_stack[i][0] += 10;
				}
			}
			keyboard_clicked = true;
			calculate_weight_center();
			glutPostRedisplay();
			break;
		case GLUT_KEY_DOWN:
			if (polygon_made == true)
			{
				for (int i = 0; i < index_coor; i++)
				{
					coordinate_stack[i][1] += 10;
				}
			}
			keyboard_clicked = true;
			calculate_weight_center();
			glutPostRedisplay();
			break;
		case GLUT_KEY_UP:
			if (polygon_made == true)
			{
				for (int i = 0; i < index_coor; i++)
				{
					coordinate_stack[i][1] -= 10;
				}
			}
			keyboard_clicked = true;
			calculate_weight_center();
			glutPostRedisplay();
			break;
		}
	}

}

void mousepress(int button, int state, int x, int y) {
	int mod = glutGetModifiers();
	if (mod == GLUT_ACTIVE_SHIFT)
	{
		if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)  && polygon_made == false) {
			//fprintf(stdout, "*** The left mouse button was pressed at (%d, %d).\n", x, y);
			x_cor = x;
			y_cor = y;
			coordinate_stack[index_coor][0] = x;
			coordinate_stack[index_coor][1] = y;
			index_coor++; 
			rightbuttonpressed = 0;
			mousescroll = false;
			glutPostRedisplay();
			//fprintf(stdout, "x_cor, y_cor %f, %f\n", x_cor, y_cor);
		}

	}
	
	if (button == GLUT_RIGHT_BUTTON && (state == GLUT_UP || state == GLUT_DOWN))
	{
		rightbuttonpressed = 1;
	}
	else
	{
		mousescroll = false;
		rightbuttonpressed = 0;
	}
}

void mousemove(int x, int y) {
	if (rightbuttonpressed)
	{
		if (polygon_made == true)
		{
			if (animation_mode != 1)
			{
				calculate_weight_center();

				//fprintf(stdout, "$$$ The right mouse button is now at (%d, %d).\n", x, y);
				for (int i = 0; i < index_coor; i++)
				{
					float origin_x = coordinate_stack[i][0];
					float origin_y = coordinate_stack[i][1];
					coordinate_stack[i][0] = (float)x - center_x + origin_x;
					coordinate_stack[i][1] = (float)y - center_y + origin_y; 
				}
				center_x = x;
				center_y = y;
				mousescroll = true;
		
				//if (animation_mode != 1)
				//{
					glutPostRedisplay();
			}
		}
	}
}
	
void reshape(int width, int height) {
	fprintf(stdout, "### The new window size is %dx%d.\n", width, height);
	window_width = width, window_height = height;
	glViewport(0, 0, width, height);
}

void close(void) {
	fprintf(stdout, "\n^^^ The control is at the close callback function now.\n\n");
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mousepress);
	glutMotionFunc(mousemove);
	glutReshapeFunc(reshape);
 	glutCloseFunc(close);
}

void initialize_renderer(void) {
	register_callbacks();
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = TRUE;
	error = glewInit();
	if (error != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "*********************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "*********************************************************\n\n");
}

void greetings(char *program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 4
void main(int argc, char *argv[]) {
	char program_name[64] = "Sogang CSE4170 SimplefreeGLUTcode";
	char messages[N_MESSAGE_LINES][256] = {
		"    - Keys used: 'r', 'g', 'b', 's', 'q'",
		"    - Special keys used: LEFT, RIGHT, UP, DOWN",
		"    - Mouse used: L-click, R-click and move",
		"    - Other operations: window size change"
	};

	glutInit(&argc, argv);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE); // <-- Be sure to use this profile for this example code!

	glutInitDisplayMode(GLUT_RGBA);

	glutInitWindowSize(500, 500);
	glutInitWindowPosition(500, 200);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	
	glutMainLoop();
	fprintf(stdout, "^^^ The control is at the end of main function now.\n\n");
}