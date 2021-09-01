
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

#define M      1000
#define N      250 * 4
#define K      1000

f32 mat0[M][K];
f32 mat1[K][N];
f32 mat2[M][N];

#include <time.h>

void matInit(f32 matrix[], int m, int n)
{
   int elementsCount = m * n;
   for (int i = 0; i < elementsCount; i++) {
      matrix[i] = (rand() % 18) - 9;
   }
}

void printMatrix(f32 matrix[], int m, int n)
{
   for (int i = 0; i < m; i++) {
      for (int j = 0; j < n; j++) {
         printf("%+6.1f ", matrix[i * n + j]);
      }
      putchar('\n');
   }
}

static void gput_GPUMatMul()
{
   const char* CSSrc = "#version 310 es\n"
      "layout(std140, binding = 0) uniform uniformData{\n"
      "  int uColsCount0;\n"
      "  int uColsCount1;\n"
      "  int uColsCount2;\n"
      "};\n"
      "layout(std430, binding = 0) buffer Matrix0 {\n"
      "  float matrix0[];\n"
      "};\n"
      "layout(std430, binding = 1) buffer Matrix1 {\n"
      "  vec4 matrix1[];\n"
      "};\n"
      "layout(std430, binding = 2) buffer Matrix2 {\n"
      "  vec4 matrix2[];\n"
      "};\n"
      "layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;"
      "void main()\n"
      "{\n"
      "  int i = int(gl_WorkGroupID.x);\n"
      "  int j = int(gl_WorkGroupID.y);\n"
      "  int commonDim = uColsCount0;\n"
      "  vec4 value = vec4(0.0, 0.0, 0.0, 0.0);"
      "  for (int k = 0; k < commonDim; k++) {\n"
      "    value += matrix0[(i * uColsCount0) + k] * \n"
      "             matrix1[(k * uColsCount1) + j];\n"
      "  }\n"
      "  matrix2[(i * uColsCount2) + j] = value;\n"
      "}\n";

   GlProgId cmpProgId = gla_createComputeProg(&CSSrc, 1);
   gla_bindProgram(cmpProgId);

   int matsDims[] = {
      K, N / 4, N / 4
   };

   GlBuffId matDimsBuffId = gla_createBuffer(
      UNIFORM_BUFFER, matsDims, 3 * sizeof(int)
   );

   GLC(glBindBufferBase(GL_UNIFORM_BUFFER, 0, matDimsBuffId));

   GlBuffId mat0Id = gla_createBuffer(
      COMPUTE_STORAGE_BUFFER, mat0, M * K * sizeof(float)
   );

   GlBuffId mat1Id = gla_createBuffer(
      COMPUTE_STORAGE_BUFFER, mat1, K * N * sizeof(float)
   );

   GlBuffId mat2Id = gla_createBuffer(
      COMPUTE_STORAGE_BUFFER, NULL, M * N * sizeof(float)
   );

   gla_bindComputeStorageBuffer(mat0Id, 0);
   gla_bindComputeStorageBuffer(mat1Id, 1);
   gla_bindComputeStorageBuffer(mat2Id, 2);

   GLC(glDispatchCompute(M, N / 4, 1));

   gla_bindBuffer(COMPUTE_STORAGE_BUFFER, mat2Id);

   float* readData = GLC(glMapBufferRange(
      COMPUTE_STORAGE_BUFFER, 0, M * N * sizeof(float), GL_MAP_READ_BIT
   ));

   memcpy(mat2, readData, M * N * sizeof(float));

   bool unmapedSuccess = GLC(glUnmapBuffer(COMPUTE_STORAGE_BUFFER));
   GPUT_ASSERT(unmapedSuccess, "Could not unmap the buffer");

   gla_deleteProgram(cmpProgId);
   gla_deleteBuffer(mat0Id);
   gla_deleteBuffer(mat1Id);
   gla_deleteBuffer(mat2Id);
}

static void gput_CPUMatMul()
{
   for (int i = 0; i < M; i++) {
      for (int j = 0; j < N; j++) {
         mat2[i][j] = 0.0f;
         for (int k = 0; k < K; k++) {
            mat2[i][j] += mat0[i][k] * mat1[k][j];
         }
      }
   }
}

void gput_test(bool useGPU)
{
   const char* glVersion = GLC(glGetString(GL_VERSION));
   GPUT_LOG_INFO("OpenGL version: %s", glVersion);

   const char* glslVersion = GLC(glGetString(GL_SHADING_LANGUAGE_VERSION));
   GPUT_LOG_INFO("GLSL version: %s", glslVersion);

   srand(clock());
   matInit((f32*)mat0, M, K);
   matInit((f32*)mat1, K, N);
   if (useGPU) {
      gput_GPUMatMul();
   }
   else {
      gput_CPUMatMul();
   }

   //gput_GPUMatMul();
   //puts("mat0:");
   //printMatrix((f32*)mat0, M, K);
   //puts("mat1:");
   //printMatrix((f32*)mat1, K, N);
   //puts("mat2:");
   //printMatrix((f32*)mat2, M, N);
   //gput_CPUMatMul();
   //puts("mat0:");
   //printMatrix((f32*)mat0, M, K);
   //puts("mat1:");
   //printMatrix((f32*)mat1, K, N);
   //puts("mat2:");
   //printMatrix((f32*)mat2, M, N);
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
