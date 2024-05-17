#ifndef PARAM_STRUCT_H
#define PARAM_STRUCT_H
#include <QString>
// 定义URDF文件中常用的数据结构

struct Pose {
    double xyz[3];         // x, y, z 坐标
    double rpy[3];         // roll, pitch, yaw 角度
};
struct Box {
    double size[3]; // 长、宽、高
};
struct Cylinder {
    double radius; // 半径
    double length; // 长度
};
struct Sphere {
    double radius; // 半径
};
struct Mesh {
    std::string filename; // 网格文件路径
    double scale[3];      // 缩放因子
};
struct Geometry {
    // 下面这些是具体的几何形状
    Box box;
    Cylinder cylinder;
    Sphere sphere;
    Mesh mesh;
};
struct Collision {
    Pose origin;           // 碰撞属性相对于link的原点
    Geometry geometry;     // 几何形状
};
struct Inertia {
    double ixx; // 惯性矩阵的Ixx分量
    double ixy; // 惯性矩阵的Ixy分量
    double ixz; // 惯性矩阵的Ixz分量
    double iyy; // 惯性矩阵的Iyy分量
    double iyz; // 惯性矩阵的Iyz分量
    double izz; // 惯性矩阵的Izz分量
};
struct Inertial {
    double mass;           // 质量
    Pose origin;           // 惯性属性相对于link的原点
    Inertia inertia_matrix; // 惯性矩阵
};
struct Visual {
    Pose origin;           // 可视化属性相对于link的原点
    Geometry geometry;     // 几何形状
    std::string material;  // 材料
};



// 定义一个URDFLink结构体
struct URDFLink {
    std::string name;                  // link的名称
    Inertial inertial;                 // link的惯性属性
    std::vector<Visual> visuals;       // link的可视化属性，可以有多个
    std::vector<Collision> collisions; // link的碰撞属性，可以有多个
};

//
struct Axis {
    double xyz[3];                    // 轴的方向
};

struct Limits {
    double lower;                     // 下限
    double upper;                     // 上限
    double effort;                    // 力矩限制
    double velocity;                  // 速度限制
};

struct Dynamics {
    double damping;                   // 阻尼
    double friction;                  // 摩擦
};

struct Calibration {
    double rising;                    // 校准参数
    double falling;                   // 校准参数
};

struct Mimic {
    std::string joint_name;           // 模仿的关节名称
    double multiplier;                // 模仿的倍数
    double offset;                    // 模仿的偏移量
};

struct SafetyController {
    std::string soft_lower_limit;     // 软下限
    std::string soft_upper_limit;     // 软上限
    double k_position;                // 位置控制增益
    double k_velocity;                // 速度控制增益
};
// 定义一个URDFJoint结构体
struct URDFJoint {
    std::string name;                 // joint的名称
    std::string type;                 // joint的类型，如"revolute"、"continuous"等
    std::string parent_link;          // 父链接的名称
    std::string child_link;           // 子链接的名称
    Pose parent_to_child_origin;      // 父链接到子链接的变换
    Axis axis;                        // 旋转关节的轴
    Limits limits;                    // 关节的限制
    Dynamics dynamics;                // 关节的动力学参数
    Calibration calibration;          // 关节的校准参数
    Mimic mimic;                      // 关节的模仿参数
    SafetyController safety_controller; // 安全控制器
};



#endif // PARAM_STRUCT_H
