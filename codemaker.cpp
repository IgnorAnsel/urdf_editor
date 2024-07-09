#include "codemaker.h"
#include <QXmlStreamReader>
#include "shape.h"
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

void codemaker::CodeBegin(QString path, QString name)
{
    // 检查路径是否存在，如果不存在则退出
    QDir dir(path);
    if (!dir.exists())
    {
        dir.mkpath(".");
    }
    QFile file(path);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(nullptr, tr("ERROR"), tr("Open failed"));
        return;
    }
    QTextStream out(&file);
    out << "<?xml version=\"1.0\"?>\n";
    out << "<robot name=\"";
    out << name;
    out << "\">\n";
    // 写入 URDF 尾信息
    out << "</robot>\n";
    file.close();
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

std::vector<Shape> codemaker::Loader(QString path)
{
    // 打开文件进行读取
    int link_id = 0;
    int joint_id = 0;
    std::vector<Shape> shapes;
    QFile file(path);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << "无法打开文件: " << file.errorString();
        return shapes;
    }

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    // 使用 QXmlStreamReader 解析 URDF 文件
    QXmlStreamReader xml(content);
    Shape currentShape;
    while (!xml.atEnd())
    {
        xml.readNext();

        if (xml.isStartElement())
        {
            if (xml.name().toString() == "link")
            {
                currentShape.link.name = xml.attributes().value("name").toString().toStdString();
            }
            else if (xml.name().toString() == "inertial")
            {
                // 处理惯性属性
                while (!(xml.isEndElement() && xml.name().toString() == "inertial"))
                {
                    xml.readNext();
                    if (xml.isStartElement())
                    {
                        if (xml.name().toString() == "origin")
                        {
                            QStringList xyz = xml.attributes().value("xyz").toString().split(" ");
                            QStringList rpy = xml.attributes().value("rpy").toString().split(" ");
                            currentShape.link.inertial.origin.xyz = QVector3D(xyz[0].toDouble(), xyz[1].toDouble(), xyz[2].toDouble());
                            currentShape.link.inertial.origin.rpy = QVector3D(rpy[0].toDouble(), rpy[1].toDouble(), rpy[2].toDouble());
                        }
                        else if (xml.name().toString() == "mass")
                        {
                            currentShape.link.inertial.mass = xml.attributes().value("value").toDouble();
                        }
                        else if (xml.name().toString() == "inertia")
                        {
                            currentShape.link.inertial.inertia_matrix = {
                                xml.attributes().value("ixx").toDouble(), xml.attributes().value("ixy").toDouble(),
                                xml.attributes().value("ixz").toDouble(), xml.attributes().value("iyy").toDouble(),
                                xml.attributes().value("iyz").toDouble(), xml.attributes().value("izz").toDouble()};
                        }
                    }
                }
            }
            else if (xml.name().toString() == "visual")
            {
                // 处理视觉属性
                while (!(xml.isEndElement() && xml.name().toString() == "visual"))
                {
                    xml.readNext();
                    if (xml.isStartElement())
                    {
                        if (xml.name().toString() == "origin")
                        {
                            QStringList xyz = xml.attributes().value("xyz").toString().split(" ");
                            QStringList rpy = xml.attributes().value("rpy").toString().split(" ");
                            currentShape.link.visuals.origin.xyz = QVector3D(xyz[0].toDouble(), xyz[1].toDouble(), xyz[2].toDouble());
                            currentShape.link.visuals.origin.rpy = QVector3D(rpy[0].toDouble(), rpy[1].toDouble(), rpy[2].toDouble());
                        }
                        else if (xml.name().toString() == "geometry")
                        {
                            // 处理几何属性
                            xml.readNext();
                            xml.readNext();
                            if (xml.isStartElement())
                            {
                                if (xml.name().toString() == "box")
                                {
                                    QStringList size = xml.attributes().value("size").toString().split(" ");
                                    currentShape.type = Shape::Cube;
                                    currentShape.link.visuals.geometry.box.size = QVector3D(size[0].toDouble(), size[1].toDouble(), size[2].toDouble());
                                }
                                else if (xml.name().toString() == "sphere")
                                {
                                    currentShape.type = Shape::Sphere;
                                    currentShape.link.visuals.geometry.sphere.radius = xml.attributes().value("radius").toDouble();
                                }
                                else if (xml.name().toString() == "cylinder")
                                {
                                    currentShape.type = Shape::Cylinder;
                                    currentShape.link.visuals.geometry.cylinder.radius = xml.attributes().value("radius").toDouble();
                                    currentShape.link.visuals.geometry.cylinder.length = xml.attributes().value("length").toDouble();
                                }
                            }
                        }
                        else if (xml.name().toString() == "material")
                        {
                            currentShape.link.visuals.material = xml.attributes().value("name").toString().toStdString();
                            // 处理颜色属性
                            xml.readNext();
                            xml.readNext();
                            if (xml.isStartElement() && xml.name().toString() == "color")
                            {
                                qDebug() << "c";
                                QStringList rgba = xml.attributes().value("rgba").toString().split(" ");
                                currentShape.link.visuals.color = QColor(rgba[0].toDouble() * 255, rgba[1].toDouble() * 255,
                                                                         rgba[2].toDouble() * 255, rgba[3].toDouble() * 255);
                            }
                        }
                    }
                }
            }
            else if (xml.name().toString() == "collision")
            {
                // 处理碰撞属性
                while (!(xml.isEndElement() && xml.name().toString() == "collision"))
                {
                    xml.readNext();
                    if (xml.isStartElement())
                    {
                        if (xml.name().toString() == "origin")
                        {
                            QStringList xyz = xml.attributes().value("xyz").toString().split(" ");
                            QStringList rpy = xml.attributes().value("rpy").toString().split(" ");
                            currentShape.link.collisions.origin.xyz = QVector3D(xyz[0].toDouble(), xyz[1].toDouble(), xyz[2].toDouble());
                            currentShape.link.collisions.origin.rpy = QVector3D(rpy[0].toDouble(), rpy[1].toDouble(), rpy[2].toDouble());
                        }
                        else if (xml.name().toString() == "geometry")
                        {
                            // 处理几何属性
                            xml.readNext();
                            xml.readNext();
                            if (xml.isStartElement())
                            {
                                if (xml.name().toString() == "box")
                                {
                                    QStringList size = xml.attributes().value("size").toString().split(" ");
                                    currentShape.link.collisions.geometry.box.size = QVector3D(size[0].toDouble(), size[1].toDouble(), size[2].toDouble());
                                }
                                else if (xml.name().toString() == "sphere")
                                {
                                    currentShape.link.collisions.geometry.sphere.radius = xml.attributes().value("radius").toDouble();
                                }
                                else if (xml.name().toString() == "cylinder")
                                {
                                    currentShape.link.collisions.geometry.cylinder.radius = xml.attributes().value("radius").toDouble();
                                    currentShape.link.collisions.geometry.cylinder.length = xml.attributes().value("length").toDouble();
                                }
                            }
                        }
                    }
                }
            }
        }
        else if (xml.isEndElement())
        {
            if (xml.name().toString() == "link")
            {
                currentShape.id = link_id;
                link_id++;
                shapes.push_back(currentShape);
                currentShape = Shape(); // 重置 currentShape
            }
            else if (xml.name().toString() == "joint")
            {
                // 为 joint 赋值 parent_id
                for (auto &shape : shapes)
                {
                    qDebug() << "name" << currentShape.joint.parent_link;
                    if (shape.link.name == currentShape.joint.parent_link)
                    {
                        currentShape.joint.parent_id = shape.id;
                        break;
                    }
                    else if (shape.link.name == currentShape.joint.child_link)
                    {
                        currentShape.joint.child_id = shape.id;
                        break;
                    }
                }
            }
        }
    }

    if (xml.hasError())
    {
        qDebug() << "XML 解析错误: " << xml.errorString();
        return shapes;
    }

    // 解析后的形状数据可以在此处进行进一步处理
    shapes = Loader_joint(shapes, path);
    qDebug() << "URDF 文件已加载，包含 " << shapes.size() << " 个形状";
    return shapes;
}

std::vector<Shape> codemaker::Loader_joint(std::vector<Shape> shapes, QString path)
{
    QFile file(path);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << "无法打开文件: " << file.errorString();
        return shapes; // 如果文件无法打开，则返回未修改的向量
    }

    QXmlStreamReader xml(&file);

    while (!xml.atEnd() && !xml.hasError())
    {
        xml.readNext();
        if (xml.isStartElement() && xml.name().toString() == "joint")
        {
            QString jointName = xml.attributes().value("name").toString();
            QString type = xml.attributes().value("type").toString();
            QString parentLink, childLink;
            QVector3D origin_xyz;
            QVector3D origin_rpy;
            Axis axis;
            // 默认设置
            Limits limits = {};
            Dynamics dynamics = {};
            Calibration calibration = {};
            Mimic mimic = {};
            SafetyController safety;

            while (!(xml.isEndElement() && xml.name().toString() == "joint")) {
                xml.readNext();
                if (xml.isStartElement()) {
                    if (xml.name().toString() == "parent")
                        parentLink = xml.attributes().value("link").toString();
                    else if (xml.name().toString() == "child")
                        childLink = xml.attributes().value("link").toString();
                    else if (xml.name().toString() == "origin") {
                        QStringList xyz = xml.attributes().value("xyz").toString().split(" ");
                        QStringList rpy = xml.attributes().value("rpy").toString().split(" ");
                        origin_xyz = QVector3D(xyz[0].toDouble(), xyz[1].toDouble(), xyz[2].toDouble());
                        origin_rpy = QVector3D(rpy[0].toDouble(), rpy[1].toDouble(), rpy[2].toDouble());
                    } else if (xml.name().toString() == "axis") {
                        QStringList xyz = xml.attributes().value("xyz").toString().split(" ");
                        axis.xyz = QVector3D(xyz[0].toDouble(), xyz[1].toDouble(), xyz[2].toDouble());
                    } else if (xml.name().toString() == "limit") {
                        limits.lower = xml.attributes().value("lower").toDouble();
                        limits.upper = xml.attributes().value("upper").toDouble();
                        limits.effort = xml.attributes().value("effort").toDouble();
                        limits.velocity = xml.attributes().value("velocity").toDouble();
                    } else if (xml.name().toString() == "dynamics") {
                        dynamics.damping = xml.attributes().value("damping").toDouble();
                        dynamics.friction = xml.attributes().value("friction").toDouble();
                    } else if (xml.name().toString() == "calibration") {
                        calibration.rising = xml.attributes().value("rising").toDouble();
                        calibration.falling = xml.attributes().value("falling").toDouble();
                    } else if (xml.name().toString() == "mimic") {
                        mimic.joint_name = xml.attributes().value("joint").toString().toStdString();
                        mimic.multiplier = xml.attributes().value("multiplier").toDouble();
                        mimic.offset = xml.attributes().value("offset").toDouble();
                    } else if (xml.name().toString() == "safety_controller") {
                        safety.soft_lower_limit = xml.attributes().value("soft_lower_limit").toDouble();
                        safety.soft_upper_limit = xml.attributes().value("soft_upper_limit").toDouble();
                        safety.k_position = xml.attributes().value("k_position").toDouble();
                        safety.k_velocity = xml.attributes().value("k_velocity").toDouble();
                    }
                }
            }
            auto findShapeByName = [&](const std::string &name) -> Shape *
            {
                for (auto &shape : shapes)
                {
                    if (shape.link.name == name)
                        return &shape;
                }
                return nullptr;
            };

            Shape *parentShape = findShapeByName(parentLink.toStdString());
            Shape *childShape = findShapeByName(childLink.toStdString());

            if (childShape)
            {
                URDFJoint joint;
                joint.name = jointName.toStdString();
                joint.type = type.toStdString();
                joint.parent_link = parentLink.toStdString();
                joint.child_link = childLink.toStdString();
                joint.child_id = childShape->id;
                joint.parent_to_child_origin.xyz = origin_xyz;
                qDebug() << joint.parent_to_child_origin.xyz;
                joint.parent_to_child_origin.rpy = origin_rpy;
                joint.axis = axis;
                joint.limits = limits;
                joint.dynamics = dynamics;
                joint.calibration = calibration;
                joint.mimic = mimic;
                joint.safety_controller = safety;
                joint.id = childShape->id;
                qDebug() << "child_id:" << joint.child_id;
                if (parentShape)
                {
                    joint.parent_id = parentShape->id;
                }
                else
                {
                    joint.parent_id = -1; // 无父节点时设为-1
                }
                childShape->joint = joint; // 将关节信息添加到对应的shape中
                childShape->isjointset = true;
            }
        }
    }
    // 遍历所有shapes，检查是否每个shape都已经设置了joint
    for (auto &shape : shapes)
    {
        if (!shape.isjointset)
        {
            URDFJoint defaultJoint;
            defaultJoint.name = shape.link.name; // 使用shape的link名称作为关节名称
            defaultJoint.type = "fixed"; // 默认类型为fixed
            defaultJoint.child_id = shape.id;
            defaultJoint.parent_id = -1; // 没有父节点
            defaultJoint.id = shape.id;
            shape.joint = defaultJoint;
            qDebug() << "为" << QString::fromStdString(shape.link.name) << "设置默认关节";
        }
    }
    if (xml.hasError())
    {
        qDebug() << "XML解析错误: " << xml.errorString();
    }

    file.close();
    return shapes;
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
