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

typedef enum {
   VERTEX_SHADER = GL_VERTEX_SHADER,
   FRAGMENT_SHADER = GL_FRAGMENT_SHADER
} ShaderType;

typedef enum {
   VERTEX_BUFFER = GL_ARRAY_BUFFER,
   INDEX_BUFFER = GL_ELEMENT_ARRAY_BUFFER
} BufferType;

GlShaderId gla_createShader(
   ShaderType shaderType, const char* sources[], int srcsCount
);

void gla_deleteShader(GlShaderId shaderId);

GlProgId gla_linkProgram(GlShaderId vertexShader, GlShaderId fragmentShader);

void gla_deleteProgram(GlProgId progId);

GlBuffId gla_createBuffer(
   BufferType bufferType, const void* bufferData, size_t size
);

void gla_bindBuffer(BufferType bufferType, GlBuffId bufferId);

void gla_deleteBuffer(GlBuffId bufferId);
