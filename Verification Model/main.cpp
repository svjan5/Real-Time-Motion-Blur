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

#include "SOIL.h"
#include "Utils.h"
#include "Shaders.h"
#include "Buffers.h"
#include "Objects.h"
#include "Textures.h"

#define GLM_FORCE_RADIANS
#define RANDOM_TEXTURE_SIZE (256U)

#define ZOOM_STEP 0.01f
#define CenterX(x) (-(x - (WIDTH / 2)))
#define CenterY(y) ((HEIGHT / 2) - y)

using namespace glm;

enum Display{MAIN, COLOR, DEPTH, VELOCITY, TILE_MAX, NEIGHBOR_MAX} display = MAIN;
enum Control{ROTATE, PAN, NONE} control=NONE;

int mK = 20, mS = 40;

int WIDTH = 1024, HEIGHT = 768;
float fps = 0, cameraNear =  0.1f, cameraFar = 60.0f;
float speed = -15;
float displace = 0.0f, change = 0.01f;

// mat4 Model, Projection, View, scaleMat, transMat, MVP, PrevView, PrevModel;

void updateTitle();
Shaders *shaders;
Buffers *buffers;
Textures *textures;
Sphere *sphere; Cube *cube; Quad *quad;
TrackBall *trackball;

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


void computeFPS() {
        static LL lastFrameTime = 0;
        static int frameCount = 0;

        frameCount++;

        struct timeval tv;
        gettimeofday(&tv, NULL);

        LL currentTime = (LL)(tv.tv_sec) * 1000 + (LL)(tv.tv_usec) / 1000;

        if (currentTime - lastFrameTime > 1000) {
                fps = frameCount * 1000.0 / (currentTime - lastFrameTime);
                lastFrameTime = currentTime;
                frameCount = 0;
        }
        updateTitle();
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
        mat4 currMVP, prevMVP, postTransMat;

        void draw(GLuint texID){
                model->pushMatrix();
                
                model->genMVP();
                currMVP = model->V * model->M;
                glUniformMatrix4fv(shaders->CZ->MVP,    1, GL_FALSE, &model->MVP[0][0]);
                glUniformMatrix4fv(shaders->CZ->Model,  1, GL_FALSE, &model->M[0][0]);
                glUniformMatrix4fv(shaders->CZ->View,   1, GL_FALSE, &model->V[0][0]);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_CUBE_MAP, texID);
                glUniform1i(shaders->CZ->TEX, 0);

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
                postTransMat = prevMVP * inverse(currMVP);
        }

} skybox, box1, box2, box3;




static void renderScene() {
        computeFPS();

        model->reset();
        model->P = perspective(25.0f, (float)WIDTH/(float)HEIGHT, cameraNear, cameraFar);
        model->V = lookAt( vec3(0,0,speed), vec3(0,0,0), vec3(0,1,0));  /*Lookat( position, lookat, upvector) */

        /* Trackball update */
        trackball->update();
        model->Scale(vec3(trackball->zoom));
        model->append(trackball->Rotate);
        model->append(trackball->Pan);

        /****************Constructing Objects***************/
        bindFrameBuffer(buffers->FBO_CZ, WIDTH, HEIGHT);
        shaders->CZ->init();

        model->pushMatrix();
        model->Scale(vec3(2.0));
                skybox.draw(textures->skybox);
        model->popMatrix();

        model->pushMatrix();
        model->Scale(vec3(0.05));
                model->pushMatrix();
                model->Translate(vec3(10*sin(displace), 0,0));
                        box1.draw(textures->boxMain);
                model->popMatrix();

                model->pushMatrix();
                model->Translate(vec3(-3.0f, 0.0f, -5.0f));
                        box2.draw(textures->boxBackground);
                model->popMatrix();

                model->pushMatrix();
                model->Translate(vec3(3.0f, 0.0f, 5.0f));
                        box3.draw(textures->boxBackground);
                model->popMatrix();
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
                        box3.sendPosition();
                        box1.sendPosition();
                        box2.sendPosition();
                glDisable(GL_BLEND);
        shaders->VL->Disable();

        /*********************Tile Max**********************/
        bindFrameBuffer(buffers->FBO_TM, WIDTH/mK, HEIGHT/mK);
        shaders->TM->init();
                glUniform1i(shaders->TM->K, mK);
                attachTexture(shaders->TM->TEX, GL_TEXTURE0, buffers->TEX_VL);
                quad->draw();
        shaders->TM->Disable();
        
        /*********************Neighbor Max**********************/
        bindFrameBuffer(buffers->FBO_NM, WIDTH/mK, HEIGHT/mK);
        shaders->NM->init();
                attachTexture(shaders->NM->TEX, GL_TEXTURE0, buffers->TEX_TM);
                quad->draw();
        shaders->NM->Disable();

        /*********************Gather All**********************/
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
                case 'r': case 'R':     trackball->Rotate = mat4(1.0f); glutPostRedisplay(); break;
                case 'Q': case 'q':     buffers->clear();               exit(0);             break;
                case ' ':               displace = 0.0f; change = 0.0f; glutPostRedisplay(); break;
                case 'p': case 'P':     sleep(1);                                            break;
                case 'w': case 'W':     speed += 0.1f;                  glutPostRedisplay(); break;
                case 's': case 'S':     speed -= 0.1f;                  glutPostRedisplay(); break;
        }
}

void SpecialKeyCB(int key, int x, int y){
        switch(key) {
                case GLUT_KEY_LEFT:
                        display = (Display)((int(display) - 1 + 6)%6);
                        // if(display == NEIGHBOR_MAX) change /= 5;
                        updateTitle(); glutPostRedisplay(); break;
                case GLUT_KEY_RIGHT:
                        display = (Display)((int(display) + 1 + 6)%6);
                        // if(display == COLOR) change /= 5; 
                        updateTitle(); glutPostRedisplay(); break;
                case GLUT_KEY_UP:       change += .001;         break;
                case GLUT_KEY_DOWN:     change -= .001;         break;
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

void swingBall(){
        displace += change;
        glutPostRedisplay();
}

void InitCallbacks() {
        glutDisplayFunc(renderScene);
        // glutReshapeFunc(adjustAspectRatio);
        glutIdleFunc(swingBall);
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