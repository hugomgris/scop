/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:15:57 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/04 17:00:39 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef PARSER_HPP
# define PARSER_HPP

# include <cmath>
# include <algorithm>
# include <limits>
# include <vector>
# include <unordered_map>
# include <functional>
# include <iostream>
# include <glm/glm.hpp>

# include "./Types.hpp"

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

class Parser {
	public:
		Parser();
		~Parser();

		const std::vector<Vertex> &getVertices() const;
		const std::vector<unsigned int> &getIndices() const;
		size_t getRows() const;
		size_t getColumns() const;
		int getMode() const;

		void setMode(std::string &filePath);

		void checkExtension(const std::string &filePath) const;
		void parse(const std::string &filePath);
		void parseOBJ(const std::string &filePath);
		void parseFDF(const std::string &filePath);
		void countFDFPositions(const std::string &filePath);
		void calculateFDFSpacing();
		void calculateNormals();
		void updateMinMaxZ(float newZ);
		float getZDifference() const;
		const BoundingBox& getBoundingBox() const;
		void updateBoundingBox(const glm::vec3& position);
		float getOptimalCameraDistance() const;

	private:
		BoundingBox _boundingBox;
		int _mode;
		float _maxZ = std::numeric_limits<float>::min();
		float _minZ = std::numeric_limits<float>::max();
	
		//Raw storage
		std::vector<glm::vec3> _positions;
		std::vector<glm::vec2> _texCoords;
		std::vector<glm::vec3> _normals;

		//Final buffers
		std::vector<Vertex> _vertices;
		std::vector<unsigned int> _indices;

		//Lookup map
		std::unordered_map<FaceKey, unsigned int> _faceMap;

		//FDF stuff
		size_t _cols, _rows;
		float _xSpacing, _ySpacing, _zSpacing;
};

#endif