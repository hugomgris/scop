/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   App.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:16:31 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/07/30 18:46:37 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "./Mesh.hpp"
#include "./Shader.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class App {
	public:
		App(Mesh *mesh, Shader *shader);
		~App();
		void run();

	private:
		Mesh *_mesh;
		Shader *_shader;
	
		GLFWwindow *window;
		static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
};
