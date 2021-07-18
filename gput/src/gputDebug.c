

#include "gputDebug.h"

#ifdef GPUT_DEBUG

const char* getGlErrorStr(int errorCode)
{
   switch (errorCode){
   case GL_INVALID_ENUM:
      return "GL_INVALID_ENUM";
   case GL_INVALID_VALUE:
      return "GL_INVALID_VALUE";
   case GL_INVALID_OPERATION:
      return "GL_INVALID_OPERATION";
   case GL_STACK_OVERFLOW:
      return "GL_STACK_OVERFLOW";
   case GL_STACK_UNDERFLOW:
      return "GL_STACK_UNDERFLOW";
   case GL_OUT_OF_MEMORY:
      return "GL_OUT_OF_MEMORY";
   case GL_INVALID_FRAMEBUFFER_OPERATION:
      return "GL_INVALID_FRAMEBUFFER_OPERATION";
   default:
      GPUT_ASSERT(false, "Unsupported GL error code");
   }
}

#endif // ifdef GPUT_DEBUG
