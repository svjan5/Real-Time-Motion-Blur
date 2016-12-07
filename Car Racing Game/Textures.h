
class Textures{
public:
        GLuint skybox, boxBackground, boxMain1, boxMain2, road, ground;

        Textures(){
                allocateTextures();



        }

        void allocateTextures(){
                vector<const GLchar*> faces;
                faces.push_back("textures/skyboxes/sky/right.png");
                faces.push_back("textures/skyboxes/sky/left.png");
                faces.push_back("textures/skyboxes/sky/top.png");
                faces.push_back("textures/skyboxes/sky/bottom2.png");
                faces.push_back("textures/skyboxes/sky/front.png");
                faces.push_back("textures/skyboxes/sky/front.png");
                skybox = loadCubemap(faces);  

                // faces.clear();                
                // faces.push_back("textures/box/right.jpg");
                // faces.push_back("textures/box/left.jpg");
                // faces.push_back("textures/box/top.jpg");
                // faces.push_back("textures/box/bottom.jpg");
                // faces.push_back("textures/box/back.jpg");
                // faces.push_back("textures/box/front.jpg");
                // boxBackground = loadCubemap(faces);

                faces.clear();
                faces.push_back("textures/box/mainbox1.png");
                faces.push_back("textures/box/mainbox1.png");
                faces.push_back("textures/box/mainbox1.png");
                faces.push_back("textures/box/mainbox1.png");
                faces.push_back("textures/box/mainbox1.png");
                faces.push_back("textures/box/mainbox1.png");
                boxMain2 = loadCubemap(faces);

                faces.clear();
                faces.push_back("textures/box/mainbox2.png");
                faces.push_back("textures/box/mainbox2.png");
                faces.push_back("textures/box/mainbox2.png");
                faces.push_back("textures/box/mainbox2.png");
                faces.push_back("textures/box/mainbox2.png");
                faces.push_back("textures/box/mainbox2.png");
                boxMain1 = loadCubemap(faces);

                // faces.clear();
                // faces.push_back("textures/Road/Road3.png");
                // faces.push_back("textures/Road/Road3.png");
                // faces.push_back("textures/Road/Road3.png");
                // faces.push_back("textures/Road/Road3.png");
                // faces.push_back("textures/Road/Road3.png");
                // faces.push_back("textures/Road/Road3.png");
                // road = loadCubemap(faces);

                ground = SOIL_load_OGL_texture( "textures/Road/bottom3.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS );
                glBindTexture(GL_TEXTURE_2D, ground);
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
                glBindTexture( GL_TEXTURE_2D, 0 );

                road = SOIL_load_OGL_texture( "textures/Road/Road3.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS );
                glBindTexture(GL_TEXTURE_2D, road);
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
                glBindTexture( GL_TEXTURE_2D, 0 );


        }

private:
        GLuint loadCubemap(vector<const GLchar*> faces) {
                GLuint textureID;
                glGenTextures(1, &textureID);
                glActiveTexture(GL_TEXTURE0);

                int width, height;
                unsigned char* image;

                for (GLuint i = 0; i < faces.size(); i++)
                {
                        
                        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
                        image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
                        glTexImage2D(
                                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                                GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image
                        );
                }
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
                glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

                return textureID;
        }

};