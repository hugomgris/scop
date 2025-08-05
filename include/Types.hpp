/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Types.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 16:38:59 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/05 11:54:31 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef TYPES_HPP
# define TYPES_HPP

# include "glm/glm.hpp"
# include "glm/gtc/matrix_transform.hpp"
# include "glm/gtc/type_ptr.hpp"

enum Type {
  OBJ,
  FDF,
};

struct BoundingBox {
    glm::vec3 min;
    glm::vec3 max;
    
    BoundingBox() : min(FLT_MAX), max(-FLT_MAX) {}
    
    glm::vec3 getCenter() const {
        return (min + max) * 0.5f;
    }
    
    glm::vec3 getSize() const {
        return max - min;
    }
    
    float getMaxDimension() const {
        glm::vec3 size = getSize();
        return std::max(std::max(size.x, size.y), size.z);
    }
    
    float getDiagonal() const {
        return glm::length(getSize());
    }
};

struct Vertex {
	glm::vec3 position;
	glm::vec2 texCoord;
	glm::vec3 normal;

	bool operator==(const Vertex& other) const {
		return position == other.position &&
		       texCoord == other.texCoord &&
		       normal == other.normal;
	}
};

struct FaceKey {
	int posIndex;
	int texIndex;
	int normIndex;

	bool operator==(const FaceKey& other) const {
		return posIndex == other.posIndex &&
		       texIndex == other.texIndex &&
		       normIndex == other.normIndex;
	}
};

namespace std {
	template <>
	struct hash<FaceKey> {
		std::size_t operator()(const FaceKey& key) const {
			std::size_t h1 = std::hash<int>()(key.posIndex);
			std::size_t h2 = std::hash<int>()(key.texIndex);
			std::size_t h3 = std::hash<int>()(key.normIndex);

			return h1 ^ (h2 << 1) ^ (h3 << 2);
		}
	};
}

struct ShaderProgramSource {
	std::string vertexSource;
	std::string fragmentSource;
};

struct Material {
    std::string name;
    
    // Colors
    glm::vec3 ambient = glm::vec3(0.2f);     // Ka
    glm::vec3 diffuse = glm::vec3(0.8f);     // Kd  
    glm::vec3 specular = glm::vec3(0.0f);    // Ks
    glm::vec3 emission = glm::vec3(0.0f);    // Ke
    
    // Material properties
    float shininess = 0.0f;                  // Ns
    float opacity = 1.0f;                    // d or Tr
    float refractiveIndex = 1.0f;            // Ni
    int illuminationModel = 2;               // illum
    
    // Texture maps
    std::string diffuseMap;                  // map_Kd
    std::string normalMap;                   // map_Bump or bump
    std::string specularMap;                 // map_Ks
    std::string ambientMap;                  // map_Ka
    std::string opacityMap;                  // map_d
    std::string displacementMap;             // disp
    
    Material() = default;
    Material(const std::string& materialName) : name(materialName) {}
};

#endif
