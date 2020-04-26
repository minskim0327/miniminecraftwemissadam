#include "mygl.h"
#include <glm_includes.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>
#include <qdatetime.h>
#include <blocktypeworker.h>
#include <vboworker.h>
#include <QThreadPool>
#include <thread>


MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent),
      m_worldAxes(this),
      m_progLambert(this), m_progFlat(this),
      m_terrain(this), m_player(glm::vec3(48.f, 129.f, 48.f), m_terrain),
      m_currMSecSinceEpoch(QDateTime::currentMSecsSinceEpoch())
{
    // Connect the timer to a function so that when the timer ticks the function is executed
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));
    // Tell the timer to redraw 60 times per second
    m_timer.start(16);
    setFocusPolicy(Qt::ClickFocus);

    setMouseTracking(true); // MyGL will track the mouse's movements even if a mouse button is not pressed
    //setCursor(Qt::BlankCursor); // Make the cursor invisible
    setCursor(Qt::CrossCursor);
}

MyGL::~MyGL() {
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
}


void MyGL::moveMouseToCenter() {
//    setMouseTracking(false);
    QCursor::setPos(this->mapToGlobal(QPoint(width() / 2, height() / 2)));
    //QCursor::setPos(QPoint(width() / 2, height() / 2));
    //std::cout << QCursor::pos().x() << std::endl;
//    setMouseTracking(true);
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.37f, 0.74f, 1.0f, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    //Create the instance of the world axes
    m_worldAxes.create();

    // Create and set up the diffuse shader
    m_progLambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    m_progFlat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");

    // Set a color with which to draw geometry.
    // This will ultimately not be used when you change
    // your program to render Chunks with vertex colors
    // and UV coordinates
    m_progLambert.setGeometryColor(glm::vec4(0,1,0,1));

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    glBindVertexArray(vao);

    m_terrain.CreateTestScene();
    //m_terrain.updateScene(m_player.mcr_position);


}

void MyGL::resizeGL(int w, int h) {
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    m_player.setCameraWidthHeight(static_cast<unsigned int>(w), static_cast<unsigned int>(h));
    glm::mat4 viewproj = m_player.mcr_camera.getViewProj();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)

    m_progLambert.setViewProjMatrix(viewproj);
    m_progFlat.setViewProjMatrix(viewproj);

    printGLErrorLog();

}


// MyGL's constructor links tick() to a timer that fires 60 times per second.
// We're treating MyGL as our game engine class, so we're going to perform
// all per-frame actions here, such as performing physics updates on all
// entities in the scene.
void MyGL::tick() {
    // Calculate dT
    float dT = (QDateTime::currentMSecsSinceEpoch() - m_currMSecSinceEpoch) / 1000.0f;
    m_currMSecSinceEpoch = QDateTime::currentMSecsSinceEpoch();

    m_player.tick(dT, m_inputs);

    // For every terrain generation zone in this radius that does not yet exist in Terrain's m_generatedTerrain,
    // spawn a thread to fill that zone's Chunks with procedural height field BlockType data.
    // Check if new Terrain Zene Chunks need to be crasted an populated
    std::vector<int64_t> terrainNotExpanded = m_terrain.checkExpansion(m_player.getPosition());

    // expected :: 24
    //std::cout << terrainNotExpanded.size() << std::endl;

//    if (terrainNotExpanded.size() != 0) {
//        // Spawn worker threads to populate BlockType data in new Chunks
//        for (unsigned int i = 0; i < terrainNotExpanded.size(); i++) {
//            BlockTypeWorker *bWorker = new BlockTypeWorker(&m_terrain, terrainNotExpanded.at(i), &m_terrain.mutexChunksWithVBOData);
//            QThreadPool::globalInstance()->start(bWorker);
//        }

//        std::cout<<m_terrain.chunksWithOnlyBlockData.size() << std::endl;

//        if (m_terrain.chunksWithOnlyBlockData.size() != 0) {
//            for (Chunk *c : m_terrain.chunksWithOnlyBlockData) {
//                VBOWorker *vboWorker = new VBOWorker(&m_terrain,
//                                                     &m_terrain.chunksWithVBOData,
//                                                     c,
//                                                     &m_terrain.mutexChunksWithVBOData);
//                QThreadPool::globalInstance()->start(vboWorker);
//            }

//        }
//        if (m_terrain.chunksWithVBOData.size() != 0) {
//            for (ChunkVBOData c: m_terrain.chunksWithVBOData) {
//                c.associated_chunk->sendToGPU(&c.vertex_data, &c.idx_data);
//            }
//        }
//    }



    update(); // Calls paintGL() as part of a larger QOpenGLWidget pipeline
    sendPlayerDataToGUI(); // Updates the info in the secondary window displaying player data
    m_terrain.updateScene(m_player.mcr_position, &m_progLambert); //as player moves, send position to create new a chunk (Elaine 1st)
}

void MyGL::sendPlayerDataToGUI() const {
    emit sig_sendPlayerPos(m_player.posAsQString());
    emit sig_sendPlayerVel(m_player.velAsQString());
    emit sig_sendPlayerAcc(m_player.accAsQString());
    emit sig_sendPlayerLook(m_player.lookAsQString());
    glm::vec2 pPos(m_player.mcr_position.x, m_player.mcr_position.z);
    glm::ivec2 chunk(16 * glm::ivec2(glm::floor(pPos / 16.f)));
    glm::ivec2 zone(64 * glm::ivec2(glm::floor(pPos / 64.f)));
    emit sig_sendPlayerChunk(QString::fromStdString("( " + std::to_string(chunk.x) + ", " + std::to_string(chunk.y) + " )"));
    emit sig_sendPlayerTerrainZone(QString::fromStdString("( " + std::to_string(zone.x) + ", " + std::to_string(zone.y) + " )"));
}

// This function is called whenever update() is called.
// MyGL's constructor links update() to a timer that fires 60 times per second,
// so paintGL() called at a rate of 60 frames per second.
void MyGL::paintGL() {
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_progFlat.setViewProjMatrix(m_player.mcr_camera.getViewProj());
    m_progLambert.setViewProjMatrix(m_player.mcr_camera.getViewProj());
    m_progLambert.setModelMatrix(glm::mat4());

    renderTerrain();

    glDisable(GL_DEPTH_TEST);
    m_progFlat.setModelMatrix(glm::mat4());
    m_progFlat.setViewProjMatrix(m_player.mcr_camera.getViewProj());
    m_progFlat.draw(m_worldAxes);
    glEnable(GL_DEPTH_TEST);
}

// TODO: Change this so it renders the nine zones of generated
// terrain that surround the player (refer to Terrain::m_generatedTerrain
// for more info) (Elaine 1st)
void MyGL::renderTerrain() {
    int minx = 0;
    int maxx = 64;
    int minz = 0;
    int maxz = 64;

    //depending on the player's position, render terrain including a new chunk
    if (m_player.mcr_position.x < 0) {
        minx = m_player.mcr_position.x;
    }
    if (m_player.mcr_position.x > 64) {
        maxx = m_player.mcr_position.x;
    }
    if (m_player.mcr_position.z < 0) {
        minz = m_player.mcr_position.z;
    }
    if (m_player.mcr_position.z > 64) {
        maxz = m_player.mcr_position.z;
    }
    m_terrain.draw(minx, maxx, minz, maxx, &m_progLambert);
}

// construct an inputbundle in keypress event with appropriate info
// and read the info to update the velocity and position
void MyGL::keyPressEvent(QKeyEvent *e) {

    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    }
    if (!e->isAutoRepeat()) {
        keyPressUpdate(e);
    }
}

void MyGL::keyReleaseEvent(QKeyEvent *e) {
    if (!e->isAutoRepeat()) {
        keyReleaseUpdate(e);
    }

}

void MyGL::mouseMoveEvent(QMouseEvent *e) {
    // For windows
    float dx = e->pos().x() - this->width()/2;
    float dy = e->pos().y() - this->height()/2;

    // For mac

    m_player.rotateOnUpGlobal(-dx * 360 * 0.0002f);
    m_player.rotateOnRightLocal(-dy * 360 * 0.0002f);

    moveMouseToCenter();
}

void MyGL::mousePressEvent(QMouseEvent *e) {
    if (e->button() == Qt::LeftButton) {
        m_player.destroyBlock(&m_terrain);
    } else if (e->button() == Qt::RightButton) {
        m_player.createBlock(&m_terrain);
    }
}

void MyGL::keyPressUpdate(QKeyEvent *e) {
    if (m_inputs.isFlightMode) {
        if (e->key() == Qt::Key_W) {
            m_inputs.wPressed = true;
        } else if (e->key() == Qt::Key_S) {
            m_inputs.sPressed = true;
        } else if (e->key() == Qt::Key_D) {
            m_inputs.dPressed = true;
        } else if (e->key() == Qt::Key_A) {
            m_inputs.aPressed = true;
        } else if (e->key() == Qt::Key_Q) {
            m_inputs.qPressed = true;
        } else if (e->key() == Qt::Key_E) {
            m_inputs.ePressed = true;
        } else if (e->key() == Qt::Key_F) {
            m_inputs.isFlightMode = false;
        }
    } else {
        if (e->key() == Qt::Key_W) {
            m_inputs.wPressed = true;
        } else if (e->key() == Qt::Key_S) {
            m_inputs.sPressed = true;
        } else if (e->key() == Qt::Key_D) {
            m_inputs.dPressed = true;
        } else if (e->key() == Qt::Key_A) {
            m_inputs.aPressed = true;
        } else if (e->key() == Qt::Key_Space) {
            m_inputs.spacePressed = true;
        } else if (e->key() == Qt::Key_F) {
            m_inputs.isFlightMode = true;
        }
    }
}

void MyGL::keyReleaseUpdate(QKeyEvent *e) {
    if (e->key() == Qt::Key_W) {
        m_inputs.wPressed = false;
    } else if (e->key() == Qt::Key_S) {
        m_inputs.sPressed = false;
    } else if (e->key() == Qt::Key_D) {
        m_inputs.dPressed = false;
    } else if (e->key() == Qt::Key_A) {
        m_inputs.aPressed = false;
    } else if (e->key() == Qt::Key_Q) {
        m_inputs.qPressed = false;
    } else if (e->key() == Qt::Key_E) {
        m_inputs.ePressed = false;
    } else if (e->key() == Qt::Key_Space) {
        m_inputs.spacePressed = false;
    }
}
