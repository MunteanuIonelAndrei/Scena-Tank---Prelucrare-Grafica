#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "SkyBox.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"

#include <iostream>
float sensitivity = 1.0f;
float pitch, yaw;
bool firstMouse = true;
float lastX, lastY;
///iau doua cuburi si ii dau pozitile unu la camera target si una la camera position.


// window
gps::Window myWindow;
gps::SkyBox mySkyBox;

// matrices
glm::mat4 model;
glm::mat4 modelElice;


glm::mat4 view;
glm::mat4 projection;


glm::mat3 normalMatrix;
glm::mat3 normalMatrixElice;
glm::mat3 normalMatrixAvioane;
glm::mat3 normaltankBMatrix;
glm::mat3 normaltankTMatrix;

//avion matrix
glm::mat4 avioaneMatrix;
glm::mat4 tankBMatrix;
glm::mat4 tankTMatrix;

// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;

// shader uniform locations
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;

//pt lumina
glm::vec3 lumina;
glm::vec3 Culoarelumina;
GLint luminaColorPOs;
GLint luminaPozitie;

int starting = 0;
float unghiRotatie;



// camera
//gps::Camera myCamera(
//  //  glm::vec3(89.7208f, -0.959054f,- 0.23857f),
//    glm::vec3(0.0f,2.0f, 5.0f),
//    glm::vec3(0.0f, 0.0f, -700.0f),
//    glm::vec3(0.0f, 1.0f, 0.0f));
std::vector <const GLchar*> faces;

  gps::Camera myCamera(
    glm::vec3(0.0f,2.0f, 5.0f),
    glm::vec3(700.0f, 700.0f, 700.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));


GLfloat cameraSpeed = 0.5f;
GLboolean pressedKeys[1024];



void previewing()
{
    if (starting == 1)
    {
        unghiRotatie += 0.03f;
        myCamera.animationOfCamera(unghiRotatie);
    }
}


// models
gps::Model3D teapot;
gps::Model3D elice;
gps::Model3D avioane;
gps::Model3D tankBody;
gps::Model3D turret;

GLint transmitere=0;
GLfloat angle;
int declansator = 0;

// shaders
gps::Shader myBasicShader;
gps::Shader mySkyBoxShader;

GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		std::string error;
		switch (errorCode) {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_STACK_OVERFLOW:
                error = "STACK_OVERFLOW";
                break;
            case GL_STACK_UNDERFLOW:
                error = "STACK_UNDERFLOW";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
        }
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
	//TODO
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

	if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        } else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
}


void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    //TODO
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xDiff = (float)xpos - lastX;
    float yDiff = (float)ypos - lastY;
    lastX = (float)xpos;
    lastY = (float)ypos;

    xDiff *= sensitivity;
    yDiff *= sensitivity;

    yaw -= xDiff;
    pitch -= yDiff;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    myCamera.rotate(pitch, yaw);

    view = myCamera.getViewMatrix();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDir));
}

    void processMovement() {
        if (pressedKeys[GLFW_KEY_W]) {
            myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
            //update view matrix
            view = myCamera.getViewMatrix();
            myBasicShader.useShaderProgram();
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            // compute normal matrix for teapot
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        }

        if (pressedKeys[GLFW_KEY_S]) {
            myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
            //update view matrix
            view = myCamera.getViewMatrix();
            myBasicShader.useShaderProgram();
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            // compute normal matrix for teapot
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        }

        if (pressedKeys[GLFW_KEY_A]) {
            myCamera.move(gps::MOVE_LEFT, cameraSpeed);
            //update view matrix
            view = myCamera.getViewMatrix();
            myBasicShader.useShaderProgram();
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            // compute normal matrix for teapot
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        }

        if (pressedKeys[GLFW_KEY_D]) {
            myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
            //update view matrix
            view = myCamera.getViewMatrix();
            myBasicShader.useShaderProgram();
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            // compute normal matrix for teapot
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        }

        if (pressedKeys[GLFW_KEY_Q]) {
            angle -= 1.0f;
            // update model matrix for teapot
            model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
            // update normal matrix for teapot
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        }

        if (pressedKeys[GLFW_KEY_E]) {
            angle += 1.0f;
            // update model matrix for teapot
            model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
            // update normal matrix for teapot
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        }

        //modul solid
        if (pressedKeys[GLFW_KEY_U]) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        //modul wireframe
        if (pressedKeys[GLFW_KEY_I]) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

        //modul polygon
        if (pressedKeys[GLFW_KEY_O]) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        }
        if (pressedKeys[GLFW_KEY_M]) //ceata
        {
            myBasicShader.useShaderProgram();
            transmitere = 1;
            int fogLoc = glGetUniformLocation(myBasicShader.shaderProgram, "transmitere");
            glUniform1i(fogLoc, transmitere);
        }
        if (pressedKeys[GLFW_KEY_N]) //revenire din modul ceata
        {
            myBasicShader.useShaderProgram();
            transmitere = 0;
            int fogLoc = glGetUniformLocation(myBasicShader.shaderProgram, "transmitere");
            glUniform1i(fogLoc, transmitere);
        }
        if (pressedKeys[GLFW_KEY_P])
        {
            myBasicShader.useShaderProgram();
            lightColor = glm::vec3(0.0f, 0.0f, 0.0f); //black light
            lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
            // send light color to shader
            glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
            declansator = 1;
        }
        if (pressedKeys[GLFW_KEY_T])
        {
            myBasicShader.useShaderProgram();
            lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
            lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
            // send light color to shader
            glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
            declansator = 0;
        }
        if (pressedKeys[GLFW_KEY_Z])
        {
            starting = 1;
        }

        if (pressedKeys[GLFW_KEY_X])
        {
            starting = 0;
        }

      
    }

void initOpenGLWindow() {
    myWindow.Create(1024, 768, "OpenGL Project Core");
}

void setWindowCallbacks() {
	glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
    glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void initOpenGLState() {
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
  //  glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
  // glEnable(GL_CULL_FACE); // cull face/ //////PT APA SI PT POMI DIESABILA
//	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initModels() {
    teapot.LoadModel("models/tank/ScenaFaraCopaci2.obj");
    elice.LoadModel("models/elice.obj");
    tankBody.LoadModel("models/tank/treesBuneCred.obj");
 //   turret.LoadModel("models/tank/tankturret.obj");
    avioane.LoadModel("models/avioane/avioane.obj");
}

void initShaders() {
	myBasicShader.loadShader(
        "shaders/basic.vert",
        "shaders/basic.frag");
    myBasicShader.useShaderProgram();
    mySkyBoxShader.loadShader(
        "shaders/skybox.vert",
        "shaders/skybox.frag");
    mySkyBoxShader.useShaderProgram();
}

void initUniforms() {
	myBasicShader.useShaderProgram();

    // create model matrix for teapot
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelElice = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));

	modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

	// get view matrix for current camera
	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
	// send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
    normalMatrixElice = glm::mat3(glm::inverseTranspose(view*modelElice));
	normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

	// create projection matrix
	projection = glm::perspective(glm::radians(45.0f),
                               (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
                               0.1f, 500.0f);
	projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
	// send projection matrix to shader
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));	

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
	lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
	// send light dir to shader
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
	// send light color to shader
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));




    //-----pentru lumina

    lumina = glm::vec3(-49.057, -0.016016, -20.149);
    luminaPozitie = glGetUniformLocation(myBasicShader.shaderProgram, "luminaObiect");
    // send light dir to shader
    glUniform3fv(luminaPozitie, 1, glm::value_ptr(lumina));
    //set light color for felinar
    Culoarelumina = glm::vec3(1.0f, 0.0f, 0.0f); //red light
    luminaColorPOs = glGetUniformLocation(myBasicShader.shaderProgram, "luminaColora");
    // send light color to shader
    glUniform3fv(luminaColorPOs, 1, glm::value_ptr(Culoarelumina));

}

void renderTeapot(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    //send teapot model matrix data to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    //send teapot normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // draw teapot
    teapot.Draw(shader);
}

float avioaneMovement = 0.0f;

void renderAvioane(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    avioaneMatrix = glm::mat4(1.0f);
    avioaneMatrix = glm::rotate(model, glm::radians(-1*avioaneMovement), glm::vec3(0, 1, 0));
    //send avioane model matrix data to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(avioaneMatrix));

    //send avioane normal matrix data to shader
    normalMatrixAvioane = glm::mat3(glm::inverseTranspose(view * avioaneMatrix));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrixAvioane));

    // draw avioane
    avioane.Draw(shader);
}

void renderTank(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    tankBMatrix = glm::mat4(1.0f);
   // tankBMatrix = glm::rotate(model, glm::radians(-1 * avioaneMovement), glm::vec3(0, 1, 0));*/
    //send avioane model matrix data to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(tankBMatrix));

    //send avioane normal matrix data to shader
    normaltankBMatrix = glm::mat3(glm::inverseTranspose(view * tankBMatrix));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normaltankBMatrix));

    // draw avioane
    glDisable(GL_CULL_FACE);
    tankBody.Draw(shader);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); // cull back face

}

void renderTurret(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    tankTMatrix = glm::mat4(1.0f);
   // tankTMatrix = glm::rotate(model, glm::radians(-1 * avioaneMovement), glm::vec3(0, 1, 0));*/
    //send avioane model matrix data to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(tankTMatrix));

    //send avioane normal matrix data to shader
    normaltankTMatrix = glm::mat3(glm::inverseTranspose(view * tankBMatrix));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normaltankTMatrix));

    // draw avioane
    turret.Draw(shader);
}

void renderElice(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();
    //for (int i = 0; i < 3; i++) {
    //    for (int j = 0; j < 3; j++) {
    //        printf("%lf",modelElice[i][j]);
    //    }
    //}
    //modelElice = glm::translate(modelElice, glm::vec3(33.179f, 5.3573f, 18.37f));
    //modelElice = glm::rotate(modelElice, glm::radians(-1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    //modelElice = glm::translate(modelElice, glm::vec3(-33.179f, -5.3573f,- 18.37f));

    modelElice = glm::translate(modelElice, glm::vec3(34.949f, 5.4216f, 18.006f));
    modelElice = glm::rotate(modelElice, glm::radians(-1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    modelElice = glm::translate(modelElice, glm::vec3(-34.949f, -5.4216f, -18.006f));

    //send teapot model matrix data to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelElice));

    //send teapot normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrixElice));

    // draw teapot
    // //asta ne da viteza de deplasare a obiectului
    //for(int i=0;i<1000;i++)
    elice.Draw(shader);
}

void renderScene() {
        glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//render the scene
    previewing();

	// render the teapot
        mySkyBoxShader.useShaderProgram();
    view = myCamera.getViewMatrix();
    glUniformMatrix4fv(glGetUniformLocation(mySkyBoxShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    projection = glm::perspective(glm::radians(45.0f), (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height, 0.1f, 1000.0f);
    glUniformMatrix4fv(glGetUniformLocation(mySkyBoxShader.shaderProgram, "projection"), 1, GL_FALSE,
        glm::value_ptr(projection));
    mySkyBox.Draw(mySkyBoxShader, view, projection);


  
	renderTeapot(myBasicShader);
    renderElice(myBasicShader);
    renderAvioane(myBasicShader);
    renderTurret(myBasicShader);
    renderTank(myBasicShader);

}

void cleanup() {
    myWindow.Delete();
    //cleanup code for your own data
}

void initCube()
{
    faces.push_back("models/skybox/posx.jpg");
    faces.push_back("models/skybox/negx.jpg");
    faces.push_back("models/skybox/posy.jpg");
    faces.push_back("models/skybox/negy.jpg");
    faces.push_back("models/skybox/posz.jpg");
    faces.push_back("models/skybox/negz.jpg");
    mySkyBox.Load(faces);
}

int main(int argc, const char * argv[]) {

    try {
        initOpenGLWindow();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
	initModels();
	initShaders();
	initUniforms();
    initCube();
    setWindowCallbacks();


	glCheckError();
	// application loop
	while (!glfwWindowShouldClose(myWindow.getWindow())) {
        processMovement();
	    renderScene();

		glfwPollEvents();
		glfwSwapBuffers(myWindow.getWindow());

        if (avioaneMovement < 360.0f) {
            avioaneMovement += 0.5f;
        }
        else {
            avioaneMovement = 0;
        }


		glCheckError();
	}

	cleanup();

    return EXIT_SUCCESS;
}
