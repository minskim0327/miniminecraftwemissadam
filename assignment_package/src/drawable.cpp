#include "drawable.h"
#include <glm_includes.h>

Drawable::Drawable(OpenGLContext* context)
    : m_count(-1), m_count_opq(-1), m_count_tran(-1), m_count_npc_opq(-1),
      m_bufIdx(), m_bufPos(), m_bufNor(), m_bufCol(), m_bufAllOpaque(), m_bufAllTransparent(),
      m_bufIdxOpq(), m_bufIdxTran(), m_bufNPCIdxOpq(), m_bufNPCAllOpq(), m_npcIdxOpqGenerated(false), m_npcAllOpqGenerated(false),
      m_idxGenerated(), m_posGenerated(), m_norGenerated(), m_colGenerated(),
      m_idxOpqGenerated(false), m_idxTranGenerated(false),
      m_allOpaqueGenerated(false), m_allTransparentGenerated(false),
      mp_context(context)
{}

Drawable::~Drawable()
{}


void Drawable::destroy()
{
    mp_context->glDeleteBuffers(1, &m_bufAllOpaque);
    mp_context->glDeleteBuffers(1, &m_bufAllTransparent);
    mp_context->glDeleteBuffers(1, &m_bufIdxOpq);
    mp_context->glDeleteBuffers(1, &m_bufIdxTran);
    mp_context->glDeleteBuffers(1, &m_bufNPCIdxOpq);
    mp_context->glDeleteBuffers(1, &m_bufNPCAllOpq);
    m_allOpaqueGenerated = m_allTransparentGenerated = m_idxOpqGenerated = m_idxTranGenerated = false;
    m_npcAllOpqGenerated = m_npcIdxOpqGenerated = false;

    m_count_tran = -1;
    m_count_opq = -1;
    m_count_npc_opq = -1;
}

GLenum Drawable::drawMode()
{
    // Since we want every three indices in bufIdx to be
    // read to draw our Drawable, we tell that the draw mode
    // of this Drawable is GL_TRIANGLES

    // If we wanted to draw a wireframe, we would return GL_LINES

    return GL_TRIANGLES;
}

int Drawable::elemCount()
{
    return m_count;
}

int Drawable::elemCountOpq()
{
    return m_count_opq;
}

int Drawable::elemCountTran()
{
    return m_count_tran;
}

int Drawable::elemCountNPCOpq() {
    return m_count_npc_opq;
}

void Drawable::generatePos()
{
    m_posGenerated = true;
    // Create a VBO on our GPU and store its handle in bufPos
    mp_context->glGenBuffers(1, &m_bufPos);
}

void Drawable::generateNor()
{
    m_norGenerated = true;
    // Create a VBO on our GPU and store its handle in bufNor
    mp_context->glGenBuffers(1, &m_bufNor);
}

void Drawable::generateCol()
{
    m_colGenerated = true;
    // Create a VBO on our GPU and store its handle in bufCol
    mp_context->glGenBuffers(1, &m_bufCol);
}

void Drawable::generateIdx()
{
    m_idxGenerated = true;
    // Create a VBO on our GPU and store its handle in bufIdx
    mp_context->glGenBuffers(1, &m_bufIdx);
}


void Drawable::generateIdxOpq()
{
    m_idxOpqGenerated = true;
    // Create a VBO on our GPU and store its handle in bufIdx
    mp_context->glGenBuffers(1, &m_bufIdxOpq);
}

void Drawable::generateIdxTran()
{
    m_idxTranGenerated = true;
    // Create a VBO on our GPU and store its handle in bufIdx
    mp_context->glGenBuffers(1, &m_bufIdxTran);
}


//(Elaine 1st)
void Drawable::generateAllOpaque() {
    mp_context->glGenBuffers(1, &m_bufAllOpaque);
    m_allOpaqueGenerated = true;
}

void Drawable::generatedAllTransparent() {
    mp_context->glGenBuffers(1, &m_bufAllTransparent);
    m_allTransparentGenerated = true;
}

// MIN MS3
void Drawable::generateNPCIdxOpq() {
    mp_context->glGenBuffers(1, &m_bufNPCIdxOpq);
    m_npcIdxOpqGenerated = true;
}

void Drawable::generateNPCAllOpq() {
    mp_context->glGenBuffers(1, &m_bufNPCAllOpq);
    m_npcAllOpqGenerated = true;
}

bool Drawable::bindPos()
{
    if(m_posGenerated){
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufPos);
    }
    return m_posGenerated;
}

bool Drawable::bindNor()
{
    if(m_norGenerated){
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufNor);
    }
    return m_norGenerated;
}

bool Drawable::bindCol()
{
    if(m_colGenerated){
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufCol);
    }
    return m_colGenerated;
}

bool Drawable::bindIdxOpq()
{
    if(m_idxOpqGenerated) {
        mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufIdxOpq);
    }
    return m_idxOpqGenerated;
}

bool Drawable::bindIdx()
{
    if(m_idxGenerated) {
        mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufIdx);
    }
    return m_idxGenerated;
}

bool Drawable::bindIdxTran()
{
    if(m_idxTranGenerated) {
        mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufIdxTran);
    }
    return m_idxTranGenerated;
}

//(Elaine 1st)
bool Drawable::bindAllOpaque() {
    if (m_allOpaqueGenerated) {
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufAllOpaque);
    }
    return m_allOpaqueGenerated;
}

bool Drawable::bindAllTransparent() {
    if (m_allTransparentGenerated) {
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufAllTransparent);
    }
    return m_allTransparentGenerated;
}

bool Drawable::bindNPCIdxOpq() {
    if(m_npcIdxOpqGenerated) {
            mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufNPCIdxOpq);
        }
    return m_npcIdxOpqGenerated;
}

bool Drawable::bindNPCAllOpq() {
    if(m_npcAllOpqGenerated) {
            mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufNPCAllOpq);
        }
    return m_npcAllOpqGenerated;
}


