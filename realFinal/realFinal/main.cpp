#include<GL/glut.h>
#include<GL/GL.h>
#include<GL/GLU.h>
#include<opencv2/opencv.hpp>
#include<math.h>
#include<stdio.h>
#include"model.hpp"

using namespace cv;

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 600

string filepath[2] = { "E:/blenderObj/object/p1.obj" , "E:/blenderObj/object/p2.obj" };
CModel m;

const double pi = 3.14;

//함수 선언
void initLight(GLfloat r, GLfloat g, GLfloat b, GLfloat light);
void displayObjs();

//카메라
GLfloat camx = 0, camy = 1000, camz = 0;
GLfloat cam2x = 0, cam2y = 0, cam2z = -2;
GLfloat cam_upx = 0, cam_upy = 1, cam_upz = 0;

//마우스
int mouse_left_click = 0;
int start_x = 0;
int start_z = 0;

//플레이어
int player = 1;
int tern_check = 1;
int player_available_black[2] = { 0, 0 };
int player_win = 0;

//홀 클래스
class hole {
public:
	float hole_location[2];

	hole(float *loc) {
		hole_location[0] = loc[0];
		hole_location[1] = loc[1];
	}
};

//공 클래스
class ball {
public:
	float ball_color[3];
	float ball_location[2];
	float ball_init_location[2];
	float ball_next_location[2];
	float ball_speed;
	float ball_angle;
	int ball_draw;
	int ball_player;
	int ball_holein;

	ball(float *color, float *loc, float *loc_in, float angle, int player) {
		ball_color[0] = color[0];
		ball_color[1] = color[1];
		ball_color[2] = color[2];

		ball_location[0] = loc[0];
		ball_location[1] = loc[1];

		ball_init_location[0] = loc_in[0];
		ball_init_location[1] = loc_in[1];

		ball_next_location[0] = 0;
		ball_next_location[1] = 0;

		ball_speed = 0;

		ball_angle = angle;

		ball_draw = 1;

		ball_player = player;

		ball_holein = 0;
	}

	//다음 위치 계산
	void nextLocation() {
		ball_next_location[0] = ball_location[0] + cos(ball_angle)*ball_speed;
		ball_next_location[1] = ball_location[1] + sin(ball_angle)*ball_speed;
	}

	//홀 인
	void holeIn(hole &h) {
		if (pow(h.hole_location[0] - ball_next_location[0], 2) + pow(h.hole_location[1] - ball_next_location[1], 2) <= 3025) {
			//자신 턴의 공을 하나라도 넣었는지 체크
			if (ball_player == player) {
				tern_check = 1;
			}

			//자신의 다른 공을 다 넣었으면 검은 공도 홀인 가능
			if (ball_player == 3) {
				if (player_available_black[player - 1]) {
					//공을 밖으로 이동
					ball_location[0] = ball_init_location[0];
					ball_location[1] = ball_init_location[1];
					ball_speed = 0;
					ball_draw = 0;
					ball_holein = 1;

					//승리 표시 플래그
					player_win = player;
				}
			}
			else {
				//공을 밖으로 이동
				ball_location[0] = ball_init_location[0];
				ball_location[1] = ball_init_location[1];
				ball_speed = 0;
				ball_draw = 0;
				ball_holein = 1;
			}
			
		}
	}

	//벽 충돌
	void collisionWall() {
		if (ball_next_location[0] >= 430 || ball_next_location[0] <= -430) {
			ball_angle = pi - ball_angle;
			nextLocation();
		}
		if (ball_next_location[1] >= 190 || ball_next_location[1] <= -190) {
			ball_angle = -ball_angle;
			nextLocation();
		}
	}

	//공 충돌
	void collisionBall(ball &b) {
		//다음위치가 충돌했다면
		if (pow(b.ball_next_location[0] - ball_next_location[0], 2) + pow(b.ball_next_location[1] - ball_next_location[1], 2) <= 2500) {
			//충돌각
			GLfloat theta_col = atan2(b.ball_next_location[1] - ball_next_location[1], b.ball_next_location[0] - ball_next_location[0]);

			//공 벡터 성분 나누기
			GLfloat vx_1 = cos((ball_angle - theta_col))*ball_speed;
			GLfloat vz_1 = sin((ball_angle - theta_col))*ball_speed;
			GLfloat vx_2 = cos((b.ball_angle - theta_col))*b.ball_speed;
			GLfloat vz_2 = sin((b.ball_angle - theta_col))*b.ball_speed;

			//공 속도 및 방향 계산
			ball_speed = sqrt(pow(vx_2, 2) + pow(vz_1, 2));
			b.ball_speed = sqrt(pow(vx_1, 2) + pow(vz_2, 2));

			ball_angle = theta_col + atan2(vz_1, vx_2);
			b.ball_angle = theta_col + atan2(vz_2, vx_1);

			nextLocation();
			b.nextLocation();
		}
	}

	//다음 위치로 이동
	void moveBall() {
		ball_location[0] = ball_next_location[0];
		ball_location[1] = ball_next_location[1];

		//마찰 감속
		ball_speed *= 0.99;
		if (ball_speed < 0.3) {
			ball_speed = 0;
		}
	}

};

//공 리스트
ball *ball_list[16];

//홀 리스트
hole *hole_list[6];

//공 위치
float ball_loc[16][2] = {
	{ 250, 0 },	//수구
	{ -150, 0 },	//1
	{ -350, 120 },	//11
	{ -350, -120 },	//15
	{ -350, 0 },	//13
	{ -300, -30 },	//9
	{ -250, -60 },	//7
	{ -300, 90 },	//8
	{ -250, 0.0 },	//8번구
	{ -300, -90 },	//10
	{ -200, 30 },	//2
	{ -350, 60 },	//12
	{ -300, 30 },	//6
	{ -250, 60 },	//4
	{ -200, -30 },	//3
	{ -350, -60 }	//14

};

//홀인 후 표시 위치 
float ball_loc_in[16][2] = {
	{ 0, 0 },	//수구
	{ -400, 300 },
	{ -350, 300 },
	{ -300, 300 },
	{ -250, 300 },
	{ -200, 300 },
	{ -150, 300 },
	{ -100, 300 },
	{ -550, 30 },	//8번구
	{ 100, -300 },
	{ 150, -300 },
	{ 200, -300 },
	{ 250, -300 },
	{ 300, -300 },
	{ 350, -300 },
	{ 400, -300 }
};


//공 색상
float ball_clr[16][3] = {
	{ 1, 1, 1 },	//수구
	{ 0.4, 0.7, 0.5 },
	{ 1, 0.5, 1 },
	{ 0.5, 1, 1 },
	{ 1, 0.5, 0.5 },
	{ 1, 1, 0 },
	{ 0.5, 0.5, 1 },
	{ 0.5, 0.5, 0.5 },
	{ 0, 0, 0 },
	{ 1, 0, 1 },
	{ 0, 1, 1 },
	{ 1, 0, 0 },
	{ 0, 0.5, 0 },
	{ 0, 0, 1 },
	{ 0.8, 0.5, 0 },
	{ 0, 0.5, 0.8 }
};

/*
//공 색상 단색 버전
float ball_clr[16][3] = {
	{ 1, 1, 1 },	//수구
	{ 0.7, 0, 0 },
	{ 0.7, 0, 0 },
	{ 0.7, 0, 0 },
	{ 0.7, 0, 0 },
	{ 0.7, 0, 0 },
	{ 0.7, 0, 0 },
	{ 0.7, 0, 0 },
	{ 0, 0, 0 },
	{ 0, 0, 0.7 },
	{ 0, 0, 0.7 },
	{ 0, 0, 0.7 },
	{ 0, 0, 0.7 },
	{ 0, 0, 0.7 },
	{ 0, 0, 0.7 },
	{ 0, 0, 0.7 },
};
*/

//공 플레이어 입력
int ball_p[16] = {
	0,
	1, 1, 1, 1, 1, 1, 1,
	3,
	2, 2, 2, 2, 2, 2, 2
};

//홀 위치
float hole_loc[6][2] = {
	{ -450, 200 },
	{ 0, 220 },
	{ 450, 200 },
	{ -450, -200 },
	{ 0, -220 },
	{ 450, -200 }
};

//포켓볼 초기화
void initPocket() {
	for (int i = 0; i < 16; i++) {
		ball_list[i] = new ball(ball_clr[i], ball_loc[i], ball_loc_in[i], 90, ball_p[i]);
	}

	for (int i = 0; i < 6; i++) {
		hole_list[i] = new hole(hole_loc[i]);
	}
}

void initLight(GLfloat r, GLfloat g, GLfloat b, GLfloat light) {
	//확산 방사
	GLfloat mat_diffuse[] = { r, g, b, 1.0 };
	//경면 반사
	GLfloat mat_specular[] = { 1 * light, 1 * light, 1 * light, 1 };
	//주변 반사
	GLfloat mat_ambient[] = { r * light, g * light, b * light, 1.0 };
	//광택 계수
	GLfloat mat_shininess[] = { 50.0 };
	GLfloat light_diffuse[] = { 0.7, 0.7, 0.7, 1.0 };
	GLfloat light_specular[] = { 1, 1, 1, 1.0 };
	GLfloat light_ambient[] = { 0.7, 0.7, 0.7, 1.0 };
	//광원 위치
	GLfloat light_position[] = { 0, 1500, 100, 0.0 };
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

	//0번 광원 활성화
	glEnable(GL_LIGHT0);

	//광원 정보
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);

	//물체 정보
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
}

void myReshape(int NewWidth, int NewHeight) {
	GLfloat left = 0, bottom = 0;
	GLfloat WidthFactor = (GLfloat)NewWidth / (GLfloat)WINDOW_WIDTH;
	GLfloat HeighthFactor = (GLfloat)NewHeight / (GLfloat)WINDOW_HEIGHT;
	glViewport(left, bottom, NewWidth - left, NewHeight - bottom);	// 그래픽 표시 영역 설정

	//매트릭스 초기화
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	GLfloat ratio = (float)(NewWidth - left) / (NewHeight - bottom);
	gluPerspective(40, ratio, 0.1, 5000);
}

//직육면체
void drawCuboid(GLfloat sx, GLfloat sy, GLfloat sz) {
	glPushMatrix();
	glScalef(sx, sy, sz);
	glutSolidCube(1);
	glPopMatrix();
}

//당구 홀
void drawHole(GLfloat x, GLfloat z) {
	initLight(0, 0, 0, 0);

	glPushMatrix();

	glTranslatef(x, 15, z);
	glRotatef(90, 1, 0, 0);

	glBegin(GL_POLYGON);
	for (int i = 0; i < 360; i++)
	{
		float angle = i * pi / 180;
		float cx = 35 * cos(angle);
		float cz = 35 * sin(angle);
		glVertex2f(cx, cz);
	}
	glEnd();

	glPopMatrix();

	initLight(1, 1, 1, 1);
}

//당구공
void drawBall(GLfloat x, GLfloat z, GLfloat r, GLfloat g, GLfloat b, int p) {
	int ball_light;

	//8번공은 따로 하이라이트 체크
	if ((player_available_black[player-1] && (p == 3)) || (p == 0)) {
		ball_light = 1;
	}
	else {
		ball_light = (p == player) ? 1 : 0;
	}

	initLight(r, g, b, ball_light);

	glPushMatrix();

	glTranslatef(x, 40, z);
	glutSolidSphere(25, 20, 20);

	glPopMatrix();

	initLight(1, 1, 1, 1);
}

//당구대
void drawTable() {
	initLight(0, 0.5, 0, 1);

	drawCuboid(1000, 25, 500);

	initLight(0.6, 0.3, 0, 0.7);

	glPushMatrix();

	glTranslatef(487.5, 25, 0);
	drawCuboid(25, 25, 500);

	glTranslatef(-975, 0, 0);
	drawCuboid(25, 25, 500);

	glTranslatef(487.5, 0, -237.5);
	drawCuboid(950, 25, 25);

	glTranslatef(0, 0, 475);
	drawCuboid(950, 25, 25);

	glPopMatrix();

	initLight(1, 1, 1, 1);
}

//모델
void loadModel() {
	ifstream fin(filepath[player_win - 1]);
	m.loadObj(fin);
	fin.close();
}

//오브젝트 표시
void displayObjs() {
	GLfloat x, y, z, nx, ny, nz, tx, ty;
	int v_id, vt_id, vn_id, idx = 1;

	for (int o = 0; o < m.objs.size(); o++) {
		int nFaces = m.objs[o].f.size();
		for (int k = 0; k < nFaces; k++) {
			int nPoints = m.objs[0].f[k].v_pairs.size();
			glBegin(GL_POLYGON);
			for (int i = 0; i < nPoints; i++) {
				v_id = m.objs[0].f[k].v_pairs[i].d[0];
				vt_id = m.objs[0].f[k].v_pairs[i].d[1];
				vn_id = m.objs[0].f[k].v_pairs[i].d[2];
				x = m.objs[0].v[v_id - 1].d[0];
				y = m.objs[0].v[v_id - 1].d[1];
				z = m.objs[0].v[v_id - 1].d[2];

				nx = m.objs[0].vn[vn_id - 1].d[0];
				ny = m.objs[0].vn[vn_id - 1].d[1];
				nz = m.objs[0].vn[vn_id - 1].d[2];

				tx = m.objs[0].vt[vt_id - 1].d[0];
				ty = m.objs[0].vt[vt_id - 1].d[1];

				glNormal3f(nx, ny, nz);
				glTexCoord2f(tx, ty);
				glVertex3f(x, y, z);
			}
			glEnd();
		}
	}
}

//화면
void myDisplay() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(camx, camy, camz, cam2x, cam2y, cam2z, cam_upx, cam_upy, cam_upz);

	//테이블
	glPushMatrix();
	drawTable();
	glPopMatrix();

	//홀
	for (int i = 0; i < 6; i++) {
		glPushMatrix();
		drawHole(hole_list[i]->hole_location[0], hole_list[i]->hole_location[1]);
		glPopMatrix();
	}

	//공
	for (int i = 0; i < 16; i++) {
		glPushMatrix();
		drawBall(ball_list[i]->ball_location[0], ball_list[i]->ball_location[1], ball_list[i]->ball_color[0], ball_list[i]->ball_color[1], ball_list[i]->ball_color[2], ball_list[i]->ball_player);
		glPopMatrix();
	}

	//승리 표시
	if (player_win != 0) {
		loadModel();
		glPushMatrix();
		initLight(0, 0, 0, 0);
		glTranslatef(0, 300, 0);
		displayObjs();

		glPopMatrix();
	}

	glutSwapBuffers();
}

//타이머
void myTimer(int value) {
	for (int i = 0; i < 16; i++) {
		ball_list[i]->nextLocation();
	}

	for (int i = 1; i < 16; i++) {
		for (int j = 0; j < 6; j++) {
			ball_list[i]->holeIn(*hole_list[j]);
		}
	}

	for (int i = 0; i < 16; i++) {
		ball_list[i]->collisionWall();
		for (int j = i; j < 16; j++) {
			ball_list[i]->collisionBall(*ball_list[j]);
		}
	}

	for (int i = 0; i < 16; i++) {
		ball_list[i]->moveBall();
	}

	glutTimerFunc(20, myTimer, 1);
	glutPostRedisplay();
}

//키보드입력
void myKeyboard(unsigned char KeyPressed, int X, int Y) {
	switch (KeyPressed) {
	case 'f':
		camy += 50;	break;
	case 'd':
		camy -= 50;	break;
	}
}

//방향키
void mySpecialKeyboard(int KeyPressed, int X, int Y) {
	switch (KeyPressed) {
	case GLUT_KEY_LEFT:
		camx -= 50;	break;
	case GLUT_KEY_RIGHT:
		camx += 50;	break;
	case GLUT_KEY_UP:
		camz -= 50;	break;
	case GLUT_KEY_DOWN:
		camz += 50;	break;
	}
}

//마우스
void myMouse(int button, int state, int x, int z) {
	x = x - 500;
	z = 300 - z;

	//좌클릭
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && ball_list[0]->ball_speed == 0) {
		mouse_left_click = 1;
		start_x = x;
		start_z = z;
		//printf("x = %d, z = %d, click = %d\n", x, z, mouse_left_click);

		//플레이어 전환
		if (!tern_check) {
			player = player % 2 + 1;
		}

		//8번구 홀인가능 체크
		if (player_available_black[player - 1] == 0) {
			//일단 가능으로 두고
			player_available_black[player - 1] = 1;
			//홀인되지 않은 자기번호가 존재하면 불가능으로 교체후 브레이크
			for (int i = 1; i < 8; i++) {
				if (ball_list[i + 8 * (player - 1)]->ball_holein == 0) {
					player_available_black[player - 1] = 0;
					break;
				}
			}
		}
	}
	//좌클릭 해제
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP && mouse_left_click) {
		mouse_left_click = 0;
		tern_check = 0;

		//공 방향 및 속도 계산
		ball_list[0]->ball_angle = atan2(double(z - start_z), double(start_x - x));
		ball_list[0]->ball_speed = sqrt(pow(start_x - x, 2.0) + pow(start_z - z, 2.0)) / 20;
	}

}

//메인
int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Final Project");

	glClearColor(0.8, 0.8, 0.8, 1.0);

	initLight(1, 1, 1, 1);
	initPocket();

	glutDisplayFunc(myDisplay);
	glutReshapeFunc(myReshape);

	glutTimerFunc(20, myTimer, 1);

	//다른 각도에서의 상태를 확인하기 위한 키보드 함수
	glutKeyboardFunc(myKeyboard);
	glutSpecialFunc(mySpecialKeyboard);
	glutMouseFunc(myMouse);

	glutMainLoop();
	return 0;
}