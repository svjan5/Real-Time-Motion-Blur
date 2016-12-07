bool ReadFile(const char* pFileName, string& outFile);
static void AddShader(GLuint pID, const char* srcCode, GLenum ShaderType);
static GLuint CompileShaders(const char *vsFileName, const char *fgFileName);


class ShaderProgram {
public:
        GLuint pID;

        ShaderProgram(const char* vsFileName, const char* fgFileName) {
                pID = CompileShaders( vsFileName, fgFileName);
        }

        void Enable()   { glUseProgram(pID);    }
        void Disable()  { glUseProgram(0);      }
        void init();
};

// class MLShader: public ShaderProgram{
// public:
// 	GLuint MVP, MV, 
// }

class CZShader: public ShaderProgram{
public:
	GLuint MVP, Model, View, LightCol, LightPos, TEX3D, TEX2D, colorMode;

	void init(){
		this->Enable();
	        Model		= glGetUniformLocation(pID, "M");
	        View         	= glGetUniformLocation(pID, "V");
	        MVP          	= glGetUniformLocation(pID, "MVP");
	        LightPos     	= glGetUniformLocation(pID, "LightPos");
	        LightCol     	= glGetUniformLocation(pID, "LightColor");
	        TEX3D          	= glGetUniformLocation(pID, "texCube");
	        TEX2D          	= glGetUniformLocation(pID, "texRect");
	        colorMode	= glGetUniformLocation(pID, "colorMode");

	        glUniform3f(LightPos, 0.0f, 6.0f, -4.0f);
	        glUniform3f(LightCol, 1.0f, 1.0f, 1.0f);
	}
};

class VLShader: public ShaderProgram{
public:
	GLuint currMVP, prevMVP, proj;

	void init(){
		this->Enable();
	        currMVP 	= glGetUniformLocation(pID, "currMVP");
	        prevMVP 	= glGetUniformLocation(pID, "prevMVP");
	        proj		= glGetUniformLocation(pID, "proj");
	}
};

class TMShader: public ShaderProgram{
public:
	GLuint K, TEX, Pos;
	
	void init(){
		this->Enable();
	        K    	= glGetUniformLocation(pID, "K");
	        TEX 	= glGetUniformLocation(pID, "TEX_VL");
	        Pos  	= glGetAttribLocation(pID, "position");
	}
};

class NMShader: public ShaderProgram{
public:
	GLuint Pos, TEX;
	
	void init(){
		this->Enable();
	        TEX = glGetUniformLocation(pID, "TEX_TM");
	        Pos  = glGetAttribLocation(pID, "position");
	}
};



class VWShader: public ShaderProgram{
public:
	GLuint Pos, Tex, TexType, K, cameraFar, cameraNear;
	
	void init(){
		this->Enable();
	        Pos 		= glGetAttribLocation(pID, "position");
	        Tex 		= glGetUniformLocation(pID, "renderedTex");
	        K 		= glGetUniformLocation(pID, "mK");
	        TexType 	= glGetUniformLocation(pID, "texType");
	        cameraNear	= glGetUniformLocation(pID, "cameraNear");
		cameraFar	= glGetUniformLocation(pID, "cameraFar");
	}
};

class GAShader: public ShaderProgram{
public:
	GLuint TEX_C, TEX_VL, TEX_Z, TEX_NM, K, S, Exposure, MaxSD, cameraNear, cameraFar, fps;
	
	void init(){
		this->Enable();
	        TEX_C 		= glGetUniformLocation(pID, "TEX_C");
	        TEX_VL 		= glGetUniformLocation(pID, "TEX_VL");
	        TEX_Z 		= glGetUniformLocation(pID, "TEX_Z");
	        TEX_NM 		= glGetUniformLocation(pID, "TEX_NM");
	        K 		= glGetUniformLocation(pID, "K");
	        S 		= glGetUniformLocation(pID, "S");
	        fps		= glGetUniformLocation(pID, "fps"); 		
	        Exposure 	= glGetUniformLocation(pID, "Exposure");
	        MaxSD 		= glGetUniformLocation(pID, "MaxSD");
		cameraNear	= glGetUniformLocation(pID, "cameraNear");
		cameraFar	= glGetUniformLocation(pID, "cameraFar");
	}
};

class Shaders{
public:
	CZShader* CZ; VLShader* VL; TMShader* TM; NMShader* NM; GAShader* GA; VWShader* View;

	Shaders(){
		CZ = (CZShader*) new ShaderProgram("shaders/colorDepth.vs", "shaders/colorDepth.fs");
		VL = (VLShader*) new ShaderProgram("shaders/velocity.vs", "shaders/velocity.fs");
		TM = (TMShader*) new ShaderProgram("shaders/base.vs", "shaders/tileMax.fs");
		NM = (NMShader*) new ShaderProgram("shaders/base.vs", "shaders/neighborMax.fs");
		GA = (GAShader*) new ShaderProgram("shaders/base.vs", "shaders/gatherAll.fs");
		View = (VWShader*) new ShaderProgram("shaders/viewer.vs", "shaders/viewer.fs");	
	}
};


bool ReadFile(const char* pFileName, string& outFile) {
        ifstream f(pFileName);

        bool ret = false;

        if (f.is_open()) {
                string line;
                while (getline(f, line)) {
                        outFile.append(line);
                        outFile.append("\n");
                }

                f.close();

                ret = true;
        } else {
                fprintf(stderr, "Error in loading file: '%s'\n", pFileName);
        }
        return ret;
}

static void AddShader(GLuint pID, const char* srcCode, GLenum ShaderType) {
        GLuint ShaderObj = glCreateShader(ShaderType);  if (ShaderObj == 0) {fprintf(stderr, "Error creating shader type %d\n", ShaderType); exit(0); }

        const GLchar * p[1];    p[0] = srcCode;
        GLint Lengths[1];       Lengths[0] = strlen(srcCode);

        glShaderSource(ShaderObj, 1, p, Lengths);
        glCompileShader(ShaderObj);
        GLint success;
        glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

        if (!success) {
                GLchar InfoLog[1024]; glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
                fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
                exit(1);
        }

        glAttachShader(pID, ShaderObj);
}

static GLuint CompileShaders(const char *vsFileName, const char *fgFileName) {
        GLuint pID = glCreateProgram();
        if (pID == 0) {fprintf(stderr, "Error creating shader program\n"); exit(1); }

        string vs, fs;
        GLint Success = 0;
        GLchar ErrorLog[1024] = {0};

        if (!ReadFile(vsFileName, vs)) {exit(1); }
        if (!ReadFile(fgFileName, fs)) {exit(1); }

        AddShader(pID, vs.c_str(), GL_VERTEX_SHADER);
        AddShader(pID, fs.c_str(), GL_FRAGMENT_SHADER);

        glLinkProgram(pID);
        glGetProgramiv(pID, GL_LINK_STATUS, &Success);

        if (Success == 0) {
                glGetProgramInfoLog(pID, sizeof (ErrorLog), NULL, ErrorLog);
                fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog); exit(1);
        }

        glValidateProgram(pID);
        glGetProgramiv(pID, GL_VALIDATE_STATUS, &Success);
        if (!Success) {
                glGetProgramInfoLog(pID, sizeof (ErrorLog), NULL, ErrorLog);
                fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog); exit(1);
        }

        return pID;
}
