// Based on: http://www.cs.unm.edu/~angel/BOOK/INTERACTIVE_COMPUTER_GRAPHICS/SIXTH_EDITION/CODE/CHAPTER03/WINDOWS_VERSIONS/example2.cpp
// Modified to isolate the main program and use GLM

#include "common.h"

#include <iostream>

#include "imgui.h"
#include "imgui_impl_glut.h"
#include "imgui_impl_opengl3.h"

// Create a NULL-terminated string by reading the provided file
static char*
readShaderSource(const char* shaderFile)
{
   FILE* fp = fopen(shaderFile, "rb");

   if ( fp == NULL ) { return NULL; }

   fseek(fp, 0L, SEEK_END);
   long size = ftell(fp);

   fseek(fp, 0L, SEEK_SET);
   char* buf = new char[size + 1];
   fread(buf, 1, size, fp);

   buf[size] = '\0';
   fclose(fp);

   return buf;
}


// Create a GLSL program object from vertex and fragment shader files
GLuint
InitShader(const char* vShaderFile, const char* fShaderFile)
{
   struct Shader {
      const char*  filename;
      GLenum       type;
      GLchar*      source;
   }  shaders[2] = {
      { vShaderFile, GL_VERTEX_SHADER, NULL },
      { fShaderFile, GL_FRAGMENT_SHADER, NULL }
   };

   GLuint program = glCreateProgram();
    
   for ( int i = 0; i < 2; ++i ) {
      Shader& s = shaders[i];
      s.source = readShaderSource( s.filename );
      if ( shaders[i].source == NULL ) {
         std::cerr << "Failed to read " << s.filename << std::endl;
         exit( EXIT_FAILURE );
      }

      GLuint shader = glCreateShader( s.type );
      glShaderSource( shader, 1, (const GLchar**) &s.source, NULL );
      glCompileShader( shader );

      GLint  compiled;
      glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
      if ( !compiled ) {
         std::cerr << s.filename << " failed to compile:" << std::endl;
         GLint  logSize;
         glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logSize );
         char* logMsg = new char[logSize];
         glGetShaderInfoLog( shader, logSize, NULL, logMsg );
         std::cerr << logMsg << std::endl;
         delete [] logMsg;

         exit( EXIT_FAILURE );
      }

      delete [] s.source;

      glAttachShader( program, shader );
   }

   /* link  and error check */
   glLinkProgram(program);

   GLint  linked;
   glGetProgramiv( program, GL_LINK_STATUS, &linked );
   if ( !linked ) {
      std::cerr << "Shader program failed to link" << std::endl;
      GLint  logSize;
      glGetProgramiv( program, GL_INFO_LOG_LENGTH, &logSize);
      char* logMsg = new char[logSize];
      glGetProgramInfoLog( program, logSize, NULL, logMsg );
      std::cerr << logMsg << std::endl;
      delete [] logMsg;

      exit( EXIT_FAILURE );
   }

   /* use program object */
   glUseProgram(program);

   return program;
}



void
timer(int unused)
{
   update();
   glutPostRedisplay();
   glutTimerFunc( FRAME_RATE_MS, timer, 0 );
}

void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id,
	GLenum severity, GLsizei length,
	const GLchar* msg, const void* data)
{
	char* _source;
	char* _type;
	char* _severity;

	switch (source) {
	case GL_DEBUG_SOURCE_API:
		_source = "API";
		break;

	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		_source = "WINDOW SYSTEM";
		break;

	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		_source = "SHADER COMPILER";
		break;

	case GL_DEBUG_SOURCE_THIRD_PARTY:
		_source = "THIRD PARTY";
		break;

	case GL_DEBUG_SOURCE_APPLICATION:
		_source = "APPLICATION";
		break;

	case GL_DEBUG_SOURCE_OTHER:
		_source = "UNKNOWN";
		break;

	default:
		_source = "UNKNOWN";
		break;
	}

	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		_type = "ERROR";
		break;

	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		_type = "DEPRECATED BEHAVIOR";
		break;

	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		_type = "UDEFINED BEHAVIOR";
		break;

	case GL_DEBUG_TYPE_PORTABILITY:
		_type = "PORTABILITY";
		break;

	case GL_DEBUG_TYPE_PERFORMANCE:
		_type = "PERFORMANCE";
		break;

	case GL_DEBUG_TYPE_OTHER:
		_type = "OTHER";
		break;

	case GL_DEBUG_TYPE_MARKER:
		_type = "MARKER";
		break;

	default:
		_type = "UNKNOWN";
		break;
	}

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:
		_severity = "HIGH";
		break;

	case GL_DEBUG_SEVERITY_MEDIUM:
		_severity = "MEDIUM";
		break;

	case GL_DEBUG_SEVERITY_LOW:
		_severity = "LOW";
		break;

	case GL_DEBUG_SEVERITY_NOTIFICATION:
		_severity = "NOTIFICATION";
		return;
		break;

	default:
		_severity = "UNKNOWN";
		break;
	}

	printf("%d: %s of %s severity, raised from %s: %s\n",
		id, _type, _severity, _source, msg);

}

int
main( int argc, char **argv )
{
   glutInit( &argc, argv );
   glutSetOption(GLUT_MULTISAMPLE, 16);
   glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE );
   glutInitWindowSize( 1400, 900 );
   glutInitContextVersion( 3, 2 );
   glutInitContextProfile( GLUT_CORE_PROFILE );
   glutCreateWindow( WINDOW_TITLE );

#ifdef EXPERIMENTAL
   glewExperimental = GL_TRUE;
#endif
   
   glewInit();

   // Disable vsync
   /*if (wglewIsSupported("WGL_EXT_swap_control")) {
      wglSwapIntervalEXT(0);
   }*/

   /*glEnable(GL_DEBUG_OUTPUT);
   glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
   glDebugMessageCallback(GLDebugMessageCallback, nullptr);*/

   init();

   glutDisplayFunc( display );

   // ImGui Setup
   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImGuiIO& io = ImGui::GetIO(); (void)io;
   io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
   ImGui::StyleColorsDark();
   ImGui_ImplGLUT_Init();
   ImGui_ImplOpenGL3_Init();

   glutKeyboardFunc( keyboard );
   glutKeyboardUpFunc(keyboardUp);
   glutSpecialFunc(special);
   glutSpecialUpFunc(specialUp);
   glutMouseFunc( mouse );
   glutMouseWheelFunc(mouseWheel);
   glutMotionFunc(motion);
   glutPassiveMotionFunc(passiveMotion);
   glutReshapeFunc(reshape);
   glutTimerFunc(FRAME_RATE_MS, timer, 0);

   //ImGui_ImplGLUT_InstallFuncs();
   
   glutMainLoop();

   // ImGui Teardown
   ImGui_ImplOpenGL3_Shutdown();
   ImGui_ImplGLUT_Shutdown();
   ImGui::DestroyContext();

   return 0;
}
