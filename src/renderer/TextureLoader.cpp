/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TextureLoader.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 12:00:00 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/07 17:40:11 by hmunoz-g         ###   ########.fr       */
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

/**
 * Load Texture - Loads and caches texture from file path
 * 
 * FLOW:
 * 1. Validate file path is not empty
 * 2. Check texture cache for previously loaded texture
 * 3. Validate file exists and has supported format
 * 4. Create new Texture object and handle loading exceptions
 * 5. Cache successfully loaded texture for future use
 * 6. Log loading success and return shared pointer
 * 7. Return nullptr on any failure (file not found, unsupported format, load error)
 */
std::shared_ptr<Texture> TextureLoader::loadTexture(const std::string& filePath) {
    if (filePath.empty()) {
        return nullptr;
    }

    auto it = _textureCache.find(filePath);
    if (it != _textureCache.end()) {
        return it->second;
    }

    if (!isValidTexturePath(filePath)) {
        std::cerr << "Warning: Texture file not found or unsupported format: " << filePath << std::endl;
        return nullptr;
    }

    try {
        auto texture = std::make_shared<Texture>(filePath);
        
        _textureCache[filePath] = texture;
        
        std::cout << "Loaded texture: " << filePath << std::endl;
        return texture;
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading texture " << filePath << ": " << e.what() << std::endl;
        return nullptr;
    }
}

/**
 * Load Material Textures - Loads textures from material definition
 * 
 * FLOW:
 * 1. Check for diffuse texture map in material:
 *    - Attempt to load diffuse texture
 *    - Return immediately if successful
 * 2. Fallback to ambient texture map:
 *    - Attempt to load ambient texture if diffuse unavailable
 *    - Return if successful
 * 3. Log texture loading results and material usage
 * 4. Return nullptr if no valid textures found for material
 */
std::shared_ptr<Texture> TextureLoader::loadMaterialTextures(const Material& material) {
    if (!material.diffuseMap.empty()) {
        auto texture = loadTexture(material.diffuseMap);
        if (texture) {
            std::cout << "Using diffuse texture for material: " << material.name << std::endl;
            return texture;
        }
    }
    
    if (!material.ambientMap.empty()) {
        auto texture = loadTexture(material.ambientMap);
        if (texture) {
            std::cout << "Using ambient texture for material: " << material.name << std::endl;
            return texture;
        }
    }
    
    std::cout << "No valid textures found for material: " << material.name << std::endl;
    return nullptr;
}

/**
 * Load All Material Textures - Batch loads textures for multiple materials
 * 
 * FLOW:
 * 1. Initialize texture map for material index -> texture mapping
 * 2. Iterate through all materials in vector:
 *    - Attempt to load texture for each material
 *    - Map successful loads to material index
 *    - Log loading status for each material
 * 3. Generate comprehensive loading summary:
 *    - Count successful texture loads
 *    - Report total materials processed
 * 4. Return map of material indices to loaded textures
 */
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
    if (!std::filesystem::exists(filePath)) {
        return false;
    }
    
    return isSupportedFormat(filePath);
}

void TextureLoader::clearCache() {
    _textureCache.clear();
}

bool TextureLoader::isSupportedFormat(const std::string& filePath) const {
    std::string lowerPath = filePath;
    std::transform(lowerPath.begin(), lowerPath.end(), lowerPath.begin(), ::tolower);
    
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
