/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorManager.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 09:38:42 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/07 16:42:06 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORMANAGER_HPP
# define ERRORMANAGER_HPP

# include <iostream>
# include <iomanip> 
# include "glm/glm.hpp"
# include "glm/gtc/matrix_transform.hpp"
# include "glm/gtc/type_ptr.hpp"
# include <glad/glad.h>

class ErrorManager {
	public:
		static void GLClearError();
		static bool GLLogCall(const char *function, const char *file, int line);
};

// Debug break so that execution stops when glError triggers
# ifdef _MSC_VER
    # define DEBUG_BREAK() __debugbreak()
# else
    # include <csignal>
    # define DEBUG_BREAK() std::raise(SIGTRAP)
# endif

// Macro assertion to go through the debug break and provoke the stop
# define GLCall(x) ErrorManager::GLClearError();\
	x;\
	ASSERT(ErrorManager::GLLogCall(#x, __FILE__, __LINE__))

#define ASSERT(x) if (!(x)) DEBUG_BREAK();

#endif