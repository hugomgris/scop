/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:15:57 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/07 16:49:25 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file Parser.hpp
 * @brief Declaration of the Parser class for 3D model file parsing.
 *
 * The Parser class is responsible for reading and interpreting both OBJ and FDF file formats,
 * converting raw file data into structured vertex, index, and material information that can
 * be used by the rendering pipeline. It handles material loading, texture coordinate generation,
 * normal calculation, and bounding box computation for optimal camera positioning.
 */

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

/**
 * @class Parser
 * @brief Handles parsing of 3D model files (OBJ and FDF formats).
 *
 * The Parser class provides comprehensive file parsing capabilities for the SCOP renderer:
 * - OBJ file parsing with full material (.mtl) support
 * - FDF heightmap parsing for wireframe visualization
 * - Automatic texture coordinate generation for models without UVs
 * - Normal vector calculation using cross products
 * - Bounding box computation for camera positioning
 * - Material grouping for multi-texture rendering
 *
 * The parser automatically detects file format based on extension and handles
 * various edge cases like missing normals, texture coordinates, or materials.
 */
class Parser {
	private:
		BoundingBox _boundingBox; 
		int _mode;
		float _maxZ = std::numeric_limits<float>::min();
		float _minZ = std::numeric_limits<float>::max();
	
		std::vector<glm::vec3> _positions;
		std::vector<glm::vec2> _texCoords;
		std::vector<glm::vec3> _normals;

		std::vector<Vertex> _vertices;
		std::vector<unsigned int> _indices;
		std::vector<Material> _materials;
		std::vector<MaterialGroup> _materialGroups;

		std::string _currentMaterial;
		int _currentMaterialIndex;

		std::unordered_map<FaceKey, unsigned int> _faceMap;

		size_t _cols, _rows;
		float _xSpacing, _ySpacing, _zSpacing;
		
		void generatePlanarUVs();
		void generateSphericalUVs();
		void generateCubicUVs();
	
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
};

#endif