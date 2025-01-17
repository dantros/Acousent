#pragma once
#include "../Audio/SoundDevice.h"
#include "../Audio/SoundSource.h"
#include "../Audio/SoundLibrary.h"

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <imgui.h>
#include "examples/imgui_impl_opengl3.cpp"
#include "examples/imgui_impl_glfw.cpp" 

#include <glm/ext/vector_float3.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/transform.hpp>
#include "../root_directory.h"

#include "../GUI/imgui-style.h"
#include "../Core/utils.h"



namespace ac = Acousent;

namespace Acousent {
SoundDevice* sd = LISTENER->Get();
int sound1 = SE_LOAD(ac::getPath("Resources/Audio/Wav/Sound1_R.wav").string().c_str());
int sound2 = SE_LOAD(ac::getPath("Resources/Audio/Wav/Sound1_R.wav").string().c_str());
SoundSource source_1;
SoundSource source_2;
const constexpr float WIN_WIDTH = 900, WIN_HEIGHT = 900;
const constexpr int ROWS = 20, COLUMNS = 20;
const constexpr float CELLSIZE = (float)WIN_HEIGHT / ROWS;  // assumes ROWS == COLUMNS for simplicity
enum class GraphicKey { UNSEARCHED, SEARCHED, PLAYER1, SOURCE, SOURCE1, STAGE };
GraphicKey map[ROWS][COLUMNS];
bool mapHasChanged = true;  //process a map draw update
GLuint programID;
float aColor[3] = { 0,1,0 };

struct Actor {
	Actor() {};
	Actor(int x, int y) { mapPos[0] = x; mapPos[1] = y;};
	glm::vec3 Pos = { 0,0,0 };
	int mapPos[2] = { 0,0 };
	enum class Orientation { PRONE, LEFT, UP, RIGHT, DOWN } direction = Orientation::PRONE;  //character to represent facing direction
	bool hasUnprocessedMoved = false;
};
int source1_locx = ROWS-(ROWS / 4)-1;
int source1_locy = COLUMNS - (COLUMNS / 4) - 1;
int source2_locx = (ROWS / 4);
int source2_locy = COLUMNS - (COLUMNS / 4) - 1;
Actor PLAYER1, SOURCE(source1_locx, source1_locy), SOURCE1(source2_locx, source2_locy);

std::string fontName = "JetBrainsMono-ExtraLight.ttf";
float highDPIscaleFactor = 1.0;
bool show_demo_window = false;
bool show_another_window = false;
int counter = 0;

float source1_volume = .8f;
float source2_volume = .8f;



GLFWwindow* window;
void loadSquare() {
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	static const GLfloat g_vertex_buffer_data[] = {
		-.5f,-.5f,-.5f, // triangle 1 : begin
		-.5f,-.5f, .5f,
		-.5f, .5f, .5f, // triangle 1 : end
		.5f, .5f,-.5f, // triangle 2 : begin
		-.5f,-.5f,-.5f,
		-.5f, .5f,-.5f, // triangle 2 : end
		.5f,-.5f, .5f,
		-.5f,-.5f,-.5f,//3
		.5f,-.5f,-.5f,
		.5f, .5f,-.5f,
		.5f,-.5f,-.5f,//4
		-.5f,-.5f,-.5f,
		-.5f,-.5f,-.5f,
		-.5f, .5f, .5f,//5
		-.5f, .5f,-.5f,
		.5f,-.5f, .5f,
		-.5f,-.5f, .5f,//6
		-.5f,-.5f,-.5f,
		-.5f, .5f, .5f,
		-.5f,-.5f, .5f,//7
		.5f,-.5f, .5f,
		.5f, .5f, .5f,
		.5f,-.5f,-.5f,//8
		.5f, .5f,-.5f,
		.5f,-.5f,-.5f,
		.5f, .5f, .5f,//9
		.5f,-.5f, .5f,
		.5f, .5f, .5f,
		.5f, .5f,-.5f,//10
		-.5f, .5f,-.5f,
		.5f, .5f, .5f,
		-.5f, .5f,-.5f,//11
		-.5f, .5f, .5f,
		.5f, .5f, .5f,
		-.5f, .5f, .5f,//12
		.5f,-.5f, .5f
	};

	// This will identify our vertex buffer
	GLuint vertexbuffer;
	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexbuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// 1st attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	// Draw the triangle !
	//glDrawArrays(GL_TRIANGLES, 0, 12*3); // Starting from vertex 0; 3 vertices total -> 1 triangle
	//glDisableVertexAttribArray(0);
}

GLuint loadShader(const char* vertexshader, const char* fragmentshader) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	//// Read the Vertex Shader code from the file
	//std::string VertexShaderCode;
	//std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	//if(VertexShaderStream.is_open()){
	//	std::stringstream sstr;
	//	sstr << VertexShaderStream.rdbuf();
	//	VertexShaderCode = sstr.str();
	//	VertexShaderStream.close();
	//}else{
	//	printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
	//	getchar();
	//	return 0;
	//}

	//// Read the Fragment Shader code from the file
	//std::string FragmentShaderCode;
	//std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	//if(FragmentShaderStream.is_open()){
	//	std::stringstream sstr;
	//	sstr << FragmentShaderStream.rdbuf();
	//	FragmentShaderCode = sstr.str();
	//	FragmentShaderStream.close();
	//}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	//printf("Compiling shader : %s\n", vertex_file_path);
	//char const * VertexSourcePointer = VertexShaderCode.c_str();
	//glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glShaderSource(VertexShaderID, 1, &vertexshader, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	//printf("Compiling shader : %s\n", fragment_file_path);
	//char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	//glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glShaderSource(FragmentShaderID, 1, &fragmentshader, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

// ImGui

bool initializeDearImGui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.Fonts->AddFontFromFileTTF(
		ac::getPath("Resources/Font/" + fontName).string().c_str(),
		24.0f * highDPIscaleFactor,
		NULL,
		NULL
	);
	setImGuiStyle(highDPIscaleFactor);

	// setup platform/renderer bindings
	if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) { return false; }
	if (!ImGui_ImplOpenGL3_Init()) { return false; }

	return true;
}

void composeDearImGuiFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();

	ImGui::NewFrame();

	// standard demo window
	if (show_demo_window) { ImGui::ShowDemoWindow(&show_demo_window); }

	// a window is defined by Begin/End pair
	{
		int glfw_width = 0, glfw_height = 0, controls_width = 0;
		// get the window size as a base for calculating widgets geometry
		glfwGetFramebufferSize(window, &glfw_width, &glfw_height);
		controls_width = glfw_width;
		// make controls widget width to be 1/3 of the main window width
		if ((controls_width /= 3) < 300) { controls_width = 300; }

		// position the controls widget in the top-right corner with some margin
		ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
		// here we set the calculated width and also make the height to be
		// be the height of the main window also with some margin
		ImGui::SetNextWindowSize(
			ImVec2(static_cast<float>(controls_width), static_cast<float>(glfw_height - 20)),
			ImGuiCond_Always
		);

		ImGui::SetNextWindowBgAlpha(0.7f);
		// create a window and append into it
		ImGui::Begin("Controls", NULL, ImGuiWindowFlags_NoResize);

		ImGui::Dummy(ImVec2(0.0f, 1.0f));
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Time");
		ImGui::Text("%s", currentTime(std::chrono::system_clock::now()).c_str());

		ImGui::Dummy(ImVec2(0.0f, 3.0f));
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Application");
		ImGui::Text("Main window width: %d", glfw_width);
		ImGui::Text("Main window height: %d", glfw_height);

		ImGui::Dummy(ImVec2(0.0f, 3.0f));
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "GLFW");
		ImGui::Text("%s", glfwGetVersionString());

		ImGui::Dummy(ImVec2(0.0f, 3.0f));
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Dear ImGui");
		ImGui::Text("%s", IMGUI_VERSION);

		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0.0f, 10.0f));

		// buttons and most other widgets return true when clicked/edited/activated
		if (ImGui::Button("Counter button"))
		{
			std::cout << "counter button clicked" << std::endl;
			counter++;
			if (counter == 9) { ImGui::OpenPopup("Easter egg"); }
		}
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		if (ImGui::BeginPopupModal("Easter egg", NULL))
		{
			ImGui::Text("Ho-ho, you found me!");
			if (ImGui::Button("Buy Ultimate Orb")) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}

		ImGui::Dummy(ImVec2(0.0f, 15.0f));
		if (!show_demo_window)
		{
			if (ImGui::Button("Open standard demo"))
			{
				show_demo_window = true;
			}
		}

		ImGui::Checkbox("show a custom window", &show_another_window);
		if (show_another_window)
		{
			ImGui::SetNextWindowSize(
				ImVec2(250.0f, 150.0f),
				ImGuiCond_FirstUseEver // after first launch it will use values from imgui.ini
			);
			// the window will have a closing button that will clear the bool variable
			ImGui::Begin("A custom window", &show_another_window);

			ImGui::Dummy(ImVec2(0.0f, 1.0f));
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Some label");

			ImGui::TextColored(ImVec4(128 / 255.0f, 128 / 255.0f, 128 / 255.0f, 1.0f), "%s", "another label");
			ImGui::Dummy(ImVec2(0.0f, 0.5f));

			ImGui::Dummy(ImVec2(0.0f, 1.0f));
			if (ImGui::Button("Close"))
			{
				std::cout << "close button clicked" << std::endl;
				show_another_window = false;
			}

			ImGui::End();
		}

		ImGui::End();
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_W && action == GLFW_PRESS)
	{
		PLAYER1.direction = Actor::Orientation::UP;
		PLAYER1.hasUnprocessedMoved = true;
	}
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		PLAYER1.direction = Actor::Orientation::DOWN;
		PLAYER1.hasUnprocessedMoved = true;
	}
	if (key == GLFW_KEY_A && action == GLFW_PRESS)
	{
		PLAYER1.direction = Actor::Orientation::LEFT;
		PLAYER1.hasUnprocessedMoved = true;
	}
	if (key == GLFW_KEY_D && action == GLFW_PRESS)
	{
		PLAYER1.direction = Actor::Orientation::RIGHT;
		PLAYER1.hasUnprocessedMoved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		if (source_2.isPlaying())
			source_2.Stop();
		else
			source_2.Play(sound2);
	}

}
void setPLAYER1ListenerLoc()
{
	sd->SetLocation(
		(float)PLAYER1.mapPos[0],
		(float)PLAYER1.mapPos[1],
		0
	);
	switch (PLAYER1.direction)
	{
		case Actor::Orientation::LEFT:
		sd->SetOrientation(-1.f, 0.f, 0.f, 0.f, 0.f, 1.f);
		break;
		case Actor::Orientation::RIGHT:
			sd->SetOrientation(1.f, 0.f, 0.f, 0.f, 0.f, 1.f);
		break;
		case Actor::Orientation::UP:
			sd->SetOrientation(0.f, 1.f, 0.f, 0.f, 0.f, 1.f);
		break;
		case Actor::Orientation::DOWN:
			sd->SetOrientation(0.f, -1.f, 0.f, 0.f, 0.f, 1.f);
		break;
		default:
		break;
	}
};
void init() {
	if (!glfwInit())
		throw("error in glfw init");
	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 
	window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "stage top down", nullptr/*mainmonitor*/, nullptr/*share*/);
	if (!window) {
		throw("error with window init");
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		throw("error with glad init");
	glViewport(0, 0, COLUMNS * CELLSIZE, ROWS * CELLSIZE);
	glfwSetKeyCallback(window, key_callback);
	loadSquare();
	const char* vertshader =
		"#version 330 core                                                  \n"
		"layout(location = 0) in vec3 vertexPosition_modelspace;            \n"
		"uniform mat4 modelmatrix;                                          \n"
		"void main(){                                                       \n"
		"  gl_Position = modelmatrix * vec4(vertexPosition_modelspace, 1.0);\n"
		"}";
	const char* fragshader =
		"#version 330 core   \n"
		"out vec3 color;     \n"
		"uniform vec3 ucolor;\n"
		"void main() {       \n"
		"  color = ucolor;   \n"
		"}";
	programID = loadShader(vertshader, fragshader);
	ALint attunation = AL_INVERSE_DISTANCE_CLAMPED;
	sd->SetAttunation(attunation);
	sd->SetLocation(0.f, 0.f, 0.f);
	sd->SetOrientation(0.f, 1.f, 0.f, 0.f, 0.f, 1.f);

	source_1.SetLooping(true);
	source_1.SetPosition(SOURCE.mapPos[0], SOURCE.mapPos[1], 0);
	source_2.SetPosition(SOURCE1.mapPos[0], SOURCE1.mapPos[1], 0);
	source_1.Play(sound1);


}
void processMapChanges()
{
	for (int x = 0; x < ROWS; x++)  //rows
	{
		for (int y = 0; y < COLUMNS; y++)  //columns
		{
			if (x == PLAYER1.mapPos[0] && y == PLAYER1.mapPos[1])
				map[x][y] = GraphicKey::PLAYER1;
			else if (x == SOURCE.mapPos[0] && y == SOURCE.mapPos[1])
				map[x][y] = GraphicKey::SOURCE;
			else if (x == SOURCE1.mapPos[0] && y == SOURCE1.mapPos[1])
				map[x][y] = GraphicKey::SOURCE;

			//Stage setting:
			else if (y >= (COLUMNS/2))
				map[x][y] = GraphicKey::STAGE;
			else
				map[x][y] = GraphicKey::UNSEARCHED;
		}
	}
}
void processPlayer(float dt)
{
	static const float MOVECD = .1667f;
	static float stopwatch = .1667f;
	stopwatch += dt;
	if (stopwatch > MOVECD)
	{
		if (PLAYER1.hasUnprocessedMoved)
		{
			stopwatch = 0.f;
			PLAYER1.hasUnprocessedMoved = false;
			mapHasChanged = true;
			switch (PLAYER1.direction)
			{
			case Actor::Orientation::UP:
				if (PLAYER1.mapPos[1] < (ROWS/2) - 1) 
					PLAYER1.mapPos[1]++;
				PLAYER1.direction = Actor::Orientation::UP;							
				break;
			case Actor::Orientation::DOWN:
				if (PLAYER1.mapPos[1] > 0)
					PLAYER1.mapPos[1]--;
				PLAYER1.direction = Actor::Orientation::UP;
				break;
			case Actor::Orientation::LEFT:
				if (PLAYER1.mapPos[0] > 0)
					PLAYER1.mapPos[0]--;
				PLAYER1.direction = Actor::Orientation::UP;
				break;
			case Actor::Orientation::RIGHT:
				if (PLAYER1.mapPos[0] < COLUMNS -1)
					PLAYER1.mapPos[0]++;
				PLAYER1.direction = Actor::Orientation::UP;
				break;
			case Actor::Orientation::PRONE:
				PLAYER1.direction = Actor::Orientation::UP;
				break;
			default:
				PLAYER1.direction = Actor::Orientation::UP;
				break;
			}
			setPLAYER1ListenerLoc();
		}
	}

	// Va a process audio
	alSourcef(sound1, AL_GAIN, source1_volume);
	alSourcef(sound2, AL_GAIN, source2_volume);

}
void processAI(float dt) {
	if (SOURCE.hasUnprocessedMoved)
	{
		switch (SOURCE.direction)
		{
		case Actor::Orientation::UP:
			break;
		case Actor::Orientation::DOWN:
			break;
		case Actor::Orientation::LEFT:
			break;
		case Actor::Orientation::RIGHT:
			break;
		case Actor::Orientation::PRONE:
			break;
		default:
			break;
		}
	}
}

void setCellDrawColor(const int& x, const int& y) {
	processMapChanges();
	switch (map[x][y]) {
	case GraphicKey::UNSEARCHED:
		aColor[0] = 0.1f;
		aColor[1] = 0.4f;
		aColor[2] = 0.1f;
		break;
	case GraphicKey::SEARCHED:
		aColor[0] = 0.1f;
		aColor[1] = 0.1f;
		aColor[2] = 0.1f;
		break;
	case GraphicKey::PLAYER1:
		aColor[0] = 0.1f;
		aColor[1] = 0.1f;
		aColor[2] = 0.9f;
		break;
	case GraphicKey::SOURCE:
		aColor[0] = 0.9f;
		aColor[1] = 0.1f;
		aColor[2] = 0.1f;
		break;
	case GraphicKey::STAGE:
		aColor[0] = 0.86f;
		aColor[1] = 0.86f;
		aColor[2] = 0.76f;
		break;
	default:
		break;
	}
}
/*
void ImGuiWindow()
{
	// ImGUI window creation

	ImGui::Begin("Acousent");
	// Text that appears in the window
	ImGui::Text("Hello there musicians!");
	// Button that appears in the window
	if (ImGui::Button("Play left source"))
	{
		if (source_2.isPlaying())
			source_2.Stop();
		else
			source_2.Play(sound2);
	}
	// Slider that appears in the window
	ImGui::SliderFloat("Volume Right Source", &source1_volume, 0.f, 1.f);
	// Slider that appears in the window
	ImGui::SliderFloat("Volume Right Source", &source1_volume, 0.f, 1.f);
	// Ends the window
	ImGui::End();

}
*/
void clearScreen()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void renderScene()
{
	if (mapHasChanged)
	{

		// Aca esta el problema de cambiar cantidad de asientos
		mapHasChanged = false;
		clearScreen();

		glUseProgram(programID);
		//float iteT = (((float)ROWS / 10)*.09f);
		//float iteS = ((float)CELLSIZE / 40);
		//float iteR = (((float)ROWS / 10)-1.f) / (float)ROWS;
		//float iteC = (((float)COLUMNS / 10) - 1.f) / (float)COLUMNS;
		//float iteST = (float)CELLSIZE / 10;

		float iteT = 2.f * .09f;
		float iteS = ((float)CELLSIZE / 40);
		float iteR = 1.f / (float)ROWS;
		float iteC = 1.f/ (float)COLUMNS;
		float iteST = (float)CELLSIZE / 10;
		/// <summary>
		/// Where map[0][0] is the Bottom Left, and map[ROWS-1][COLUMNS-1] is the Top Right.
		/// </summary>
		for (int r = 0; r < ROWS; r++) {
			for (int c = 0; c < COLUMNS; c++)
			{
				setCellDrawColor(r, c);
				glUniform3fv(glGetUniformLocation(programID, "ucolor"), 1, &aColor[0]);
				glm::mat4 ModelMatrix(1.0f);
				glm::vec3 Translate((float)(((float)r) / (float)COLUMNS) * 2.f - .9f - iteR, (float)(((float)c) / (float)ROWS) * 2.f - .9f - iteR, 0.f);

				//glm::vec3 Translate((float)(((float)r) / (float)COLUMNS) * 2.f - .9f - iteR, (float)(((float)c) / (float)ROWS) * 2.f - .9f - iteR, 0.f);
				ModelMatrix = glm::translate(ModelMatrix, Translate);
				glm::vec3 Scale(1);
				if(c<(COLUMNS/2))
					Scale = glm::vec3((10.f / (float)CELLSIZE) * iteS -  iteT);
				else
					Scale = glm::vec3(iteST/ (float)CELLSIZE);
				ModelMatrix = glm::scale(ModelMatrix, Scale);
				glUniformMatrix4fv(glGetUniformLocation(programID, "modelmatrix"), 1, false, &ModelMatrix[0][0]);
				glDrawArrays(GL_TRIANGLES, 0, 12 * 3); // 12*3 indices starting at 0 -> 12 triangles -> 6 squares
			}
		}
		
		// Dear ImGui frame
		composeDearImGuiFrame();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}
	glfwPollEvents();
}
bool isRunning() { return (!glfwWindowShouldClose(window)); }
}  // end namespace TTD


