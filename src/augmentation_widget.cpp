// augmentation_widget.cpp

#include "augmentation_widget.hpp"

#include <QOpenGLExtraFunctions>
#include <QTemporaryFile>
#include <QVector2D>
#include <QVector3D>
#include <math.h>

augmentation_widget::augmentation_widget (QWidget* parent)
: QOpenGLWidget (parent)
, _scale_factor (1.0f)
, _x_pos (0.0f)
, _y_pos (0.0f) {
}

augmentation_widget::~augmentation_widget () {
}

QSize augmentation_widget::minimumSizeHint () const {
    return QSize (600, 350);
}

QSize augmentation_widget::sizeHint () const {
    return QSize (600, 350);
}

bool augmentation_widget::loadObject (const QString& resource_path) {
    bool status = false;

    // extract model from resources into filesystem and parse it
    QFile resource_file (resource_path);
    if (resource_file.exists ()) {
        auto temp_file  = QTemporaryFile::createNativeFile (resource_file);
        QString fs_path = temp_file->fileName ();

        if (!fs_path.isEmpty ()) {
            std::vector<float> model_interleaved = _object.load (fs_path.toStdString ());

            glBindBuffer (GL_ARRAY_BUFFER, _interleaved_vbo);
            glBufferData (GL_ARRAY_BUFFER, sizeof (float) * model_interleaved.size (),
            model_interleaved.data (), GL_STATIC_DRAW);

            status = true;
        }
    }
    update ();
    return status;
}

void augmentation_widget::setBackground (image_t image) {
    // create background texture
    glBindTexture (GL_TEXTURE_2D, _texture_background);

    GLint format_gl;

    switch (image.format) {
        case RGB24: {
            format_gl = GL_RGB;
            break;
        }
        case YUV:
        case GREY8:
        case BINARY8: {
#ifdef OPENGL_ES
            format_gl = GL_LUMINANCE;
#else
            format_gl = GL_RED;
#endif // OPENGL_ES
            break;
        }
    }
    glTexImage2D (GL_TEXTURE_2D, 0, format_gl, image.width, image.height, 0,
    format_gl, GL_UNSIGNED_BYTE, image.data);

    // normalize coordinates
    // TODO: if needed, might be redundant if self written shader already
    // normalizes texture coords


    /*    glMatrixMode (GL_TEXTURE);
        glLoadIdentity ();
        glScalef (1.0 / image.width, 1.0 / image.height, 1);
        glMatrixMode (GL_MODELVIEW);*/
}

void augmentation_widget::setScale (const float scale) {
    _scale_factor = scale;
}

void augmentation_widget::setXPosition (const float location) {
    _x_pos = location;
}

void augmentation_widget::setYPosition (const float location) {
    _y_pos = location;
}

void augmentation_widget::setXRotation (const GLfloat angle) {
    _mat_x_rot.setToIdentity ();
    _mat_x_rot.rotate (angle, 1, 0, 0);
}

void augmentation_widget::setYRotation (const GLfloat angle) {
    _mat_y_rot.setToIdentity ();
    _mat_y_rot.rotate (angle, 0, 1, 0);
}

void augmentation_widget::setZRotation (const GLfloat angle) {
    _mat_z_rot.setToIdentity ();
    _mat_z_rot.rotate (angle, 0, 0, 1);
}

void augmentation_widget::initializeGL () {
    int status = 0;
    initializeOpenGLFunctions ();

    glClearColor (1, 0.5, 1, 1.0f);
    glEnable (GL_DEPTH_TEST);
    // glEnable (GL_CULL_FACE);

    // TODO: add lighting back
    /*    glShadeModel (GL_SMOOTH);
        glEnable (GL_LIGHTING);
        glEnable (GL_LIGHT0);
        glEnable (GL_COLOR_MATERIAL);
            glMatrixMode (GL_PROJECTION);*/
    glEnable (GL_TEXTURE_2D);

    glGenTextures (1, &_texture_background);
    glBindTexture (GL_TEXTURE_2D, _texture_background);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    _program.addShaderFromSourceFile (QOpenGLShader::Vertex, ":/GL_shaders/basic_vs.glsl");
    _program.addShaderFromSourceFile (QOpenGLShader::Fragment, ":/GL_shaders/basic_fs.glsl");
    _program.link ();
    _program.bind ();

    // TODO: add lighting back
    /*glMatrixMode (GL_MODELVIEW);
    static GLfloat lightPosition[4] = { 0, 0, 10, 1.0 };
    glLightfv (GL_LIGHT0, GL_POSITION, lightPosition);
    mat_identity ();
    // gluPerspective (33.7, 1.3, 0.1, 100.0);*/

    // generate vertex array buffers
    glGenBuffers (1, &_interleaved_vbo);
    glGenVertexArrays (1, &_model_vao);

    glBindVertexArray (_model_vao);
    glBindBuffer (GL_ARRAY_BUFFER, _interleaved_vbo);

    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*> (0));
    glVertexAttribPointer (2, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*> (3));
    glVertexAttribPointer (1, 4, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*> (6));
    glEnableVertexAttribArray (0);
    glEnableVertexAttribArray (1);
    glEnableVertexAttribArray (2);

    emit initialized ();
}

void augmentation_widget::resizeGL (int width, int height) {
    int side = qMin (width, height);
    glViewport ((width - side) / 2, (height - side) / 2, side, side);

    _mat_projection.setToIdentity ();
    // TODO: replace with perspective
    _mat_projection.ortho (-2.0f, +2.0f, -2.0f, +2.0f, 1.0f, 25.0f);
}

void augmentation_widget::paintGL () {
    QMatrix4x4 mat_modelview;
    // QOpenGLFunctions* f = QOpenGLContext::currentContext
    // ()->functions
    // ();
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // draw background
    mat_modelview.translate (0.0, 0.0, -10.0);
    // draw_background ();

    // TODO: why is pushing/duplicating the matrix needed?
    // glPushMatrix ();

    // draw object
    // TODO: findout if this is the correct order, should translation not happen
    // after rotation?
    mat_modelview.translate (_x_pos, _y_pos, 0);
    mat_modelview.scale (_scale_factor);
    mat_modelview = mat_modelview * _mat_x_rot;
    mat_modelview = mat_modelview * _mat_y_rot;
    mat_modelview = mat_modelview * _mat_z_rot;

    // TODO: draw object
    // draw_object ();

    // TODO: also see todo at glPushMatrix, is pushing and popping needed here?
    // glPopMatrix ();
}

void augmentation_widget::draw_object () {
    // TODO: bind buffer here
    glDrawArrays (GL_TRIANGLES, 0, _polygon_count);
}

void augmentation_widget::draw_background () {
    /*
        glBindTexture (GL_TEXTURE_2D, _texture_background);
        glBegin (GL_QUADS);
        glColor3f (1, 1, 1);
        glTexCoord2f (0.0, 1.0);
        glVertex3f (-2.0, -2.0, -2.0);
        glTexCoord2f (1.0, 1.0);
        glVertex3f (2.0, -2.0, -2.0);
        glTexCoord2f (1.0, 0.0);
        glVertex3f (2.0, 2.0, -2.0);
        glTexCoord2f (0.0, 0.0);
        glVertex3f (-2.0, 2.0, -2.0);
        glEnd ();*/
    // clang-format off
    GLfloat vertices_buff[6 * 3] = {    -4.0, -3.0, -2.0,   // poly 1 a
                                        4.0, -3.0, -2.0,    // poly 1 b
                                        4.0, 3.0, -2.0,     // poly 1 c
                                        -4.0, -3.0, -.0,   // poly 2 a
                                        4.0, 3.0, -0.0,     // poly 2 b
                                        -4.0, 3.0, -0.0 };  // poly 2 c

    GLfloat normals_buff[6 * 3] = {     0, 0, 1,           // poly 1 a
                                        0, 0, 1,           // poly 1 b
                                        0, 0, 1,           // poly 1 c
                                        0, 0, 1,           // poly 2 a
                                        0, 0, 1,           // poly 2 b
                                        0, 0, 1 };         // poly 2 c

    GLfloat texture_buff[6 * 2] = {     0.0, 1.0,           // poly 1 a
                                        1.0, 1.0,           // poly 1 b
                                        1.0, 0.0,           // poly 1 c
                                        0.0, 1.0,           // poly 2 a
                                        1.0, 0.0,           // poly 2 b
                                        0.0, 0.0 };         // poly 2 c

    GLfloat colors_buff[6 * 4];
    // clang-format on
    // all collors white
    for (int i = 0; i < (6 * 4); i++) {
        colors_buff[i] = 1.0f;
    }

    /*glEnableClientState (GL_VERTEX_ARRAY);
    glEnableClientState (GL_NORMAL_ARRAY);
    glEnableClientState (GL_COLOR_ARRAY);
    // glEnableClientState (GL_TEXTURE_COORD_ARRAY);

    glBindTexture (GL_TEXTURE_2D, _texture_background);
    glVertexPointer (3, GL_FLOAT, 0, vertices_buff);
    glNormalPointer (GL_FLOAT, 0, normals_buff);
    glColorPointer (4, GL_FLOAT, 0, colors_buff);
    // glTexCoordPointer (2, GL_FLOAT, 0, texture_buff);

    glDrawArrays (GL_TRIANGLES, 0, 2); // draw the 2 triangles

    glDisableClientState (GL_VERTEX_ARRAY);
    glDisableClientState (GL_NORMAL_ARRAY);
    glDisableClientState (GL_COLOR_ARRAY);*/
    // glDisableClientState (GL_TEXTURE_COORD_ARRAY);
}
