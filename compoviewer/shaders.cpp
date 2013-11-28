#include"shaders.h"
#include"core.h"
#include<iostream>
#include<string>
program::program(GLuint &p) : p(p) {}
std::map<std::string, attribute*> program::attribs;
std::map<std::string, uniform*> program::uniforms;

int program::attribindex = 0;

#define SWITCHVECSET(BTYPE, C, TYPE) \
	case GL_ ## BTYPE :				glUniform1##C##v(pos, 1, (TYPE*)value); break; \
	case GL_ ## BTYPE ## _VEC2 :	glUniform2##C##v(pos, 1, (TYPE*)value); break; \
	case GL_ ## BTYPE ## _VEC3 :	glUniform3##C##v(pos, 1, (TYPE*)value); break; \
	case GL_ ## BTYPE ## _VEC4 :	glUniform4##C##v(pos, 1, (TYPE*)value); break;
#define SWITCHMATSET(BTYPE, C, TYPE) \
	case GL_ ## BTYPE ## _MAT2 :	glUniformMatrix2##C##v(pos, 1, false, (TYPE*)value); break; \
	case GL_ ## BTYPE ## _MAT3 :	glUniformMatrix3##C##v(pos, 1, false, (TYPE*)value); break; \
	case GL_ ## BTYPE ## _MAT4 :	glUniformMatrix4##C##v(pos, 1, false, (TYPE*)value); break; \
	case GL_ ## BTYPE ## _MAT2x3 :	glUniformMatrix2x3##C##v(pos, 1, false, (TYPE*)value); break; \
	case GL_ ## BTYPE ## _MAT2x4 :	glUniformMatrix2x4##C##v(pos, 1, false, (TYPE*)value); break; \
	case GL_ ## BTYPE ## _MAT3x2 :	glUniformMatrix3x2##C##v(pos, 1, false, (TYPE*)value); break; \
	case GL_ ## BTYPE ## _MAT3x4 :	glUniformMatrix3x4##C##v(pos, 1, false, (TYPE*)value); break; \
	case GL_ ## BTYPE ## _MAT4x2 :	glUniformMatrix4x2##C##v(pos, 1, false, (TYPE*)value); break; \
	case GL_ ## BTYPE ## _MAT4x3 :	glUniformMatrix4x3##C##v(pos, 1, false, (TYPE*)value); break;

#define SWITCHVECGEN(BTYPE, TYPE) \
	case GL_ ## BTYPE :			v->value = (void*) new TYPE[1]; break; \
	case GL_ ## BTYPE ## _VEC2: v->value = (void*) new TYPE[2]; break; \
	case GL_ ## BTYPE ## _VEC3: v->value = (void*) new TYPE[3]; break; \
	case GL_ ## BTYPE ## _VEC4: v->value = (void*) new TYPE[4]; break;
#define SWITCHMATGEN(BTYPE, TYPE) \
	case GL_ ## BTYPE ## _MAT2: v->value = (void*) new TYPE[4]; break; \
	case GL_ ## BTYPE ## _MAT3: v->value = (void*) new TYPE[9]; break; \
	case GL_ ## BTYPE ## _MAT4: v->value = (void*) new TYPE[16]; break; \
	case GL_ ## BTYPE ## _MAT2x3: v->value = (void*) new TYPE[6]; break; \
	case GL_ ## BTYPE ## _MAT2x4: v->value = (void*) new TYPE[8]; break; \
	case GL_ ## BTYPE ## _MAT3x2: v->value = (void*) new TYPE[6]; break; \
	case GL_ ## BTYPE ## _MAT3x4: v->value = (void*) new TYPE[12]; break; \
	case GL_ ## BTYPE ## _MAT4x2: v->value = (void*) new TYPE[8]; break; \
	case GL_ ## BTYPE ## _MAT4x3: v->value = (void*) new TYPE[12]; break;

#define ATTRIBVECGEN(TYPE) \
	case GL_##TYPE##: \
	case GL_##TYPE##_VEC2: \
	case GL_##TYPE##_VEC3: \
	case GL_##TYPE##_VEC4: \
		attribindex += 1; \
		break;

#define ATTRIBMATGEN(TYPE) \
	case GL_##TYPE##_MAT2: \
	case GL_##TYPE##_MAT2x3: \
	case GL_##TYPE##_MAT2x4: \
		attribindex += 2; \
		break; \
	case GL_##TYPE##_MAT3: \
	case GL_##TYPE##_MAT3x2: \
	case GL_##TYPE##_MAT3x4: \
		attribindex += 3; \
		break; \
	case GL_##TYPE##_MAT4: \
	case GL_##TYPE##_MAT4x2: \
	case GL_##TYPE##_MAT4x3: \
		attribindex += 4; \
		break;

#define ATTRIBMATSTATE(TYPE) \
	case GL_##TYPE##_MAT2: \
	case GL_##TYPE##_MAT3x2: \
	case GL_##TYPE##_MAT4x2: \
		v->format = GL_##TYPE; \
		v->size = 2; \
		break; \
	case GL_##TYPE##_MAT2x3: \
	case GL_##TYPE##_MAT3: \
	case GL_##TYPE##_MAT4x3: \
		v->format = GL_##TYPE; \
		v->size = 3; \
		break;  \
	case GL_##TYPE##_MAT2x4: \
	case GL_##TYPE##_MAT3x4: \
	case GL_##TYPE##_MAT4: \
		v->format = GL_##TYPE; \
		v->size = 4; \
		break; 

#define ATTRIBVECSTATE(TYPE) \
	case GL_##TYPE##: \
		v->format = GL_##TYPE; \
		v->size = 1; \
		break; \
	case GL_##TYPE##_VEC2: \
		v->format = GL_##TYPE; \
		v->size = 2; \
		break; \
	case GL_##TYPE##_VEC3: \
		v->format = GL_##TYPE; \
		v->size = 3; \
		break;  \
	case GL_##TYPE##_VEC4: \
		v->format = GL_##TYPE; \
		v->size = 4; \
		break; 


static char infolog[2048];
program *program::createProgram(std::vector<GLuint> &shaders){
	GLuint p = glCreateProgram();
	std::for_each(shaders.begin(), shaders.end(), [&](GLuint s){glAttachShader(p, s);});
	int len = 0;
	glLinkProgram(p);
	glGetProgramiv(p, GL_LINK_STATUS, &len);
	if(len != GL_TRUE){
		glGetProgramInfoLog(p, sizeof(infolog), &len, infolog);
		std::cerr<<infolog<<std::endl;
		core::die();
	}
	program *prg = new program(p);
	prg->bind();
	
	int total = 0;

	glGetProgramiv(p, GL_ACTIVE_ATTRIBUTES, &total);
	for(int i=0;i<total;++i){
		int nl = 0, n = 0;
		GLenum type = GL_ZERO;
		char name[100];
		glGetActiveAttrib(p, i, 99, &nl, &n, &type, name);
		name[nl] = 0;
		if(program::attribs.count(std::string(name)) == 0){
			auto &v = program::attribs[name];
			v = new attribute;
			v->name = std::string(name);
			v->pos = attribindex;
			v->touched = false;
			v->type = type;
			std::cout<<"Found new attribute "<<name<<std::endl;
			switch(type){
				ATTRIBMATGEN(FLOAT);
				ATTRIBMATGEN(DOUBLE);

				ATTRIBVECGEN(FLOAT);
				ATTRIBVECGEN(DOUBLE);
				ATTRIBVECGEN(INT);
				ATTRIBVECGEN(UNSIGNED_INT);
			}
			switch(type){
				ATTRIBMATSTATE(FLOAT);
				ATTRIBMATSTATE(DOUBLE);
				
				ATTRIBVECSTATE(FLOAT);
				ATTRIBVECSTATE(DOUBLE);
				ATTRIBVECSTATE(INT);
				ATTRIBVECSTATE(UNSIGNED_INT);
			}
		}
		auto &v = program::attribs[name];
		if(v->type != type){
			std::cerr<<"Non-matching attribute type for "<<name<<std::endl;
			core::die();
		}
		glBindAttribLocation(p, v->pos, v->name.c_str());
	}

	glLinkProgram(p);

	glGetProgramiv(p, GL_ACTIVE_UNIFORMS, &total);
	
	for(int i=0;i<total;++i){
		int nl = 0, n = 0;
		GLenum type = GL_ZERO;
		char name[100];
		glGetActiveUniform(p, i, 99, &nl, &n, &type, name);
		name[nl] = 0;
		if(program::uniforms.count(std::string(name)) == 0 || program::uniforms[std::string(name)]->type == GL_ZERO){
			auto &v = program::uniforms[name];
			v = new uniform;
			v->name = std::string(name);
			v->type = type;
			std::cout<<"found uniform "<<name<<" of type "<<v->type<<std::endl;
			switch(type){
				SWITCHVECGEN(FLOAT, GLfloat);
				SWITCHVECGEN(INT, GLint);
				SWITCHVECGEN(DOUBLE, GLdouble);
				SWITCHVECGEN(UNSIGNED_INT, GLuint);
				SWITCHVECGEN(BOOL, GLboolean);
				
				SWITCHMATGEN(FLOAT, GLfloat);
				SWITCHMATGEN(DOUBLE, GLdouble);

				//TODO: atomic counters?
			default:
				v->value = new int[1];
			}
		} else if (program::uniforms[name]->type != type) {
			std::cerr<<"Non-matching uniform type for "<<name;
			core::die();
		}
		uniformconfig *uc = new uniformconfig();
		uc->touched = true;
		uc->pos = glGetUniformLocation(p, name);
		uc->u = program::uniforms[name];
		prg->uniformconfigs.insert(uc);
		program::uniforms[std::string(name)]->configs.insert(uc);
	}
	return prg;
}
std::vector<GLuint>& program::shader(GLenum type, std::string src, std::vector<GLuint> &shaders){
	GLuint sh = glCreateShader(type);
	int len = 0;
	const char *srcp = src.c_str();
	int srcl = src.length();
	glShaderSource(sh, 1, &srcp, &srcl);
	glCompileShader(sh);
	glGetShaderiv(sh, GL_COMPILE_STATUS, &len);
	if(len != GL_TRUE){
		glGetShaderInfoLog(sh, sizeof(infolog), &len, infolog);
		std::cerr<<src<<std::endl<<infolog<<std::endl;
		core::die();
	}
	shaders.push_back(sh);
	return shaders;
}

GLuint program::bound = -1;
void program::bind(){
	if(bound != p)
		glUseProgram(p);
	bound = p;
}
void program::update(){
	bind();
	std::for_each(uniformconfigs.begin(),uniformconfigs.end(),[&](uniformconfig *uc){
		if(!uc->touched) return;
		auto &value = uc->u->value;
		auto &pos = uc->pos;
		auto &name = uc->u->name;
#ifndef NDEBUG
		std::cout<<"Updating "<<name<<std::endl;
#endif
		uc->touched = false;
		switch(uc->u->type){
			SWITCHVECSET(FLOAT, f, GLfloat);
			SWITCHVECSET(DOUBLE, d, GLdouble);
			SWITCHVECSET(INT, i, GLint);
			SWITCHVECSET(UNSIGNED_INT, ui, GLuint);
			SWITCHMATSET(FLOAT, f, GLfloat);
			SWITCHMATSET(DOUBLE, d, GLdouble);
		default:
			glUniform1i(pos, *(GLint *)value);
		}
	});
}
