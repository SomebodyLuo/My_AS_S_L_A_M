/*
 *  Shader.cpp
 *  Virtual Vision
 *
 *  Created by Abdallah Dib Abdallah.dib@virtual-vison.net
 *  Copyright 2011 Virtual Vision. All rights reserved.
 *
 */

#include "../include/Shader.h"
#include <stdio.h>
#include <fstream>
#include <iostream>
#include "myJNIHelper.h"
namespace vvision
{
    
    CShader::CShader()
    {
        m_uShadersProgram = glCreateProgram();
    }
    CShader::~CShader()
    {
        if (m_uVertShader)
            glDeleteShader(m_uVertShader);
        
        if (m_uFragShader)
            glDeleteShader(m_uFragShader);
        
        if (m_uShadersProgram)
        {
            glDeleteProgram(m_uShadersProgram);
            CShader::m_uShadersProgram = 0;
        }
    }
    
    bool CShader::LoadShadersFromMemory(string vertex_shader, string fragment_shader)
    {
        if(!m_uShadersProgram)
            m_uShadersProgram = glCreateProgram();
        LOG("m_uShadersProgram:%d",m_uShadersProgram);

        std::string vertexShaderCode;
        if (!ReadShaderCode(vertexShaderCode, vertex_shader)) {
            LOG("Error in reading Vertex shader");
            return 0;
        }
        //LOG("vertexShaderCode:%s",vertexShaderCode.c_str());
        if ( !CompileShader(m_uVertShader, GL_VERTEX_SHADER, vertexShaderCode) )
            return false;

        // read and compile the fragment shader
        std::string fragmentShaderCode;
        if (!ReadShaderCode(fragmentShaderCode, fragment_shader)) {
            LOG("Error in reading Fragment shader");
            return 0;
        }
        if ( !CompileShader(m_uFragShader, GL_FRAGMENT_SHADER, fragmentShaderCode) )
            return false;
        
        // Attach vertex shader to ShadersProgram.
        glAttachShader(m_uShadersProgram, m_uVertShader);
        
        // Attach fragment shader to ShadersProgram.
        glAttachShader(m_uShadersProgram, m_uFragShader);
        
        // Bind attribute locations.
        // This needs to be done prior to linking.
        glBindAttribLocation(m_uShadersProgram, ATTRIB_VERTEX, "position");
        glBindAttribLocation(m_uShadersProgram, ATTRIB_NORMAL, "normal");
        glBindAttribLocation(m_uShadersProgram, ATTRIB_COLOR, "aColor");
        glBindAttribLocation(m_uShadersProgram, ATTRIB_TANGENT, "tangent");
        glBindAttribLocation(m_uShadersProgram, ATTRIB_TEXTURE0, "texCoord0");
        
        glBindAttribLocation(m_uShadersProgram, ATTRIB_BONES_INDICES, "bones");
        glBindAttribLocation(m_uShadersProgram, ATTRIB_BONES_WEIGHTS, "weights");
        
        if (!LinkProgram())
        {
            //print log here
            
            if (m_uVertShader)
            {
                glDeleteShader(m_uVertShader);
                m_uVertShader = 0;
            }
            
            if (m_uFragShader)
            {
                glDeleteShader(m_uFragShader);
                m_uFragShader = 0;
            }
            
            return false;
        }
        
        LoadShaderUniforms();
        return true;
    }
    bool CShader::ReadShaderCode(std::string & shaderCode, std::string & shaderFileName) {
        // android shaders are stored in assets
        // read them using MyJNIHelper
        bool isFilePresent = gHelperObject->ExtractAssetReturnFilename(shaderFileName,
                                                                       shaderFileName);
        if( !isFilePresent ) {
            return false;
        }

        std::ifstream shaderStream(shaderFileName.c_str(), std::ios::in);
        if (shaderStream.is_open()) {
            std::string Line = "";
            while (getline(shaderStream, Line)) {
                shaderCode += "\n" +  Line;
            }
            shaderStream.close();
        } else {
            LOG("Cannot open %s", shaderFileName.c_str());
            return false;
        }

        LOG("Read successfully");
        return true;
    }

    bool CShader::CompileShader(GLuint &shader, GLenum type, string file )
    {
        const GLchar *source = (GLchar*) file.c_str();

        if (!source || !strlen(source))
            return false;

        shader = glCreateShader(type);

        LOG("shader:%d",shader);

        glShaderSource(shader, 1, &source, NULL);
        glCompileShader(shader);

        GLint logLength = -1;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

        if (logLength > 0)
        {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, NULL, infoLog);

            LOG("infoLog:%s",infoLog);
        }
        else
        {
            GLint status = 0;

            glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

            LOG("status:%d",status);
            if (status == 0)
            {
                char infoLog[512];
                glGetShaderInfoLog(shader, 512, NULL, infoLog);

                LOG("infoLog:%s",infoLog);

                glDeleteShader(shader);
                return false;
            }
        }

        return true;
    }

    bool CShader::LinkProgram()
    {
        GLint logLength = -1, status = 0;

        glLinkProgram(m_uShadersProgram);

        glGetProgramiv(m_uShadersProgram, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0)
        {
            char infoLog[512];
            glGetProgramInfoLog(m_uShadersProgram, logLength, &logLength, infoLog);

            LOG("infoLog:%s",infoLog);
        }
        else
        {
            glGetProgramiv(m_uShadersProgram, GL_LINK_STATUS, &status);
            if (status != 0)
                return true;
        }

        return false;
    }

    bool CShader::ValidateProgram()
    {
        GLint logLength = -1, status = 0;
        
        glValidateProgram(m_uShadersProgram);
        glGetProgramiv(m_uShadersProgram, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0)
        {
            char infoLog[512];
            glGetProgramInfoLog(m_uShadersProgram, logLength, &logLength, infoLog);

            LOG("infoLog:%s",infoLog);
        }
        else
        {
            glGetProgramiv(m_uShadersProgram, GL_VALIDATE_STATUS, &status);
            if (status != 0)
                return true;
        }

        return false;
    }
    bool CShader::SetUniform1f(const GLchar* uniformName, GLfloat value,GLint location)
    {
        if (location == -1) {
            int loc =glGetUniformLocation(m_uShadersProgram, uniformName);
            if (loc != -1)
            {
                glUniform1f(loc, value);
                return true;
            }
            else
                return false;
            
        }
        else {
            //if u are here that mean that u have already  verified  that the location alread exist using getuniformlocation, so be careful
            glUniform1f(location, value);
            return true;
        }
        
    }
    bool CShader::SetUniform1i(const GLchar* uniformName, GLint value, GLint location )
    {
        if (location == -1) {
            int loc =glGetUniformLocation(m_uShadersProgram, uniformName);
            
            if (loc != -1)
            {
                glUniform1i(loc, value);
                return true;
            }
            else
                return false;
            
        }
        else {
            //if u are here that mean that u have already  verified  that the location alread exist using getuniformlocation, so be careful
            glUniform1i(location, value);
            
            return true;
        }
    }
    bool CShader::SetUniform2fv(const GLchar* uniformName, GLsizei count, const GLfloat* value, GLint location)
    {
        if (location == -1) {
            int loc =glGetUniformLocation(m_uShadersProgram, uniformName);
            if (loc != -1)
            {
                glUniform2fv(loc, count, value);
                return true;
            }
            else
                return false;
            
        }
        else {
            //if u are here that mean that u have already  verified  that the location alread exist using getuniformlocation, so be careful
            glUniform2fv(location, count, value);
            return true;
        }
    }
    bool CShader::SetUniform3fv(const GLchar* uniformName, GLsizei count, const GLfloat* value, GLint location)
    {
        if (location == -1) {
            int loc =glGetUniformLocation(m_uShadersProgram, uniformName);
            if (loc != -1)
            {
                glUniform3fv(loc, count, value);
                return true;
            }
            else
                return false;
            
        }
        else {
            //if u are here that mean that u have already  verified  that the location alread exist using getuniformlocation, so be careful
            glUniform3fv(location, count, value);
            return true;
        }
    }
    
    bool CShader::SetUniform4fv(const GLchar* uniformName, GLsizei count, const GLfloat* value, GLint location)
    {
        if (location == -1) {
            int loc =glGetUniformLocation(m_uShadersProgram, uniformName);
            if (loc != -1)
            {
                glUniform4fv(loc, count, value);
                return true;
            }
            else
                return false;
            
        }
        else {
            //if u are here that mean that u have already  verified  that the location alread exist using getuniformlocation, so be careful
            glUniform4fv(location, count, value);
            return true;
        }
    }
    
    bool CShader::SetUniformMatrix4x4fv(const GLchar* uniformName, GLsizei count, GLboolean transpose, const GLfloat* value,int location)
    {
        if (location == -1) {
            int loc =glGetUniformLocation(m_uShadersProgram, uniformName);
            if (loc != -1)
            {
                glUniformMatrix4fv(loc, count, transpose, value);
                return true;
            }
            else
                return false;
        }
        else {
            glUniformMatrix4fv(location, count, transpose, value);
            return true;
        }
    }
    bool CShader::SetUniformMatrix3x3fv(const GLchar* uniformName, GLsizei count, GLboolean transpose, const GLfloat* value, int location)
    {
        if (location == -1) {
            int loc =glGetUniformLocation(m_uShadersProgram, uniformName);
            if (loc != -1)
            {
                glUniformMatrix3fv(loc, count, transpose, value);
                return true;
            }
            else
                return false;
        }
        else {
            glUniformMatrix3fv(location, count, transpose, value);
            return true;
        }
    }
    GLint CShader::GetUnifomLocation(const GLchar* uniformName)
    {
        return glGetUniformLocation(m_uShadersProgram, uniformName);
    }
    void CShader::Begin()
    {
        if (m_uShadersProgram <= 0)
            return;

        glUseProgram(m_uShadersProgram);
    }
    void CShader::End()
    {
        glUseProgram(0);
    }
    void CShader::LoadShaderUniforms()
    {
        //matrices
        matproj = glGetUniformLocation(m_uShadersProgram,               "matProj");

        matview = glGetUniformLocation(m_uShadersProgram,               "matView");
        matmodel = glGetUniformLocation(m_uShadersProgram,              "matModel");
        matViewModel  = glGetUniformLocation(m_uShadersProgram,          "matViewModel");
        matprojviewmodel = glGetUniformLocation(m_uShadersProgram,      "matProjViewModel");
        matnormal = glGetUniformLocation(m_uShadersProgram,             "matNormal");

        //attribute
        position = glGetAttribLocation(m_uShadersProgram,   "position");
        pointcolor = glGetAttribLocation(m_uShadersProgram, "aColor");
        normal = glGetAttribLocation(m_uShadersProgram,     "normal");
        texcoord0 = glGetAttribLocation(m_uShadersProgram,  "texCoord0");
        color = glGetAttribLocation(m_uShadersProgram,      "color");
        tangent = glGetAttribLocation(m_uShadersProgram,    "tangent");
        ///diffuse
        texture0 = glGetUniformLocation(m_uShadersProgram, "texture0");
        
        //cube map
        textureCubeMap = glGetUniformLocation(m_uShadersProgram, "textureCubeMap");
        
        //bump
        textureBump = glGetUniformLocation(m_uShadersProgram, "textureBump");
        
        //specular
        textureSpecular = glGetUniformLocation(m_uShadersProgram, "textureSpecular");
        //detail
        textureDetail = glGetUniformLocation(m_uShadersProgram,     "textureDetail");
        useDetailTexture = glGetUniformLocation(m_uShadersProgram,  "useDetailTexture");
        detailFactor = glGetUniformLocation(m_uShadersProgram,      "detailFactor");
        
        //material
        matColorAmbient = glGetUniformLocation(m_uShadersProgram,   "matColorAmbient");
        matColorDiffuse = glGetUniformLocation(m_uShadersProgram,   "matColorDiffuse");
        matColorSpecular = glGetUniformLocation(m_uShadersProgram,  "matColorSpecular");
        matShininess = glGetUniformLocation(m_uShadersProgram,      "matShininess");
        matOpacity  = glGetUniformLocation(m_uShadersProgram,       "matOpacity");
        matColorEmissive = glGetUniformLocation(m_uShadersProgram,  "matColorEmissive");
        
        //fog
        fcolor = glGetUniformLocation(m_uShadersProgram,    "fColor");
        fstartend = glGetUniformLocation(m_uShadersProgram, "fStartEnd");
        
        //light
        lightColorAmbient = glGetUniformLocation(m_uShadersProgram,     "lightColorAmbient");
        lightColorDiffuse = glGetUniformLocation(m_uShadersProgram,     "lightColorDiffuse");
        lightColorSpecular = glGetUniformLocation(m_uShadersProgram,    "lightColorSpecular");
        lightPosition = glGetUniformLocation(m_uShadersProgram,         "lightPosition");
        lightAttenuation = glGetUniformLocation(m_uShadersProgram,      "lightAttenuation");
        
        //time
        time = glGetUniformLocation(m_uShadersProgram, "Time");
        
        //position
        cameraPosition = glGetUniformLocation(m_uShadersProgram, "cameraPosition");
        
    }
}
