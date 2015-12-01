// Assignment05CG.cpp : Defines the entry point for the console application.
#include "stdafx.h"
#include "Shader.h"
#include "Object.h"
#include "RigidBody.h"
#include "Texture.h"
#include <math.h>
#include <cmath>
#include "Skybox.h"
// Macro for indexing vertex buffer
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

using namespace std;

int width = 800;
int height = 600;

float foward = 0.0f;
float sideMov = 0.0f;

GLfloat rotation_camera_y = 0.0f;
GLfloat rotation_camera_x = 0.0f;

Texture* earthTexture;
Texture* frigateTexture;
Texture* moonTexture;
Texture* sunTexture;

Shader* shader;
Shader* nonDiffuseShader;
Shader* nonTextureShader;
Shader* skyBoxShader;

Object* earth;
RigidBody* frigate;
Object* moon;
Object* sun;

Skybox* skybox;


// Direction that the camera is towarding
vec3 ViewDir;

vec3 camera_position = vec3(0.0, 0.0, -150.0f);

float cameraSpeedFoward = 1.5f;
float cameraSpeedRotation = 0.04f;

bool cameraRotationXEnabled = false;
bool cameraRotationYEnabled = false;

// Init the view and projection static for every object
mat4 Object::view = identity_mat4();
mat4 Object::projection = identity_mat4();
int Object::vboId = 0;

float t = 0.001;

void display() {
	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 view = identity_mat4();
	mat4 projection = perspective(45.0, (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT), 0.1, 2000.0);

	// Camera rotation control
	frigate->update();

	// Moving the camera in the direction of the view
	camera_position = camera_position + (ViewDir*foward);
	foward = 0;

	rotation_camera_x = (frigate->getRotation()).v[0];
	rotation_camera_y = (frigate->getRotation()).v[1];
	
	mat4 rotation = rotate_y_deg(identity_mat4(), rotation_camera_y);
	rotation = rotate_x_deg(rotation, rotation_camera_x);
	vec3 offset = vec3(rotation*vec4(0.0, 10.0, -25.0, 1.0));

	vec3 cam_pos_lookat = frigate->getPosition() + offset;
	vec3 up = vec3(0.0, 1.0, 0.0);

	view = look_at(cam_pos_lookat, frigate->getPosition(), vec3(rotation*vec4(0.0, 1.0, 0.0, 1.0)));

	// Set up projection and view matrix for all the objects statically
	Object::setProjectionMatrix(projection);
	Object::setViewMatrix(view);
	skybox->display(projection, view);

	moon->rotate(0, 0.06f, 0);

	earth->rotate(0, 0.05f, 0);
	
	sun->rotate(0, 0.01f, 0);
	sun->display();

	frigate->display();

	glutSwapBuffers();
}

void init()
{
	// Set up the shaders
	shader = new Shader("Shaders/simpleVertexShader.hlsl", "Shaders/simpleFragmentShader.txt");
	nonDiffuseShader = new Shader("Shaders/simpleVertexShader.hlsl", "Shaders/nonDiffuseFragmentShader.hlsl");
	nonTextureShader = new Shader("Shaders/simpleVertexShader.hlsl", "Shaders/nonTextureFragmentShader.txt");

	skyBoxShader = new Shader("Shaders/skyBoxVertexShader.hlsl", "Shaders/skyBoxFragmentShader.hlsl");

	skybox = new Skybox("", "skybox.jpg", skyBoxShader);
	// Instantiate the textures
	earthTexture = new Texture("models/Earth_Diffuse.jpg");
	moonTexture = new Texture("models/moonmap2k.jpg");
	sunTexture = new Texture("models/sun.jpg");
	frigateTexture = new Texture("models/Maps/zqw1b.jpg");

	// Instantitate the objects
	frigate = new RigidBody(nonTextureShader, "models/space_frigate.obj");
	earth = new Object(shader, "models/Earth.obj", earthTexture);
	moon = new Object(shader, "models/Earth.obj", moonTexture);
	sun = new Object(nonDiffuseShader, "models/Earth.obj", sunTexture);

	// Doing initial transformations
	earth->rotate(0.0, 0.0, 180.0);
	earth->move(100.0, 0.0, 200.0);
	earth->scaleAll(0.5);

	frigate->scaleAll(0.1);
	frigate->move(0, 0.0f, 100.0f);
	frigate->rotate(0, 90, 0);

	moon->scaleAll(0.3);
	moon->move(75.0f, 0, 0.0f);

	earth->addChild(moon);
	sun->addChild(earth);
}


GLint midWindowX = width / 2;         // Middle of the window horizontally
GLint midWindowY = height / 2;         // Middle of the window vertically

void mouseMove(int x, int y) {

	bool changed = false;

	if (x > glutGet(GLUT_WINDOW_WIDTH) - 50 && x < glutGet(GLUT_WINDOW_WIDTH)) {
		cameraRotationXEnabled = true;
		cameraSpeedRotation *= cameraSpeedRotation > 0 ? -1 : 1;
		changed = true;
	}

	if (x < 50 && x >= 0) {
		cameraRotationXEnabled = true;
		cameraSpeedRotation *= cameraSpeedRotation < 0 ? -1 : 1;
		changed = true;
	}

	if (y > glutGet(GLUT_WINDOW_HEIGHT) - 50 && x < glutGet(GLUT_WINDOW_HEIGHT)) {
		cameraRotationYEnabled = true;
		cameraSpeedRotation *= cameraSpeedRotation > 0 ? -1 : 1;
		changed = true;
	}

	if (y < 50 && y >= 0) {
		cameraRotationYEnabled = true;
		cameraSpeedRotation *= cameraSpeedRotation < 0 ? -1 : 1;
		changed = true;
	}


	if (changed) {
		return;
	}

	cameraRotationXEnabled = false;
	cameraRotationYEnabled = false;
}

// Placeholder code for the keypress
void keypress(unsigned char key, int x, int y) {
	rotation_camera_x = (frigate->getRotation()).v[0];
	rotation_camera_y = (frigate->getRotation()).v[1];

	vec3 Step1, Step2;
	//Rotate around Y-axis:
	Step1.v[0] = cos((rotation_camera_y + 90.0) * ONE_DEG_IN_RAD);
	Step1.v[2] = -sin((rotation_camera_y + 90.0) * ONE_DEG_IN_RAD);

	//Rotate around X-axis:
	double cosX = cos(rotation_camera_x * ONE_DEG_IN_RAD);
	Step2.v[0] = Step1.v[0] * cosX;
	Step2.v[2] = Step1.v[2] * cosX;
	Step2.v[1] = sin(rotation_camera_x * ONE_DEG_IN_RAD);

	//Rotation around Z-axis not yet implemented, so:
	ViewDir = Step2;


	if (key == 'x') {
		exit(0);
	}

	if (key == 'a') {
		frigate->rotate(0, 1, 0);
	}

	if (key == 'd') {
		frigate->rotate(0,-1,0);
	}

	if (key == 'w') {
		frigate->rotate(1, 0, 0);
	}

	if (key == 's') {
		frigate->rotate(-1, 0, 0);
	}

	if (key == 'k') {
		frigate->applyForce(ViewDir*0.000001);
	}

	if (key == 'j') {
		frigate->applyForce(ViewDir*-0.000001);
	}

	
}

void updateScene() {
	// Placeholder code, if you want to work with framerate
	// Wait until at least 16ms passed since start of last frame (Effectively caps framerate at ~60fps)
	static DWORD  last_time = 0;
	DWORD  curr_time = timeGetTime();
	float  delta = (curr_time - last_time) * 0.001f;
	if (delta > 0.03f)
		delta = 0.03f;
	last_time = curr_time;

	// Draw the next frame
	glutPostRedisplay();
}

int main(int argc, char** argv) {
	// Set up the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutCreateWindow("Space Jam");

	// Tell glut where the display function is
	glutDisplayFunc(display);
	glutIdleFunc(updateScene);
	glutKeyboardFunc(keypress);

	glutPassiveMotionFunc(mouseMove);

	// A call to glewInit() must be done after glut is initialized!
	GLenum res = glewInit();
	// Check for any errors
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}
	// Set up your objects and shaders
	init();
	// Begin infinite event loop
	glutMainLoop();
	return 0;
}











