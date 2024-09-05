#include "meshgenerator.h"


std::shared_ptr<MeshObject> MeshGenerator::generateSphereMesh(float radius, int slices, int stacks, const QVector3D &color)
{
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;

    for (int i = 0; i <= stacks; ++i) {
        float phi = M_PI * i / stacks;
        float y = cos(phi) * radius;
        float r = sin(phi) * radius;

        for (int j = 0; j <= slices; ++j) {
            float theta = 2.0f * M_PI * j / slices;
            float x = r * cos(theta);
            float z = r * sin(theta);

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            vertices.push_back(color.x());
            vertices.push_back(color.y());
            vertices.push_back(color.z());
        }
    }

    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < slices; ++j) {
            int first = (i * (slices + 1)) + j;
            int second = first + slices + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

    return std::make_shared<MeshObject>(vertices, indices);
}

std::shared_ptr<MeshObject> MeshGenerator::generateCylinderMesh(float radius, float height, int slices, const QVector3D &color)
{
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
    float halfHeight = height / 2.0f;

    // Bottom circle
    vertices.push_back(0.0f);
    vertices.push_back(-halfHeight);
    vertices.push_back(0.0f);
    vertices.push_back(color.x());
    vertices.push_back(color.y());
    vertices.push_back(color.z());

    for (int i = 0; i <= slices; ++i) {
        float theta = 2.0f * M_PI * i / slices;
        float x = cos(theta) * radius;
        float z = sin(theta) * radius;
        vertices.push_back(x);
        vertices.push_back(-halfHeight);
        vertices.push_back(z);
        vertices.push_back(color.x());
        vertices.push_back(color.y());
        vertices.push_back(color.z());
    }

    // Top circle
    vertices.push_back(0.0f);
    vertices.push_back(halfHeight);
    vertices.push_back(0.0f);
    vertices.push_back(color.x());
    vertices.push_back(color.y());
    vertices.push_back(color.z());

    for (int i = 0; i <= slices; ++i) {
        float theta = 2.0f * M_PI * i / slices;
        float x = cos(theta) * radius;
        float z = sin(theta) * radius;
        vertices.push_back(x);
        vertices.push_back(halfHeight);
        vertices.push_back(z);
        vertices.push_back(color.x());
        vertices.push_back(color.y());
        vertices.push_back(color.z());
    }

    // Bottom face indices
    for (int i = 1; i <= slices; ++i) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    // Top face indices
    for (int i = slices + 2; i <= 2 * slices + 1; ++i) {
        indices.push_back(slices + 1);
        indices.push_back(i + 1);
        indices.push_back(i);
    }

    // Side faces indices
    for (int i = 1; i <= slices; ++i) {
        int next = (i % slices) + 1;
        indices.push_back(i);
        indices.push_back(i + slices + 1);
        indices.push_back(next + slices + 1);

        indices.push_back(i);
        indices.push_back(next + slices + 1);
        indices.push_back(next);
    }
    qDebug() << "Vertices count:" << vertices.size() / 6;  // 每个顶点有6个值：3个位置 + 3个颜色
    qDebug() << "Indices count:" << indices.size();

    return std::make_shared<MeshObject>(vertices, indices);
}

std::shared_ptr<MeshObject> MeshGenerator::generateCubeMesh(const QVector3D& size, const QVector3D& color)
{
    std::vector<GLfloat> vertices = {
        // Positions                             // Colors
        -size.x() / 2, -size.y() / 2,  size.z() / 2,  color.x(), color.y(), color.z(),  // 前面左下角
        size.x() / 2, -size.y() / 2,  size.z() / 2,  color.x(), color.y(), color.z(),  // 前面右下角
        size.x() / 2,  size.y() / 2,  size.z() / 2,  color.x(), color.y(), color.z(),  // 前面右上角
        -size.x() / 2,  size.y() / 2,  size.z() / 2,  color.x(), color.y(), color.z(),  // 前面左上角
        -size.x() / 2, -size.y() / 2, -size.z() / 2,  color.x(), color.y(), color.z(),  // 后面左下角
        size.x() / 2, -size.y() / 2, -size.z() / 2,  color.x(), color.y(), color.z(),  // 后面右下角
        size.x() / 2,  size.y() / 2, -size.z() / 2,  color.x(), color.y(), color.z(),  // 后面右上角
        -size.x() / 2,  size.y() / 2, -size.z() / 2,  color.x(), color.y(), color.z()   // 后面左上角
    };

    std::vector<GLuint> indices = {
        0, 1, 2, 2, 3, 0, // Front face
        4, 5, 6, 6, 7, 4, // Back face
        0, 4, 7, 7, 3, 0, // Left face
        1, 5, 6, 6, 2, 1, // Right face
        0, 1, 5, 5, 4, 0, // Bottom face
        3, 2, 6, 6, 7, 3  // Top face
    };
    qDebug() << "Vertices count:" << vertices.size() / 6;  // 每个顶点有6个值：3个位置 + 3个颜色
    qDebug() << "Indices count:" << indices.size();

    return std::make_shared<MeshObject>(vertices, indices);
}
