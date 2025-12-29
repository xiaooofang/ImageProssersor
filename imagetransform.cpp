#include "ImageTransform.h"
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

    inWin->setPixmap(QPixmap::fromImage(srcImg)
                         .scaled(inWin->size(),
                                 Qt::KeepAspectRatio));
}



void ImageTransform::mirroredImage()
{
    if (srcImg.isNull()) return;

    bool h = hCheckBox->isChecked();
    bool v = vCheckBox->isChecked();

    dstImg = srcImg.mirrored(h, v);
    inWin->setPixmap(QPixmap::fromImage(dstImg)
                         .scaled(inWin->size(),
                                 Qt::KeepAspectRatio));
}

void ImageTransform::rotatedImage(int angle)
{
    if (srcImg.isNull()) return;

    QTransform transform;
    transform.rotate(angle);

    dstImg = srcImg.transformed(transform);
    inWin->setPixmap(QPixmap::fromImage(dstImg)
                         .scaled(inWin->size(),
                                 Qt::KeepAspectRatio));
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


