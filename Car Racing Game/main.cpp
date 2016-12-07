#include <bits/stdc++.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <sys/time.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <sys/time.h>
#include <unistd.h>

#include <SFML/Audio.hpp>


#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "SOIL.h"
#include "Utils.h"
#include "Shaders.h"
#include "Buffers.h"
#include "Objects.h"
#include "Textures.h"
#include "Mesh.h"
#include "SceneLoader.h"


#define GLM_FORCE_RADIANS
#define RANDOM_TEXTURE_SIZE (256U)

#define ZOOM_STEP 0.01f
#define CenterX(x) (-(x - (WIDTH / 2)))
#define CenterY(y) ((HEIGHT / 2) - y)

#define CarType 1
using namespace glm;

enum Display{MAIN, COLOR, DEPTH, VELOCITY, TILE_MAX, NEIGHBOR_MAX} display = MAIN;
enum Control{ROTATE, PAN, NONE} control=NONE;
enum ColorMode{SUPPLY_COLOR, SUPPLY_TEX3D, SUPPLY_TEX2D};
enum Mode{BACK, SIDE, STAY} mode = BACK;

int mK = 20, mS = 40;

#define BEGIN_LOC -120.0f
#define DEST_LOC 175.0f

int WIDTH = 1024, HEIGHT = 768;
float fps = 0, cameraNear =  0.1f, cameraFar = 1000.0f;
float camPos = -149.0;
float speed = 0.10f;
float displace = BEGIN_LOC, volume = 0;
meshLoader* scene;

// mat4 Model, Projection, View, scaleMat, transMat, MVP, PrevView, PrevModel;

Shaders *shaders;
Buffers *buffers;
Textures *textures;
Sphere *sphere; Cube *cube; Quad *quad;
TrackBall *trackball;
sf::Music sndStartEngine, sndRunning;
/* Flags */

class Model{
public:
        mat4 M, V, P, MVP;
        stack<mat4> st;

        Model(){ reset(); }
        void Rotate(float angle, vec3 axis){    M = rotate(M, angle, axis); }
        void Translate(vec3 move){              M = translate(M, move); }
        void Scale(vec3 times){                 M = scale(M, times); }
        void append(mat4 &m2){                  M = m2 * M; }
        void genMVP(){                          MVP = P * V * M;}
        void reset(){                           M = V = P = mat4(1.0f);}
        void pushMatrix(){                      st.push(M); }
        void popMatrix(){
                if(!st.empty()) M = st.top(), st.pop();
                else printf("Stack is empty!!\n");
        }

} *model;

float FPS;
void computeFPS() {
        static LL lastFrameTime = 0;
        static int frameCount = 0;

        frameCount++;

        struct timeval tv;
        gettimeofday(&tv, NULL);

        LL currentTime = (LL)(tv.tv_sec) * 1000 + (LL)(tv.tv_usec) / 1000;

        // fps = frameCount * 1000.0 / (currentTime - lastFrameTime);
        // lastFrameTime = currentTime;
        // frameCount = 0;

        if (currentTime - lastFrameTime > 1000) {
                fps = frameCount * 1000.0 / (currentTime - lastFrameTime); 
                lastFrameTime = currentTime;
                frameCount = 0;
        }
}


void bindFrameBuffer(GLuint FB, int w, int h){
        glBindFramebuffer(GL_FRAMEBUFFER, FB);
        glViewport(0, 0, w, h); 
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void attachTexture(GLuint destID, GLenum texture, GLuint texID){
        glActiveTexture(texture); 
        glBindTexture(GL_TEXTURE_2D, texID);
        glUniform1i(destID, int(texture - GL_TEXTURE0));
}


class DrawCube{
public:
        bool isFirstFrame = true;
        mat4 currMVP, prevMVP;

        void draw(GLuint texID){
                model->pushMatrix();
                model->genMVP();
                        currMVP = model->V * model->M;
                        glUniformMatrix4fv(shaders->CZ->MVP,    1, GL_FALSE, &model->MVP[0][0]);
                        glUniformMatrix4fv(shaders->CZ->Model,  1, GL_FALSE, &model->M[0][0]);
                        glUniformMatrix4fv(shaders->CZ->View,   1, GL_FALSE, &model->V[0][0]);

                        glUniform1i(shaders->CZ->colorMode, SUPPLY_TEX3D);
                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_CUBE_MAP, texID);
                        glUniform1i(shaders->CZ->TEX3D, 0);

                        cube->draw();

                        if(isFirstFrame){
                                prevMVP = currMVP;
                                isFirstFrame = false;
                        }

                model->popMatrix();
        }

        void sendPosition(){
                glUniformMatrix4fv(shaders->VL->currMVP, 1, GL_FALSE, &currMVP[0][0]);
                glUniformMatrix4fv(shaders->VL->prevMVP, 1, GL_FALSE, &prevMVP[0][0]);
                glUniformMatrix4fv(shaders->VL->proj, 1, GL_FALSE, &model->P[0][0]);
                cube->draw();

                prevMVP = currMVP;
        }

} skybox, box1, box2, box3, box4, box5;


class DrawQuad{
public:
        bool isFirstFrame = true;
        mat4 currMVP, prevMVP;

        void draw(GLuint texID){
                model->pushMatrix();
                model->genMVP();
                        currMVP = model->V * model->M;
                        glUniformMatrix4fv(shaders->CZ->MVP,    1, GL_FALSE, &model->MVP[0][0]);
                        glUniformMatrix4fv(shaders->CZ->Model,  1, GL_FALSE, &model->M[0][0]);
                        glUniformMatrix4fv(shaders->CZ->View,   1, GL_FALSE, &model->V[0][0]);

                        glUniform1i(shaders->CZ->colorMode, SUPPLY_TEX2D);
                        attachTexture(shaders->CZ->TEX2D,  GL_TEXTURE1, texID);
                        quad->draw();

                        if(isFirstFrame){
                                prevMVP = currMVP;
                                isFirstFrame = false;
                        }

                model->popMatrix();
        }

        void sendPosition(){
                glUniformMatrix4fv(shaders->VL->currMVP, 1, GL_FALSE, &currMVP[0][0]);
                glUniformMatrix4fv(shaders->VL->prevMVP, 1, GL_FALSE, &prevMVP[0][0]);
                glUniformMatrix4fv(shaders->VL->proj, 1, GL_FALSE, &model->P[0][0]);
                quad->draw();

                prevMVP = currMVP;
        }

} road, ground;


class DrawCar{
public:
        bool isFirstFrame = true;
        mat4 currMVP, prevMVP;

        void draw(){
                model->pushMatrix();
                model->genMVP();
                        currMVP = model->V * model->M;
                        glUniformMatrix4fv(shaders->CZ->MVP,    1, GL_FALSE, &model->MVP[0][0]);
                        glUniformMatrix4fv(shaders->CZ->Model,  1, GL_FALSE, &model->M[0][0]);
                        glUniformMatrix4fv(shaders->CZ->View,   1, GL_FALSE, &model->V[0][0]);
                        glUniform1i(shaders->CZ->colorMode, SUPPLY_COLOR);

                        scene->draw(shaders->CZ->pID);

                        if(isFirstFrame){
                                prevMVP = currMVP;
                                isFirstFrame = false;
                        }

                model->popMatrix();
        }

        void sendPosition(){
                glUniformMatrix4fv(shaders->VL->currMVP, 1, GL_FALSE, &currMVP[0][0]);
                glUniformMatrix4fv(shaders->VL->prevMVP, 1, GL_FALSE, &prevMVP[0][0]);
                glUniformMatrix4fv(shaders->VL->proj, 1, GL_FALSE, &model->P[0][0]);
                scene->draw(shaders->CZ->pID);

                prevMVP = currMVP;
        }
} car;

void updateTitle();

static void renderScene() {
        computeFPS();
        updateTitle();

        model->reset();
        model->P = perspective(25.0f, (float)WIDTH/(float)HEIGHT, cameraNear, cameraFar);
        model->V = lookAt( vec3(0,2,-150), vec3(0,0,0), vec3(0,1,0));  /*Lookat( position, lookat, upvector) */

        /* Trackball update */
        trackball->update();
        model->Scale(vec3(trackball->zoom));
        model->append(trackball->Rotate);
        model->append(trackball->Pan);

        /****************Constructing Objects***************/
        bindFrameBuffer(buffers->FBO_CZ, WIDTH, HEIGHT);
        shaders->CZ->init();

                switch(mode){
                        case SIDE:
                                model->Rotate(90, vec3(0,1,0));
                                model->Translate(vec3(26.5, 0, 27.0));
                                break;
                        case STAY:
                                model->Rotate(90, vec3(0,1,0));
                                model->Translate(vec3(26.5,0, -149.0));
                                break;
                        default: break;
                }

                model->pushMatrix();

                model->Scale(vec3(4.0, 4.0, 150.0));
                model->Translate(vec3(0,1,0));
                        glUniform1i(shaders->CZ->colorMode, SUPPLY_TEX3D);
                        skybox.draw(textures->skybox);
                model->popMatrix();


                model->pushMatrix();
                switch(mode){ case BACK: case SIDE: model->Translate(vec3(0,0,-displace)); default: break;}
                
                        model->pushMatrix();
                        model->Translate(vec3(-2,0.5,0));
                        model->Scale(vec3(0.5));
                                box1.draw(textures->boxMain2);
                        model->popMatrix();

                        model->pushMatrix();
                        model->Translate(vec3(2,0.5,40));
                        model->Scale(vec3(0.5));
                                box2.draw(textures->boxMain2);
                        model->popMatrix();

                        model->pushMatrix();
                        model->Translate(vec3(2,0.5,-40));
                        model->Scale(vec3(0.5));
                                box3.draw(textures->boxMain1);
                        model->popMatrix();

                        model->pushMatrix();
                        model->Translate(vec3(-2,0.5,80));
                        model->Scale(vec3(0.5));
                                box4.draw(textures->boxMain1);
                        model->popMatrix();

                        model->pushMatrix();
                        model->Translate(vec3(0,0.01,0));
                        model->Scale(vec3(50, 1, 250));
                        model->Rotate(90, vec3(1,0,0));
                                ground.draw(textures->ground);
                        model->popMatrix();

                        model->pushMatrix();
                        model->Translate(vec3(0,0.015,0));
                        model->Scale(vec3(0.60, 1, 250));
                        model->Rotate(90, vec3(1,0,0));
                                road.draw(textures->road);
                        model->popMatrix();

                model->popMatrix();
                
                model->pushMatrix();
                switch(mode){case STAY: model->Translate(vec3(0,0,displace)); default: break;}

                model->Scale(vec3(0.3));
                // printf("%f\n", displace);

                switch(CarType){
                        case 0: /* For jeep */
                                model->Translate(vec3(0,-0.5,-90));
                                break;
                        case 1: /* For lemborgini */
                                model->Rotate(90.0, vec3(0,1,0));
                                model->Translate(vec3(90,-0.20,0));
                        default: break;
                }

                        car.draw();
                model->popMatrix();

        shaders->CZ->Disable();

        if(display == COLOR){
                ViewTextures();
                return;
        }

        /****************Velocity Calculation***************/
        bindFrameBuffer(buffers->FBO_VL, WIDTH, HEIGHT);
        shaders->VL->init();
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                        // skybox.sendPosition();
                        box1.sendPosition();
                        box2.sendPosition();
                        box3.sendPosition();
                        box4.sendPosition();
                        road.sendPosition();
                        ground.sendPosition();
                        car.sendPosition();

                glDisable(GL_BLEND);
        shaders->VL->Disable();

        // /*********************Tile Max**********************/
        bindFrameBuffer(buffers->FBO_TM, WIDTH/mK, HEIGHT/mK);
        shaders->TM->init();
                glUniform1i(shaders->TM->K, mK);
                attachTexture(shaders->TM->TEX, GL_TEXTURE0, buffers->TEX_VL);
                quad->draw();
        shaders->TM->Disable();
        
        // /*********************Neighbor Max**********************/
        bindFrameBuffer(buffers->FBO_NM, WIDTH/mK, HEIGHT/mK);
        shaders->NM->init();
                attachTexture(shaders->NM->TEX, GL_TEXTURE0, buffers->TEX_TM);
                quad->draw();
        shaders->NM->Disable();

        // /*********************Gather All**********************/
        bindFrameBuffer(buffers->FBO_GA, WIDTH, HEIGHT);
        shaders->GA->init();
                attachTexture(shaders->GA->TEX_C,  GL_TEXTURE0, buffers->TEX_C);
                attachTexture(shaders->GA->TEX_Z,  GL_TEXTURE1, buffers->TEX_Z);
                attachTexture(shaders->GA->TEX_VL, GL_TEXTURE2, buffers->TEX_VL);
                attachTexture(shaders->GA->TEX_NM, GL_TEXTURE3, buffers->TEX_NM);

                glUniform1f(shaders->GA->cameraFar, cameraFar);
                glUniform1f(shaders->GA->cameraNear, cameraNear);
                glUniform1i(shaders->GA->K, mK);
                glUniform1i(shaders->GA->S, mS);
                glUniform1f(shaders->GA->fps, fps);


                quad->draw();        
        shaders->GA->Disable();

        ViewTextures();
}

void ViewTextures(){

        shaders->View->init();
                bindFrameBuffer(0, WIDTH, HEIGHT);

                GLuint toBind = 0;
                switch(display){
                        case COLOR:             toBind = buffers->TEX_C;        break;
                        case DEPTH:             toBind = buffers->TEX_Z;        break;
                        case VELOCITY:          toBind = buffers->TEX_VL;       break;
                        case TILE_MAX:          toBind = buffers->TEX_TM;       break;
                        case NEIGHBOR_MAX:      toBind = buffers->TEX_NM;       break;
                        case MAIN:              toBind = buffers->TEX_GA;       break;
                }
                attachTexture(shaders->View->Tex, GL_TEXTURE0, toBind);
                glUniform1i(shaders->View->TexType, int(display));                
                glUniform1f(shaders->View->cameraFar, cameraFar);
                glUniform1f(shaders->View->cameraNear, cameraNear);
                glUniform1i(shaders->View->K, mK);

                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                        quad->draw();
                glDisable(GL_BLEND);

        shaders->View->Disable();

        glutSwapBuffers();
}

void init() {
        buffers = new Buffers(WIDTH, HEIGHT, mK);
        textures = new Textures();
        shaders = new Shaders();
        trackball = new TrackBall(WIDTH, HEIGHT);
        model = new Model();

        sphere = new Sphere(20, 20);
        cube = new Cube();
        quad = new Quad();
        switch(CarType){
                case 0:
                        scene = new meshLoader("textures/Car/jeep.obj");
                        break;
                case 1:
                        scene = new meshLoader("textures/Car/mycar.obj");
                        break;
                
        }

        
        if (!sndStartEngine.openFromFile("startEngine.ogg")) {printf("Error!!!\n"); }
        if (!sndRunning.openFromFile("Running.ogg")) {printf("Error!!!\n"); }



        sndStartEngine.setPitch(2);           // increase the pitch
        sndStartEngine.setVolume(50);         // reduce the volume
        sndStartEngine.setLoop(false);         // make it loop
        sndStartEngine.play();

        sndRunning.setPitch(2);
        sndRunning.setLoop(true);
        sndRunning.setVolume(volume);
        sndRunning.play();
}

void updateTitle(){
        char *title = (char*) malloc(sizeof(char) * 100);
        string viewMode;
        switch(display){
                case MAIN:              viewMode = "MAIN OUTPUT";       break;
                case COLOR:             viewMode = "COLOR OUTPUT";      break;
                case DEPTH:             viewMode = "DEPTH OUTPUT";      break;
                case VELOCITY:          viewMode = "VELOCITY OUTPUT";   break;
                case TILE_MAX:          viewMode = "TILE MAX OUTPUT";   break;
                case NEIGHBOR_MAX:      viewMode = "NEIGHBOR MAX OUTPUT";break;
        }
        sprintf(title, "%s | Parameters => K: %d, S: %d | FPS: %f", viewMode.c_str(), mK, mS, fps);
        glutSetWindowTitle(title);
}

void KeyboardCB(unsigned char key, int x, int y){
        switch(key){
                case 'r':       trackball->Rotate = mat4(1.0f);         glutPostRedisplay(); break;
                case 'q':       buffers->clear();                       exit(0);             break;
                case ' ':       displace = 0.0f; speed = 0.0f;          glutPostRedisplay(); break;
                case 'p':       sleep(1);                                                    break;
                case 'w':       camPos += 1.0f;                         glutPostRedisplay(); break;
                case 's':       camPos -= 1.0f;                         glutPostRedisplay(); break;
                case 'a':       mode = (Mode)((int(mode) - 1 + 3)%3);   glutPostRedisplay(); break;
                case 'd':       mode = (Mode)((int(mode) + 1 + 3)%3);   glutPostRedisplay(); break;
        }
}

void SpecialKeyCB(int key, int x, int y){
        switch(key) {
                case GLUT_KEY_LEFT:
                        display = (Display)((int(display) - 1 + 6)%6);
                        updateTitle(); glutPostRedisplay(); break;
                case GLUT_KEY_RIGHT:
                        display = (Display)((int(display) + 1 + 6)%6);
                        updateTitle(); glutPostRedisplay(); break;
                case GLUT_KEY_UP:       speed += .3;    volume += 0.6;  sndRunning.setVolume(volume);break;
                case GLUT_KEY_DOWN:     speed -= .3;    volume -= 0.6;  sndRunning.setVolume(volume);break;
                case GLUT_KEY_PAGE_UP:   mS += 1; updateTitle(); glutPostRedisplay(); break;
                case GLUT_KEY_PAGE_DOWN: mS -= 1; updateTitle(); glutPostRedisplay(); break;
                case GLUT_KEY_HOME:      mK += 1; updateTitle(); glutPostRedisplay(); break;
                case GLUT_KEY_END:       mK -= 1; updateTitle(); glutPostRedisplay(); break;
        }
}

void mouseMotion(int x, int y){
        trackball->prev_x = trackball->curr_x; 
        trackball->prev_y = trackball->curr_y;
        trackball->curr_x = CenterX(x);
        trackball->curr_y = CenterY(y);

        switch(control){
                case ROTATE:    if( abs(trackball->curr_x - trackball->prev_x) > .025 && abs(trackball->curr_y - trackball->prev_y) > .025) 
                                        trackball->toRotate = true;     break;
                case PAN:               trackball->toPan = true;        break;
                case NONE:      break;
        }
        glutPostRedisplay();    
}

void mouseCB(int button, int state, int x, int y) {
        switch(button){
                case 3: trackball->zoom += ZOOM_STEP; glutPostRedisplay(); break;
                case 4: trackball->zoom -= ZOOM_STEP; glutPostRedisplay(); break;
                case GLUT_LEFT_BUTTON:
                        if(state == GLUT_DOWN){
                                control = ROTATE;
                                trackball->curr_x = trackball->prev_x = CenterX(x);
                                trackball->curr_y = trackball->prev_y = CenterY(y);
                        } else{control = NONE;}
                        break;
                case GLUT_RIGHT_BUTTON:
                        if(state == GLUT_DOWN){
                                control = PAN;
                                trackball->curr_x = trackball->prev_x = CenterX(x);
                                trackball->curr_y = trackball->prev_y = CenterY(y);
                        } else{control = NONE;}
                        break;
        }
}

void moveCar(){
        if(displace > DEST_LOC){
                displace = BEGIN_LOC;
                switch(mode){case STAY: sleep(5.0);default: break;}
        }

        displace += speed;                        


        glutPostRedisplay();
}

void InitCallbacks() {
        glutDisplayFunc(renderScene);
        // glutReshapeFunc(adjustAspectRatio);
        glutIdleFunc(moveCar);
        glutMotionFunc(mouseMotion);
        glutKeyboardFunc(KeyboardCB);
        glutSpecialFunc(SpecialKeyCB);
        glutMouseFunc(mouseCB);
}

int main(int argc, char **argv) {

        glutInit(&argc, argv);
        glutInitWindowSize(WIDTH, HEIGHT);
        glutInitWindowPosition(100, 100);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
        glutCreateWindow("Final Project");

        InitCallbacks();
        updateTitle();


        GLenum res = glewInit(); if (res != GLEW_OK) {fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res)); return 1; }

        printf("GL version: %s\n", glGetString(GL_VERSION));
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        init();


        
        glEnable(GL_DEPTH_TEST);
        glutMainLoop();
        return 0;
}