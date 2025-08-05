/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TextureLoader.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 12:00:00 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/05 12:20:52 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/TextureLoader.hpp"
#include <iostream>
#include <algorithm>
#include <vector>

TextureLoader::TextureLoader() {}

TextureLoader::~TextureLoader() {
    clearCache();
}

std::shared_ptr<Texture> TextureLoader::loadTexture(const std::string& filePath) {
    if (filePath.empty()) {
        return nullptr;
    }

    // Check if texture is already in cache
    auto it = _textureCache.find(filePath);
    if (it != _textureCache.end()) {
        return it->second;
    }

    // Check if file exists and is a supported format
    if (!isValidTexturePath(filePath)) {
        std::cerr << "Warning: Texture file not found or unsupported format: " << filePath << std::endl;
        return nullptr;
    }

    try {
        // Create new texture
        auto texture = std::make_shared<Texture>(filePath);
        
        // Cache the texture
        _textureCache[filePath] = texture;
        
        std::cout << "Loaded texture: " << filePath << std::endl;
        return texture;
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading texture " << filePath << ": " << e.what() << std::endl;
        return nullptr;
    }
}

std::shared_ptr<Texture> TextureLoader::loadMaterialTextures(const Material& material) {
    // Try to load diffuse texture first (most common)
    if (!material.diffuseMap.empty()) {
        auto texture = loadTexture(material.diffuseMap);
        if (texture) {
            std::cout << "Using diffuse texture for material: " << material.name << std::endl;
            return texture;
        }
    }
    
    // If no diffuse texture, try ambient texture
    if (!material.ambientMap.empty()) {
        auto texture = loadTexture(material.ambientMap);
        if (texture) {
            std::cout << "Using ambient texture for material: " << material.name << std::endl;
            return texture;
        }
    }
    
    // If no textures found, return nullptr (will use default color)
    std::cout << "No valid textures found for material: " << material.name << std::endl;
    return nullptr;
}

std::unordered_map<int, std::shared_ptr<Texture>> TextureLoader::loadAllMaterialTextures(const std::vector<Material>& materials) {
    std::unordered_map<int, std::shared_ptr<Texture>> textureMap;
    
    for (size_t i = 0; i < materials.size(); ++i) {
        auto texture = loadMaterialTextures(materials[i]);
        if (texture) {
            textureMap[static_cast<int>(i)] = texture;
            std::cout << "Loaded texture for material " << i << ": " << materials[i].name << std::endl;
        } else {
            std::cout << "No texture found for material " << i << ": " << materials[i].name << std::endl;
        }
    }
    
    std::cout << "Loaded " << textureMap.size() << " textures from " << materials.size() << " materials" << std::endl;
    return textureMap;
}

std::shared_ptr<Texture> TextureLoader::getTexture(const std::string& filePath) {
    auto it = _textureCache.find(filePath);
    if (it != _textureCache.end()) {
        return it->second;
    }
    return nullptr;
}

bool TextureLoader::isValidTexturePath(const std::string& filePath) const {
    // Check if file exists
    if (!std::filesystem::exists(filePath)) {
        return false;
    }
    
    // Check if it's a supported format
    return isSupportedFormat(filePath);
}

void TextureLoader::clearCache() {
    _textureCache.clear();
}

bool TextureLoader::isSupportedFormat(const std::string& filePath) const {
    // Convert to lowercase for case-insensitive comparison
    std::string lowerPath = filePath;
    std::transform(lowerPath.begin(), lowerPath.end(), lowerPath.begin(), ::tolower);
    
    // Check for supported extensions (stb_image supports these and more)
    const std::vector<std::string> supportedExtensions = {
        ".png", ".jpg", ".jpeg", ".bmp", ".tga", ".gif", ".psd", ".hdr", ".pic"
    };
    
    for (const auto& ext : supportedExtensions) {
        if (lowerPath.length() >= ext.length() && 
            lowerPath.substr(lowerPath.length() - ext.length()) == ext) {
            return true;
        }
    }
    
    return false;
}
