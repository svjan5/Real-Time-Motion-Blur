
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <glm/glm.hpp>

#define ToRadian(x) (float)(((x) * M_PI / 180.0f))
#define ToDegree(x) (float)(((x) * 180.0f / M_PI))

using namespace std;
using namespace glm;

void ViewTextures();

typedef unsigned long long LL;

class TrackBall{
public:
        bool toPan = false, toRotate = false;
        float zoom = 5.0f, ballRadius;
        mat4 Rotate = mat4(1.0), Pan = mat4(1.0f);
        int prev_x, prev_y, curr_x, curr_y;

        TrackBall(int width, int height){
                ballRadius = std::min((width / 2.0), (height / 2.0));
        }

        void update(){
                if(toRotate) calRotation();
                if(toPan)    calPan();
        }

        void calRotation(){
                vec3 v1, v2;

                v1 = posVector(ballRadius, prev_x, prev_y);
                v2 = posVector(ballRadius, curr_x, curr_y);

                float rotationAngle = ToDegree(acos(dot(normalize(v1),normalize(v2))));
                if(!isnan(rotationAngle) && rotationAngle < 20){
                        vec3 axis = normalize(cross(v2, v1));
                        vec4 axis1 = vec4(axis, 1.0f) * Rotate;
                        axis = vec3(axis1);
                        Rotate = rotate(Rotate, rotationAngle, axis);
                }

                toRotate = false;
        }

        void calPan(){
                float dx = (curr_x - prev_x)/200.0;
                float dy = (curr_y - prev_y)/200.0;

                vec3 move = vec3(dx, dy, 0);
                Pan = translate(Pan, move);
                toPan = false;
        }


private:
        vec3 posVector(float ballRadius, int x, int y){
                float d = x * x + y * y; float radiusSquared = ballRadius * ballRadius;
                if (d > radiusSquared) return normalize(vec3((float)x, (float)y, 0 ));
                else return normalize(vec3((float)x, (float)y, sqrt(radiusSquared - d)));
        }
};