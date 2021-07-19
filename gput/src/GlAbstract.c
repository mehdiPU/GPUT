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

void gla_deleteProgram(GlProgId progId)
{
   GPUT_DEBUG_SCOPE(
      int deleted;
      GLC(glGetProgramiv(progId, GL_DELETE_STATUS, &deleted));
      GPUT_ASSERT(!deleted, "Attempt to delete an already deleted shader")
   )
   GLC(glDeleteProgram(progId))
}
