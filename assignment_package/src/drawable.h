#pragma once
#include <openglcontext.h>
#include <glm_includes.h>

//This defines a class which can be rendered by our shader program.
//Make any geometry a subclass of ShaderProgram::Drawable in order to render it with the ShaderProgram class.
class Drawable
{
protected:
    int m_count;
    int m_count_opq;     // The number of indices stored in bufIdx opaque.
    int m_count_tran; //The number of indices stored in bufIdx opaque.

    GLuint m_bufIdx; // A Vertex Buffer Object that we will use to store triangle indices (GLuints)
    GLuint m_bufPos; // A Vertex Buffer Object that we will use to store mesh vertices (vec4s)
    GLuint m_bufNor; // A Vertex Buffer Object that we will use to store mesh normals (vec4s)
    GLuint m_bufCol; // Can be used to pass per-vertex color information to the shader, but is currently unused.
                   // Instead, we use a uniform vec4 in the shader to set an overall color for the geometry


    //for interleaved data separated buffer for vbo transparent and opaque
    GLuint m_bufAllOpaque;
    GLuint m_bufAllTransparent;

      //for interleaved data separated buffer for idx transparent and opaque
    GLuint m_bufIdxOpq;
    GLuint m_bufIdxTran;

    //not used for terrain
    bool m_idxGenerated; // Set to TRUE by generateIdx(), returned by bindIdx().
    bool m_posGenerated;
    bool m_norGenerated;
    bool m_colGenerated;

    //for interleaved data
    bool m_allOpaqueGenerated;
    bool m_allTransparentGenerated;

    bool m_idxOpqGenerated;
    bool m_idxTranGenerated;

    OpenGLContext* mp_context; // Since Qt's OpenGL support is done through classes like QOpenGLFunctions_3_2_Core,
                          // we need to pass our OpenGL context to the Drawable in order to call GL functions
                          // from within this class.


public:
    Drawable(OpenGLContext* mp_context);
    virtual ~Drawable();

    virtual void create() = 0; // To be implemented by subclasses. Populates the VBOs of the Drawable.
    void destroy(); // Frees the VBOs of the Drawable.

    // Getter functions for various GL data
    virtual GLenum drawMode();
    int elemCount();
    int elemCountOpq();
    int elemCountTran();

     //not used for terrain
    // Call these functions when you want to call glGenBuffers on the buffers stored in the Drawable
    // These will properly set the values of idxBound etc. which need to be checked in ShaderProgram::draw()
    void generateIdx();
    void generatePos();
    void generateNor();
    void generateCol();

    //separated idx for transparent and opaque blocks
    void generateIdxOpq();
    void generateIdxTran();

    //for interleaved data separated for transparent and opaque blocsk
    void generateAllOpaque();
    void generatedAllTransparent();

     //not used for terrain
    bool bindIdx();
    bool bindPos();
    bool bindNor();
    bool bindCol();

    //separated bind idx for transparent and opaque blocsk
    bool bindIdxOpq();
    bool bindIdxTran();

    //for interleaved data separated interleaved for transparent and opaque blocsk
    bool bindAllOpaque();
    bool bindAllTransparent();

};
