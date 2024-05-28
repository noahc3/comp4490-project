#pragma once
#include <glew.h>
#include "../src/ObjUtils.h"
#include <glm/gtc/type_ptr.hpp>

glm::vec4 light_direction(1, 1, 1, 0);
glm::vec4 light_ambient(0.1f, 0.1f, 0.1f, 1);
glm::vec4 light_diffuse(1.0, 1.0, 0.9, 1);
glm::vec4 light_specular(1.0, 1.0, 0.9, 1);


void ApplyBlinnPhong(Obj* model) {
	

	for (int i = 0; i < model->numMeshes; i++) {
		GLuint vbo;
		ShaderProgram* shader = new ShaderProgram();
		Mesh* mesh = model->meshes[i];

		glGenVertexArrays(1, &shader->vao);

		shader->program = InitShader("shaders/blinnphong.vert", "shaders/blinnphong.frag");

		// Gen buffers and copy vertex/normal data
		glUseProgram(shader->program);
		glBindVertexArray(shader->vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, model->meshes[i]->numVertices * (sizeof(glm::vec4) + sizeof(glm::vec3)), NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, model->meshes[i]->numVertices * sizeof(glm::vec4), glm::value_ptr(*model->meshes[i]->vertices));
		glBufferSubData(GL_ARRAY_BUFFER, model->meshes[i]->numVertices * sizeof(glm::vec4), model->meshes[i]->numVertices * sizeof(glm::vec3), glm::value_ptr(*model->meshes[i]->normals));

		// Init buffer handles
		GLuint vPosition = glGetAttribLocation(shader->program, "vPosition");
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

		GLuint vNormal = glGetAttribLocation(shader->program, "vNormal");
		glEnableVertexAttribArray(vNormal);
		glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(model->meshes[i]->numVertices * sizeof(glm::vec4)));

		// Init uniforms

		GLuint AmbientProduct = glGetUniformLocation(shader->program, "AmbientProduct");
		GLuint DiffuseProduct = glGetUniformLocation(shader->program, "DiffuseProduct");
		GLuint SpecularProduct = glGetUniformLocation(shader->program, "SpecularProduct");
		GLuint Shininess = glGetUniformLocation(shader->program, "Shininess");
		GLuint LightDirection = glGetUniformLocation(shader->program, "LightDirection");

		GLuint ModelView = glGetUniformLocation(shader->program, "ModelView");
		GLuint ModelViewInverseTranspose = glGetUniformLocation(shader->program, "ModelViewInverseTranspose");
		GLuint Model = glGetUniformLocation(shader->program, "Model");
		GLuint View = glGetUniformLocation(shader->program, "View");
		GLuint Projection = glGetUniformLocation(shader->program, "Projection");

		GLuint DrawFur = glGetUniformLocation(shader->program, "DrawFur");
		GLuint ShellHeight = glGetUniformLocation(shader->program, "ShellHeight");
		GLuint ShellMaxHeight = glGetUniformLocation(shader->program, "ShellMaxHeight");
		GLuint Thickness = glGetUniformLocation(shader->program, "Thickness");
		GLuint Resolution = glGetUniformLocation(shader->program, "Resolution");

		GLuint WindDirection = glGetUniformLocation(shader->program, "WindDirection");

		// Copy handle IDs to shader struct
		shader->AmbientProduct = AmbientProduct;
		shader->DiffuseProduct = DiffuseProduct;
		shader->SpecularProduct = SpecularProduct;
		shader->Shininess = Shininess;
		shader->LightDirection = LightDirection;

		shader->Model = Model;
		shader->View = View;
		shader->ModelView = ModelView;
		shader->ModelViewInverseTranspose = ModelViewInverseTranspose;
		shader->Projection = Projection;

		shader->DrawFur = DrawFur;
		shader->ShellHeight = ShellHeight;
		shader->ShellMaxHeight = ShellMaxHeight;
		shader->Thickness = Thickness;
		shader->Resolution = Resolution;

		shader->WindDirection = WindDirection;

		// Copy fur data to GPU
		if (mesh->material->furProps.useFur) {
			glUniform1i(DrawFur, 1);
			glUniform1f(ShellMaxHeight, mesh->material->furProps.shellMaxHeight);
			glUniform1f(Thickness, mesh->material->furProps.thickness);
			glUniform1i(Resolution, mesh->material->furProps.resolution);
		}
		else {
			glUniform1i(DrawFur, 0);
		}

		model->meshes[i]->shaderProgram = shader;
	}
}

void DrawBlinnPhong(Obj* object, glm::mat4 model, glm::mat4 view, glm::mat4 Projection) {
	if (!object->draw) return;

	glm::mat4 ModelView = view * model;
	glm::mat4 ModelViewInverseTranspose = glm::transpose(glm::inverse(ModelView));

	for (int i = 0; i < object->numMeshes; i++) {
		Mesh* mesh = object->meshes[i];
		Material* material = mesh->material;
		ShaderProgram* shader = mesh->shaderProgram;
		glUseProgram(shader->program);

		// Bake lighting data
		glm::vec4 ambient_product = light_ambient * mesh->material->ambient;
		glm::vec4 diffuse_product = light_diffuse * mesh->material->diffuse;
		glm::vec4 specular_product = light_specular * mesh->material->specular;

		// Copy baked values to GPU
		glUniform4fv(shader->AmbientProduct, 1, glm::value_ptr(ambient_product));
		glUniform4fv(shader->DiffuseProduct, 1, glm::value_ptr(diffuse_product));
		glUniform4fv(shader->SpecularProduct, 1, glm::value_ptr(specular_product));
		glUniform4fv(shader->LightDirection, 1, glm::value_ptr(light_direction));
		glUniform1f(shader->Shininess, mesh->material->shininess);

		glUniformMatrix4fv(shader->View, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(shader->ModelView, 1, GL_FALSE, glm::value_ptr(ModelView));
		glUniformMatrix4fv(shader->ModelViewInverseTranspose, 1, GL_FALSE, glm::value_ptr(ModelViewInverseTranspose));
		glUniformMatrix4fv(shader->Projection, 1, GL_FALSE, glm::value_ptr(Projection));

		glUniform1i(shader->DrawFur, 0);

		glBindVertexArray(shader->vao);
		glDrawArrays(GL_TRIANGLES, 0, mesh->numVertices);

		if (material->furProps.useFur) {
			FurProps* fur = &material->furProps;
			glUniform1i(shader->DrawFur, 1);
			glUniform1f(shader->ShellMaxHeight, fur->shellMaxHeight);
			glUniform1f(shader->Thickness, fur->thickness);
			glUniform1i(shader->Resolution, fur->resolution);
			for (int j = 0; j < fur->shellCount; j++) {
				glUniform1f(shader->ShellHeight,    ( ((float)j) / ((float)fur->shellCount) )   );
				glDrawArrays(GL_TRIANGLES, 0, mesh->numVertices);
			}
		}
	}
}

void DrawBlinnPhong(Obj* object, glm::mat4 view, glm::mat4 Projection) {
	glm::mat4 model = glm::mat4();
	model = glm::translate(model, object->position);
	model = glm::scale(model, glm::vec3(object->scale));
	model = glm::rotate(model, glm::radians(object->rotation.x), glm::vec3(1, 0, 0));
	model = glm::rotate(model, glm::radians(object->rotation.y), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(object->rotation.z), glm::vec3(0, 0, 1));
	DrawBlinnPhong(object, model, view, Projection);
}