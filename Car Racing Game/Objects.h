
class Object{
public:
        GLuint Vertices, Indices, TexCoords, Normals, numVertices, numTriangles;
        void allocate();
};

class Sphere: public Object{
public:
        int stacks, slices;

        Sphere(int stacks, int slices){
                this->stacks = stacks;
                this->slices = slices;
                allocate();
        }

        void allocate() {
                using namespace glm;
                using namespace std;

                float radius = 1.0;
                const float pi = 3.1415926535897932384626433832795f;
                const float _2pi = 2.0f * pi;

                vector<vec3> positions;
                vector<vec2> textureCoords;

                for ( int i = 0; i <= stacks; ++i ) {
                        // V texture coordinate.
                        float V = i / (float)stacks;
                        float phi = V * pi;

                        for ( int j = 0; j <= slices; ++j ) {
                                // U texture coordinate.
                                float U = j / (float)slices;
                                float theta = U * _2pi;

                                float X = cos(theta) * sin(phi);
                                float Y = cos(phi);
                                float Z = sin(theta) * sin(phi);

                                positions.push_back( vec3( X, Y, Z) * radius );
                                textureCoords.push_back( vec2(U, V) );
                        }
                }

                // Now generate the index buffer
                vector<GLuint> indices;

                for ( int i = 0; i < slices * stacks + slices; ++i ) {
                        indices.push_back( i );
                        indices.push_back( i + slices + 1  );
                        indices.push_back( i + slices );

                        indices.push_back( i + slices + 1  );
                        indices.push_back( i );
                        indices.push_back( i + 1 );
                }

                numVertices = positions.size();
                numTriangles = indices.size() / 3;


                glGenBuffers(1, &Vertices);
                glBindBuffer(GL_ARRAY_BUFFER, Vertices);
                glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(vec3), &positions[0], GL_STATIC_DRAW);

                glGenBuffers(1, &Indices);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Indices);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, numTriangles * 3 *  sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

                glGenBuffers(1, &TexCoords);
                glBindBuffer(GL_ARRAY_BUFFER, TexCoords);
                glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(vec2), &textureCoords[0], GL_STATIC_DRAW);
        }
};

class Cube: public Object{
public:
        Cube(){
                allocate();
        }

        void allocate(){
                GLfloat cubeVertices[] = {
                        -1.0f,  1.0f, -1.0f,    -1.0f, -1.0f, -1.0f,    1.0f, -1.0f, -1.0f,     1.0f, -1.0f, -1.0f,     1.0f,  1.0f, -1.0f,     -1.0f,  1.0f, -1.0f,
                        -1.0f, -1.0f,  1.0f,    -1.0f, -1.0f, -1.0f,    -1.0f,  1.0f, -1.0f,    -1.0f,  1.0f, -1.0f,    -1.0f,  1.0f,  1.0f,    -1.0f, -1.0f,  1.0f,
                        1.0f, -1.0f, -1.0f,     1.0f, -1.0f,  1.0f,     1.0f,  1.0f,  1.0f,     1.0f,  1.0f,  1.0f,     1.0f,  1.0f, -1.0f,     1.0f, -1.0f, -1.0f,
                        -1.0f, -1.0f,  1.0f,    -1.0f,  1.0f,  1.0f,    1.0f,  1.0f,  1.0f,     1.0f,  1.0f,  1.0f,     1.0f, -1.0f,  1.0f,     -1.0f, -1.0f,  1.0f,      
                        -1.0f,  1.0f, -1.0f,    1.0f,  1.0f, -1.0f,     1.0f,  1.0f,  1.0f,     1.0f,  1.0f,  1.0f,     -1.0f,  1.0f,  1.0f,    -1.0f,  1.0f, -1.0f,
                        -1.0f, -1.0f, -1.0f,    -1.0f, -1.0f,  1.0f,    1.0f, -1.0f, -1.0f,     1.0f, -1.0f, -1.0f,     -1.0f, -1.0f,  1.0f,    1.0f, -1.0f,  1.0f
                };

                GLfloat cubeNormals[] = {
                        0.0f, 0.0f, -1.0f,      0.0f, 0.0f, -1.0f,      0.0f, 0.0f, -1.0f,      0.0f, 0.0f, -1.0f,      0.0f, 0.0f, -1.0f,      0.0f, 0.0f, -1.0f,
                        1.0f, 0.0f, 0.0f,       -1.0f, 0.0f, 0.0f,     -1.0f, 0.0f, 0.0f,       -1.0f, 0.0f, 0.0f,      -1.0f, 0.0f, 0.0f,      -1.0f, 0.0f, 0.0f,
                        1.0f, 0.0f, 0.0f,       1.0f, 0.0f, 0.0f,       1.0f, 0.0f, 0.0f,       1.0f, 0.0f, 0.0f,       1.0f, 0.0f, 0.0f,       1.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 1.0f,       0.0f, 0.0f, 1.0f,       0.0f, 0.0f, 1.0f,       0.0f, 0.0f, 1.0f,       0.0f, 0.0f, 1.0f,       0.0f, 0.0f, 1.0f,
                        0.0f, 1.0f, 0.0f,       0.0f, 1.0f, 0.0f,       0.0f, 1.0f, 0.0f,       0.0f, 1.0f, 0.0f,       0.0f, 1.0f, 0.0f,       0.0f, 1.0f, 0.0f,
                        0.0f, -1.0f, 0.0f,      0.0f, -1.0f, 0.0f,      0.0f, -1.0f, 0.0f,      0.0f, -1.0f, 0.0f,      0.0f, -1.0f, 0.0f,      0.0f, -1.0f, 0.0f 
                };

                numVertices = sizeof(cubeVertices)/sizeof(GLfloat);
                numTriangles = numVertices / 3;

                glGenBuffers(1, &Vertices);
                glBindBuffer(GL_ARRAY_BUFFER, Vertices);
                glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(GLfloat), cubeVertices, GL_STATIC_DRAW);

                glGenBuffers(1, &Normals);
                glBindBuffer(GL_ARRAY_BUFFER, Normals);
                glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(GLfloat), cubeNormals, GL_STATIC_DRAW);                
        }

        void draw(){
                glEnableVertexAttribArray(0);
                glBindBuffer(GL_ARRAY_BUFFER, Vertices);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

                glEnableVertexAttribArray(1);
                glBindBuffer(GL_ARRAY_BUFFER, Normals);
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

                glDrawArrays(GL_TRIANGLES, 0, numTriangles);
                glDisableVertexAttribArray(0);
                glDisableVertexAttribArray(1);
        }
};

class Quad: public Object{
public:
        Quad(){ 
                allocate(); 
        }

        void allocate(){
                const GLfloat quadVertices[] = {
                        -1.0f, -1.0f, 0.0f,
                        1.0f, -1.0f, 0.0f,
                        -1.0f,  1.0f, 0.0f,
                        -1.0f,  1.0f, 0.0f,
                        1.0f, -1.0f, 0.0f,
                        1.0f,  1.0f, 0.0f,
                };

                numVertices = sizeof(quadVertices) / sizeof(GLfloat);
                numTriangles = numVertices / 3;
                glGenBuffers(1, &Vertices);
                glBindBuffer(GL_ARRAY_BUFFER, Vertices);
                glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(GLfloat), quadVertices, GL_STATIC_DRAW);
        }

        void draw(){
                glEnableVertexAttribArray(0);   
                glBindBuffer(GL_ARRAY_BUFFER, Vertices);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
                glDrawArrays(GL_TRIANGLES, 0, numTriangles);
                glDisableVertexAttribArray(0);
        }
};