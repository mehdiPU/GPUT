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
   PIX_1_8I    = GL_R8I,
   PIX_1_8UI   = GL_R8UI,
   PIX_1_16I   = GL_R16I,
   PIX_1_16UI  = GL_R16UI,
   PIX_1_32I   = GL_R32I,
   PIX_1_32UI  = GL_R32UI,
   PIX_1_16F   = GL_R16F,
   PIX_1_32F   = GL_R32F,

   PIX_2_8I    = GL_RG8I,
   PIX_2_8UI   = GL_RG8UI,
   PIX_2_16I   = GL_RG16I,
   PIX_2_16UI  = GL_RG16UI,
   PIX_2_32I   = GL_RG32I,
   PIX_2_32UI  = GL_RG32UI,
   PIX_2_16F   = GL_RG16F,
   PIX_2_32F   = GL_RG32F,

   PIX_3_8I    = GL_RGB8I,
   PIX_3_8UI   = GL_RGB8UI,
   PIX_3_16I   = GL_RGB16I,
   PIX_3_16UI  = GL_RGB16UI,
   PIX_3_32I   = GL_RGB32I,
   PIX_3_32UI  = GL_RGB32UI,
   PIX_3_16F   = GL_RGB16F,
   PIX_3_32F   = GL_RGB32F,

   PIX_4_8I    = GL_RGBA8I,
   PIX_4_8UI   = GL_RGBA8UI,
   PIX_4_16I   = GL_RGBA16I,
   PIX_4_16UI  = GL_RGBA16UI,
   PIX_4_32I   = GL_RGBA32I,
   PIX_4_32UI  = GL_RGBA32UI,
   PIX_4_16F   = GL_RGBA16F,
   PIX_4_32F   = GL_RGBA32F,
} PixelFormat;

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
   PixelFormat pixFormat, int width, int height, void* texData
);

void gla_bindTexture(GlTexId textureId);

void gla_unbindTexture();

void gla_deleteTexture(GlTexId textureId);

GlFramebufferId gla_createFramebuffer(GlTexId colorAttachment);

void gla_bindFramebuffer(GlFramebufferId framebufferId);

void gla_unbindFramebuffer();

void gla_deleteFramebuffer(GlFramebufferId framebufferId);
