/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostProcessor.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 17:00:00 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/06 13:52:56 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POSTPROCESSOR_HPP
# define POSTPROCESSOR_HPP

# include <glad/glad.h>
# include <glm/glm.hpp>
# include <memory>
# include "./Shader.hpp"
# include "./ErrorManager.hpp"

class PostProcessor {
private:
    unsigned int _framebuffer;
    unsigned int _colorTexture;
    unsigned int _depthRenderbuffer;
    unsigned int _quadVAO;
    unsigned int _quadVBO;
    
    std::unique_ptr<Shader> _postProcessShader;
    
    int _width;
    int _height;
    bool _enableCRT;
    float _time;
    
    void setupFramebuffer();
    void setupQuad();
    void cleanup();

public:
    PostProcessor(int width, int height);
    ~PostProcessor();
    
    void bind();
    void unbind();
    void render();
    void resize(int width, int height);
    
    void setEnableCRT(bool enable) { _enableCRT = enable; }
    bool getEnableCRT() const { return _enableCRT; }
    
    void updateTime(float time) { _time = time; }
    
    unsigned int getFramebuffer() const { return _framebuffer; }
    unsigned int getColorTexture() const { return _colorTexture; }
};

#endif
