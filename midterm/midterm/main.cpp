#include<GL/glut.h>
#include<GL/GL.h>
#include<GL/GLU.h>
#include<math.h>
#include<stdio.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

const double pi = 3.14;

//�� ��ġ ����
GLfloat body_location = 7;
GLfloat body_rotate = 0;

//��, �ٸ� , ť�� ������
GLfloat angle_upper_arm = 0;
GLfloat angle_lower_arm = 0;
GLfloat angle_upper_leg = 0;
GLfloat angle_lower_leg = 0;
GLfloat angle_cue = 0;
GLfloat size_cue = 2;

//��, �ٸ� ��ġ
int dir_upper_arm = 1;
int dir_lower_arm = 1;
int dir_upper_leg = 1;
int dir_lower_leg = 1;

//�Ʒ��ٸ� �����̱� �б�
int flag_lower_leg_left = 0;
int flag_lower_leg_right = 0;

//�� ��ġ
GLfloat ball_x_1 = -0.1;
GLfloat ball_z_1 = 1;
GLfloat ball_x_2 = -0.3;
GLfloat ball_z_2 = 0;

//�� ������ġ
GLfloat next_x_1 = 0;
GLfloat next_z_1 = 0;
GLfloat next_x_2 = 0;
GLfloat next_z_2 = 0;

//�� �ӵ�
GLfloat ball_s_1 = 0;
GLfloat ball_s_2 = 0;
int flag_ball = 0;

//�� ����
GLfloat angle_ball_1 = 0;
GLfloat angle_ball_2 = 0;

//ī�޶�
GLfloat camx = -7, camy = 3, camz = 20;
GLfloat cam2x = 0, cam2y = 0, cam2z = 0;
GLfloat cam_upx = 0, cam_upy = 1, cam_upz = 0;

//������
int count = 0;

void initLight(GLfloat r, GLfloat g, GLfloat b, GLfloat light) {
	//Ȯ�� ���
	GLfloat mat_diffuse[] = { 0.9*r, 0.9*g, 0.9*b, 1.0 };
	//��� �ݻ�
	GLfloat mat_specular[] = { 0.5, 0.5, 0.5, 1.0 };
	//�ֺ� �ݻ�
	GLfloat mat_ambient[] = { 0.9*r, 0.9*g, 0.9*b, 1.0 };
	//���� ���
	GLfloat mat_shininess[] = { 50.0 * light };
	GLfloat light_diffuse[] = { 0.7, 0.7, 0.7, 1.0 };
	GLfloat light_specular[] = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat light_ambient[] = { 0.7, 0.7, 0.7, 1.0 };
	//���� ��ġ
	GLfloat light_position[] = { 0, 6, 0, 0.0 };
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);

	//0�� ���� Ȱ��ȭ
	glEnable(GL_LIGHT0);

	//���� ����
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);

	//��ü ����
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	//glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
}

void initVisibility() {
	//glEnable(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);

	//glFrontFace(GL_CW);
	//glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
}

void myReshape(int NewWidth, int NewHeight) {
	GLfloat left = 0, bottom = 0;
	glViewport(left, bottom, NewWidth - left, NewHeight - bottom);	// �׷��� ǥ�� ���� ����

	//��Ʈ���� �ʱ�ȭ
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	GLfloat ratio = (float)(NewWidth - left) / (NewHeight - bottom);
	gluPerspective(40, ratio, 0.1, 100);
}

//������ü
void drawCuboid(GLfloat sx, GLfloat sy, GLfloat sz) {
	glPushMatrix();
	glScalef(sx, sy, sz);
	glutSolidCube(1);
	glPopMatrix();
}

//����
void drawBody() {
	drawCuboid(0.5, 1, 0.25);
}

//����
void drawUpperArm(GLfloat angle, int dir_arm) {
	glTranslatef(dir_arm*0.3, 0.4, 0);
	glRotatef(-90 + 20*dir_arm, 0, 0, 1);
	glRotatef(angle, 0, 1, 0);
	glTranslatef(0.25, 0, 0);

	drawCuboid(0.5, 0.2, 0.2);
}

//�Ʒ���
void drawLowerArm(GLfloat angle) {
	glTranslatef(0.25, 0, 0);
	glRotatef(angle, 0, 1, 0);
	glTranslatef(0.25, 0, 0);

	drawCuboid(0.5, 0.2, 0.2);
}

//��
void drawHand() {
	glTranslatef(0.35, 0, 0);

	glutSolidSphere(0.1, 10, 10);
}

//�Ӹ�
void drawHead() {
	glTranslatef(0, 0.7, 0);

	glutSolidSphere(0.2, 10, 10);
}

//�����
void drawUpperLeg(GLfloat angle, int dir_leg) {
	glTranslatef(dir_leg*0.17, -0.35, 0);
	glRotatef(angle, angle, 0, 1);
	glTranslatef(0, -0.5, 0);

	drawCuboid(0.2, 0.7, 0.2);
}

//���Ƹ�
void drawLowerLeg(GLfloat angle) {
	glTranslatef(0, -0.35, 0);
	glRotatef(180-angle/2, 180-angle/2, 0, 1);
	glTranslatef(0, 0.3, 0);

	drawCuboid(0.2, 0.6, 0.2);
}

//�籸��
void drawBall(GLfloat x, GLfloat z, GLfloat r, GLfloat g, GLfloat b) {
	initLight(1*r, 1*g, 1*b, 1);

	glPushMatrix();

	glTranslatef(x, 0.1, z);
	glutSolidSphere(0.05, 20, 20);

	glPopMatrix();

	initLight(1, 1, 1, 1);
}

//�籸�� �ٸ�
void drawTableLeg(){
	initLight(1, 0.5, 0, 0.4);

	glTranslatef(-0.95, -0.8, -1.95);
	drawCuboid(0.1, 1.5, 0.1);

	glTranslatef(1.9, 0, 0);
	drawCuboid(0.1, 1.5, 0.1);

	glTranslatef(0, 0, 3.9);
	drawCuboid(0.1, 1.5, 0.1);

	glTranslatef(-1.9, 0, 0);
	drawCuboid(0.1, 1.5, 0.1);

	initLight(1, 1, 1, 1);
}

//�籸��
void drawTable() {
	initLight(0, 0.5, 0, 0.4);

	drawCuboid(2, 0.1, 4);

	glPushMatrix();

	glTranslatef(0.95, 0.1, 0);
	drawCuboid(0.1, 0.1, 4);

	glTranslatef(-1.9, 0, 0);
	drawCuboid(0.1, 0.1, 4);

	glTranslatef(0.95, 0, -1.95);
	drawCuboid(1.8, 0.1, 0.1);

	glTranslatef(0, 0, 3.9);
	drawCuboid(1.8, 0.1, 0.1);

	glPopMatrix();

	initLight(1, 1, 1, 1);
}

//ť��
void drawCue(GLfloat angle, GLfloat x, GLfloat y, GLfloat z) {
	initLight(1, 0.5, 0, 0.8);

	glTranslatef(0.1, 0, 0);
	glRotatef(angle, x, y, z);
	glRotatef(70, 0, 0, 1);
	glTranslatef(0.05, 0.8, 0);

	drawCuboid(0.1, size_cue, 0.1);

	initLight(1, 1, 1, 1);
}

//�� �浹
void collisionWall() {
	//���
	if (next_x_1 >= 0.9 || next_x_1 <= -0.9) {
		angle_ball_1 = pi - angle_ball_1;
	}
	if (next_z_1 >= 1.9 || next_z_1 <= -1.9) {
		angle_ball_1 = -angle_ball_1;
	}

	//������
	if (next_x_2 >= 0.9 || next_x_2 <= -0.9) {
		angle_ball_2 = pi - angle_ball_2;
	}
	if (next_z_2 >= 1.9 || next_z_2 <= -1.9) {
		angle_ball_2 = -angle_ball_2;
	}
}

//�� �浹
void collisionBall() {
	//������ġ�� �浹�ߴٸ�
	if (pow(next_x_2 - next_x_1, 2) + pow(next_z_2 - next_z_1, 2) <= 0.01) {
		//�浹��
		GLfloat theta_col = atan2(next_z_2 - next_z_1, next_x_2 - next_x_1);

		//�� ���� ���� ������
		GLfloat vx_1 = cos((angle_ball_1 - theta_col))*ball_s_1;
		GLfloat vz_1 = sin((angle_ball_1 - theta_col))*ball_s_1;
		GLfloat vx_2 = cos((angle_ball_2 - theta_col))*ball_s_2;
		GLfloat vz_2 = sin((angle_ball_2 - theta_col))*ball_s_2;

		//�� �ӵ� �� ���� ���
		ball_s_1 = sqrt(pow(vx_2, 2) + pow(vz_1, 2));
		ball_s_2 = sqrt(pow(vx_1, 2) + pow(vz_2, 2));

		angle_ball_1 = theta_col + atan2(vz_1, vx_2);
		angle_ball_2 = theta_col + atan2(vz_2, vx_1);
	}
}

//ȭ��
void myDisplay() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glColor3f(0.9, 0.9, 0.9);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(camx, camy, camz, cam2x, cam2y, cam2z, cam_upx, cam_upy, cam_upz);

	//���̺�
	glPushMatrix();
	drawTable();
	drawTableLeg();
	glPopMatrix();

	//������
	glPushMatrix();
	drawBall(ball_x_2, ball_z_2, 1, 0, 0);
	glPopMatrix();

	//���
	glPushMatrix();
	drawBall(ball_x_1, ball_z_1, 1, 1, 1);
	glPopMatrix();

	//��
	glTranslatef(-0.5, 0.5, body_location);
	glRotatef(body_rotate, 0, 1, 0);
	drawBody();
	glPushMatrix();
	drawHead();
	glPopMatrix();

	//������
	glPushMatrix();
	drawUpperArm(angle_upper_arm, 1);
	drawLowerArm(angle_lower_arm);
	drawHand();
	drawCue(angle_cue, 1, 0.5, 0.2);
	glPopMatrix();

	//����
	glPushMatrix();
	drawUpperArm(angle_upper_arm, -1);
	drawLowerArm(angle_lower_arm);
	drawHand();
	glPopMatrix();
	
	//�����ٸ�
	glPushMatrix();
	drawUpperLeg(360-angle_upper_leg, 1);
	if (!flag_lower_leg_right && (angle_upper_leg <= -20)) {
		flag_lower_leg_right = 1;
	}
	if(flag_lower_leg_right)
		drawLowerLeg(angle_lower_leg+20);
	else
		drawLowerLeg(0);
	glPopMatrix();

	//�޴ٸ�
	glPushMatrix();
	drawUpperLeg(360+angle_upper_leg, -1);
	if (!flag_lower_leg_left && (angle_upper_leg >= 20)) {
		flag_lower_leg_left = 1;
	}
	if (flag_lower_leg_left)
		drawLowerLeg(20-angle_lower_leg);
	else
		drawLowerLeg(0);
	glPopMatrix();

	//glFlush();
	glutSwapBuffers();
}

//Ÿ�̸�
void myTimer(int value) {
	//ī�޶� ��ü ȸ��
	//0~7��
	if (count >= 0 && count < 280) {
		GLfloat theta = 0.02;

		camx = camx * cos(theta) + camz * sin(theta);
		camz = -camx * sin(theta) + camz * cos(theta);
	}

	//�ɾ
	//7~12��
	else if (count >= 280 && count < 480) {
		//�ٸ�
		angle_upper_leg += dir_upper_leg;
		angle_lower_leg += dir_lower_leg;

		if (angle_upper_leg >= 20)
			dir_upper_leg = -1;
		else if (angle_upper_leg <= -20)
			dir_upper_leg = 1;

		if (angle_lower_leg >= 20)
			dir_lower_leg = -1;
		else if (angle_lower_leg <= -20)
			dir_lower_leg = 1;

		body_location -= 0.02;
		camx += 0.04;
		camz -= 0.04;
	}
	
	//ť�� ����
	//12~15
	else if (count >= 480 && count < 600) {
		body_rotate -= 0.7;
		angle_upper_arm += 0.25;
		angle_lower_arm += 0.3;
		angle_cue -= 1.5;
	}
	
	//ī�޶� ���� ����
	//15~18
	else if (count >= 600 && count < 720) {
		camx += 0.07;
		camy -= 0.015;
	}

	//�� ħ
	//18~22
	else if (count >= 720 && count < 880) {
		if (count < 780)
			size_cue += 0.02;
		else if (count < 860)
			size_cue -= 0.02;
		else if (count > 870)
			size_cue += 0.2;
	}

	//�� �����ٴ�
	//22~27
	else if (count >= 880 && count < 1080) {

		//�ڼ� �������
		size_cue -= 0.01;
		if (count < 1000) {
			body_rotate += 0.7;
			angle_upper_arm -= 0.25;
			angle_lower_arm -= 0.3;
			angle_cue += 1.5;
		}

		//�� ������
		//�ӵ� �ο�
		if (flag_ball == 0) {
			ball_s_1 = 0.1;
			angle_ball_1 = 2*pi-(atan((double)1/5) + pi/2);
			flag_ball = 1;
		}

		//���� ��ġ ���
		next_x_1 = ball_x_1 + cos(angle_ball_1)*ball_s_1;
		next_z_1 = ball_z_1 + sin(angle_ball_1)*ball_s_1;
		next_x_2 = ball_x_2 + cos(angle_ball_2)*ball_s_2;
		next_z_2 = ball_z_2 + sin(angle_ball_2)*ball_s_2;

		//�� �浹
		collisionWall();

		//�� �浹
		collisionBall();

		//�� ��ǥ �̵�
		ball_x_1 = next_x_1;
		ball_z_1 = next_z_1;
		ball_x_2 = next_x_2;
		ball_z_2 = next_z_2;

		//���� ����
		if (ball_s_1 > 0)
			ball_s_1 *= 0.99;
		if (ball_s_2 > 0)
			ball_s_2 *= 0.99;

		//���� ��ȯ
		camx += 0.0035;
		camy += 0.015;
		camz += 0.0125;
	}

	count++;

	glutTimerFunc(20, myTimer, 1);
	glutPostRedisplay();
}
/*Ű���� �Լ�, �ִϸ��̼ǰ��� ���� ����
//Ű�����Է�
void myKeyboard(unsigned char KeyPressed, int X, int Y) {
	switch (KeyPressed) {
	case 'f':
		camy += 0.2;	break;
	case 'd':
		camy -= 0.2;	break;
	}
}

//����Ű
void mySpecialKeyboard(int KeyPressed, int X, int Y) {
	switch (KeyPressed) {
	case GLUT_KEY_LEFT:
		camx -= 0.2;	break;
	case GLUT_KEY_RIGHT:
		camx += 0.2;	break;
	case GLUT_KEY_UP:
		camz -= 0.2;	break;
	case GLUT_KEY_DOWN:
		camz += 0.2;	break;
	}
}
*/

//����
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Mid Term Project");

	glClearColor(0.4, 0.4, 0.4, 1.0);

	initLight(1, 1, 1, 1);
	initVisibility();

	glutDisplayFunc(myDisplay);
	glutReshapeFunc(myReshape);

	glutTimerFunc(20, myTimer, 1);

	//�ٸ� ���������� ���¸� Ȯ���ϱ� ���� Ű���� �Լ�
	//glutKeyboardFunc(myKeyboard);
	//glutSpecialFunc(mySpecialKeyboard);

	glutMainLoop();
	return 0;
}