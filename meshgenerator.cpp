#include "meshgenerator.h"


std::shared_ptr<MeshObject> MeshGenerator::generateSphereMesh(float radius, int slices, int stacks)
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

std::shared_ptr<MeshObject> MeshGenerator::generateCylinderMesh(float radius, float height, int slices)
{
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
    float halfHeight = height / 2.0f;

    // Bottom circle
    vertices.push_back(0.0f);
    vertices.push_back(-halfHeight);
    vertices.push_back(0.0f);

    for (int i = 0; i <= slices; ++i) {
        float theta = 2.0f * M_PI * i / slices;
        float x = cos(theta) * radius;
        float z = sin(theta) * radius;
        vertices.push_back(x);
        vertices.push_back(-halfHeight);
        vertices.push_back(z);
    }

    // Top circle
    vertices.push_back(0.0f);
    vertices.push_back(halfHeight);
    vertices.push_back(0.0f);

    for (int i = 0; i <= slices; ++i) {
        float theta = 2.0f * M_PI * i / slices;
        float x = cos(theta) * radius;
        float z = sin(theta) * radius;
        vertices.push_back(x);
        vertices.push_back(halfHeight);
        vertices.push_back(z);
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

    // Side faces indices (修正后的部分)
    for (int i = 1; i <= slices; ++i) {
        int next = (i == slices) ? 1 : i + 1; // 修正闭合问题
        indices.push_back(i);
        indices.push_back(i + slices + 1);
        indices.push_back(next + slices + 1);

        indices.push_back(i);
        indices.push_back(next + slices + 1);
        indices.push_back(next);
    }

    return std::make_shared<MeshObject>(vertices, indices);
}


std::shared_ptr<MeshObject> MeshGenerator::generateCubeMesh(const QVector3D& size)
{
    std::vector<GLfloat> vertices = {
        // Positions
        -size.x() / 2, -size.y() / 2,  size.z() / 2,  // 前面左下角
        size.x() / 2, -size.y() / 2,  size.z() / 2,  // 前面右下角
        size.x() / 2,  size.y() / 2,  size.z() / 2,  // 前面右上角
        -size.x() / 2,  size.y() / 2,  size.z() / 2,  // 前面左上角
        -size.x() / 2, -size.y() / 2, -size.z() / 2,  // 后面左下角
        size.x() / 2, -size.y() / 2, -size.z() / 2,  // 后面右下角
        size.x() / 2,  size.y() / 2, -size.z() / 2,  // 后面右上角
        -size.x() / 2,  size.y() / 2, -size.z() / 2   // 后面左上角
    };

    std::vector<GLuint> indices = {
        0, 1, 2, 2, 3, 0, // Front face
        4, 5, 6, 6, 7, 4, // Back face
        0, 4, 7, 7, 3, 0, // Left face
        1, 5, 6, 6, 2, 1, // Right face
        0, 1, 5, 5, 4, 0, // Bottom face
        3, 2, 6, 6, 7, 3  // Top face
    };

    return std::make_shared<MeshObject>(vertices, indices);
}

std::shared_ptr<MeshObject> MeshGenerator::generateConeMesh(float radius, float height, int slices)
{
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
    float halfHeight = height / 2.0f;

    // 顶点
    // 顶点：圆锥的顶点在中间上方
    vertices.push_back(0.0f);  // x
    vertices.push_back(halfHeight);  // y
    vertices.push_back(0.0f);  // z

    // 底部圆的顶点
    for (int i = 0; i <= slices; ++i) {
        float theta = 2.0f * M_PI * i / slices;
        float x = cos(theta) * radius;
        float z = sin(theta) * radius;

        vertices.push_back(x);
        vertices.push_back(-halfHeight);  // y 为负半高
        vertices.push_back(z);
    }

    // 索引
    // 圆锥侧面
    for (int i = 1; i <= slices; ++i) {
        indices.push_back(0);    // 顶点索引 0
        indices.push_back(i);    // 当前点
        indices.push_back(i + 1);  // 下一个点
    }

    // 底部圆面
    int centerIndex = slices + 2;  // 底部圆的中心点
    vertices.push_back(0.0f);  // 中心点的 x
    vertices.push_back(-halfHeight);  // 中心点的 y
    vertices.push_back(0.0f);  // 中心点的 z

    for (int i = 1; i <= slices; ++i) {
        indices.push_back(centerIndex);
        indices.push_back(i + 1);
        indices.push_back(i);
    }

    return std::make_shared<MeshObject>(vertices, indices);
}

std::shared_ptr<MeshObject> MeshGenerator::generateTorusMesh(float mainRadius, float tubeRadius, int radialSegments, int tubularSegments, float angleDegrees)
{
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;

    // 将角度从度数转换为弧度
    float maxU = qDegreesToRadians(angleDegrees);

    // 循环生成圆环上的点
    for (int i = 0; i <= radialSegments; ++i) {
        float u = (float)i / radialSegments * maxU;  // 计算主环的角度
        float cosU = cos(u);
        float sinU = sin(u);

        // 对于每个主圆环上的点，生成对应管道上的点
        for (int j = 0; j <= tubularSegments; ++j) {
            float v = (float)j / tubularSegments * 2.0f * M_PI;
            float cosV = cos(v);
            float sinV = sin(v);

            // 计算圆环上每个点的3D坐标
            float x = (mainRadius + tubeRadius * cosV) * cosU;
            float y = (mainRadius + tubeRadius * cosV) * sinU;
            float z = tubeRadius * sinV;

            // 添加顶点坐标
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }

    // 生成索引
    for (int i = 0; i < radialSegments; ++i) {
        for (int j = 0; j < tubularSegments; ++j) {
            int first = (i * (tubularSegments + 1)) + j;
            int second = first + tubularSegments + 1;

            // 添加两个三角形组成一个四边形
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



