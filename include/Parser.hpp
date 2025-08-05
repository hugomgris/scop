/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:15:57 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/05 12:20:52 by hmunoz-g         ###   ########.fr       */
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

class Parser {
	public:
		Parser();
		~Parser();

		const std::vector<Vertex> &getVertices() const;
		const std::vector<unsigned int> &getIndices() const;
		const std::vector<Material> &getMaterials() const;
		const std::vector<MaterialGroup> &getMaterialGroups() const;
		const Material* getMaterialByName(const std::string& name) const;
		int getMaterialIndex(const std::string& name) const;
		size_t getRows() const;
		size_t getColumns() const;
		int getMode() const;

		void setMode(std::string &filePath);

		void checkExtension(const std::string &filePath) const;
		void parse(const std::string &filePath);
		void parseOBJ(const std::string &filePath);
		void parseMTL(const std::string &filePath);
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
		std::vector<Material> _materials;
		std::vector<MaterialGroup> _materialGroups;

		//Material tracking
		std::string _currentMaterial;
		int _currentMaterialIndex;

		//Lookup map
		std::unordered_map<FaceKey, unsigned int> _faceMap;

		//FDF stuff
		size_t _cols, _rows;
		float _xSpacing, _ySpacing, _zSpacing;

		//UV generation
		void generatePlanarUVs();
		void generateSphericalUVs();
		void generateCubicUVs();
};

#endif