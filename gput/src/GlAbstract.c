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

static void gla_getGlPixFormatAndType(
   PixelFormat pixFormat, GLenum* format, GLenum* type
){
   switch (pixFormat) {
   case PIX_1_8I:
      *format = GL_RED_INTEGER;
      *type = GL_BYTE;
   break;
   case PIX_1_8UI:
      *format = GL_RED_INTEGER;
      *type = GL_UNSIGNED_BYTE;
   break;
   case PIX_1_16I:
      *format = GL_RED_INTEGER;
      *type = GL_SHORT;
   break;
   case PIX_1_16UI:
      *format = GL_RED_INTEGER;
      *type = GL_UNSIGNED_SHORT;
   break;
   case PIX_1_32I:
      *format = GL_RED_INTEGER;
      *type = GL_INT;
   break;
   case PIX_1_32UI:
      *format = GL_RED_INTEGER;
      *type = GL_UNSIGNED_INT;
   break;
   case PIX_1_16F:
      *format = GL_RED;
      *type = GL_HALF_FLOAT;
   break;
   case PIX_1_32F:
      *format = GL_RED;
      *type = GL_FLOAT;
   break;

   case PIX_2_8I:
      *format = GL_RG_INTEGER;
      *type = GL_BYTE;
   break;
   case PIX_2_8UI:
      *format = GL_RG_INTEGER;
      *type = GL_UNSIGNED_BYTE;
   break;
   case PIX_2_16I:
      *format = GL_RG_INTEGER;
      *type = GL_SHORT;
   break;
   case PIX_2_16UI:
      *format = GL_RG_INTEGER;
      *type = GL_UNSIGNED_SHORT;
   break;
   case PIX_2_32I:
      *format = GL_RG_INTEGER;
      *type = GL_INT;
   break;
   case PIX_2_32UI:
      *format = GL_RG_INTEGER;
      *type = GL_UNSIGNED_INT;
   break;
   case PIX_2_16F:
      *format = GL_RG;
      *type = GL_HALF_FLOAT;
   break;
   case PIX_2_32F:
      *format = GL_RG;
      *type = GL_FLOAT;
   break;

   case PIX_3_8I:
      *format = GL_RGB_INTEGER;
      *type = GL_BYTE;
   break;
   case PIX_3_8UI:
      *format = GL_RGB_INTEGER;
      *type = GL_UNSIGNED_BYTE;
   break;
   case PIX_3_16I:
      *format = GL_RGB_INTEGER;
      *type = GL_SHORT;
   break;
   case PIX_3_16UI:
      *format = GL_RGB_INTEGER;
      *type = GL_UNSIGNED_SHORT;
   break;
   case PIX_3_32I:
      *format = GL_RGB_INTEGER;
      *type = GL_INT;
   break;
   case PIX_3_32UI:
      *format = GL_RGB_INTEGER;
      *type = GL_UNSIGNED_INT;
   break;
   case PIX_3_16F:
      *format = GL_RGB;
      *type = GL_HALF_FLOAT;
   break;
   case PIX_3_32F:
      *format = GL_RGB;
      *type = GL_FLOAT;
   break;

   case PIX_4_8I:
      *format = GL_RGBA_INTEGER;
      *type = GL_BYTE;
   break;
   case PIX_4_8UI:
      *format = GL_RGBA_INTEGER;
      *type = GL_UNSIGNED_BYTE;
   break;
   case PIX_4_16I:
      *format = GL_RGBA_INTEGER;
      *type = GL_SHORT;
   break;
   case PIX_4_16UI:
      *format = GL_RGBA_INTEGER;
      *type = GL_UNSIGNED_SHORT;
   break;
   case PIX_4_32I:
      *format = GL_RGBA_INTEGER;
      *type = GL_INT;
   break;
   case PIX_4_32UI:
      *format = GL_RGBA_INTEGER;
      *type = GL_UNSIGNED_INT;
   break;
   case PIX_4_16F:
      *format = GL_RGBA;
      *type = GL_HALF_FLOAT;
   break;
   case PIX_4_32F:
      *format = GL_RGBA;
      *type = GL_FLOAT;
   break;
   default:
      GPUT_ASSERT(false, "Unsupported pixel format");
   }
}

GlTexId gla_createTexture(
   PixelFormat pixFormat, int width, int height, void* texData
){
   GlTexId textureId;
   GLC(glGenTextures(1, &textureId));
   GLC(glBindTexture(GL_TEXTURE_2D, textureId));

   GLenum extFormat, extType;
   gla_getGlPixFormatAndType(pixFormat, &extFormat, &extType);
   GLC(glTexImage2D(
      GL_TEXTURE_2D, 0, pixFormat, width, height, 0, extFormat, extType, texData
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
