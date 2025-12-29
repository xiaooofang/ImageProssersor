#include "imageprossersor.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QFileDialog>
#include <QDebug>
#include <QPixmap>
#include <QAction>
#include <QStatusBar>
#include <QMouseEvent>
#include "imagetransform.h"

ImageProssersor::ImageProssersor(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("影像處理"));

    // 創建中央窗口和佈局
    central = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    // 顯示圖像的 QLabel
    imgWin = new QLabel();
    imgWin->setMouseTracking(true);  // 啟用滑鼠追蹤
    imgWin->installEventFilter(this);  // 安裝事件過濾器以捕獲 imgWin 的滑鼠事件
    QPixmap *initPixmap = new QPixmap(300, 200);
    gWin = new ImageTransform();
    initPixmap->fill(QColor(255, 255, 255));
    imgWin->resize(300, 200);
    imgWin->setScaledContents(true);
    imgWin->setPixmap(*initPixmap);

    mainLayout->addWidget(imgWin);

    // 創建底部資訊顯示區域
    QHBoxLayout *infoLayout = new QHBoxLayout();

    // 左下側：點擊位置資訊
    clickInfoLabel = new QLabel(QStringLiteral("點擊位置: 無"));
    clickInfoLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    clickInfoLabel->setMinimumHeight(30);
    clickInfoLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // 右下側：即時滑鼠位置資訊
    mousePosLabel = new QLabel(QStringLiteral("位置: 無"));
    mousePosLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    mousePosLabel->setMinimumHeight(30);
    mousePosLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    infoLayout->addWidget(clickInfoLabel, 1);
    infoLayout->addWidget(mousePosLabel, 1);

    mainLayout->addLayout(infoLayout);
    setCentralWidget(central);

    createActions();
    createMenus();
    createToolBars();

    // 創建狀態列
    statusBar()->showMessage("準備就緒");
}

bool ImageProssersor::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == imgWin) {
        if (event->type() == QEvent::MouseMove) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            updateMousePosition(mouseEvent);
            
            // 如果正在選取區域，更新選取範圍
            if (isSelecting && (mouseEvent->buttons() & Qt::LeftButton)) {
                selectionEnd = mouseEvent->pos();
                
                // 使用快取的基礎圖片，僅添加選取框
                QPixmap pixmap = baseScaledPixmap;
                QPainter painter(&pixmap);
                painter.setPen(QPen(Qt::red, 2, Qt::DashLine));
                
                // 計算選取矩形
                int x = qMin(selectionStart.x(), selectionEnd.x());
                int y = qMin(selectionStart.y(), selectionEnd.y());
                int w = qAbs(selectionEnd.x() - selectionStart.x());
                int h = qAbs(selectionEnd.y() - selectionStart.y());
                selectionRect = QRect(x, y, w, h);
                
                painter.drawRect(selectionRect);
                painter.end();
                
                imgWin->setPixmap(pixmap);
            }
            return false;
        } else if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                updateClickPosition(mouseEvent);
                
                // 開始區域選取
                if (!img.isNull()) {
                    isSelecting = true;
                    selectionStart = mouseEvent->pos();
                    selectionEnd = selectionStart;
                    
                    // 快取基礎縮放圖片
                    baseScaledPixmap = QPixmap::fromImage(img).scaled(imgWin->size(), 
                        Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                }
            }
            return false;
        } else if (event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton && isSelecting) {
                // 完成區域選取
                selectionEnd = mouseEvent->pos();
                isSelecting = false;
                
                // 計算最終選取矩形
                int x = qMin(selectionStart.x(), selectionEnd.x());
                int y = qMin(selectionStart.y(), selectionEnd.y());
                int w = qAbs(selectionEnd.x() - selectionStart.x());
                int h = qAbs(selectionEnd.y() - selectionStart.y());
                selectionRect = QRect(x, y, w, h);
                
                // 如果選取了有效區域（不是單點擊），則自動跳轉至幾何轉換
                if (w > 5 && h > 5) {  // 最小選取範圍 5x5 像素
                    showGeometryTransform();
                } else {
                    // 恢復原圖顯示（清除選取框）
                    imgWin->setPixmap(baseScaledPixmap);
                }
            }
            return false;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void ImageProssersor::updateMousePosition(QMouseEvent *event)
{
    if (img.isNull()) {
        mousePosLabel->setText(QStringLiteral("位置: 無圖片"));
        return;
    }

    // 獲取滑鼠在 imgWin 中的位置
    QPoint pos = event->pos();

    // 計算實際圖片座標
    int imgX = pos.x() * img.width() / imgWin->width();
    int imgY = pos.y() * img.height() / imgWin->height();

    // 確保座標在圖片範圍內
    if (imgX >= 0 && imgY >= 0 && imgX < img.width() && imgY < img.height()) {
        // 獲取對應位置的像素顏色
        QColor pixelColor = img.pixelColor(imgX, imgY);

        // 計算灰階值
        int grayValue = qRound(0.299 * pixelColor.red() +
                               0.587 * pixelColor.green() +
                               0.114 * pixelColor.blue());

        // 更新右下側標籤
        QString infoText = QString("位置: (%1, %2) | 灰階: %3 | RGB: (%4, %5, %6)")
                               .arg(imgX)
                               .arg(imgY)
                               .arg(grayValue)
                               .arg(pixelColor.red())
                               .arg(pixelColor.green())
                               .arg(pixelColor.blue());

        mousePosLabel->setText(infoText);
    } else {
        mousePosLabel->setText(QStringLiteral("位置: 超出範圍"));
    }
}

void ImageProssersor::updateClickPosition(QMouseEvent *event)
{
    if (img.isNull()) {
        clickInfoLabel->setText(QStringLiteral("點擊位置: 無圖片"));
        return;
    }

    // 獲取滑鼠在 imgWin 中的位置
    QPoint pos = event->pos();

    // 計算實際圖片座標
    int imgX = pos.x() * img.width() / imgWin->width();
    int imgY = pos.y() * img.height() / imgWin->height();

    // 確保座標在圖片範圍內
    if (imgX >= 0 && imgY >= 0 && imgX < img.width() && imgY < img.height()) {
        // 獲取對應位置的像素顏色
        QColor pixelColor = img.pixelColor(imgX, imgY);

        // 計算灰階值
        int grayValue = qRound(0.299 * pixelColor.red() +
                               0.587 * pixelColor.green() +
                               0.114 * pixelColor.blue());

        // 更新左下側標籤
        QString clickText = QString("點擊位置: (%1, %2) | 灰階: %3")
                                .arg(imgX)
                                .arg(imgY)
                                .arg(grayValue);

        clickInfoLabel->setText(clickText);
    } else {
        clickInfoLabel->setText(QStringLiteral("點擊位置: 超出範圍"));
    }
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

    // 重置資訊標籤
    clickInfoLabel->setText(QStringLiteral("點擊位置: 無"));
    mousePosLabel->setText(QStringLiteral("位置: 無"));
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

void ImageProssersor::showGeometryTransform()
{
    if(!img.isNull()) {
        // 如果有選取區域，則只傳遞選取的部分
        if (!selectionRect.isNull() && selectionRect.width() > 0 && selectionRect.height() > 0) {
            // 將 imgWin 的座標轉換為原始圖片座標 (使用浮點數提升精度)
            qreal scaleX = static_cast<qreal>(img.width()) / imgWin->width();
            qreal scaleY = static_cast<qreal>(img.height()) / imgWin->height();
            
            int imgX = qRound(selectionRect.x() * scaleX);
            int imgY = qRound(selectionRect.y() * scaleY);
            int imgW = qRound(selectionRect.width() * scaleX);
            int imgH = qRound(selectionRect.height() * scaleY);
            
            // 確保座標在圖片範圍內
            imgX = qMax(0, qMin(imgX, img.width() - 1));
            imgY = qMax(0, qMin(imgY, img.height() - 1));
            imgW = qMin(imgW, img.width() - imgX);
            imgH = qMin(imgH, img.height() - imgY);
            
            // 擷取選取區域
            QImage croppedImg = img.copy(imgX, imgY, imgW, imgH);
            gWin->srcImg = croppedImg;
            
            // 清除選取框
            selectionRect = QRect();
            imgWin->setPixmap(baseScaledPixmap);
        } else {
            gWin->srcImg = img;
        }
    }

    // 初始化 displayPixmap
    gWin->displayPixmap = QPixmap::fromImage(gWin->srcImg).scaled(gWin->inWin->size(), 
        Qt::KeepAspectRatio, Qt::SmoothTransformation);
    gWin->inWin->setPixmap(gWin->displayPixmap);
    gWin->show();
}
