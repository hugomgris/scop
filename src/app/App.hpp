#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class App {
	public:
		App();
		~App();
		void run();

	private:
		GLFWwindow *window;
		static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
};
