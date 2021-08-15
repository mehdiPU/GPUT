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
   FRAGMENT_SHADER = GL_FRAGMENT_SHADER,
   COMPUTE_SHADER = GL_COMPUTE_SHADER
} ShaderType;

typedef enum {
   VERTEX_BUFFER = GL_ARRAY_BUFFER,
   INDEX_BUFFER = GL_ELEMENT_ARRAY_BUFFER
} BufferType;

typedef GLbyte    i8;
typedef GLshort   i16;
typedef GLint     i32;
typedef GLhalf    f16;
typedef GLfloat   f32;
typedef GLubyte   ui8;
typedef GLushort  ui16;
typedef GLuint    ui32;

typedef struct {
   i8 x;
   i8 y;
} vec2_i8;

typedef struct {
   i16 x;
   i16 y;
} vec2_i16;

typedef struct {
   i32 x;
   i32 y;
} vec2_i32;

typedef struct {
   f16 x;
   f16 y;
} vec2_f16;

typedef struct {
   f32 x;
   f32 y;
} vec2_f32;

typedef struct {
   ui8 x;
   ui8 y;
} vec2_ui8;

typedef struct {
   ui16 x;
   ui16 y;
} vec2_ui16;

typedef struct {
   ui32 x;
   ui32 y;
} vec2_ui32;

typedef struct {
   i8 x;
   i8 y;
   i8 z;
} vec3_i8;

typedef struct {
   i16 x;
   i16 y;
   i16 z;
} vec3_i16;

typedef struct {
   i32 x;
   i32 y;
   i32 z;
} vec3_i32;

typedef struct {
   f16 x;
   f16 y;
   f16 z;
} vec3_f16;

typedef struct {
   f32 x;
   f32 y;
   f32 z;
} vec3_f32;

typedef struct {
   ui8 x;
   ui8 y;
   ui8 z;
} vec3_ui8;

typedef struct {
   ui16 x;
   ui16 y;
   ui16 z;
} vec3_ui16;

typedef struct {
   ui32 x;
   ui32 y;
   ui32 z;
} vec3_ui32;

typedef struct {
   i8 x;
   i8 y;
   i8 z;
   i8 w;
} vec4_i8;

typedef struct {
   i16 x;
   i16 y;
   i16 z;
   i16 w;
} vec4_i16;

typedef struct {
   i32 x;
   i32 y;
   i32 z;
   i32 w;
} vec4_i32;

typedef struct {
   f16 x;
   f16 y;
   f16 z;
   f16 w;
} vec4_f16;

typedef struct {
   f32 x;
   f32 y;
   f32 z;
   f32 w;
} vec4_f32;

typedef struct {
   ui8 x;
   ui8 y;
   ui8 z;
   ui8 w;
} vec4_ui8;

typedef struct {
   ui16 x;
   ui16 y;
   ui16 z;
   ui16 w;
} vec4_ui16;

typedef struct {
   ui32 x;
   ui32 y;
   ui32 z;
   ui32 w;
} vec4_ui32;

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

GlProgId gla_createComputeProg(const char* cmpShaderSrcs[], int srcsCount);

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

void gla_setVertexLayout(GlDataType attribsDataTypes[], int attribsCount);

void gla_readPixelData(int width, int height, void* dataBuffer);
