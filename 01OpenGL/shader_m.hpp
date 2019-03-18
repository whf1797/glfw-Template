//
//  shader_m.hpp
//  01OpenGL
//
//  Created by 王洪飞 on 2019/3/18.
//  Copyright © 2019 Miss CC. All rights reserved.
//

#ifndef shader_m_hpp
#define shader_m_hpp

#include <stdio.h>

//
//  shader_m.cpp
//  01OpenGL
//
//  Created by 王洪飞 on 2019/3/18.
//  Copyright © 2019 Miss CC. All rights reserved.
//

#include "shader_m.hpp"
#include "include/math3d.h"
#include <glad/glad.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "include/math3d.h"

class Shader
{
public:
    unsigned int ID;
    Shader(const char *vertexPath, const char* fragmentPath, const char *geometryPath = nullptr){
        // 1 retrieve the vertex/fragment source code from filepath
        std::string vertexCode;
        std::string fragmentCode;
        std::string geometryCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        std::ifstream gShaderFile;
        
        vShaderFile.exceptions(std::ifstream::failbit|std::ifstream::badbit);
        fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        gShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        
        try{
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream , fShaderStream;
            // read file`s buffer content into stream
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
            // if geometry shader path is present , also load a geometry shader
            if (geometryPath != nullptr) {
                gShaderFile.open(geometryPath);
                std::stringstream gShaderStream;
                gShaderStream << gShaderFile.rdbuf();
                gShaderFile.close();
                geometryCode = gShaderStream.str();
            }
        }catch(std::ifstream::failure e){
            std::cout << "error :: shader:: file not succesfully read" << std::endl;
        }
        
        const char *vShaderCode = vertexCode.c_str();
        const char *fShaderCode = fragmentCode.c_str();
        
        // comple shaders
        GLuint vertex, fragment;
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex,1,&vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment,1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        
        GLuint geometry = 0;
        if (geometryPath != nullptr) {
            const char* gShaderCode = geometryCode.c_str();
            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry,1, &gShaderCode, NULL);
            glCompileShader(geometry);
            checkCompileErrors(geometry, "GEOMETRY");
        }
        
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        if (geometryPath != nullptr) {
            glAttachShader(ID, geometry);
        }
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if (geometryPath != nullptr) {
            glDeleteShader(geometry);
        }
    };
    
    void use(){
        glUseProgram(ID);
    }
    
    void setBool(const std::string &name ,bool value) const
    {
        glUniform1i(glGetUniformLocation(ID , name.c_str()),(int)value);
    }
    
    void setInt(const std::string &name, int value) const
    {
        GLint location = glGetUniformLocation(ID, name.c_str());
        glUniform1i(location,value);
    }
    
    void setFloat(const std::string &name, int value) const {
        GLint location = glGetUniformLocation(ID, name.c_str());
        glUniform1f(location,value);
    }
    void setVec2(const std::string &name,const M3DVector2f value) const {
        GLint location = glGetUniformLocation(ID, name.c_str());
        glUniform2fv(location,1,value);
    }
    
    void setVec2(const std::string &name , float x, float y) const {
        GLint location = glGetUniformLocation(ID, name.c_str());
        //(GLint location, GLfloat v0, GLfloat v1);
        glUniform2f(location,x,y);
    }
    
    // ------------------------------------------------------------------------
    void setVec3(const std::string &name, const M3DVector3f value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, value);
    }
    void setVec3(const std::string &name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string &name, const M3DVector4f value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, value);
    }
    void setVec4(const std::string &name, float x, float y, float z, float w)
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
    // ------------------------------------------------------------------------
    //    void setMat2(const std::string &name, const glm::mat2 &mat) const
    //    {
    //        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    //    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string &name, const M3DMatrix33f mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, mat);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string &name, const M3DMatrix44f mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, mat);
    }
    
private:
    void checkCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];
        // (GLuint shader, GLenum pname, GLint *params);
        // (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if(!success){
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout<<"init shader error " << type << infoLog << std::endl;
            }else {
                std::cout << "init shader "<< type << " success " << std::endl;
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout<<"error:program init failed" <<infoLog << std::endl;
            } else {
                std::cout<< "init program "<< type <<" success " << std::endl;
            }
        }
    }
};


#endif /* shader_m_hpp */
