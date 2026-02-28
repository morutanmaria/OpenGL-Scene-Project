//template taken from lab9
#if defined (__APPLE__)
    #define GLFW_INCLUDE_GLCOREARB
    #define GL_SILENCE_DEPRECATION
#else
    #define GLEW_STATIC
    #include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "Model3D.hpp"
#include "Camera.hpp"

#include <iostream>

int glWindowWidth = 800;
int glWindowHeight = 600;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;
unsigned int quadVAO, quadVBO, quadEBO;

enum RenderMode {
	SOLID,
	WIREFRAME
};

RenderMode renderMode = SOLID;
bool flatShading = false;
bool fogEnabled = true;

//lab 5 camera movement
float lastX = 400.0f;
float lastY = 300.0f;
bool firstMouse = true;

float yaw = -90.0f;
float pitch = 0.0f;
float mouseSensitivity = 0.2f;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat4 lightRotation;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

gps::Camera myCamera(
				glm::vec3(0.0f, 1.0f, 10.5f), 
				glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f));
float cameraSpeed = 0.2f;

bool pressedKeys[1024];
float angleY = 0.0f;
GLfloat lightAngle;

gps::Model3D cottage;
gps::Model3D ground;
gps::Model3D apa;
gps::Model3D catel;
gps::Model3D gull;
gps::Model3D screenQuad;
gps::Model3D normalcat;
gps::Model3D padure1;
gps::Model3D sky;

gps::Shader myCustomShader;
gps::Shader lightShader;
gps::Shader screenQuadShader;
gps::Shader depthShader;

float uvScale = 5.0f;
GLuint uvScaleLoc;

GLuint shadowMapFBO;
GLuint depthMapTexture;

bool showDepthMap;

//Gull
float radius = 5.0f;
float speed = 0.5f;

//Dog
enum TransformMode { NONE, TRANSLATE, ROTATE, SCALE };
TransformMode dogMode = NONE;
//Initial trans scale rotate
glm::vec3 dogPosition = glm::vec3(1.0f, -1.1f, -0.2f); 
glm::vec3 dogRotation = glm::vec3(0.0f); 
glm::vec3 dogScale = glm::vec3(1.0f); 
float transformStep = 0.1f; 
float rotationStep = 5.0f;  
float scaleStep = 0.05f;

//Rain
bool rainEnabled = false;
const int NR_RAIN_DROPS = 2000;
glm::vec3 rainPositions[NR_RAIN_DROPS];
float rainSpeed = 0.5f;
gps::Model3D rainDrop;


GLenum glCheckError_(const char *file, int line) {
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)
//lab9
void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	glWindowWidth = width;
	glWindowHeight = height;

	glfwGetFramebufferSize(window, &retina_width, &retina_height);
	glViewport(0, 0, retina_width, retina_height);

	std::cout << "Window resized: " << retina_width << " x " << retina_height << std::endl;
	myCustomShader.useShaderProgram();
	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}
//lab 6
void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {

	if (key == GLFW_KEY_X && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		firstMouse = true;
	}

	if (key == GLFW_KEY_M && action == GLFW_PRESS)
		showDepthMap = !showDepthMap;

	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
		renderMode = SOLID;

	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
		renderMode = WIREFRAME;

	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
		flatShading = false; 

	if (key == GLFW_KEY_4 && action == GLFW_PRESS)
		flatShading = true; 

	if (key == GLFW_KEY_5 && action == GLFW_PRESS)
		fogEnabled = !fogEnabled;

	if (key == GLFW_KEY_6 && action == GLFW_PRESS) {
		rainEnabled = !rainEnabled;
	}

	if (key == GLFW_KEY_R && action == GLFW_PRESS)
		dogMode = ROTATE;


	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}

}
//lab5 camera movement
void mouseCallback(GLFWwindow* window, double xpos, double ypos) {

	if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED) {
		return;
	}
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; 

	lastX = xpos;
	lastY = ypos;

	xoffset *= mouseSensitivity;
	yoffset *= mouseSensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;

	myCamera.rotate(pitch, yaw);

	if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED) {
		return;
	}

	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

}
//lab5
void processMovement()
{
	if (pressedKeys[GLFW_KEY_Q]) {
		angleY -= 1.0f;		
	}

	if (pressedKeys[GLFW_KEY_E]) {
		angleY += 1.0f;		
	}

	if (pressedKeys[GLFW_KEY_J]) {
		lightAngle -= 1.0f;		
	}

	if (pressedKeys[GLFW_KEY_L]) {
		lightAngle += 1.0f;
	}

	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);		
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);		
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);		
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);		
	}
	if (pressedKeys[GLFW_KEY_R]) {
		dogRotation.y += 1.0f;   
	}

	glm::vec3 camPos = myCamera.getCameraPosition();
	camPos.y = 0.05f; 
	myCamera.setCameraPosition(camPos);

}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

    glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

    glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
	glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent(glWindow);

	glfwSwapInterval(1);

#if not defined (__APPLE__)
    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit();
#endif

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

	glEnable(GL_FRAMEBUFFER_SRGB);
}

void initObjects() {
	cottage.LoadModel("objects/cottage/Cottage.obj");
	normalcat.LoadModel("objects/normalcat/normalcat.obj");
	ground.LoadModel("objects/munti/munti.obj");
	apa.LoadModel("objects/apa/apa.obj");
	catel.LoadModel("objects/catel/Catel.obj");
	gull.LoadModel("objects/gull/gull.obj");
	padure1.LoadModel("objects/padure1/padure1.obj");
	sky.LoadModel("objects/sky/sky.obj");
}

void initShaders() {
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	myCustomShader.useShaderProgram();
	screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
	screenQuadShader.useShaderProgram();
	depthShader.loadShader("shaders/depthShader.vert", "shaders/depthShader.frag");
	depthShader.useShaderProgram();
}
void initScreenQuad() {
	float quadVertices[] = {
		0.0f, 0.0f,    0.0f, 0.0f,  
		1.0f, 0.0f,    1.0f, 0.0f, 
		1.0f, 1.0f,    1.0f, 1.0f,  
		0.0f, 1.0f,    0.0f, 1.0f   
	};

	unsigned int quadIndices[] = {
		0, 1, 2,
		2, 3, 0
	};

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glGenBuffers(1, &quadEBO);

	glBindVertexArray(quadVAO);

	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void initUniforms() {
	myCustomShader.useShaderProgram();

	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); 
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	uvScaleLoc = glGetUniformLocation(myCustomShader.shaderProgram, "uvScale");
	glUniform1f(uvScaleLoc, uvScale);

	glm::vec3 camPos = myCamera.getCameraPosition();
	glm::vec3 camDir = myCamera.getCameraFrontDirection();

	glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "spotPos"), 1, glm::value_ptr(camPos));
	glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "spotDir"), 1, glm::value_ptr(camDir));
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "spotColor"), 3.0f, 3.0f, 2.5f);

	//Spotlight cone
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "cutOff"), glm::cos(glm::radians(5.5f)));
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "outerCutOff"), glm::cos(glm::radians(8.5f)));

	//Attenuation
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "constant"), 1.0f);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "linear"), 0.09f);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "quadratic"), 0.032f);

	//Fog
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "fogEnabled"),GL_TRUE);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "fogColor"), 0.6f, 0.7f, 0.8f);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "fogDensity"), 0.02f);

}

void initFBO() {
	glGenFramebuffers(1, &shadowMapFBO);

	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture); 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f }; 
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
	glDrawBuffer(GL_NONE); 
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
}

glm::mat4 computeLightSpaceTrMatrix() {
	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec3 rotatedLightDir = glm::mat3(lightRotation) * lightDir;

	glm::vec3 lightPos = rotatedLightDir * 50.0f;
	glm::vec3 target = glm::vec3(0.0f);
	glm::mat4 lightView = glm::lookAt(lightPos, target, glm::vec3(0, 1, 0));

	float lightSize = 30.0f;
	float near_plane = 1.0f, far_plane = 200.0f;
	glm::mat4 lightProjection = glm::ortho(-lightSize, lightSize, -lightSize, lightSize, near_plane, far_plane);

	return lightProjection * lightView;
}

void renderRain(gps::Shader shader) {
	if (!rainEnabled) return;

	shader.useShaderProgram();
	for (int i = 0; i < NR_RAIN_DROPS; i++) {
		rainPositions[i].y -= rainSpeed;

		if (rainPositions[i].y < -1.0f) {
			rainPositions[i].y = 50.0f;
		}

		model = glm::translate(glm::mat4(1.0f), rainPositions[i]);
		model = glm::scale(model, glm::vec3(0.02f, 0.1f, 0.02f)); 
		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		apa.Draw(shader);
	}
}

void drawObjects(gps::Shader shader, bool depthPass) {
		
	shader.useShaderProgram();
	//Cottage
	model = glm::mat4(1.0f);
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(2.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	uvScale = 1.0f; 
	glUniform1f(uvScaleLoc, uvScale);
	cottage.Draw(shader);

	//Ground
	model = glm::mat4(1.0f);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(5.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	uvScale = 1.0f;
	glUniform1f(uvScaleLoc, uvScale);
	ground.Draw(shader);	

	//Sky
	sky.Draw(shader);

	//Water
	apa.Draw(shader);

	//Cat 
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	uvScale = 1.0f; 
	glUniform1f(uvScaleLoc, uvScale);
	normalcat.Draw(shader);

	//Tree 
	model = glm::scale(model, glm::vec3(0.25f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	uvScale = 2.0f;
	glUniform1f(uvScaleLoc, uvScale);
	padure1.Draw(shader);

	//Catel
	glm::mat4 modelDog = glm::mat4(1.0f);
	modelDog = glm::translate(modelDog, dogPosition);
	modelDog = glm::rotate(modelDog, glm::radians(dogRotation.x), glm::vec3(1, 0, 0));
	modelDog = glm::rotate(modelDog, glm::radians(dogRotation.y), glm::vec3(0, 1, 0));
	modelDog = glm::rotate(modelDog, glm::radians(dogRotation.z), glm::vec3(0, 0, 1));
	modelDog = glm::scale(modelDog, dogScale);

	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelDog));

	if (!depthPass) {
		glm::mat3 normalMatrixDog = glm::mat3(glm::inverseTranspose(view * modelDog));
		glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrixDog));
	}

	catel.Draw(shader);

	//Gull
	float t = glfwGetTime(); 
	float x = radius * cos(t * speed);
	float z = radius * sin(t * speed);
	glm::vec3 gullPosition = glm::vec3(x, 0.0f, z);

	glm::vec3 dirToCottage = glm::normalize(glm::vec3(0.0f, 0.0f, 0.0f) - gullPosition);

	float angleY = atan2(dirToCottage.x, dirToCottage.z);

	glm::mat4 modelGull = glm::translate(glm::mat4(1.0f), gullPosition);
	modelGull = glm::rotate(modelGull, angleY, glm::vec3(0, 1, 0));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelGull));
	gull.Draw(shader);

}

void renderScene() {
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	depthShader.useShaderProgram();
	glm::mat4 lightSpaceTrMatrix = computeLightSpaceTrMatrix();
	glUniformMatrix4fv(glGetUniformLocation(depthShader.shaderProgram, "lightSpaceTrMatrix"),
		1, GL_FALSE, glm::value_ptr(lightSpaceTrMatrix));

	drawObjects(depthShader, true);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Final Rendering
	if (showDepthMap) {
		glViewport(0, 0, retina_width, retina_height);
		glClear(GL_COLOR_BUFFER_BIT);
		screenQuadShader.useShaderProgram();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);
		glBindVertexArray(quadVAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
	else {
		glViewport(0, 0, retina_width, retina_height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Toggle Solid/Wireframe
		if (renderMode == SOLID) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}

		myCustomShader.useShaderProgram();

		//Flat Shading
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "flatShading"), (int)flatShading);

		//Fog
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "fogEnabled"), (int)fogEnabled);

		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		//Spotlight
		glm::vec3 spotPosEye = glm::vec3(view * glm::vec4(myCamera.getCameraPosition(), 1.0f));
		glm::vec3 spotDirEye = glm::mat3(view) * myCamera.getCameraFrontDirection();
		glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "spotPos"), 1, glm::value_ptr(spotPosEye));
		glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "spotDir"), 1, glm::value_ptr(spotDirEye));

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);
		glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceTrMatrix));

		if (rainEnabled) {
			renderRain(myCustomShader);
		}

		drawObjects(myCustomShader, false);
	}
}
void cleanup() {
	glDeleteTextures(1,& depthMapTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &shadowMapFBO);
	glfwDestroyWindow(glWindow);
	glfwTerminate();
}

int main(int argc, const char * argv[]) {

	if (!initOpenGLWindow()) {
		glfwTerminate();
		return 1;
	}

	initOpenGLState();
	initObjects();
	initShaders();
	initScreenQuad();
	initUniforms();
	initFBO();

	glCheckError();

	for (int i = 0; i < NR_RAIN_DROPS; i++) {
		rainPositions[i] = glm::vec3(
			(rand() % 100) - 50.0f, 
			(rand() % 50),          
			(rand() % 100) - 50.0f  
		);
	}

	while (!glfwWindowShouldClose(glWindow)) {
		processMovement();
		renderScene();		

		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	cleanup();

	return 0;
}
