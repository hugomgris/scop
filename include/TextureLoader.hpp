/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TextureLoader.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 12:00:00 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/05 12:20:52 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef TEXTURELOADER_HPP
# define TEXTURELOADER_HPP

# include <string>
# include <unordered_map>
# include <memory>
# include <filesystem>
# include "Texture.hpp"
# include "Types.hpp"

class TextureLoader {
public:
    TextureLoader();
    ~TextureLoader();

    // Load a texture from a file path, returns nullptr if failed
    std::shared_ptr<Texture> loadTexture(const std::string& filePath);
    
    // Load textures for a material, returns the diffuse texture or nullptr
    std::shared_ptr<Texture> loadMaterialTextures(const Material& material);
    
    // Load all materials' textures and return a map of material index to texture
    std::unordered_map<int, std::shared_ptr<Texture>> loadAllMaterialTextures(const std::vector<Material>& materials);
    
    // Get a texture by file path (from cache)
    std::shared_ptr<Texture> getTexture(const std::string& filePath);
    
    // Check if a texture file exists and is valid
    bool isValidTexturePath(const std::string& filePath) const;
    
    // Clear the texture cache
    void clearCache();

private:
    // Cache for loaded textures to avoid loading the same texture multiple times
    std::unordered_map<std::string, std::shared_ptr<Texture>> _textureCache;
    
    // Supported texture extensions
    bool isSupportedFormat(const std::string& filePath) const;
};

#endif