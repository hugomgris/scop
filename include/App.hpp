/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   App.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:16:31 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/07/30 14:16:31 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
