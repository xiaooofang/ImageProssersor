#include "imagetransform.h"
#include <QFileDialog>
#include <QPixmap>
#include <QTransform>

ImageTransform::ImageTransform(QWidget *parent)
    : QWidget(parent)
{
    toolBar = new QToolBar(this);
    openAction = toolBar->addAction("Open");
    saveAction = toolBar->addAction("Save");
    inWin = new QLabel("No Image");
    inWin->setAlignment(Qt::AlignCenter);
    inWin->setMouseTracking(true);  // 啟用滑鼠追蹤以支援繪圖
    inWin->installEventFilter(this);  // 安裝事件過濾器以捕獲 inWin 的滑鼠事件
    
    mirrorGroup = new QGroupBox("Mirror");
    hCheckBox = new QCheckBox("Horizontal");
    vCheckBox = new QCheckBox("Vertical");
    mirrorButton = new QPushButton("Apply");
    QVBoxLayout *groupLayout = new QVBoxLayout;
    rotateDial = new QDial;
    rotateDial->setRange(0, 360);
    leftLayout = new QVBoxLayout;
    leftLayout->addWidget(toolBar);
    leftLayout->addWidget(mirrorGroup);
    leftLayout->addWidget(rotateDial);
    mainLayout = new QHBoxLayout(this);
    mainLayout->addLayout(leftLayout);
    mainLayout->addWidget(inWin);
    groupLayout->addWidget(hCheckBox);
    groupLayout->addWidget(vCheckBox);
    groupLayout->addWidget(mirrorButton);
    mirrorGroup->setLayout(groupLayout);
    connect(openAction, &QAction::triggered,
            this, &ImageTransform::openImage);
    connect(saveAction, &QAction::triggered,
            this, &ImageTransform::saveImage);
    connect(mirrorButton, &QPushButton::clicked,
            this, &ImageTransform::mirroredImage);
    connect(rotateDial, &QDial::valueChanged,
            this, &ImageTransform::rotatedImage);
    setLayout(mainLayout);
}

ImageTransform::~ImageTransform() {}

void ImageTransform::openImage()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Open Image",
        "",
        "PNG Files (*.png);;JPEG Files (*.jpg *.jpeg)"
        );

    if (fileName.isEmpty())
        return;

    srcImg.load(fileName);
    dstImg = srcImg;

    displayPixmap = QPixmap::fromImage(srcImg).scaled(inWin->size(),
                                                       Qt::KeepAspectRatio);
    inWin->setPixmap(displayPixmap);
}



void ImageTransform::mirroredImage()
{
    if (srcImg.isNull()) return;

    bool h = hCheckBox->isChecked();
    bool v = vCheckBox->isChecked();

    dstImg = srcImg.mirrored(h, v);
    displayPixmap = QPixmap::fromImage(dstImg).scaled(inWin->size(),
                                                       Qt::KeepAspectRatio);
    inWin->setPixmap(displayPixmap);
}

void ImageTransform::rotatedImage(int angle)
{
    if (srcImg.isNull()) return;

    QTransform transform;
    transform.rotate(angle);

    dstImg = srcImg.transformed(transform);
    displayPixmap = QPixmap::fromImage(dstImg).scaled(inWin->size(),
                                                       Qt::KeepAspectRatio);
    inWin->setPixmap(displayPixmap);
}
void ImageTransform::saveImage()
{
    if (dstImg.isNull()) return;

    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Save Image",
        "",
        "PNG Files (*.png)"
        );

    if (!fileName.isEmpty())
        dstImg.save(fileName, "PNG");
}

bool ImageTransform::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == inWin && !srcImg.isNull()) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                isDrawing = true;
                lastDrawPoint = mouseEvent->pos();
                
                // 如果 displayPixmap 為空，則初始化
                if (displayPixmap.isNull()) {
                    displayPixmap = inWin->pixmap();
                }
            }
            return false;
        } else if (event->type() == QEvent::MouseMove) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (isDrawing && (mouseEvent->buttons() & Qt::LeftButton)) {
                // 在 displayPixmap 上繪製線條
                if (!displayPixmap.isNull()) {
                    QPainter painter(&displayPixmap);
                    painter.setPen(QPen(Qt::red, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                    painter.drawLine(lastDrawPoint, mouseEvent->pos());
                    painter.end();
                    
                    // 更新顯示
                    inWin->setPixmap(displayPixmap);
                    lastDrawPoint = mouseEvent->pos();
                }
            }
            return false;
        } else if (event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                isDrawing = false;
            }
            return false;
        }
    }
    return QWidget::eventFilter(obj, event);
}


