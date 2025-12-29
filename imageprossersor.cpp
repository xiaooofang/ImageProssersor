#include "imageprossersor.h"
#include <QHBoxLayout>
#include <QMenuBar>
#include <QFileDialog>
#include <QDebug>
#include <QPixmap>
#include <QAction>
#include "imagetransform.h"

ImageProssersor::ImageProssersor(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("影像處理"));
    central = new QWidget();
    QHBoxLayout *mainLayout = new QHBoxLayout(central);

    imgWin = new QLabel();
    QPixmap *initPixmap = new QPixmap(300, 200);
    gWin = new ImageTransform();
    initPixmap->fill(QColor(255, 255, 255));
    imgWin->resize(300, 200);
    imgWin->setScaledContents(true);
    imgWin->setPixmap(*initPixmap);

    mainLayout->addWidget(imgWin);
    setCentralWidget(central);

    createActions();
    createMenus();
    createToolBars();
}

ImageProssersor::~ImageProssersor() {}

void ImageProssersor::createActions() {
    openFileAction = new QAction(QStringLiteral("開啟檔案(&O)"), this);
    openFileAction->setShortcut(tr("Ctrl+O"));
    openFileAction->setStatusTip(QStringLiteral("開啟外部檔案"));
    connect(openFileAction, SIGNAL(triggered()), this, SLOT(showOpenFile()));

    exitAction = new QAction(QStringLiteral("結束(&X)"), this);
    exitAction->setShortcut(tr("Ctrl+Q"));
    exitAction->setStatusTip(QStringLiteral("退出程式"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    zoomInAction = new QAction(QStringLiteral("放大(&Z)"), this);
    zoomInAction->setShortcut(tr("Ctrl++"));
    zoomInAction->setStatusTip(QStringLiteral("放大影像"));
    connect(zoomInAction, SIGNAL(triggered()), this, SLOT(zoomIn()));

    zoomOutAction = new QAction(QStringLiteral("縮小(&X)"), this);
    zoomOutAction->setShortcut(tr("Ctrl+-"));
    zoomOutAction->setStatusTip(QStringLiteral("縮小影像"));
    connect(zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOut()));

    geometryAction = new QAction(QStringLiteral("幾何轉換"),this);
    geometryAction->setShortcut(tr("Ctrl+G"));
    geometryAction->setStatusTip(QStringLiteral("影像幾何轉換"));
    connect(geometryAction,SIGNAL(triggered()),this,SLOT(showGeometryTransform()));
    connect(exitAction,SIGNAL(triggered()),gWin,SLOT(close()));

}

void ImageProssersor::createMenus() {
    fileMenu = menuBar()->addMenu(QStringLiteral("檔案(&F)"));
    fileMenu->addAction(openFileAction);
    fileMenu->addAction(geometryAction);
    fileMenu->addAction(exitAction);
}

void ImageProssersor::createToolBars() {
    fileTool = addToolBar("file");
    fileTool->addAction(openFileAction);
    fileTool->addAction(zoomInAction);
    fileTool->addAction(zoomOutAction);
    fileTool->addAction(geometryAction);
}

void ImageProssersor::loadFile(QString filename)
{
    img.load(filename);
    imgWin->setPixmap(QPixmap::fromImage(img));

    scaleFactor = 1.0;
    imgWin->resize(img.size());   // 初始顯示大小
}


void ImageProssersor::showOpenFile() {
    filename = QFileDialog::getOpenFileName(this,
                                            QStringLiteral("開啟影像"),
                                            tr("."),
                                            "bmp(*.bmp);;png(*.png);;Jpeg(*.jpg)");

    if (!filename.isEmpty()) {
        if (img.isNull()) {
            loadFile(filename);
        } else {
            ImageProssersor *newIPWin = new ImageProssersor();
            newIPWin->show();
            newIPWin->loadFile(filename);
        }
    }
}
void ImageProssersor::zoomIn()
{
    scaleFactor *= 1.2;
    imgWin->resize(img.size() * scaleFactor);
}

void ImageProssersor::zoomOut()
{
    scaleFactor /= 1.2;
    imgWin->resize(img.size() * scaleFactor);
}
void ImageProssersor:: showGeometryTransform()
{
    if(!img.isNull())
    gWin->srcImg = img;

    gWin->inWin->setPixmap(QPixmap::fromImage(gWin->srcImg));
    gWin->show();
}

