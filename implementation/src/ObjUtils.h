#pragma once
#include <glm/glm.hpp>
#include <string.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

typedef glm::vec4 point4;
typedef glm::vec3 point3;
typedef glm::vec4 color4;

typedef struct furProps {
	bool useFur = false;
	int shellCount = 16;
	float shellMaxHeight = 1.0f;
	float thickness = 0.5f;
	int resolution = 32;
} FurProps;

typedef struct {
	GLuint program = 0;
	GLuint vao = 0;
	GLuint AmbientProduct, DiffuseProduct, SpecularProduct, Shininess, LightDirection = 0;
	GLuint Model, View, ModelView, ModelViewInverseTranspose, Projection = 0;
	GLuint DrawFur, ShellHeight, ShellMaxHeight, Thickness, Resolution = 0;
	GLuint WindDirection;
} ShaderProgram;

typedef struct {
	color4 ambient = color4(0);
	color4 diffuse = color4(0);
	color4 specular = color4(0);
	float dissolve = 0.0f;
	float shininess = 0.0f;
	std::string name = "";
	FurProps furProps;
} Material;

typedef struct {
	int numVertices = 0;
	point4* vertices = nullptr;
	point3* normals = nullptr;
	Material* material = nullptr;
	Material* defaultMaterial = nullptr;
	ShaderProgram* shaderProgram = nullptr;
} Mesh;

typedef struct {
	int numMeshes = 0;
	Mesh** meshes = nullptr;
	bool draw = true;
	glm::vec3 position = glm::vec3(0);
	glm::vec3 rotation = glm::vec3(0);
	float scale = 1;
} Obj;

void ResetMaterial(Mesh* mesh) {
	mesh->material = (Material*) calloc(1, sizeof(Material));
	memcpy(mesh->material, mesh->defaultMaterial, sizeof(Material));
}

void ResetMaterials(Obj* model) {
	for (int i = 0; i < model->numMeshes; i++) {
		ResetMaterial(model->meshes[i]);
	}
}

std::vector<Material*>* LoadMtl(char path[]) {
	std::ifstream file(path);

	if (!file.is_open()) {
		std::cout << "Could not open file " << path << std::endl;
		return nullptr;
	}

	std::vector<Material*>* materials = new std::vector<Material*>();

	std::string line;
	Material* currentMaterial = nullptr;
	while (std::getline(file, line)) {
		if (line.substr(0, 6) == "newmtl") {
			if (currentMaterial != nullptr && currentMaterial->ambient != color4(0)) {
				materials->push_back(currentMaterial);
			}
			currentMaterial = (Material*) calloc(1, sizeof(Material));
			currentMaterial->name = line.substr(7);
		}
		else if (line.substr(0, 3) == "Ka ") {
			std::istringstream s(line.substr(3));
			s >> currentMaterial->ambient.x;
			s >> currentMaterial->ambient.y;
			s >> currentMaterial->ambient.z;
			currentMaterial->ambient.w = 1;
		}
		else if (line.substr(0, 3) == "Kd ") {
			std::istringstream s(line.substr(3));
			s >> currentMaterial->diffuse.x;
			s >> currentMaterial->diffuse.y;
			s >> currentMaterial->diffuse.z;
			currentMaterial->diffuse.w = 1;
		}
		else if (line.substr(0, 3) == "Ks ") {
			std::istringstream s(line.substr(3));
			s >> currentMaterial->specular.x;
			s >> currentMaterial->specular.y;
			s >> currentMaterial->specular.z;
			currentMaterial->specular.w = 1;
		}
		else if (line.substr(0, 3) == "d ") {
			std::istringstream s(line.substr(3));
			s >> currentMaterial->dissolve;
		}
		else if (line.substr(0, 3) == "Ns ") {
			std::istringstream s(line.substr(3));
			s >> currentMaterial->shininess;
		} else if (line.substr(0, 4) == "fur ") {
			FurProps* furProps = (FurProps*) calloc(1, sizeof(FurProps));
			std::istringstream s(line.substr(4));
			int shellCount, resolution;
			float shellMaxHeight, thickness = 0;
			s >> shellCount;
			s >> shellMaxHeight;
			s >> thickness;
			s >> resolution;

			furProps->useFur = true;
			furProps->shellCount = shellCount;
			furProps->shellMaxHeight = shellMaxHeight;
			furProps->thickness = thickness;
			furProps->resolution = resolution;

			memcpy(&currentMaterial->furProps, furProps, sizeof(FurProps));
		}
	}

	if (currentMaterial != nullptr && currentMaterial->ambient != glm::vec4(0)) {
		materials->push_back(currentMaterial);
	}

	//close file
	file.close();

	return materials; 
}

Obj* LoadObj(char objPath[], char mtlPath[]) {
	Obj* model = new Obj();
	std::vector<Material*>* materials = LoadMtl(mtlPath);

	std::ifstream file(objPath);
	if (!file.is_open()) {
		std::cout << "Could not open file " << objPath << std::endl;
		return model;
	}

	Material* currentMaterial = nullptr;

	int numMeshes = 0;
	Mesh** meshes = nullptr;

	std::vector<point3> rawVertices;
	std::vector<point3> rawNormals;
	int numVertices = 0;
	point4* vertices = nullptr;
	point3* normals = nullptr;

	// File format:
	// v x y z
	// vn x y z
	// usemtl name
	// f v1//vn1 v2//vn2 v3//vn3

	std::string line;
	while (std::getline(file, line)) {
		if (line.substr(0, 2) == "v ") {
			std::istringstream s(line.substr(2));
			float x, y, z = 0;
			s >> x;
			s >> y;
			s >> z;

			rawVertices.push_back(point3(x, y, z));
		}
		else if (line.substr(0, 3) == "vn ") {
			std::istringstream s(line.substr(3));
			float x, y, z = 0;
			s >> x;
			s >> y;
			s >> z;

			rawNormals.push_back(point3(x, y, z));
		}
		else if (line.substr(0, 2) == "f ") {
			std::istringstream s(line.substr(2));

			int a, b, c;
			int j, k, l;
			s >> a;
			s.ignore(2);
			s >> j;

			s >> b;
			s.ignore(2);
			s >> k;

			s >> c;
			s.ignore(2);
			s >> l;


			numVertices += 3;

			if (numVertices == 3) {
				vertices = (point4*)calloc(numVertices, sizeof(point4));
			}
			else {
				vertices = (point4*)realloc(vertices, numVertices * sizeof(point4));
			}

			vertices[numVertices - 3] = point4(rawVertices[a - 1], 1);
			vertices[numVertices - 2] = point4(rawVertices[b - 1], 1);
			vertices[numVertices - 1] = point4(rawVertices[c - 1], 1);

			if (normals == nullptr) {
				normals = (point3*)calloc(numVertices, sizeof(point3));
			}
			else {
				normals = (point3*)realloc(normals, numVertices * sizeof(point3));
			}

			normals[numVertices - 3] = rawNormals[j - 1];
			normals[numVertices - 2] = rawNormals[k - 1];
			normals[numVertices - 1] = rawNormals[l - 1];
		}
		else if (line.substr(0, 7) == "usemtl ") {
			if (numVertices > 0) {
				Mesh* mesh = (Mesh*) calloc(1, sizeof(Mesh));
				mesh->numVertices = numVertices;
				mesh->vertices = vertices;
				mesh->normals = normals;
				mesh->defaultMaterial = currentMaterial;

				if (numMeshes == 0) {
					meshes = (Mesh**) calloc(1, sizeof(Mesh*));
				}
				else {
					meshes = (Mesh**) realloc(meshes, (numMeshes + 1) * sizeof(Mesh*));
				}

				meshes[numMeshes] = mesh;
				numMeshes++;

				numVertices = 0;
				vertices = nullptr;
				normals = nullptr;
			}

			std::string materialName = line.substr(7);
			for (int i = 0; i < materials->size(); i++) {
				Material* mat = materials->at(i);
				if (mat->name == materialName) {
					currentMaterial = mat;
					break;
				}
			}
		}
	}

	if (numVertices > 0) {
		Mesh* mesh = (Mesh*) calloc(1, sizeof(Mesh));
		mesh->numVertices = numVertices;
		mesh->vertices = vertices;
		mesh->normals = normals;
		mesh->defaultMaterial = currentMaterial;

		if (numMeshes == 0) {
			meshes = (Mesh**) calloc(1, sizeof(Mesh*));
		}
		else {
			meshes = (Mesh**) realloc(meshes, (numMeshes + 1) * sizeof(Mesh*));
		}

		meshes[numMeshes] = mesh;
		numMeshes++;
	}

	file.close();

	// normalize vertices to center and scale to unit size
	glm::vec3 min = glm::vec3(FLT_MAX);
	glm::vec3 max = glm::vec3(-FLT_MAX);
	for (int i = 0; i < rawVertices.size(); i++) {
		min = glm::min(min, rawVertices[i]);
		max = glm::max(max, rawVertices[i]);
	}

	glm::vec4 center = point4((min + max) / 2.0f, 0);
	float scale = 0.1f;

	for (int i = 0; i < numMeshes; i++) {
		Mesh* mesh = meshes[i];
		for (int j = 0; j < mesh->numVertices; j++) {
			mesh->vertices[j] = (mesh->vertices[j] - center - glm::vec4(0, min.y, 0, 0)) * scale;
			mesh->vertices[j].w = 1;
		}
	}

	model->numMeshes = numMeshes;
	model->meshes = meshes;

	ResetMaterials(model);

	return model;
}