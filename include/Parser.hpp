/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:15:57 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/07/31 12:09:50 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <glm/glm.hpp>

enum Type {
  OBJ,
  FDF,
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
		int getMode() const;

		void setMode(std::string &filePath);

		void checkExtension(const std::string &filePath) const;
		void parse(const std::string &filePath);
		void parseOBJ(const std::string &filePath);
		void parseFDF(const std::string &filePath);
		void countFDFPositions(const std::string &filePath);
		void calculateFDFSpacing();

	private:
		int _mode;
	
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
		int _xSpacing, _ySpacing, _zSpacing;
};