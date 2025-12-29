#ifndef IMAGETRANSFORM_H
#define IMAGETRANSFORM_H

#include <QWidget>
#include <QLabel>
#include <QGroupBox>
#include <QCheckBox>
#include <QPushButton>
#include <QDial>
#include <QSpacerItem>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QImage>
#include <QToolBar>
#include <QAction>

class ImageTransform : public QWidget
{
    Q_OBJECT

public:
    ImageTransform(QWidget *parent = nullptr);
    ~ImageTransform();
    QImage       srcImg;
    QLabel      *inWin;
private slots:
    void openImage();        // 開啟 png
    void saveImage();        // 儲存圖片
    void mirroredImage();
    void rotatedImage(int angle);

private:
    QToolBar    *toolBar;
    QAction     *openAction;
    QAction     *saveAction;

    QGroupBox   *mirrorGroup;
    QCheckBox   *hCheckBox;
    QCheckBox   *vCheckBox;
    QPushButton *mirrorButton;
    QDial       *rotateDial;

    QHBoxLayout *mainLayout;
    QVBoxLayout *leftLayout;


    QImage       dstImg;
};

#endif // IMAGETRANSFORM_H
