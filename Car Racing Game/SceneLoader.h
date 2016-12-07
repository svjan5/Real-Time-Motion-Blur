#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"

#include <glm/glm.hpp>

using namespace std;
using namespace glm;

typedef unsigned int uint;

class meshLoader {
private:
	vector<Mesh*> meshes;
	void recursiveProcess(aiNode* node, const aiScene* scene) {
		//process
		for (uint i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			processMesh(mesh, scene);
		}

		//recursion
		for (uint i = 0; i < node->mNumChildren; i++) {
			recursiveProcess(node->mChildren[i], scene);
		}
	}

	void processMesh(aiMesh* mesh, const aiScene* scene) {
			
		vector<vertexData> data;
		vector<uint> indices;
		vector<textureData> textures;
		aiColor4D col;
		aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
		aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &col);
		vec3 defaultColor(col.r, col.g, col.b);

		/* Get Vertex Data */
		for (uint i = 0; i < mesh->mNumVertices; i++) {
			vertexData tmp;
			vec3 tmpVec;

			//position
			tmpVec.x = mesh->mVertices[i].x;
			tmpVec.y = mesh->mVertices[i].y;
			tmpVec.z = mesh->mVertices[i].z;
			tmp.position = tmpVec;

			//normals
			tmpVec.x = mesh->mNormals[i].x;
			tmpVec.y = mesh->mNormals[i].y;
			tmpVec.z = mesh->mNormals[i].z;
			tmp.normal = tmpVec;


			//tangent
			if (mesh->mTangents) {
				tmpVec.x = mesh->mTangents[i].x;
				tmpVec.y = mesh->mTangents[i].y;
				tmpVec.z = mesh->mTangents[i].z;
			} 
			else {
				tmpVec.x = 1.0;
				tmpVec.y = tmpVec.z = 0;
			}
			tmp.tangent = tmpVec;


			//colors
			if (mesh->mColors[0]) {
				//!= material color
				tmpVec.x = mesh->mColors[0][i].r;
				tmpVec.y = mesh->mColors[0][i].g;
				tmpVec.z = mesh->mColors[0][i].b;
			} 
			else {
				tmpVec = defaultColor;
			}
			tmp.color = tmpVec;

			//color
			if (mesh->mTextureCoords[0]) {
				tmpVec.x = mesh->mTextureCoords[0][i].x;
				tmpVec.y = mesh->mTextureCoords[0][i].y;
			} 
			else {
				tmpVec.x = tmpVec.y = tmpVec.z = 0.0;
			}
			tmp.U = tmpVec.x;
			tmp.V = tmpVec.y;
			data.push_back(tmp);
		}

		/* Get Indices */
		for (uint i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (uint j = 0; j < face.mNumIndices; j++) /*0..2 */{
				indices.push_back(face.mIndices[j]);
			}
		}

		/* Get Textures */
		for (uint i = 0; i < mat->GetTextureCount(aiTextureType_DIFFUSE); i++) {
			aiString str;
			mat->GetTexture(aiTextureType_DIFFUSE, i, &str);
			textureData tmp;
			tmp.id = loadTexture(str.C_Str());
			tmp.type = 0;
			textures.push_back(tmp);
		}
		meshes.push_back(new Mesh(&data, &indices, &textures));
	}
		
	unsigned int loadTexture(const char* filename) {
		GLuint textureID;

		textureID = SOIL_load_OGL_texture( "textures/earth.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS );
	        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	        glBindTexture( GL_TEXTURE_2D, 0 );
                return textureID;
	}

public:
	meshLoader(const char* filename) {
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filename, aiProcess_GenSmoothNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_FlipUVs);
		if (scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {cout << "The file wasn't successfuly opened " << filename << endl; return; }
		recursiveProcess(scene->mRootNode, scene);
	}

	~meshLoader() {
		for (uint i = 0; i < meshes.size(); i++) delete meshes[i]; 
	}

	void draw(uint programId) {
		for (uint i = 0; i < meshes.size(); i++) meshes[i]->draw(programId); 
	}

	vector<Mesh*>& getMeshes() {return meshes;}
};