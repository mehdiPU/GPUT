
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

float data[8][8][4];

void gput_test()
{
   const char* glVersion = GLC(glGetString(GL_VERSION));
   GPUT_LOG_INFO("OpenGL version: %s", glVersion);

   const char* glslVersion = GLC(glGetString(GL_SHADING_LANGUAGE_VERSION));
   GPUT_LOG_INFO("GLSL version: %s", glslVersion);

   const char* VSSrc = "#version 310 es\n"
      "layout (location = 0) in vec3 aPos;\n"
      "out lowp vec4 color;"
      "void main()"
      "{"
      "  color = vec4(1.1, 1.1, 1.1, 1.1);"
      "  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);"
      "}\n";

   GlShaderId VSid = gla_createShader(VERTEX_SHADER, &VSSrc, 1);

   const char* FSSrc = "#version 310 es\n"
   ""
   "in lowp vec4 color;"
   "out lowp vec4 FragColor;\n"
   "void main()\n"
   "{\n"
   "  FragColor = color;\n"
   "}\n";

   GlShaderId FSid = gla_createShader(FRAGMENT_SHADER, &FSSrc, 1);

   GlProgId ShProgId = gla_linkProgram(VSid, FSid);

   GLuint fbo;
   GLC(glGenFramebuffers(1, &fbo));
   GLC(glBindFramebuffer(GL_FRAMEBUFFER, fbo));

   GLuint texture;
   GLC(glGenTextures(1, &texture));
   GLC(glBindTexture(GL_TEXTURE_2D, texture));
   GLC(glTexImage2D(
      GL_TEXTURE_2D, 0, GL_RGBA32F, 8, 8, 0, GL_RGBA, GL_FLOAT, NULL
   ));

   GLC(glFramebufferTexture2D(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0
   ));

   GLuint rbo;
   GLC(glGenRenderbuffers(1, &rbo));
   GLC(glBindRenderbuffer(GL_RENDERBUFFER, rbo));
   GLC(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 8, 8));

   GLC(glFramebufferRenderbuffer(
      GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo
   ));

   GLC(GPUT_ASSERT(
      glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE,
      "Framebuffer not complete"
   ));

   float vertices[] = {
       1.0f,  1.0f, 0.0f,
      -1.0f,  1.0f, 0.0f,
       0.0f, -1.0f, 0.0f
   };

   int indices[] = {
      0, 1, 2
   };

   GLuint vao;
   GLC(glGenVertexArrays(1, &vao));
   GLC(glBindVertexArray(vao));

   unsigned int vbo;
   GLC(glGenBuffers(1, &vbo));
   GLC(glBindBuffer(GL_ARRAY_BUFFER, vbo));
   GLC(glBufferData(
      GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW
   ));

   GLuint ebo;
   GLC(glGenBuffers(1, &ebo));
   GLC(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
   GLC(glBufferData(
      GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW
   ));


   GLC(glBindBuffer(GL_ARRAY_BUFFER, vbo))
   GLC(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));


   GLC(glEnableVertexAttribArray(0));
   GLC(glVertexAttribPointer(
      0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0
   ));

   GLC(glViewport(0, 0, 8, 8));

   GLC(glClearColor(0.0f, 0.0f, 0.0f, 0.0f))
   GLC(glClear(GL_COLOR_BUFFER_BIT));

   GLC(glUseProgram(ShProgId));
   GLC(glBindVertexArray(vao));
//
   GLC(glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0));

   GLC(glReadPixels(0, 0, 8, 8, GL_RGBA, GL_FLOAT, data));

   putchar('\n');
   for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 8; j++) {
         printf("[%1.0f %1.0f %1.0f %1.0f]",
            data[i][j][0], data[i][j][1], data[i][j][2], data[i][j][3]
         );
      }
      putchar('\n');
   }

   gla_deleteShader(VSid);
   gla_deleteShader(FSid);
   gla_deleteProgram(ShProgId);
   GLC(glDeleteFramebuffers(1, &fbo));
   GLC(glDeleteTextures(1, &texture))
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
