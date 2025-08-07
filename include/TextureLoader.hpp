/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TextureLoader.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 12:00:00 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/07 16:58:30 by hmunoz-g         ###   ########.fr       */
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
    private:
    std::unordered_map<std::string, std::shared_ptr<Texture>> _textureCache;
    
    bool isSupportedFormat(const std::string& filePath) const;
    
    public:
        TextureLoader();
        ~TextureLoader();

        std::shared_ptr<Texture> loadTexture(const std::string& filePath);
        std::shared_ptr<Texture> loadMaterialTextures(const Material& material);
        std::unordered_map<int, std::shared_ptr<Texture>> loadAllMaterialTextures(const std::vector<Material>& materials);
        std::shared_ptr<Texture> getTexture(const std::string& filePath);
        
        bool isValidTexturePath(const std::string& filePath) const;
        
        void clearCache();
};

#endif