
#define RANDOM_TEXTURE_SIZE (256U)

class Buffers {
public:
        GLuint  RBO_Normal, RBO_Small, quadVBO, FBO_CZ, FBO_VL, FBO_TM, FBO_NM, FBO_GA,
                TEX_C, TEX_Z, TEX_VL, TEX_TM, TEX_NM, TEX_GA;

        int WIDTH, HEIGHT, WIDTHbyK, HEIGHTbyK, K;

        Buffers(int WIDTH, int HEIGHT, int K) {
                this->WIDTH = WIDTH;
                this->HEIGHT = HEIGHT;
                this->K = (K != 0) ? K : 1;

                this->WIDTHbyK  = WIDTH  / K;
                this->HEIGHTbyK = HEIGHT / K;

                allocateTextures();
                allocateFrameBuffers();
                allocateRenderBuffers();
        }

        void allocateTextures() {
                /* COLOR */
                glGenTextures(1, &TEX_C);
                glBindTexture(GL_TEXTURE_2D, TEX_C);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
                glBindTexture(GL_TEXTURE_2D, 0);

                /* DEPTH */
                glGenTextures(1, &TEX_Z);
                glBindTexture(GL_TEXTURE_2D, TEX_Z);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri( GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE );
                glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
                glBindTexture(GL_TEXTURE_2D, 0);

                /* VELOCITY */
                glGenTextures(1, &TEX_VL);
                glBindTexture(GL_TEXTURE_2D, TEX_VL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, WIDTH, HEIGHT, 0, GL_RG, GL_FLOAT, NULL);
                glBindTexture(GL_TEXTURE_2D, 0);

                /* TILE MAX */
                glGenTextures(1, &TEX_TM);
                glBindTexture(GL_TEXTURE_2D, TEX_TM);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, WIDTHbyK, HEIGHTbyK, 0, GL_RG, GL_FLOAT, NULL);
                glBindTexture(GL_TEXTURE_2D, 0);

                /* NEIGHBOR MAX */
                glGenTextures(1, &TEX_NM);
                glBindTexture(GL_TEXTURE_2D, TEX_NM);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, WIDTHbyK, HEIGHTbyK, 0, GL_RG, GL_FLOAT, NULL);
                glBindTexture(GL_TEXTURE_2D, 0);

                /* GATHER ALL */
                glGenTextures(1, &TEX_GA);
                glBindTexture(GL_TEXTURE_2D, TEX_GA);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
                glBindTexture(GL_TEXTURE_2D, 0);

        }

        void allocateRenderBuffers() {
                glGenRenderbuffers(1, &RBO_Normal);
                glBindRenderbuffer(GL_RENDERBUFFER, RBO_Normal);
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, WIDTH, HEIGHT);

                glGenRenderbuffers(1, &RBO_Small);
                glBindRenderbuffer(GL_RENDERBUFFER, RBO_Small);
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, WIDTHbyK, HEIGHTbyK);
                glBindRenderbuffer(GL_RENDERBUFFER, 0);
        }

        void allocateFrameBuffers() {
                /* COLOR */
                glGenFramebuffers(1, &FBO_CZ);
                glBindFramebuffer(GL_FRAMEBUFFER, FBO_CZ);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TEX_C, 0);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, TEX_Z, 0);
                GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
                glDrawBuffers(1, DrawBuffers);


                /* VELOCITY */
                glGenFramebuffers(1, &FBO_VL);
                glBindFramebuffer(GL_FRAMEBUFFER, FBO_VL);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TEX_VL, 0);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO_Normal);
                glDrawBuffers(1, DrawBuffers);

                /* TILE MAX */
                glGenFramebuffers(1, &FBO_TM);
                glBindFramebuffer(GL_FRAMEBUFFER, FBO_TM);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TEX_TM, 0);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO_Small);
                glDrawBuffers(1, DrawBuffers);


                /* NEIGHBOR MAX */
                glGenFramebuffers(1, &FBO_NM);
                glBindFramebuffer(GL_FRAMEBUFFER, FBO_NM);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TEX_NM, 0);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO_Small);
                glDrawBuffers(1, DrawBuffers);

                /* GATHER ALL */
                glGenFramebuffers(1, &FBO_GA);
                glBindFramebuffer(GL_FRAMEBUFFER, FBO_GA);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TEX_GA, 0);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO_Normal);
                glDrawBuffers(1, DrawBuffers);
        }

        void clear() {
                glDeleteFramebuffers(1, &FBO_CZ);
                glDeleteFramebuffers(1, &FBO_VL);
                glDeleteFramebuffers(1, &FBO_TM);
                glDeleteFramebuffers(1, &FBO_NM);
                glDeleteFramebuffers(1, &FBO_GA);

                glDeleteRenderbuffers(1, &RBO_Normal);
                glDeleteRenderbuffers(1, &RBO_Small);

                glDeleteTextures(1, &TEX_C);
                glDeleteTextures(1, &TEX_Z);
                glDeleteTextures(1, &TEX_VL);
                glDeleteTextures(1, &TEX_TM);
                glDeleteTextures(1, &TEX_NM);
                glDeleteTextures(1, &TEX_GA);
        }

};