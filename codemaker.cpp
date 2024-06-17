#include "codemaker.h"

codemaker::codemaker(QObject *parent)
    : QObject{parent}
{
    QString currentPath = QDir::currentPath();
    QString randomPrefix = QUuid::createUuid().toString(QUuid::WithoutBraces);
    QString fileName = randomPrefix + ".urdf";
    QString filePath = currentPath + "/" + fileName;
    path = filePath;
    QFile file(filePath);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(nullptr, tr("应用程序"), tr("无法创建文件: %1").arg(file.errorString()));
        return;
    }
}

void codemaker::CodeMake(QString path, std::vector<Shape> shapes)
{
    // 检查路径是否存在，如果不存在则创建
    QDir dir(path);
    if (!dir.exists())
    {
        dir.mkpath(".");
    }
    QFile file(path);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        qDebug() << "无法创建文件: " << file.errorString();
        return;
    }

    QTextStream out(&file);

    // 写入 URDF 头信息
    out << "<?xml version=\"1.0\"?>\n";
    out << "<robot name=\"example_robot\">\n";

    // 遍历所有形状，生成 links 和 joints 信息
    for (const Shape &shape : shapes)
    {
        // 写入 link 信息
        out << "  <link name=\"" << QString::fromStdString(shape.link.name) << "\">\n";
        if (shape.link.inertial.mass != 0 ||
            isInertiaMatrixNonZero(shape.link.inertial.inertia_matrix) ||
            isOriginNonZero(shape.link.inertial.origin.xyz, shape.link.inertial.origin.rpy))
        {

            out << "    <inertial>\n";
            out << "      <origin xyz=\""
                << shape.link.inertial.origin.xyz.x() << " "
                << shape.link.inertial.origin.xyz.y() << " "
                << shape.link.inertial.origin.xyz.z() << "\" rpy=\""
                << shape.link.inertial.origin.rpy.x() << " "
                << shape.link.inertial.origin.rpy.y() << " "
                << shape.link.inertial.origin.rpy.z() << "\" />\n";

            if (shape.link.inertial.mass != 0)
                out << "      <mass value=\"" << shape.link.inertial.mass << "\" />\n";

            if (isInertiaMatrixNonZero(shape.link.inertial.inertia_matrix))
            {
                out << "      <inertia ixx=\"" << shape.link.inertial.inertia_matrix.ixx << "\" ixy=\""
                    << shape.link.inertial.inertia_matrix.ixy << "\" ixz=\""
                    << shape.link.inertial.inertia_matrix.ixz << "\" iyy=\""
                    << shape.link.inertial.inertia_matrix.iyy << "\" iyz=\""
                    << shape.link.inertial.inertia_matrix.iyz << "\" izz=\""
                    << shape.link.inertial.inertia_matrix.izz << "\" />\n";
            }

            out << "    </inertial>\n";
        }
        out << "    <visual>\n";
        out << "      <origin xyz=\""
            << shape.link.visuals.origin.xyz.x() << " "
            << shape.link.visuals.origin.xyz.y() << " "
            << shape.link.visuals.origin.xyz.z() << "\" rpy=\""
            << shape.link.visuals.origin.rpy.x() << " "
            << shape.link.visuals.origin.rpy.y() << " "
            << shape.link.visuals.origin.rpy.z() << "\" />\n";
        out << "      <geometry>\n";
        switch (shape.type)
        {
        case Shape::Cube:
            out << "        <box size=\""
                << shape.link.visuals.geometry.box.size.x() << " "
                << shape.link.visuals.geometry.box.size.y() << " "
                << shape.link.visuals.geometry.box.size.z() << "\" />\n";
            break;
        case Shape::Sphere:
            out << "        <sphere radius=\"" << shape.link.visuals.geometry.sphere.radius << "\" />\n";
            break;
        case Shape::Cylinder:
            out << "        <cylinder radius=\"" << shape.link.visuals.geometry.cylinder.radius
                << "\" length=\"" << shape.link.visuals.geometry.cylinder.length << "\" />\n";
            break;
        default:
            break;
        }
        out << "      </geometry>\n";
        out << "      <material name=\"" << QString::fromStdString(shape.link.visuals.material) << "\">\n";
        out << "        <color rgba=\"" << shape.link.visuals.color.redF() << " "
            << shape.link.visuals.color.greenF() << " "
            << shape.link.visuals.color.blueF() << " "
            << shape.link.visuals.color.alphaF() << "\" />\n";
        out << "      </material>\n";
        out << "    </visual>\n";

        out << "    <collision>\n";
        out << "      <origin xyz=\""
            << shape.link.collisions.origin.xyz.x() << " "
            << shape.link.collisions.origin.xyz.y() << " "
            << shape.link.collisions.origin.xyz.z() << "\" rpy=\""
            << shape.link.collisions.origin.rpy.x() << " "
            << shape.link.collisions.origin.rpy.y() << " "
            << shape.link.collisions.origin.rpy.z() << "\" />\n";
        out << "      <geometry>\n";
        switch (shape.type)
        {
        case Shape::Cube:
            out << "        <box size=\""
                << shape.link.collisions.geometry.box.size.x() << " "
                << shape.link.collisions.geometry.box.size.y() << " "
                << shape.link.collisions.geometry.box.size.z() << "\" />\n";
            break;
        case Shape::Sphere:
            out << "        <sphere radius=\"" << shape.link.collisions.geometry.sphere.radius << "\" />\n";
            break;
        case Shape::Cylinder:
            out << "        <cylinder radius=\"" << shape.link.collisions.geometry.cylinder.radius
                << "\" length=\"" << shape.link.collisions.geometry.cylinder.length << "\" />\n";
            break;
        default:
            break;
        }
        out << "      </geometry>\n";
        out << "    </collision>\n";

        out << "  </link>\n";

        // 写入 joint 信息，跳过 parent_link 为空的关节
        if (!shape.joint.parent_link.empty())
        {
            out << "  <joint name=\"" << QString::fromStdString(shape.joint.name) << "\" type=\"" << QString::fromStdString(shape.joint.type) << "\">\n";
            out << "    <parent link=\"" << QString::fromStdString(shape.joint.parent_link) << "\" />\n";
            out << "    <child link=\"" << QString::fromStdString(shape.joint.child_link) << "\" />\n";
            out << "    <origin xyz=\""
                << shape.joint.parent_to_child_origin.xyz.x() << " "
                << shape.joint.parent_to_child_origin.xyz.y() << " "
                << shape.joint.parent_to_child_origin.xyz.z() << "\" rpy=\""
                << shape.joint.parent_to_child_origin.rpy.x() << " "
                << shape.joint.parent_to_child_origin.rpy.y() << " "
                << shape.joint.parent_to_child_origin.rpy.z() << "\" />\n";
            out << "    <axis xyz=\""
                << shape.joint.axis.xyz.x() << " "
                << shape.joint.axis.xyz.y() << " "
                << shape.joint.axis.xyz.z() << "\" />\n";
            if (shape.joint.limits.lower != 0 || shape.joint.limits.upper != 0 || shape.joint.limits.effort != 0 || shape.joint.limits.velocity != 0)
            {
                out << "    <limit lower=\"" << shape.joint.limits.lower << "\" upper=\"" << shape.joint.limits.upper
                    << "\" effort=\"" << shape.joint.limits.effort << "\" velocity=\"" << shape.joint.limits.velocity << "\" />\n";
            }
            if (shape.joint.dynamics.damping != 0 || shape.joint.dynamics.friction != 0)
            {
                out << "    <dynamics damping=\"" << shape.joint.dynamics.damping << "\" friction=\"" << shape.joint.dynamics.friction << "\" />\n";
            }
            if (shape.joint.calibration.rising != 0 || shape.joint.calibration.falling != 0)
            {
                out << "    <calibration rising=\"" << shape.joint.calibration.rising << "\" falling=\"" << shape.joint.calibration.falling << "\" />\n";
            }

            if (shape.joint.mimic.joint_name != "" || shape.joint.mimic.multiplier != 0 || shape.joint.mimic.offset != 0)
            {
                out << "    <mimic joint=\"" << QString::fromStdString(shape.joint.mimic.joint_name)
                    << "\" multiplier=\"" << shape.joint.mimic.multiplier << "\" offset=\"" << shape.joint.mimic.offset << "\" />\n";
            }

            if (shape.joint.safety_controller.soft_lower_limit != "" || shape.joint.safety_controller.soft_upper_limit != "" || shape.joint.safety_controller.k_position != 0 || shape.joint.safety_controller.k_velocity != 0)
            {
                out << "    <safety_controller soft_lower_limit=\"" << QString::fromStdString(shape.joint.safety_controller.soft_lower_limit)
                    << "\" soft_upper_limit=\"" << QString::fromStdString(shape.joint.safety_controller.soft_upper_limit)
                    << "\" k_position=\"" << shape.joint.safety_controller.k_position
                    << "\" k_velocity=\"" << shape.joint.safety_controller.k_velocity << "\" />\n";
            }

            out << "  </joint>\n";
        }
    }

    // 写入 URDF 尾信息
    out << "</robot>\n";

    file.close();
    qDebug() << "URDF 文件已生成，路径为:" << path;
}
bool codemaker::isOriginNonZero(const QVector3D &position, const QVector3D &rotation)
{
    return position.x() != 0 || position.y() != 0 || position.z() != 0 ||
           rotation.x() != 0 || rotation.y() != 0 || rotation.z() != 0;
}
bool codemaker::isInertiaMatrixNonZero(const Inertia &matrix)
{
    return matrix.ixx != 0 || matrix.ixy != 0 || matrix.ixz != 0 ||
           matrix.iyy != 0 || matrix.iyz != 0 || matrix.izz != 0;
}
