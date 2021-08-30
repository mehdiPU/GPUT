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

#include <stdalign.h>

#include "GlAbstract.h"
#include "gputDebug.h"

typedef struct {
   GLenum glType;
   GLenum glFormat;
   GLenum glInternalFormat;
   unsigned char size;
   unsigned char elementsCount;
   unsigned char alignment;
   bool isInteger;
} DataTypeInfo;

typedef struct {
   GLvoid* attribPtr;
   GLenum type;
   char elementsCount;
   bool isInteger;
} AttribInfo;

static const DataTypeInfo dataTypesInfo[] = {
   // I8
   {
      GL_BYTE,                GL_RED_INTEGER,      GL_R8I,
      sizeof(GLbyte),         1,                   alignof(GLbyte),     true
   },
   // I16
   {
      GL_SHORT,               GL_RED_INTEGER,      GL_R16I,
      sizeof(GLshort),        1,                   alignof(GLshort),    true
   },
   // I32
   {
      GL_INT,                 GL_RED_INTEGER,      GL_R32I,
      sizeof(GLint),          1,                   alignof(GLint),      true
   },
   // F16
   {
      GL_HALF_FLOAT,          GL_RED,              GL_R16F,
      sizeof(GLhalf),         1,                   alignof(GLhalf),     false
   },
   // F32
   {
      GL_FLOAT,               GL_RED,              GL_R32F,
      sizeof(GLfloat),        1,                   alignof(GLfloat),    false
   },
   // UI8
   {
      GL_UNSIGNED_BYTE,       GL_RED_INTEGER,      GL_R8UI,
      sizeof(GLubyte),        1,                   alignof(GLubyte),    true
   },
   //UI16
   {
      GL_UNSIGNED_SHORT,      GL_RED_INTEGER,      GL_R16UI,
      sizeof(GLushort),       1,                   alignof(GLushort),   true
   },
   //UI32
   {
      GL_UNSIGNED_INT,        GL_RED_INTEGER,      GL_R32UI,
      sizeof(GLuint),         1,                   alignof(GLuint),     true
   },

   // VEC2_I8
   {
      GL_BYTE,                GL_RG_INTEGER,       GL_RG8I,
      2 * sizeof(GLbyte),     2,                   alignof(GLbyte),     true
   },
   // VEC2_I16
   {
      GL_SHORT,               GL_RG_INTEGER,       GL_RG16I,
      2 * sizeof(GLshort),    2,                   alignof(GLshort),    true
   },
   // VEC2_I32
   {
      GL_INT,                 GL_RG_INTEGER,       GL_RG32I,
      2 * sizeof(GLint),      2,                   alignof(GLint),      true
   },
   // VEC2_F16
   {
      GL_HALF_FLOAT,          GL_RG,               GL_RG16F,
      2 * sizeof(GLhalf),     2,                   alignof(GLhalf),     false
   },
   // VEC2_F32
   {
      GL_FLOAT,               GL_RG,               GL_RG32F,
      2 * sizeof(GLfloat),    2,                   alignof(GLfloat),    false
   },
   // VEC2_UI8
   {
      GL_UNSIGNED_BYTE,       GL_RG_INTEGER,       GL_RG8UI,
      2 * sizeof(GLubyte),    2,                   alignof(GLubyte),    true
   },
   // VEC2_UI16
   {
      GL_UNSIGNED_SHORT,      GL_RG_INTEGER,       GL_RG16UI,
      2 * sizeof(GLushort),   2,                   alignof(GLushort),   true
   },
   // VEC2_UI32
   {
      GL_UNSIGNED_INT,        GL_RG_INTEGER,       GL_RG32UI,
      2 * sizeof(GLuint),     2,                   alignof(GLuint),     true
   },

   // VEC3_I8
   {
      GL_BYTE,                GL_RGB_INTEGER,      GL_RGB8I,
      3 * sizeof(GLbyte),     3,                   alignof(GLbyte),     true
   },
   // VEC3_I16
   {
      GL_SHORT,               GL_RGB_INTEGER,      GL_RGB16I,
      3 * sizeof(GLshort),    3,                   alignof(GLshort),    true
   },
   // VEC3_I32
   {
      GL_INT,                 GL_RGB_INTEGER,      GL_RGB32I,
      3 * sizeof(GLint),      3,                   alignof(GLint),      true
   },
   // VEC3_F16
   {
      GL_HALF_FLOAT,          GL_RGB,              GL_RGB16F,
      3 * sizeof(GLhalf),     3,                   alignof(GLhalf),     false
   },
   // VEC3_F32
   {
      GL_FLOAT,               GL_RGB,              GL_RGB32F,
      3 * sizeof(GLfloat),    3,                   alignof(GLfloat),    false
   },
   // VEC3_UI8
   {
      GL_UNSIGNED_BYTE,       GL_RGB_INTEGER,      GL_RGB8UI,
      3 * sizeof(GLubyte),    3,                   alignof(GLubyte),    true
   },
   // VEC3_UI16
   {
      GL_UNSIGNED_SHORT,      GL_RGB_INTEGER,      GL_RGB16UI,
      3 * sizeof(GLushort),   3,                   alignof(GLushort),   true
   },
   // VEC3_UI32
   {
      GL_UNSIGNED_INT,        GL_RGB_INTEGER,      GL_RGB32UI,
      3 * sizeof(GLuint),     3,                   alignof(GLuint),     true
   },

   // VEC4_I8
   {
      GL_BYTE,                GL_RGBA_INTEGER,     GL_RGBA8I,
      4 * sizeof(GLbyte),     4,                   alignof(GLbyte),     true
   },
   // VEC4_I16
   {
      GL_SHORT,               GL_RGBA_INTEGER,     GL_RGBA16I,
      4 * sizeof(GLshort),    4,                   alignof(GLshort),    true
   },
   // VEC4_I32
   {
      GL_INT,                 GL_RGBA_INTEGER,     GL_RGBA32I,
      4 * sizeof(GLint),      4,                   alignof(GLint),      true
   },
   // VEC4_F16
   {
      GL_HALF_FLOAT,          GL_RGBA,             GL_RGBA16F,
      4 * sizeof(GLhalf),     4,                   alignof(GLhalf),     false
   },
   // VEC4_F32
   {
      GL_FLOAT,               GL_RGBA,             GL_RGBA32F,
      4 * sizeof(GLfloat),    4,                   alignof(GLfloat),    false
   },
   // VEC4_UI8
   {
      GL_UNSIGNED_BYTE,       GL_RGBA_INTEGER,     GL_RGBA8UI,
      4 * sizeof(GLubyte),    4,                   alignof(GLubyte),    true
   },
   // VEC4_UI16
   {
      GL_UNSIGNED_SHORT,      GL_RGBA_INTEGER,     GL_RGBA16UI,
      4 * sizeof(GLushort),   4,                   alignof(GLushort),   true
   },
   // VEC4_UI32
   {
      GL_UNSIGNED_INT,        GL_RGBA_INTEGER,     GL_RGBA32UI,
      4 * sizeof(GLuint),     4,                   alignof(GLuint),     true
   },
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
         const char* shaderTypeStr;
         switch (shaderType) {
         case VERTEX_SHADER:
            shaderTypeStr = "VERTEX SHADER";
            break;
         case FRAGMENT_SHADER:
            shaderTypeStr = "FRAGMENT SHADER";
            break;
         case COMPUTE_SHADER:
            shaderTypeStr = "COMPUTE SHADER";
            break;
         default:
            GPUT_ASSERT(false, "Invalid shader type");
         }
         GLC(glGetShaderInfoLog(shaderId, INFOLOG_SIZE, NULL, infolog));
         GPUT_ASSERT(success, "%s compile error:\n%s", shaderTypeStr, infolog);
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

GlProgId gla_createComputeProg(const char* cmpShaderSrcs[], int srcsCount)
{
   GlShaderId cmpShaderId = gla_createShader(
      COMPUTE_SHADER, cmpShaderSrcs, srcsCount
   );

   GlProgId progId = GLC(glCreateProgram());
   GLC(glAttachShader(progId, cmpShaderId));
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
   GLC(glBufferData(bufferType, size, bufferData, GL_DYNAMIC_READ));
   GLC(glBindBuffer(bufferType, 0));
   return BufferId;
}

void gla_bindBuffer(BufferType bufferType, GlBuffId bufferId)
{
   GLC(glBindBuffer(bufferType, bufferId));
}

void gla_bindComputeStorageBuffer(GlBuffId bufferId, int index)
{
   GLC(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, bufferId));
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

void gla_setVertexLayout(GlDataType attribsDataTypes[], int attribsCount)
{

   AttribInfo attribsInfo[attribsCount];
   GlDataType dType = attribsDataTypes[0];
   char maxAlignment = dataTypesInfo[dType].alignment;
   GLsizei stride = dataTypesInfo[dType].size;

   attribsInfo[0].attribPtr = (void*)0;
   attribsInfo[0].type = dataTypesInfo[dType].glType;
   attribsInfo[0].elementsCount = dataTypesInfo[dType].elementsCount;
   attribsInfo[0].isInteger = dataTypesInfo[dType].isInteger;

   for (int i = 1; i < attribsCount; i++) {

      dType = attribsDataTypes[i];
      char alignment = dataTypesInfo[dType].alignment;
      if (alignment > maxAlignment) {
         maxAlignment = alignment;
      }
      char alignmentOffset = stride % alignment;

      if (alignmentOffset != 0) {
         stride += alignment - alignmentOffset;
      }
      attribsInfo[i].attribPtr = (void*)((long)stride);
      attribsInfo[i].type = dataTypesInfo[dType].glType;
      attribsInfo[i].elementsCount = dataTypesInfo[dType].elementsCount;
      attribsInfo[i].isInteger = dataTypesInfo[dType].isInteger;

      stride += dataTypesInfo[dType].size;
   }

   char globalAlignmentOffset = stride % maxAlignment;
   if (globalAlignmentOffset != 0) {
      stride += maxAlignment - globalAlignmentOffset;
   }

   for (int i = 0; i < attribsCount; i++) {
      GLC(glEnableVertexAttribArray(i));
      if (attribsInfo[i].isInteger) {
         GLC(glVertexAttribIPointer(
            i, attribsInfo[i].elementsCount, attribsInfo[i].type,
            stride, attribsInfo[i].attribPtr
         ));
      }
      else {
         GLC(glVertexAttribPointer(
            i, attribsInfo[i].elementsCount, attribsInfo[i].type,
            GL_FALSE, stride, attribsInfo[i].attribPtr
         ));
      }
   }
}

void gla_readPixelData(int width, int height, void* dataBuffer)
{
  GLenum readFormat;
  GLenum readType;

  GLC(glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, &readFormat));
  GLC(glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_TYPE, &readType));

  GLC(glReadPixels(0, 0, width, height, readFormat, readType, dataBuffer));
}

GlUniformId gla_getUniformId(GlProgId programId, const char* uniformName)
{
   GLC(glGetUniformLocation(programId, uniformName));
}
