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
#pragma once

#include <stdlib.h>

#include "glad/glad.h"

typedef GLuint GlShaderId;
typedef GLuint GlProgId;
typedef GLuint GlBuffId;
typedef GLuint GlTexId;
typedef GLuint GlFramebufferId;

typedef enum {
   VERTEX_SHADER = GL_VERTEX_SHADER,
   FRAGMENT_SHADER = GL_FRAGMENT_SHADER
} ShaderType;

typedef enum {
   VERTEX_BUFFER = GL_ARRAY_BUFFER,
   INDEX_BUFFER = GL_ELEMENT_ARRAY_BUFFER
} BufferType;

typedef enum {
   I8,
   I16,
   I32,
   F16,
   F32,
   UI8,
   UI16,
   UI32,

   VEC2_I8,
   VEC2_I16,
   VEC2_I32,
   VEC2_F16,
   VEC2_F32,
   VEC2_UI8,
   VEC2_UI16,
   VEC2_UI32,

   VEC3_I8,
   VEC3_I16,
   VEC3_I32,
   VEC3_F16,
   VEC3_F32,
   VEC3_UI8,
   VEC3_UI16,
   VEC3_UI32,

   VEC4_I8,
   VEC4_I16,
   VEC4_I32,
   VEC4_F16,
   VEC4_F32,
   VEC4_UI8,
   VEC4_UI16,
   VEC4_UI32,
} GlDataType;

GlShaderId gla_createShader(
   ShaderType shaderType, const char* sources[], int srcsCount
);

void gla_deleteShader(GlShaderId shaderId);

GlProgId gla_linkProgram(GlShaderId vertexShader, GlShaderId fragmentShader);

void gla_bindProgram(GlProgId progId);

void gla_unbindProgram();

void gla_deleteProgram(GlProgId progId);

GlBuffId gla_createBuffer(
   BufferType bufferType, const void* bufferData, size_t size
);

void gla_bindBuffer(BufferType bufferType, GlBuffId bufferId);

void gla_unbindBuffer(BufferType bufferType);

void gla_deleteBuffer(GlBuffId bufferId);

GlTexId gla_createTexture(
   GlDataType pixDataType, int width, int height, void* texData
);

void gla_bindTexture(GlTexId textureId);

void gla_unbindTexture();

void gla_deleteTexture(GlTexId textureId);

GlFramebufferId gla_createFramebuffer(GlTexId colorAttachment);

void gla_bindFramebuffer(GlFramebufferId framebufferId);

void gla_unbindFramebuffer();

void gla_deleteFramebuffer(GlFramebufferId framebufferId);
