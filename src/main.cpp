#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <gl2d/gl2d.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <openglErrorReporting.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imguiThemes.h"

#define WIDTH 640
#define HEIGHT 480

static void error_callback(int error, const char *description)
{
	std::cout << "Error: " <<  description << "\n";
}


#define MAX_SHADER_SIZE 10000

typedef struct {
    GLuint program;
} Shader;

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
char* readShaderFile(const char* filename);
GLuint compileShader(const char* source, GLenum type);
Shader createShader(const char* vertexPath, const char* fragmentPath);

int main(void)
{

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#endif

	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "threedeey Renderer", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	  // Create shaders
    Shader shader1 = createShader("vertex1.glsl", "fragment1.glsl");
    Shader shader2 = createShader("vertex2.glsl", "fragment2.glsl");

	
	enableReportGlErrors();

	float texCoords[] = {
    0.0f, 0.0f,  // lower-left corner  
    1.0f, 0.0f,  // lower-right corner
    0.5f, 1.0f   // top-center corner
};
	glm::vec3 camera_Coords = {0.0f, 0.0f,-3.0f};
	float fov = 45.0f;

float cube[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};
glm::vec3 cubePositions[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f), 
    glm::vec3( 2.0f,  5.0f, -15.0f), 
    glm::vec3(-1.5f, -2.2f, -2.5f),  
    glm::vec3(-3.8f, -2.0f, -12.3f),  
    glm::vec3( 2.4f, -0.4f, -3.5f),  
    glm::vec3(-1.7f,  3.0f, -7.5f),  
    glm::vec3( 1.3f, -2.0f, -2.5f),  
    glm::vec3( 1.5f,  2.0f, -2.5f), 
    glm::vec3( 1.5f,  0.2f, -1.5f), 
    glm::vec3(-1.3f,  1.0f, -1.5f)  
};
    float vertices1[] = {
      0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };
    unsigned int indices[] = {  
        0, 1, 3,  // first triangle
        1, 2, 3   // second triangle
    };

    // Set up vertex data for the second triangle
    float vertices2[] = {
         0.0f, -0.5f, 0.0f,
         1.0f, -0.5f, 0.0f,
         0.5f,  0.5f, 0.0f
    };
	//glfwSwapInterval(1); //vsync


#pragma region imgui
#if REMOVE_IMGUI == 0
	ImGui::CreateContext();
	//ImGui::StyleColorsDark();				//you can use whatever imgui theme you like!
	//imguiThemes::yellow();
	//imguiThemes::gray();
	//imguiThemes::green();
	imguiThemes::red();
	//imguiThemes::embraceTheDarkness();

	ImGuiIO &io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;

	io.FontGlobalScale = 2.0f; //make text bigger please!

	ImGuiStyle &style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		//style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 0.f;
		style.Colors[ImGuiCol_DockingEmptyBg].w = 0.f;
	}

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
#endif
#pragma endregion



unsigned int texture1;
glGenTextures(1, &texture1);
glBindTexture(GL_TEXTURE_2D, texture1);
// set the texture wrapping/filtering options (on the currently bound texture object)
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
// load and generate the texture
int width, height, nrChannels;

stbi_set_flip_vertically_on_load(true);  
unsigned char *data = stbi_load("rocky.jpg", &width, &height, &nrChannels, 0);
if (data)
{
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
}
else
{
    std::cout << "Failed to load texture" << std::endl;
}
stbi_image_free(data);

unsigned int texture2;
glGenTextures(1, &texture2);
glBindTexture(GL_TEXTURE_2D, texture2);

glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
// float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
// glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);  


// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

data = stbi_load("concrete.jpg", &width, &height, &nrChannels, 0);
if (data)
{
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
}
else
{
    std::cout << "Failed to load texture" << std::endl;
}
	// gl2d::init();
	// gl2d::Renderer2D renderer;
	// renderer.create();
	GLuint VAO_CUBE, VBO_CUBE;

	glGenVertexArrays(1,&VAO_CUBE);
	glGenBuffers(1,&VBO_CUBE);
	glBindVertexArray(VAO_CUBE);
	glBindBuffer(GL_ARRAY_BUFFER,VBO_CUBE);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube),cube, GL_STATIC_DRAW);
	glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, 5*sizeof(float),(void*)0); //vertex pos
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(2,2, GL_FLOAT, GL_FALSE, 5*sizeof(float),(void*)(3*sizeof(float))); //texture UV
	glEnableVertexAttribArray(2);

	 GLuint VAO1, VBO1, EAO1;

    glGenVertexArrays(1, &VAO1);
    glGenBuffers(1, &VBO1);
  	glGenBuffers(1, &EAO1);

    glBindVertexArray(VAO1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EAO1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Create and bind VAO and VBO for the second triangle
    GLuint VAO2, VBO2;
    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &VBO2);

    glBindVertexArray(VAO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


	glm::vec2 visible={1.0,1.0};
    glUseProgram(shader1.program);
    glUniform1i(glGetUniformLocation(shader1.program, "texture1"), 0);
    glUniform1i(glGetUniformLocation(shader1.program, "texture2"), 1);
    glUniform1f(glGetUniformLocation(shader1.program, "visibility"), visible.x);
	int width_w = 1, height_w = 1;

unsigned int transformLoc = glGetUniformLocation(shader1.program, "transform");
	 // glm::mat4 transf =glm::mat4(1.0f);
	// transf = glm::rotate(transf, glm::radians(90.0f), glm::vec3(0.0,0.0,1.0));

	// transf = glm::scale(transf, glm::vec3(0.5,0.5,0.5));

	// glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transf));
    bool changed = false;

    glm::mat4 model_world = glm::mat4(1.0f);
    //model_world = glm::rotate(model_world, glm::radians(-55.0f),glm::vec3(1.0f,0.0f,0.0f));
    glm::mat4 model_view = glm::mat4(1.0f);
    model_view = glm::translate(model_view,glm::vec3(0.0f, 0.0f, -3.0f));
    glm::mat4 perspective_projection;
    perspective_projection= glm::perspective(glm::radians(45.0f),(float)width_w/(float)height_w, 0.1f,100.0f);
    
    int modelLoc = glGetUniformLocation(shader1.program, "model");
    glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(model_world));
    int viewLoc = glGetUniformLocation(shader1.program, "view");
    glUniformMatrix4fv(viewLoc,1,GL_FALSE,glm::value_ptr(model_view));
    int projectionLoc = glGetUniformLocation(shader1.program, "projection");
    glUniformMatrix4fv(projectionLoc,1,GL_FALSE,glm::value_ptr(perspective_projection));


	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window))
	{
		 glm::mat4 transf =glm::mat4(1.0f);


		 model_view =glm::mat4(1.0f);
		 model_view = glm::translate(model_view,camera_Coords);
	// transf = glm::rotate(transf, (float)glfwGetTime()* glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
	//model_world = glm::mat4{1.0f};
	//model_world = glm::rotate(model_world, (float)glfwGetTime()* glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));  
	//glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(model_world));
	//glm::mat4 transf =glm::mat4(1.0f);
	//transf = glm::scale(transf, glm::vec3(glm::sin((float)glfwGetTime()),glm::sin((float)glfwGetTime()),0.5));
	//transf = glm::rotate(transf, (float)glfwGetTime(), glm::vec3(0.2f, 0.56f, 0.6f));
	//transf = glm::translate(transf, glm::vec3(1.0f, 1.0f, 0.0f));
	
	// glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transf));
		
		
		glfwGetFramebufferSize(window, &width_w, &height_w);
		glViewport(0, 0, width_w, height_w);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(shader1.program);
		glActiveTexture(GL_TEXTURE0); 
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		glUniform1f(glGetUniformLocation(shader1.program, "visibility"), visible.x);

        //glBindVertexArray(VAO1);
 		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(VAO_CUBE);
		for(unsigned int i = 0; i < 10; i++)
		{
			transf =glm::mat4(1.0f);
		    model_world = glm::mat4(1.0f);
		    model_world = glm::translate(model_world, cubePositions[i]);
		    float angle = 20.0f * i; 
		    transf = glm::rotate(transf, (float)glfwGetTime()* glm::radians(50.0f) * i, glm::vec3(0.5f, 1.0f, 0.0f));
		    glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(model_world));
		    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transf));
		    glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glUniformMatrix4fv(viewLoc,1,GL_FALSE,glm::value_ptr(model_view));
        // Draw second triangle
        // glUseProgram(shader2.program);
        // glBindVertexArray(VAO2);
        // glDrawArrays(GL_TRIANGLES, 0, 3);

	#pragma region imgui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
	#pragma endregion


		// renderer.updateWindowMetrics(width, height);

		// renderer.renderRectangle({0,0, 100, 100}, Colors_Green);

		// renderer.flush();


		ImGui::Begin("Test");
		ImGui::Text("Hello world!");
		ImGui::Button("Press me!");
		ImGui::SliderFloat("visible", &visible[0],0.0f,1.0f);
		ImGui::SliderFloat3("Camera Controls", &camera_Coords[0],-1000.0f, 1000.0f);

		ImGui::End();


	#pragma region imgui
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
		//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow *backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	#pragma endregion

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	   glDeleteVertexArrays(1, &VAO1);
    glDeleteBuffers(1, &VBO1);
    glDeleteVertexArrays(1, &VAO2);
    glDeleteBuffers(1, &VBO2);
    glDeleteProgram(shader1.program);
    glDeleteProgram(shader2.program);
	glfwTerminate();

	return 0;
}




void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);
}

char* readShaderFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", filename);
        return NULL;
    }

    char* content = (char*)malloc(MAX_SHADER_SIZE * sizeof(char));
    size_t fileSize = fread(content, sizeof(char), MAX_SHADER_SIZE, file);
    content[fileSize] = '\0';

    fclose(file);
    return content;
}

GLuint compileShader(const char* source, GLenum type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        fprintf(stderr, "Shader compilation failed: %s\n", infoLog);
        return 0;
    }

    return shader;
}

Shader createShader(const char* vertexPath, const char* fragmentPath) {
    Shader shader;

    char* vertexSource = readShaderFile(vertexPath);
    char* fragmentSource = readShaderFile(fragmentPath);

    GLuint vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);

    shader.program = glCreateProgram();
    glAttachShader(shader.program, vertexShader);
    glAttachShader(shader.program, fragmentShader);
    glLinkProgram(shader.program);

    int success;
    char infoLog[512];
    glGetProgramiv(shader.program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader.program, 512, NULL, infoLog);
        fprintf(stderr, "Shader program linking failed: %s\n", infoLog);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    free(vertexSource);
    free(fragmentSource);

    return shader;
}