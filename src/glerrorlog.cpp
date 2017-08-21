#include <QTextStream>

#ifdef DEBUG
static
const char* glNumToStr (GLenum err){
    switch (err){
        case 0x0500 :
            return "GL_INVALID_ENUM";
        case 0x0501 :
            return "GL_INVALID_VALUE";
        case 0x0502 :
            return "GL_INVALID_OPERATION";
        case 0x0503 :
            return "GL_STACK_OVERFLOW";
        case 0x0504 :
            return "GL_STACK_UNDERFLOW";
        case 0x0505 :
            return "GL_OUT_OF_MEMORY";
        default:
            return "Undef error";
    }
}
#endif

static
void logGLError (){
#ifdef DEBUG
    for(GLenum err; (err = glGetError()) != GL_NO_ERROR;)
    {
        QTextStream(stdout) <<"OpenGl error: "<< err<< " " << glNumToStr(err)
                            << endl;
    }
#endif
}
