#ifndef IMAGEPROSSERSOR_H
#define IMAGEPROSSERSOR_H

#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QToolBar>
#include <QImage>
#include <QLabel>
#include "imagetransform.h"

class ImageProssersor : public QMainWindow
{
    Q_OBJECT

public:
    ImageProssersor(QWidget *parent = nullptr);
    ~ImageProssersor();
    void createActions();
    void createMenus();
    void createToolBars();
    void loadFile(QString filename);

private slots:
    void showOpenFile();
    void zoomIn();
    void zoomOut();
    void showGeometryTransform();
private:
    ImageTransform *gWin;
    QWidget *central;
    QMenu *fileMenu;
    QToolBar *fileTool;
    QImage img;
    QString filename;
    QLabel *imgWin;
    QAction *openFileAction;
    QAction *exitAction;
    QAction *zoomInAction;
    QAction *zoomOutAction;
    QAction *geometryAction;
    qreal scaleFactor = 1.0;  // 用於放大縮小的縮放因子
};

#endif // IMAGEPROSSERSOR_H
