#ifndef MESH_H
#define MESH_H
#include <iostream>
#include <vector>
#include <string>

#include <glm/glm.hpp>

using namespace std;
using namespace glm;

struct vertexData{
	vec3 position;
	vec3 normal;
	vec3 tangent;
	vec3 color;
	float U,V;
};

struct textureData{
	unsigned int id;
	unsigned int type;
};

typedef unsigned int uint;

class Mesh{
	vector<vertexData> data;
	vector<textureData> textures;
	vector<unsigned int> indices;
	unsigned int VBO;
	unsigned int IND; 
	public:
		Mesh(vector<vertexData>* vd, vector<unsigned int>* id, vector<textureData>* td) {
			data = *vd;
			indices = *id;
			if (td)
				textures = *td;

			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(vertexData), &data[0], GL_STATIC_DRAW);


			glGenBuffers(1, &IND);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IND);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		}

		~Mesh() {
			glDeleteBuffers(1, &VBO);
			glDeleteBuffers(1, &IND);
		}

		void draw(unsigned int programId) {
			//attribute vec3 vertex
			int vertex 	= glGetAttribLocation(programId, "position"); //0
			int normal 	= glGetAttribLocation(programId, "normal"); //1
			int tangent 	= glGetAttribLocation(programId, "tangent"); //2
			int color 	= glGetAttribLocation(programId, "color"); //3
			int UV 		= glGetAttribLocation(programId, "UV"); //4

			/* Binded all textures */
			string str = "texture";
			for (uint i = 0; i < textures.size(); i++) {
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, textures[i].id);
				glUniform1i(glGetUniformLocation(programId, (str + (char)(i + '0')).c_str()), i);
			}

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IND);

			/* Binded Vertex data */
			glEnableVertexAttribArray(vertex); 	glVertexAttribPointer(vertex, 	3, GL_FLOAT, GL_FALSE, sizeof(vertexData), 0);
			glEnableVertexAttribArray(normal); 	glVertexAttribPointer(normal, 	3, GL_FLOAT, GL_FALSE, sizeof(vertexData), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(tangent); 	glVertexAttribPointer(tangent, 	3, GL_FLOAT, GL_FALSE, sizeof(vertexData), (void*)(6 * sizeof(float)));
			glEnableVertexAttribArray(color); 	glVertexAttribPointer(color, 	3, GL_FLOAT, GL_FALSE, sizeof(vertexData), (void*)(9 * sizeof(float)));
			glEnableVertexAttribArray(UV); 		glVertexAttribPointer(UV, 	2, GL_FLOAT, GL_FALSE, sizeof(vertexData), (void*)(12 *sizeof(float)));

			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

			glDisableVertexAttribArray(vertex);
			glDisableVertexAttribArray(normal);
			glDisableVertexAttribArray(tangent);
			glDisableVertexAttribArray(color);
			glDisableVertexAttribArray(UV);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
};

#endif
