#ifndef MESHGENERATOR_H
#define MESHGENERATOR_H

#include "mesh.h"
#include <vector>
#include <memory>
#include <QVector3D>

class MeshGenerator {
public:
    static std::shared_ptr<MeshObject> generateSphereMesh(float radius, int slices, int stacks);
    static std::shared_ptr<MeshObject> generateCylinderMesh(float radius, float height, int slices);
    static std::shared_ptr<MeshObject> generateCubeMesh(const QVector3D& size);
    static std::shared_ptr<MeshObject> generateConeMesh(float radius, float height, int slices);
    static std::shared_ptr<MeshObject> generateTorusMesh(float mainRadius, float tubeRadius, int radialSegments, int tubularSegments, float angleDegrees = 360.0f);

};

#endif // MESHGENERATOR_H
