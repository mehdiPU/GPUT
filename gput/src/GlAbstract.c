/**
 * MIT License
 *
 * Copyright (c) 2021 Mehdi Nasef
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "GlAbstract.h"
#include "gputDebug.h"

typedef struct {
   int size;
   GLenum glType;
   GLenum glFormat;
   GLenum glInternalFormat;
} DataTypeInfo;

static const DataTypeInfo dataTypesInfo[] = {
   // I8
   {sizeof(GLbyte),        GL_BYTE,             GL_RED_INTEGER,   GL_R8I},
   // I16
   {sizeof(GLshort),       GL_SHORT,            GL_RED_INTEGER,   GL_R16I},
   // I32
   {sizeof(GLint),         GL_INT,              GL_RED_INTEGER,   GL_R32I},
   // F16
   {sizeof(GLhalf),        GL_HALF_FLOAT,       GL_RED,           GL_R16F},
   // F32
   {sizeof(GLfloat),       GL_FLOAT,            GL_RED,           GL_R32F},
   // UI8
   {sizeof(GLubyte),       GL_UNSIGNED_BYTE,    GL_RED_INTEGER,   GL_R8UI},
   //UI16
   {sizeof(GLushort),      GL_UNSIGNED_SHORT,   GL_RED_INTEGER,   GL_R16UI},
   //UI32
   {sizeof(GLuint),        GL_UNSIGNED_INT,     GL_RED_INTEGER,   GL_R32UI},

   // VEC2_I8
   {2 * sizeof(GLbyte),    GL_BYTE,             GL_RG_INTEGER,    GL_RG8I},
   // VEC2_I16
   {2 * sizeof(GLshort),   GL_SHORT,            GL_RG_INTEGER,    GL_RG16I},
   // VEC2_I32
   {2 * sizeof(GLint),     GL_INT,              GL_RG_INTEGER,    GL_RG32I},
   // VEC2_F16
   {2 * sizeof(GLhalf),    GL_HALF_FLOAT,       GL_RG,            GL_RG16F},
   // VEC2_F32
   {2 * sizeof(GLfloat),   GL_FLOAT,            GL_RG,            GL_RG32F},
   // VEC2_UI8
   {2 * sizeof(GLubyte),   GL_UNSIGNED_BYTE,    GL_RG_INTEGER,    GL_RG8UI},
   // VEC2_UI16
   {2 * sizeof(GLushort),  GL_UNSIGNED_SHORT,   GL_RG_INTEGER,    GL_RG16UI},
   // VEC2_UI32
   {2 * sizeof(GLuint),    GL_UNSIGNED_INT,     GL_RG_INTEGER,    GL_RG32UI},

   // VEC3_I8
   {3 * sizeof(GLbyte),    GL_BYTE,             GL_RGB_INTEGER,   GL_RGB8I},
   // VEC3_I16
   {3 * sizeof(GLshort),   GL_SHORT,            GL_RGB_INTEGER,   GL_RGB16I},
   // VEC3_I32
   {3 * sizeof(GLint),     GL_INT,              GL_RGB_INTEGER,   GL_RGB32I},
   // VEC3_F16
   {3 * sizeof(GLhalf),    GL_HALF_FLOAT,       GL_RGB,           GL_RGB16F},
   // VEC3_F32
   {3 * sizeof(GLfloat),   GL_FLOAT,            GL_RGB,           GL_RGB32F},
   // VEC3_UI8
   {3 * sizeof(GLubyte),   GL_UNSIGNED_BYTE,    GL_RGB_INTEGER,   GL_RGB8UI},
   // VEC3_UI16
   {3 * sizeof(GLushort),  GL_UNSIGNED_SHORT,   GL_RGB_INTEGER,   GL_RGB16UI},
   // VEC3_UI32
   {3 * sizeof(GLuint),    GL_UNSIGNED_INT,     GL_RGB_INTEGER,   GL_RGB32UI},

   // VEC4_I8
   {4 * sizeof(GLbyte),    GL_BYTE,             GL_RGBA_INTEGER,  GL_RGBA8I},
   // VEC4_I16
   {4 * sizeof(GLshort),   GL_SHORT,            GL_RGBA_INTEGER,  GL_RGBA16I},
   // VEC4_I32
   {4 * sizeof(GLint),     GL_INT,              GL_RGBA_INTEGER,  GL_RGBA32I},
   // VEC4_F16
   {4 * sizeof(GLhalf),    GL_HALF_FLOAT,       GL_RGBA,          GL_RGBA16F},
   // VEC4_F32
   {4 * sizeof(GLfloat),   GL_FLOAT,            GL_RGBA,          GL_RGBA32F},
   // VEC4_UI8
   {4 * sizeof(GLubyte),   GL_UNSIGNED_BYTE,    GL_RGBA_INTEGER,  GL_RGBA8UI},
   // VEC4_UI16
   {4 * sizeof(GLushort),  GL_UNSIGNED_SHORT,   GL_RGBA_INTEGER,  GL_RGBA16UI},
   // VEC4_UI32
   {4 * sizeof(GLuint),    GL_UNSIGNED_INT,     GL_RGBA_INTEGER,  GL_RGBA32UI},
};

#define INFOLOG_SIZE 512

char infolog[INFOLOG_SIZE];

GlShaderId gla_createShader(
   ShaderType shaderType, const char* sources[], int srcsCount
){
   GlShaderId shaderId = GLC(glCreateShader(shaderType));
   GLC(glShaderSource(shaderId, srcsCount, sources, NULL));
   GLC(glCompileShader(shaderId));

   GPUT_DEBUG_SCOPE(
      GLint success;
      GLC(glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success));
      if (!success) {
         GLC(glGetShaderInfoLog(shaderId, INFOLOG_SIZE, NULL, infolog));
         GPUT_ASSERT(success, "Shader compile error:\n%s", infolog);
      }
   )
   return shaderId;
}

void gla_deleteShader(GlShaderId shaderId)
{
   GPUT_DEBUG_SCOPE(
      int deleted;
      GLC(glGetShaderiv(shaderId, GL_DELETE_STATUS, &deleted));
      GPUT_ASSERT(!deleted, "Attempt to delete an already deleted shader")
   )
   GLC(glDeleteShader(shaderId));
}

GlProgId gla_linkProgram(GlShaderId vertexShader, GlShaderId fragmentShader)
{
   GPUT_DEBUG_SCOPE(
      GLint shaderType;
      GLC(glGetShaderiv(vertexShader, GL_SHADER_TYPE, &shaderType));
      GPUT_ASSERT(shaderType == GL_VERTEX_SHADER,
         "First parameter should be a vertex shader"
      );
      GLC(glGetShaderiv(fragmentShader, GL_SHADER_TYPE, &shaderType));
      GPUT_ASSERT(shaderType == GL_FRAGMENT_SHADER,
         "Second parameter should be a fragment shader"
      );
   )
   GlProgId progId = GLC(glCreateProgram());
   GLC(glAttachShader(progId, vertexShader));
   GLC(glAttachShader(progId, fragmentShader));
   GLC(glLinkProgram(progId));

   GPUT_DEBUG_SCOPE(
      GLint success;
      GLC(glGetProgramiv(progId, GL_LINK_STATUS, &success));
      if (!success) {
         GLC(glGetProgramInfoLog(progId, INFOLOG_SIZE, NULL, infolog));
         GPUT_ASSERT(success, "Program link error:\n%s", infolog);
      }
   )
   return progId;
}

void gla_bindProgram(GlProgId progId)
{
   GLC(glUseProgram(progId));
}

void gla_unbindProgram()
{
   GLC(glUseProgram(0));
}

void gla_deleteProgram(GlProgId progId)
{
   GPUT_DEBUG_SCOPE(
      int deleted;
      GLC(glGetProgramiv(progId, GL_DELETE_STATUS, &deleted));
      GPUT_ASSERT(!deleted, "Attempt to delete an already deleted shader")
   )
   GLC(glDeleteProgram(progId))
}

GlBuffId gla_createBuffer(
   BufferType bufferType, const void* bufferData, size_t size
){
   GlBuffId BufferId;
   GLC(glGenBuffers(1, &BufferId));
   GLC(glBindBuffer(bufferType, BufferId));
   GLC(glBufferData(bufferType, size, bufferData, GL_STATIC_DRAW));
   GLC(glBindBuffer(bufferType, 0));
   return BufferId;
}

void gla_bindBuffer(BufferType bufferType, GlBuffId bufferId)
{
   GLC(glBindBuffer(bufferType, bufferId));
}

void gla_unbindBuffer(BufferType bufferType)
{
   GLC(glBindBuffer(bufferType, 0));
}

void gla_deleteBuffer(GlBuffId bufferId)
{
   GlBuffId localBufferId = bufferId;
   GLC(glDeleteBuffers(1, &localBufferId));
}

GlTexId gla_createTexture(
   GlDataType pixDataType, int width, int height, void* texData
){
   GlTexId textureId;
   GLC(glGenTextures(1, &textureId));
   GLC(glBindTexture(GL_TEXTURE_2D, textureId));

   GLC(glTexImage2D(
      GL_TEXTURE_2D, 0,
      dataTypesInfo[pixDataType].glInternalFormat,
      width, height, 0,
      dataTypesInfo[pixDataType].glFormat,
      dataTypesInfo[pixDataType].glType,
      texData
   ));

   GLC(glBindTexture(GL_TEXTURE_2D, 0));
   return textureId;
}

void gla_bindTexture(GlTexId textureId)
{
   GLC(glBindTexture(GL_TEXTURE_2D, textureId));
}

void gla_unbindTexture()
{
   GLC(glBindTexture(GL_TEXTURE_2D, 0));
}

void gla_deleteTexture(GlTexId textureId)
{
   GlTexId localTextureId = textureId;
   GLC(glDeleteTextures(1, &localTextureId));
}

GlFramebufferId gla_createFramebuffer(GlTexId colorAttachment)
{
   GlFramebufferId framebufferId;
   GLC(glGenFramebuffers(1, &framebufferId));
   GLC(glBindFramebuffer(GL_FRAMEBUFFER, framebufferId));
   GLC(glFramebufferTexture2D(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment, 0
   ));
   GLC(GPUT_ASSERT(
      glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE,
      "Framebuffer not complete"
   ));
   GLC(glBindFramebuffer(GL_FRAMEBUFFER, 0));
   return framebufferId;
}

void gla_bindFramebuffer(GlFramebufferId framebufferId)
{
   GLC(glBindFramebuffer(GL_FRAMEBUFFER, framebufferId));
}

void gla_unbindFramebuffer()
{
   GLC(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void gla_deleteFramebuffer(GlFramebufferId framebufferId)
{
   GlFramebufferId localFramebufferId = framebufferId;
   GLC(glDeleteFramebuffers(1, &localFramebufferId));
}
