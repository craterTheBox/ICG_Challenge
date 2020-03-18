//ICG_Challenge
//Carter Menary - 100700587
//Edward Cao - 100697845


//This code is built off of the GOOP framework with the AudioEngine provided in the Game Sound tutorials


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "TTK/TTKContext.h"
#include "Logging.h"
#include "TTK/GraphicsUtils.h"
#include "TTK/Camera.h"
#include "AudioEngine.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include "TTK/Input.h"

#include "imgui.h"
#include "TTK/SpriteSheetQuad.h"

#define LOG_GL_NOTIFICATIONS
/*
	Handles debug messages from OpenGL
	https://www.khronos.org/opengl/wiki/Debug_Output#Message_Components
	@param source    Which part of OpenGL dispatched the message
	@param type      The type of message (ex: error, performance issues, deprecated behavior)
	@param id        The ID of the error or message (to distinguish between different types of errors, like nullref or index out of range)
	@param severity  The severity of the message (from High to Notification)
	@param length    The length of the message
	@param message   The human readable message from OpenGL
	@param userParam The pointer we set with glDebugMessageCallback (should be the game pointer)
*/
void GlDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	std::string sourceTxt;
	switch (source) {
	case GL_DEBUG_SOURCE_API: sourceTxt = "DEBUG"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM: sourceTxt = "WINDOW"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceTxt = "SHADER"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY: sourceTxt = "THIRD PARTY"; break;
	case GL_DEBUG_SOURCE_APPLICATION: sourceTxt = "APP"; break;
	case GL_DEBUG_SOURCE_OTHER: default: sourceTxt = "OTHER"; break;
	}
	switch (severity) {
	case GL_DEBUG_SEVERITY_LOW:          LOG_INFO("[{}] {}", sourceTxt, message); break;
	case GL_DEBUG_SEVERITY_MEDIUM:       LOG_WARN("[{}] {}", sourceTxt, message); break;
	case GL_DEBUG_SEVERITY_HIGH:         LOG_ERROR("[{}] {}", sourceTxt, message); break;
#ifdef LOG_GL_NOTIFICATIONS
	case GL_DEBUG_SEVERITY_NOTIFICATION: LOG_INFO("[{}] {}", sourceTxt, message); break;
#endif
	default: break;
	}
}


int main() { 

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	long long memBreak = 0;
	if (memBreak) _CrtSetBreakAlloc(memBreak);

	Logger::Init();

	// Initialize GLFW
	if (glfwInit() == GLFW_FALSE) {
		std::cout << "Failed to initialize Glad" << std::endl;
		return 1;
	}

	// Create a new GLFW window
	GLFWwindow* window = glfwCreateWindow(800, 600, "ICG_Challenge - Pong", nullptr, nullptr);

	// We want GL commands to be executed for our window, so we make our window's context the current one
	glfwMakeContextCurrent(window);

	// Initialize the TTK input system
	TTK::Input::Init(window);

	// Let glad know what function loader we are using (will call gl commands via glfw)
	if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
		std::cout << "Failed to initialize Glad" << std::endl;
		return 2;
	}

	// Display our GPU and OpenGL version
	std::cout << glGetString(GL_RENDERER) << std::endl;
	std::cout << glGetString(GL_VERSION) << std::endl;

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(GlDebugMessage, nullptr);

	TTK::Graphics::SetCameraMode3D(800, 600);
	glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
		TTK::Graphics::SetCameraMode3D(width, height);
		});
	TTK::Graphics::SetBackgroundColour(0.0f, 0.0f, 0.0f);
	TTK::Graphics::SetDepthEnabled(true);

	TTK::Camera camera;
	camera.cameraPosition = glm::vec3(0.0f, 0.0f, -10.0f);
	camera.forwardVector = glm::vec3(0.0f, 0.0f, 1.0f);

	glm::vec2 lastMousePos, mousePos;
	lastMousePos = TTK::Input::GetMousePos();

	TTK::Graphics::InitImGUI(window);

	float lastFrame = glfwGetTime();

	//// Make an AudioEngine object 
	AudioEngine audioEngine;

	//// Init AudioEngine (Don't forget to shut down and update)
	audioEngine.Init();

	//// Load a bank (Use the flag FMOD_STUDIO_LOAD_BANK_NORMAL)
	audioEngine.LoadBank("Master", FMOD_STUDIO_LOAD_BANK_NORMAL);

	//// Load an event
	audioEngine.LoadEvent("beep", "{c86b2f63-2d58-49c7-997f-c2b68a93aaaf}");

	//// Play the event
	audioEngine.PlayEvent("beep");

	glm::vec3 paddle1 = { 6.0f, 0.0f, 0.0f };	//W and S for up and down
	glm::vec3 paddle2 = { -6.0f, 0.0f, 0.0f };	//UpArrow and DownArrow for up and down
	glm::vec3 ball = { 0.0f, 0.0f, 0.0f };
	glm::vec3 ballVelocity = { 4.0f, 1.0f, 0.0f };

	glm::vec4 white = { 1.0f, 1.0f, 1.0f, 1.0f };

	float score1 = 0.0f, score2 = 0.0f;

	while (!glfwWindowShouldClose(window)) {
		// Poll for events from windows (clicks, keypressed, closing, all that)
		glfwPollEvents();

		float thisFrame = glfwGetTime();
		float dt = thisFrame - lastFrame;

		// Clear our screen every frame
		TTK::Graphics::ClearScreen();

		//Left Paddle
		if (TTK::Input::GetKeyDown(TTK::KeyCode::W) && paddle1.y < 5.0f) {
			paddle1.y += 6.0f * dt;
		} 
		if (TTK::Input::GetKeyDown(TTK::KeyCode::S) && paddle1.y > -5.0f) {
			paddle1.y -= 6.0f * dt;
		}
		//Right Paddle
		if (TTK::Input::GetKeyDown(TTK::KeyCode::Up) && paddle2.y < 5.0f) {
			paddle2.y += 6.0f * dt;
		}
		if (TTK::Input::GetKeyDown(TTK::KeyCode::Down) && paddle2.y > -5.0f) {
			paddle2.y -= 6.0f * dt;
		}
		
		//update ball position here
		ball += ballVelocity * dt;

		
		//paddle collision with ball (it's a little screwey
		if (paddle1.x < ball.x + 0.125f && paddle1.x + 0.5f > ball.x &&
			paddle1.y < ball.y + 0.125f && paddle1.y + 0.5f > ball.y) {
			ballVelocity.x *= -1.0f;
			audioEngine.PlayEvent("beep");
		}
		if (paddle2.x < ball.x + 0.125f && paddle2.x + 0.5f > ball.x &&
			paddle2.y < ball.y + 0.125f && paddle2.y + 0.5f > ball.y) {
			ballVelocity.x *= -1.0f;
			audioEngine.PlayEvent("beep");
		}
		//top and bottom of map collision
		if (ball.y >= 5.0f || ball.y <= -5.0f) {
			ballVelocity.y *= -1.0f;
			audioEngine.PlayEvent("beep");
		}
		//out of bounds reset
		if (ball.x > 10.0f) {
			ball.x = 0.0f;
			audioEngine.PlayEvent("beep");
			score2++;
			std::cout << score1 << " - " << score2 << std::endl;
		} 
		if (ball.x < -10.0f) {
			ball.x = 0.0f;
			audioEngine.PlayEvent("beep");
			score1++;
			std::cout << score1 << " - " << score2 << std::endl;
		}


		TTK::Graphics::DrawCube(paddle1, 1.0f, white);
		TTK::Graphics::DrawCube(paddle2, 1.0f, white);
		TTK::Graphics::DrawCube(ball, 0.25f, white);

		//this next bit is the bar in the middle of the screen
		TTK::Graphics::DrawLine(glm::vec3(0.0f, 8.0f, 0.0f), glm::vec3(0.0f, -8.0f, 0.0f), white);

		//// Update Audio Engine
		audioEngine.Update();


		camera.update();
		TTK::Graphics::SetCameraMatrix(camera.ViewMatrix);

		TTK::Graphics::EndFrame();
		TTK::Graphics::BeginGUI();
		TTK::Graphics::EndGUI();

		// Present our image to windows
		glfwSwapBuffers(window);

		TTK::Input::Poll();

		lastFrame = thisFrame;
	}

	glfwTerminate();


	//// Shut down audio engine
	audioEngine.Shutdown();

	TTK::Graphics::ShutdownImGUI();
	TTK::Input::Uninitialize();
	TTK::Graphics::Cleanup();
	Logger::Uninitialize();

	return 0;
}