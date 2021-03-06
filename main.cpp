#include <iostream>
#include <vector>
#include <random>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "gl_core_3_3.h"
#include <GL/freeglut.h>
#include "util.hpp"
#include "ball.h"
#include "KinectReader.h"

using namespace std;
using namespace glm;

// Global state
GLuint shader;			// Shader program
GLuint uniXform;		// Matrix location in shader
GLuint vao;				// Vertex array object
GLuint vbuf;			// Vertex buffer
GLsizei vcount;			// Number of vertices

// Texture data
GLuint colorTex;			// OpenGL texture holding color data
GLuint depthTex;			// OpenGL texture holding depth data
const GLint texWidth = 640;		// Kinect image dimensions
const GLint texHeight = 480;
vector<GLubyte> colorData;	// Color data from kinect
vector<GLubyte> depthData;	// Depth data from kinect
int viewmode;				// 0 = show color, 1 = show depth
ball b(0,0,0.1,1,1);

KinectReader* myKinect;

// Sandbox-specific parameters
#define SANDBOX_URBAN

// Kinect / projector correspondence - manually derived
// These are the texture coordinates of the projected rectangle boundaries
// Use them to fit the projection to real space
#ifdef SANDBOX_URBAN
float kpLeft = 0.0621;
float kpRight = 0.8128;
float kpTop = 0.84952;
float kpBottom = 0.1238;

int winXOffset = 0;
#else
// Parameters for laptop setup
float kpLeft = 0.08828;
float kpRight = 0.82891;
float kpTop = 0.91250;
float kpBottom = 0.14583;

// Change this to match your display configuration
int winXOffset = 2880;
#endif
const int winWidth = 1400;
const int winHeight = 1050;

// Initialization functions
void initState();
void initGLUT(int* argc, char** argv);
void initOpenGL();
void initQuad();
void initTextures();

// Callback functions
void display();
void reshape(GLint width, GLint height);
void keyRelease(unsigned char key, int x, int y);
void mouseBtn(int button, int state, int x, int y);
void mouseMove(int x, int y);
void idle();
void cleanup();

mat4 aspectMtx();
ivec2 winToTex(ivec2 winPos);

int main(int argc, char** argv) {
	try {
		// Initialize
		initState();
		initGLUT(&argc, argv);
		initOpenGL();
		initQuad();
		initTextures();
		myKinect = new KinectReader(colorData,depthData);

	} catch (const exception& e) {
		// Handle any errors
		cerr << "Fatal error: " << e.what() << endl;
		cleanup();
		return -1;
	}

	// Execute main loop
	glutMainLoop();

	return 0;
}

void initState() {
	// Initialize global state
	shader = 0;
	uniXform = 0;
	vao = 0;
	vbuf = 0;
	vcount = 0;
	colorTex = 0;
	depthTex = 0;
	viewmode = 1;	// Draw depth
}

void initGLUT(int* argc, char** argv) {
	// Set window and context settings
	glutInit(argc, argv);
	glutInitWindowSize(winWidth, winHeight);
	glutInitWindowPosition(winXOffset, 0);
	glutInitContextVersion(3, 3);
	glutInitContextFlags(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE | GLUT_BORDERLESS | GLUT_CAPTIONLESS);
	// Create the window
	glutCreateWindow("Kinect demo: press space to switch view modes");

	// GLUT callbacks
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardUpFunc(keyRelease);
	glutMouseFunc(mouseBtn);
	glutMotionFunc(mouseMove);
	glutIdleFunc(idle);
	glutCloseFunc(cleanup);
}

void initOpenGL() {
	// Set clear color and depth
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	// Set viewport
	glViewport(0, 0, winWidth, winHeight);

	// Compile and link shader program
	vector<GLuint> shaders;
	shaders.push_back(compileShader(GL_VERTEX_SHADER, "sh_v.glsl"));
	shaders.push_back(compileShader(GL_FRAGMENT_SHADER, "sh_f.glsl"));
	shader = linkProgram(shaders);
	// Release shader sources
	for (auto s = shaders.begin(); s != shaders.end(); ++s)
		glDeleteShader(*s);
	shaders.clear();
	// Locate uniforms
	uniXform = glGetUniformLocation(shader, "xform");

	// Bind texture image unit
	glUseProgram(shader);
	GLuint texLoc = glGetUniformLocation(shader, "tex");
	glUniform1i(texLoc, 0);
	glUseProgram(0);
}

void initQuad() {
	// Create a textured quad
	struct vert {
		vert(vec2 p, vec2 u) : pos(p), uv(u) {}
		vec2 pos;	// Vertex position
		vec2 uv;	// Texture coordinates
	};
	vector<vert> verts;
	verts.push_back(vert(vec2(-1.0f, -1.0f), vec2(0.0, 0.0)));
	verts.push_back(vert(vec2( 1.0f, -1.0f), vec2(1.0, 0.0)));
	verts.push_back(vert(vec2( 1.0f,  1.0f), vec2(1.0, 1.0)));
	verts.push_back(vert(vec2( 1.0f,  1.0f), vec2(1.0, 1.0)));
	verts.push_back(vert(vec2(-1.0f,  1.0f), vec2(0.0, 1.0)));
	verts.push_back(vert(vec2(-1.0f, -1.0f), vec2(0.0, 0.0)));
	vcount = verts.size();

	// Create vertex array object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create vertex buffer
	glGenBuffers(1, &vbuf);
	glBindBuffer(GL_ARRAY_BUFFER, vbuf);
	glBufferData(GL_ARRAY_BUFFER, vcount * sizeof(vert), verts.data(), GL_STATIC_DRAW);
	// Specify vertex attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vert), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vert), (GLvoid*)sizeof(vec2));
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void initTextures() {
	// Create textures to hold kinect data
	colorData = vector<GLubyte>(texWidth * texHeight * 4, 255);
	depthData = vector<GLubyte>(texWidth * texHeight, 255);

	glGenTextures(1, &colorTex);
	glBindTexture(GL_TEXTURE_2D, colorTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, colorData.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &depthTex);
	glBindTexture(GL_TEXTURE_2D, depthTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, texWidth, texHeight, 0, GL_RED, GL_UNSIGNED_BYTE, depthData.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void display() {
	try {
		// Get image from Kinect
		if (viewmode == 0)
		{
			// Upload texture to OpenGL
			glBindTexture(GL_TEXTURE_2D, colorTex);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, myKinect->kinectGetColor());
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		else
		{
			// Upload texture to OpenGL
			glBindTexture(GL_TEXTURE_2D, depthTex);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, texWidth, texHeight, 0, GL_RED, GL_UNSIGNED_BYTE, myKinect->kinectGetDepth());
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		// Clear the back buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Get ready to draw
		glUseProgram(shader);
		glBindVertexArray(vao);

		// Activate texture
		glActiveTexture(GL_TEXTURE0 + 0);
		if (viewmode == 0)
			glBindTexture(GL_TEXTURE_2D, colorTex);
		else
			glBindTexture(GL_TEXTURE_2D, depthTex);

		// Transform texture coordinates to fit projection to sandbox
		mat4 xform;
		xform [0][0] = kpRight - kpLeft;
		xform [3][0] = kpLeft;
		xform [1][1] = kpTop - kpBottom;
		xform [3][1] = kpBottom;
		// Send transformation matrix to shader
		glUniformMatrix4fv(uniXform, 1, GL_FALSE, value_ptr(xform));
		GLuint viewmodeLoc = glGetUniformLocation(shader, "viewmode");
		glUniform1i(viewmodeLoc, viewmode);

		// Draw the textured quad
		glDrawArrays(GL_TRIANGLES, 0, vcount);
		b.draw();
		// Revert context state
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
		glUseProgram(0);

		// Display the back buffer
		glutSwapBuffers();

	} catch (const exception& e) {
		cerr << "Fatal error: " << e.what() << endl;
		glutLeaveMainLoop();
	}
}

void reshape(GLint width, GLint height) {
	// Prevent resizing
	glutReshapeWindow(winWidth, winHeight);
}

void keyRelease(unsigned char key, int x, int y) {
	switch (key) {
	case 27:	// Escape key - quit program
		glutLeaveMainLoop();
		break;
	case ' ':
		viewmode = !viewmode;
		break;		
	}
}

void mouseBtn(int button, int state, int x, int y){
 
}
void mouseMove(int x, int y) {}

void idle() {
	glutPostRedisplay();
}

void cleanup() {
	// Release all resources
	if (shader) { glDeleteProgram(shader); shader = 0; }
	uniXform = 0;
	if (vao) { glDeleteVertexArrays(1, &vao); vao = 0; }
	if (vbuf) { glDeleteBuffers(1, &vbuf); vbuf = 0; }
	vcount = 0;
	if (colorTex) { glDeleteTextures(1, &colorTex); colorTex = 0; }
	if (depthTex) { glDeleteTextures(1, &depthTex); depthTex = 0; }
	colorData.clear();
	depthData.clear();
}

mat4 aspectMtx() {
	// Returns a matrix that preserves texture aspect ratio regardless of window size
	mat4 xform;
	float winAspect = (float)winWidth / (float)winHeight;
	float texAspect = (float)texWidth / (float)texHeight;
	xform [0] [0] = std::min(texAspect / winAspect, 1.0f);
	xform [1] [1] = std::min(winAspect / texAspect, 1.0f);
	return xform;
}

// Converts window pixel coordinates into texture pixel coordinates
ivec2 winToTex(ivec2 winPos) {
	// Get aspect correction matrix
	mat4 xform = aspectMtx();

	// Transform window pixel coordinates into clip-space coordinates
	vec2 clipPos(2.0f * winPos.x / (float)winWidth - 1.0f, 2.0f * (winHeight - winPos.y) / (float)winHeight - 1.0f);
	// Invert aspect correction to get quad coordinates
	vec2 quadPos = vec2(inverse(xform) * vec4(clipPos, 0.0f, 1.0f));
	// Convert quad coordinates into texture pixel coordinates
	ivec2 texPos = ivec2(((quadPos + vec2(1.0f)) / vec2(2.0f)) * vec2(texWidth, texHeight));

	return texPos;
}
