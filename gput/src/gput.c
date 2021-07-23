
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

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "glad/glad.h"

#include <fcntl.h>
#include <gbm.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include "gputDebug.h"
#include "GlAbstract.h"

typedef struct gbm_device GbmDevice;
typedef int DriDeviceFD;

static const char* driDevPath = "/dev/dri/renderD128";
static const char* eglVersion;
static const char* eglExtentions;

static DriDeviceFD driDevice;
static GbmDevice* gbmDevice;
static EGLDisplay eglDisplay;
static EGLContext coreContext;

bool gput_init()
{
   bool returnVal;

   GPUT_LOG_INIT();

   driDevice = open(driDevPath, O_RDWR);
   GPUT_ASSERT(driDevice != -1, "Could not open dri device %s", driDevPath);

   gbmDevice = gbm_create_device(driDevice);
   GPUT_ASSERT(gbmDevice != NULL, "Could not get a GBM device")

   eglDisplay = eglGetDisplay(gbmDevice);
   GPUT_ASSERT(eglDisplay != EGL_NO_DISPLAY, "Could not get an EGL display");

   EGLint major, minor;
   returnVal = eglInitialize(eglDisplay, &major, &minor);
   GPUT_ASSERT(returnVal, "Failed to initialize EGL");

   eglVersion = eglQueryString(eglDisplay, EGL_VERSION);
   GPUT_LOG_INFO("EGL version: %s", eglVersion);

   eglExtentions = eglQueryString(eglDisplay, EGL_EXTENSIONS);
   GPUT_LOG_TRACE("EGL extentions: %s", eglExtentions);

   GPUT_ASSERT(strstr(eglExtentions, "EGL_KHR_create_context"),
      "EGL_KHR_create_context extention not supported"
   )
   GPUT_ASSERT(strstr(eglExtentions, "EGL_KHR_surfaceless_context"),
      "EGL_KHR_surfaceless_context extention not supported"
   )

   const EGLint eglConfigAttribs[] = {
      EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR,
      EGL_NONE
   };

   EGLConfig eglConfig;
   EGLint eglConfigCount;

   returnVal = eglChooseConfig(
      eglDisplay, eglConfigAttribs, &eglConfig, 1, &eglConfigCount
   );
   GPUT_ASSERT(returnVal, "Failed to configure EGL");

   returnVal = eglBindAPI(EGL_OPENGL_ES_API);
   GPUT_ASSERT(returnVal, "Failed to bind OpenGL API to EGL");

   const EGLint contextAttribs[] = {
      EGL_CONTEXT_CLIENT_VERSION, 3,
      EGL_NONE
   };
   coreContext = eglCreateContext(
      eglDisplay, eglConfig, EGL_NO_CONTEXT, contextAttribs
   );
   GPUT_ASSERT(coreContext != EGL_NO_CONTEXT, "Could not create OpenGL context");

   returnVal = eglMakeCurrent(
      eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, coreContext
   );
   GPUT_ASSERT(returnVal, "Could not make context current");

   returnVal = gladLoadGLES2Loader((GLADloadproc) eglGetProcAddress);
   GPUT_ASSERT(returnVal, "Failed to load opengl function pointers");

   return true;
}

#define TEX_WIDTH    7
#define TEX_HEIGHT   7

vec4_f32 data[TEX_WIDTH][TEX_HEIGHT];

void gput_test()
{
   const char* glVersion = GLC(glGetString(GL_VERSION));
   GPUT_LOG_INFO("OpenGL version: %s", glVersion);

   const char* glslVersion = GLC(glGetString(GL_SHADING_LANGUAGE_VERSION));
   GPUT_LOG_INFO("GLSL version: %s", glslVersion);

   const char* VSSrc = "#version 310 es\n"
      "layout (location = 0) in vec2 aPos;\n"
      "layout (location = 1) in int aShort;\n"
      "layout (location = 2) in vec3 aColor;\n"
      "layout (location = 3) in int aByte;\n"
      "out vec3 vColor;\n"
      "out flat int shortI;\n"
      "out flat int byteI;\n"
      "void main()\n"
      "{\n"
      "  vColor = aColor;\n"
      "  shortI = aShort;\n"
      "  byteI = aByte;\n"
      "  gl_Position = vec4(aPos.x, aPos.y, 0.0f, 1.0f);\n"
      "}\n";

   GlShaderId VSid = gla_createShader(VERTEX_SHADER, &VSSrc, 1);

   const char* FSSrc = "#version 310 es\n"
   "in highp vec3 vColor;"
   "in flat int shortI;"
   "in flat int byteI;"
   "out highp vec4 FragColor;\n"
   "void main()\n"
   "{\n"
   "  FragColor = vec4(vColor.x, float(shortI), float(byteI), 0.0f);\n"
   "}\n";

   GlShaderId FSid = gla_createShader(FRAGMENT_SHADER, &FSSrc, 1);

   GlProgId ShProgId = gla_linkProgram(VSid, FSid);

   GlTexId textureId = gla_createTexture(VEC4_F32, TEX_WIDTH, TEX_HEIGHT, data);

   GlFramebufferId framebufferId = gla_createFramebuffer(textureId);

   typedef struct {
      vec2_f32 position;
      i16 shortI;
      vec3_f32 color;
      i8 character;
   } vertex;

   GPUT_LOG_INFO(
      "vert.pos=%d, vert.shortI=%d, vert.color=%d, vert.character=%d",
      offsetof(vertex, position), offsetof(vertex, shortI),
      offsetof(vertex, color), offsetof(vertex, character)
   );
   GPUT_LOG_INFO("sizeof(bool)=%d", sizeof(bool));

   vertex vertices[] = {
      {
         {1.0f, 1.0f},
         5,
         {9.0f, 0.0f, 0.0f},
         3
      },{
         {-1.0f, 1.0f},
         8,
         {0.0f, 9.0f, 0.0f},
         1
      },{
         {0.0f, -1.0f},
         9,
         {0.0f, 0.0f, 9.0f},
         6
      }
   };

   vec3_ui32 indices[] = {
      {0, 1, 2}
   };

   GlBuffId vertexBuffer = gla_createBuffer(
      VERTEX_BUFFER, vertices, sizeof(vertices)
   );
   GlBuffId indexBuffer = gla_createBuffer(
      INDEX_BUFFER, indices, sizeof(indices)
   );

   //GLuint vao;
   //GLC(glGenVertexArrays(1, &vao));
   //GLC(glBindVertexArray(vao));

   gla_bindBuffer(VERTEX_BUFFER, vertexBuffer);
   gla_bindBuffer(INDEX_BUFFER, indexBuffer);

   GlDataType vertexLayout[] = {VEC2_F32, I32, VEC3_F32, I32};
   gla_setVertexLayout(vertexLayout, 4);

   GLC(glViewport(0, 0, TEX_WIDTH, TEX_HEIGHT));

   gla_bindFramebuffer(framebufferId);
   gla_bindProgram(ShProgId);
   //GLC(glBindVertexArray(vao));

   GLC(glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0));

   GLenum readFormat, readType;
   GLC(glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, &readFormat));
   GLC(glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_TYPE, &readType));

   GLC(glReadPixels(0, 0, TEX_WIDTH, TEX_HEIGHT, readFormat, readType, data));

   putchar('\n');
   for (int i = 0; i < TEX_WIDTH; i++) {
      for (int j = 0; j < TEX_HEIGHT; j++) {
         printf("[%.1f|%.1f|%.1f]",
            data[i][j].x, data[i][j].y, data[i][j].z
         );
      }
      putchar('\n');
   }
   putchar('\n');

   GPUT_LOG_INFO("%-10s%-10s", "macros", "returned");
   GPUT_LOG_INFO("%-10p%-10p", GL_RGBA, readFormat);
   GPUT_LOG_INFO("%-10p%-10p", GL_FLOAT, readType);

   gla_deleteShader(VSid);
   gla_deleteShader(FSid);
   gla_deleteProgram(ShProgId);
   gla_deleteFramebuffer(framebufferId);
   gla_deleteTexture(textureId);
}

bool gput_terminate()
{
   bool returnVal;

   returnVal = eglDestroyContext(eglDisplay, coreContext);
   GPUT_ASSERT(returnVal, "Could not destroy core context");

   returnVal = eglTerminate(eglDisplay);
   GPUT_ASSERT(returnVal, "Failed to terminate EGL");

   gbm_device_destroy(gbmDevice);

   close(driDevice);

   return true;
}
