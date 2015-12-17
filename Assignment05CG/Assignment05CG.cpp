// Assignment05CG.cpp : Defines the entry point for the console application.
#include "stdafx.h"
#include "Shader.h"
#include "Object.h"
#include "RigidBody.h"
#include "Planet.h"
#include "Texture.h"
#include <math.h>
#include <cmath>
#include "Skybox.h"
// Macro for indexing vertex buffer
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

using namespace std;

int width = 800;
int height = 600;

GLfloat rotation_camera_y = 0.0f;
GLfloat rotation_camera_x = 0.0f;

Texture* earthTexture;
Texture* playerTexture;
Texture* moonTexture;
Texture* sunTexture;

Shader* shader;
Shader* nonDiffuseShader;
Shader* nonTextureShader;
Shader* skyBoxShader;
Shader* textShader;

Object* earth;
RigidBody* player;
Object* moon;
Object* sun;

Skybox* skybox;

RigidBody* bulletModel;
std::vector<Object*> asteroids;
std::vector<RigidBody*> bullets;

// Direction that the camera is towarding
vec3 ViewDir;

vec3 camera_position = vec3(0.0, 0.0, -150.0f);

// Init the view and projection static for every object
mat4 Object::view = identity_mat4();
mat4 Object::projection = identity_mat4();
int Object::vboId = 0;

float t = 0.001;
float player_rotation_z = 0;

bool gameOver = false;

// Framebuffer
GLuint fbo, fbo_texture, rbo_depth;

/* Global */
GLuint program_postproc, attribute_v_coord_postproc, uniform_fbo_texture;
GLuint uniform_fbo_offset;
Shader* postProcessingShader;

GLuint vbo_fbo_vertices;

void drawText(float x, float y, std::string text, vec4 color = vec4(1.0f,1.0f,1.0f, 1.0f)) {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0, width, 0.0, height);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glUseProgram(textShader->getProgramID());
	glRasterPos2i(x, y);
	glColor4f(color.v[0], color.v[1], color.v[2], color.v[3]);
	
	for (int i = 0; i<text.size(); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
	}

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

void updateViewDir() {
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

	ViewDir = normalise(ViewDir);
}

float orientation_rotation_y = 0;

mat4 updateCamera(){
	// Camera rotation control
	updateViewDir();
	player->setViewDir(vec3(ViewDir.v[0], 0.0, ViewDir.v[2]));
	player->update();

	mat4 rotation = rotate_x_deg(identity_mat4(), rotation_camera_x);
	rotation = rotate_y_deg(rotation, rotation_camera_y + 180 );
	vec3 offset = vec3(rotation*vec4(0.0, 0.0, -45.0, 1.0));

	vec3 cam_pos_lookat = player->getPosition() + offset;
	vec3 up = vec3(0.0, 1.0, 0.0);

	mat4 view = look_at(cam_pos_lookat, player->getPosition(), vec3(vec4(up, 1.0)));

	player->setRotation(vec3(0.0, rotation_camera_y + 90 + 180 , 0.0));
	return view;
}

void display() {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	string gameStatus;
	if(asteroids.size() > 0){
		gameStatus = "Asteroids Left: ";
		gameStatus.append(to_string(asteroids.size()));
	}else{
		gameStatus = "Game over ! You Win !";
	}
	drawText(10, 10, gameStatus);

	GLfloat fogColor[4] = { 1.0f,1.0f,1.0f,1.0f };

	/*glEnable(GL_FOG);
	glFogf(GL_FOG_MODE, GL_LINEAR);
	glFogfv(GL_FOG_COLOR, fogColor);

	glFogf(GL_FOG_START, 0.1f);
	glFogf(GL_FOG_END, 500.0f);*/

	mat4 view = updateCamera();
	mat4 projection = perspective(45.0, (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT), 0.1, 100000.0);
	
	// Set up projection and view matrix for all the objects statically
	Object::setProjectionMatrix(projection);
	Object::setViewMatrix(view);
	skybox->display(projection, view);

	moon->rotate(0, 0.06f, 0);

	earth->rotate(0, 0.05f, 0);
	player->display();

	// Displaying the bullets	
	for (std::vector<RigidBody*>::iterator it = bullets.begin(); it != bullets.end(); ) {
		(*it)->update();
		(*it)->display();

		if (length((*it)->getPosition()) > 500) {
			it = bullets.erase(it);
		}
		else {
			++it;
		}
	}

	// Displaying the asteroids and testing the collision with player and bullets
	for (std::vector<Object*>::iterator it = asteroids.begin(); it != asteroids.end();) {
		(*it)->rotate(0, 1.0f, 0);
		(*it)->display();
		
		bool hited = false;

		for (std::vector<RigidBody*>::iterator b = bullets.begin(); b != bullets.end(); ) {
			if ((*it)->checkCollision(*b)) {
				it = asteroids.erase(it);
				b  = bullets.erase(b);
				hited = true;
			}
			else {
				++b;
			}
		}

		if (!hited) {
			++it;
		}
	}
	
	
	sun->rotate(0, 0.01f, 0);
	sun->display();

	// Post-Processing
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(postProcessingShader->getProgramID());
	glBindTexture(GL_TEXTURE_2D, fbo_texture);
	glUniform1i(uniform_fbo_texture, 0);
	glEnableVertexAttribArray(attribute_v_coord_postproc);

	GLfloat move = glutGet(GLUT_ELAPSED_TIME) / 1000.0 * 2 * 3.14159 * .75;  // 3/4 of a wave cycle per second
	glUniform1f(uniform_fbo_offset, move);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_fbo_vertices);
	glVertexAttribPointer(
		attribute_v_coord_postproc,  // attribute
		2,                  // number of elements per vertex, here (x,y)
		GL_FLOAT,           // the type of each element
		GL_FALSE,           // take our values as-is
		0,                  // no extra data between each position
		0                   // offset of first element
		);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDisableVertexAttribArray(attribute_v_coord_postproc);
	
	glutSwapBuffers();
}


void init()
{
	cout << "Initialization of assets" << endl;

	// Set up the shaders
	shader = new Shader("Shaders/simpleVertexShader.hlsl", "Shaders/simpleFragmentShader.hlsl");
	nonDiffuseShader = new Shader("Shaders/simpleVertexShader.hlsl", "Shaders/nonDiffuseFragmentShader.hlsl");
	nonTextureShader = new Shader("Shaders/simpleVertexShader.hlsl", "Shaders/nonTextureFragmentShader.hlsl");
	
	textShader = new Shader("Shaders/identityVertexShader.hlsl", "Shaders/identityFragmentShader.hlsl");
	skyBoxShader = new Shader("Shaders/skyBoxVertexShader.hlsl", "Shaders/skyBoxFragmentShader.hlsl");

	postProcessingShader = new Shader("Shaders/postprocessingVertex.hlsl", "Shaders/postprocessingFragment.hlsl");

	// Skybox initialization
	skybox = new Skybox("", "skybox.jpg", skyBoxShader);
	// Instantiate the textures
	earthTexture = new Texture("models/Earth_Diffuse.jpg");
	moonTexture = new Texture("models/moonmap2k.jpg");
	sunTexture = new Texture("models/sun.jpg");
	playerTexture = new Texture("models/Maps/zqw1b.jpg");

	// Instantitate the objects
	player = new RigidBody(nonTextureShader, "models/space_frigate.obj");
	earth = new Object(shader, "models/Earth.obj", earthTexture);
	moon = new Object(shader, "models/Earth.obj", moonTexture);
	sun = new Planet(nonDiffuseShader, "models/Earth.obj", sunTexture);

	//Texture* missileTexture = new Texture("models/missile.png");
	bulletModel = new RigidBody(nonTextureShader, "models/missile.obj");
	bulletModel->setPosition(vec3(0, 0, 0));
	bulletModel->rotate(0.0f, 90.0f, 0);
	
	// Doing initial transformations
	earth->rotate(0.0, 0.0, 180.0);
	earth->move(100.0, 0.0, 400.0);
	earth->scaleAll(0.5);

	player->scaleAll(0.5f);
	player->move(0, 0.0f, 150.0f);

	moon->scaleAll(0.3);
	moon->move(75.0f, 0, 0.0f);

	earth->addChild(moon);
	sun->addChild(earth);

	// Procedurally genering the asteroids using a number and a radious to the asteroid ring
	int numberAsteroids = 200;
	float radious = 200;

	// Generating the asteroidModel
	Object* asteroidModel = new Object(nonTextureShader, "models/asteroid.3ds");
	for (int i = 0; i < numberAsteroids; i++)
	{
		Object* obj = asteroidModel->clone();
		float teta = (360 / numberAsteroids)*i;
		float r = (i % 3) * 50 + radious;
		float x = r*cos(teta);
		float y = r*sin(teta);

		obj->move(x, (rand() % 10)*10 - 50 , y);

		float sizeRand = rand() % 10;
		sizeRand = sizeRand / 10;

		obj->scaleAll(sizeRand);
		obj->rotate(rand() % 90, rand() % 90, rand() % 90);

		asteroids.push_back(obj);
	}

	bullets = std::vector<RigidBody*>();
	cout << "Finished initialization" << endl;

	/* init_resources */
	/* Create back-buffer, used for post-processing */
	
	// Post processing config
	GLchar* attribute_name = "v_coord";
	attribute_v_coord_postproc = glGetAttribLocation(postProcessingShader->getProgramID(), attribute_name);
	if (attribute_v_coord_postproc == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
	}

	GLchar* uniform_name = "fbo_texture";
	uniform_fbo_texture = glGetUniformLocation(postProcessingShader->getProgramID(), uniform_name);
	if (uniform_fbo_texture == -1) {
		fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
	}

	/*GLchar* uniform_offset = "offset";
	uniform_fbo_offset = glGetUniformLocation(postProcessingShader->getProgramID(), uniform_offset);
	if (uniform_fbo_offset == -1) {
		fprintf(stderr, "Could not bind uniform %s\n", uniform_offset);
	}*/

	/* init_resources */
	GLfloat fbo_vertices[] = {
		-1, -1,
		1, -1,
		-1,  1,
		1,  1,
	};
	glGenBuffers(1, &vbo_fbo_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_fbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_vertices), fbo_vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Texture */
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &fbo_texture);
	glBindTexture(GL_TEXTURE_2D, fbo_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	/* Depth buffer */
	glGenRenderbuffers(1, &rbo_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	/* Framebuffer to link everything together */
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth);
	GLenum status;
	if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "glCheckFramebufferStatus: error %p", status);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void keypress(unsigned char key, int x, int y) {

	if (key == 'x') {
		exit(0);
	}

	if (key == 'a') {
		rotation_camera_y += 1;
	}

	if (key == 'd') {
		rotation_camera_y -= 1;
	}

	if (key == 'w') {
		rotation_camera_x += 1;
	}

	if (key == 's') {
		rotation_camera_x -= 1;
	}

	if (key == 'k') {
		// Creating a bullet when player hits k
		RigidBody* bullet = bulletModel->clone();
		vec3 bulletViewDir = vec3(ViewDir.v[0], -ViewDir.v[1], ViewDir.v[2]);
		bullet->setViewDir(bulletViewDir);
		bullet->setVelocity(2.0f);
		bullet->setPosition(player->getPosition());
		bullets.push_back(bullet);
	}

	updateViewDir();
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

void exit() {
	glDeleteRenderbuffers(1, &rbo_depth);
	glDeleteTextures(1, &fbo_texture);
	glDeleteFramebuffers(1, &fbo);

	/* free_resources */
	glDeleteBuffers(1, &vbo_fbo_vertices);
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

	//glutPassiveMotionFunc(mouseMove);

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

	atexit(exit);
	return 0;
}


