#include "ui_collapsiblewidget.h"
#include "collapsiblewidget.h"
CollapsibleWidget::CollapsibleWidget(const QString &title, QWidget *parent)
    : QWidget(parent), ui(new Ui::CollapsibleWidget) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    // 创建标题按钮
    QPushButton *titleButton = new QPushButton(title, this);
    layout->addWidget(titleButton);

    // 创建折叠区域
    contentWidget = new QWidget(this);
    ui->setupUi(contentWidget);  // 设置UI内容
    contentWidget->setVisible(false); // 默认隐藏内容
    layout->addWidget(contentWidget);
    connect(titleButton, &QPushButton::clicked, this, &CollapsibleWidget::toggleContentVisibility);
}

CollapsibleWidget::~CollapsibleWidget() {
    delete ui;
}

void CollapsibleWidget::toggleContentVisibility() {
    contentWidget->setVisible(!contentWidget->isVisible());
}
