/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostProcessor.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 17:00:00 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/06 14:22:26 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/PostProcessor.hpp"
#include <iostream>
#include <glad/glad.h>

PostProcessor::PostProcessor(int width, int height) 
    : _framebuffer(0), _colorTexture(0), _depthRenderbuffer(0), 
      _quadVAO(0), _quadVBO(0), _width(width), _height(height), 
      _enableCRT(false), _time(0.0f) {
    
    _postProcessShader = std::make_unique<Shader>("resources/shaders/CRT_PostProcess.shader");
    _postProcessShader->compile();
    
    setupFramebuffer();
    setupQuad();
}

PostProcessor::~PostProcessor() {
    cleanup();
}

void PostProcessor::setupFramebuffer() {
    GLCall(glGenFramebuffers(1, &_framebuffer));
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer));
    
    GLCall(glGenTextures(1, &_colorTexture));
    GLCall(glBindTexture(GL_TEXTURE_2D, _colorTexture));
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _colorTexture, 0));
    
    GLCall(glGenRenderbuffers(1, &_depthRenderbuffer));
    GLCall(glBindRenderbuffer(GL_RENDERBUFFER, _depthRenderbuffer));
    GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _width, _height));
    GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _depthRenderbuffer));
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR: Framebuffer not complete!" << std::endl;
    }
    
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void PostProcessor::setupQuad() {
    float quadVertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f 
    };
    
    GLCall(glGenVertexArrays(1, &_quadVAO));
    GLCall(glGenBuffers(1, &_quadVBO));
    
    GLCall(glBindVertexArray(_quadVAO));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, _quadVBO));
    GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW));
    
    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0));
    
    GLCall(glEnableVertexAttribArray(1));
    GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float))));

    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCall(glBindVertexArray(0));
    
    std::cout << "PostProcessor quad setup complete. VAO: " << _quadVAO << ", VBO: " << _quadVBO << std::endl;
}

void PostProcessor::bind() {
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer));
    GLCall(glViewport(0, 0, _width, _height));
}

void PostProcessor::unbind() {
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void PostProcessor::render() {
    if (_quadVAO == 0) {
        std::cerr << "Error: quadVAO is 0!" << std::endl;
        return;
    }
    
    GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
    
    glDisable(GL_DEPTH_TEST);
    
    _postProcessShader->use();
    GLCall(glGetError());
    
    _postProcessShader->setUniform("u_screenTexture", 0);
    _postProcessShader->setUniform("u_time", _time);
    _postProcessShader->setUniform("u_resolution", glm::vec2(static_cast<float>(_width), static_cast<float>(_height)));

    GLint enableCRTLoc = glGetUniformLocation(_postProcessShader->getID(), "u_enableCRT");
    if (enableCRTLoc != -1) {
        glUniform1i(enableCRTLoc, _enableCRT ? 1 : 0);
    }

    GLCall(glActiveTexture(GL_TEXTURE0));
    GLCall(glBindTexture(GL_TEXTURE_2D, _colorTexture));
    
    GLCall(glBindVertexArray(_quadVAO));
    
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error before draw: " << error << std::endl;
    }

    GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
    
    if (depthTestEnabled) {
        glEnable(GL_DEPTH_TEST);
    }

    GLCall(glBindVertexArray(0));
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

void PostProcessor::resize(int width, int height) {
    _width = width;
    _height = height;
    
    if (_framebuffer != 0) {
        GLCall(glDeleteFramebuffers(1, &_framebuffer));
        _framebuffer = 0;
    }
    if (_colorTexture != 0) {
        GLCall(glDeleteTextures(1, &_colorTexture));
        _colorTexture = 0;
    }
    if (_depthRenderbuffer != 0) {
        GLCall(glDeleteRenderbuffers(1, &_depthRenderbuffer));
        _depthRenderbuffer = 0;
    }
    
    setupFramebuffer();
}

void PostProcessor::cleanup() {
    if (_framebuffer != 0) {
        GLCall(glDeleteFramebuffers(1, &_framebuffer));
        _framebuffer = 0;
    }
    if (_colorTexture != 0) {
        GLCall(glDeleteTextures(1, &_colorTexture));
        _colorTexture = 0;
    }
    if (_depthRenderbuffer != 0) {
        GLCall(glDeleteRenderbuffers(1, &_depthRenderbuffer));
        _depthRenderbuffer = 0;
    }
    if (_quadVAO != 0) {
        GLCall(glDeleteVertexArrays(1, &_quadVAO));
        _quadVAO = 0;
    }
    if (_quadVBO != 0) {
        GLCall(glDeleteBuffers(1, &_quadVBO));
        _quadVBO = 0;
    }
}
