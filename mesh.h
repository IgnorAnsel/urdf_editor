#ifndef MESH_H
#define MESH_H

#include <vector>
#include <memory>
#include <QOpenGLFunctions_4_5_Core>

class MeshObject : protected QOpenGLFunctions_4_5_Core {
public:
    MeshObject(const std::vector<GLfloat>& vertices, const std::vector<GLuint>& indices);
    ~MeshObject();
    void Draw();

private:
    std::vector<GLfloat> Vertices;
    std::vector<GLuint> Indices;
    GLuint VAO, VBO, EBO;
    GLsizei indexCount;
};

#endif // MESH_H
