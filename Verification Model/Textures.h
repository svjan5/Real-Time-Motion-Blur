
class Textures{
public:
        GLuint skybox, boxBackground, boxMain;

        Textures(){
                allocateSkyBoxTexture();
                allocateboxTexture();
        }

        void allocateSkyBoxTexture(){
                vector<const GLchar*> faces;
                faces.push_back("textures/skyboxes/grid/right.png");
                faces.push_back("textures/skyboxes/grid/left.png");
                faces.push_back("textures/skyboxes/grid/top.png");
                faces.push_back("textures/skyboxes/grid/bottom.png");
                faces.push_back("textures/skyboxes/grid/back.png");
                faces.push_back("textures/skyboxes/grid/front.png");
                skybox = loadCubemap(faces);  
        }

        void allocateboxTexture(){
                vector<const GLchar*> faces;
                
                faces.push_back("textures/box/right.jpg");
                faces.push_back("textures/box/left.jpg");
                faces.push_back("textures/box/top.jpg");
                faces.push_back("textures/box/bottom.jpg");
                faces.push_back("textures/box/back.jpg");
                faces.push_back("textures/box/front.jpg");
                boxBackground = loadCubemap(faces);

                faces.clear();
                faces.push_back("textures/box/mainbox1.png");
                faces.push_back("textures/box/mainbox1.png");
                faces.push_back("textures/box/mainbox1.png");
                faces.push_back("textures/box/mainbox1.png");
                faces.push_back("textures/box/mainbox1.png");
                faces.push_back("textures/box/mainbox1.png");
                boxMain = loadCubemap(faces);
        }

private:
        GLuint loadCubemap(vector<const GLchar*> faces) {
                GLuint textureID;
                glGenTextures(1, &textureID);
                glActiveTexture(GL_TEXTURE0);

                int width, height;
                unsigned char* image;

                glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
                for (GLuint i = 0; i < faces.size(); i++)
                {
                        image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
                        glTexImage2D(
                                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                                GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image
                        );
                }
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

                return textureID;
        }

};