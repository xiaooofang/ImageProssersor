#ifndef IMAGEPROSSERSOR_H
#define IMAGEPROSSERSOR_H

#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QToolBar>
#include <QImage>
#include <QLabel>
#include <QStatusBar>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include "imagetransform.h"

class ImageProssersor : public QMainWindow
{
    Q_OBJECT

public:
    ImageProssersor(QWidget *parent = nullptr);
    ~ImageProssersor();
    void loadFile(QString filename);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;  // 新增

private slots:
    void showOpenFile();
    void zoomIn();
    void zoomOut();
    void showGeometryTransform();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void updateMousePosition(QMouseEvent *event);  // 新增
    void updateClickPosition(QMouseEvent *event);   // 新增

    QLabel *clickInfoLabel;    // 新增：左下側點擊資訊
    QLabel *mousePosLabel;
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
    
    // 區域選取相關變數
    bool isSelecting = false;
    QPoint selectionStart;
    QPoint selectionEnd;
    QRect selectionRect;
    QPixmap baseScaledPixmap;  // 快取基礎縮放圖片以提升效能
};

#endif // IMAGEPROSSERSOR_H
