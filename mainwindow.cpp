#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <iostream>
#include <QRectF>

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include <QVector>
#include <QDirIterator>

#include <QStandardItemModel>

#include <QGraphicsScene>
#include <QWheelEvent>
#include <QMouseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setUi();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//Set Ui
void MainWindow::setUi()
{
    QStringList scales;
    scales << tr("25%") << tr("50%") << tr("75%") << tr("100%") << tr("125%") << tr("150%") << tr("175%") << tr("200%");
    ui -> comboBox -> addItems(scales);
    ui -> comboBox -> setCurrentIndex(3);

    ui->graphicsView->viewport()->installEventFilter(this);
}

//FileList
bool isImageFile(const QString &fileName) {
    QStringList imageExtensions = {"png", "jpg", "jpeg", "bmp", "gif"};
    QFileInfo fileInfo(fileName);
    return imageExtensions.contains(fileInfo.suffix(), Qt::CaseInsensitive);
}

//Ui Event
void MainWindow::on_selectButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,"Open Image File",QDir::currentPath());

    QImage image(fileName);

    if (image.isNull())
    {
        QMessageBox::information(this, tr("Image Viewer"), tr("Cannot load %1.").arg(fileName));
        return;
    }

    loadImage(image);
}

void MainWindow::on_selectFathButton_clicked()
{
    folderPath = QFileDialog::getExistingDirectory(this, tr("Select Folder"), QDir::homePath());

    // 사용자가 폴더를 선택하지 않은 경우 또는 취소한 경우
    if (folderPath.isEmpty())
    {
        return;
    }

    // 디렉토리 객체 생성
    QDir directory(folderPath);

    // 디렉토리 안의 파일 목록 가져오기
    QStringList fileList = directory.entryList(QDir::Files);

    // 모델 생성
    QStandardItemModel *model = new QStandardItemModel;

    for (const QString &file : fileList) {
        if (isImageFile(file))
        {
            QStandardItem *item = new QStandardItem(file);
            model->appendRow(item);
        }
    }

    ui->listView->setModel(model);
}

void MainWindow::on_listView_clicked(const QModelIndex &index)
{
    QString selectedFileName = ui->listView->model()->data(index, Qt::DisplayRole).toString();

    if (!isImageFile(selectedFileName)) {
        return;
    }

    QString filePath = QDir(folderPath).filePath(selectedFileName);
    QImage image(filePath);

    loadImage(image);
}

void MainWindow::on_basicButton_clicked()
{
    if (ui->graphicsView->scene() != nullptr)
    {
        currentScaleFactor = 1;
        ui->spinBox->setValue(static_cast<int>(currentScaleFactor * 100));
    }
}

void MainWindow::on_centerButton_clicked()
{
    centerViewOnImage();
}

void MainWindow::on_fitButton_clicked()
{
    fitImage();
}

void MainWindow::on_spinBox_textChanged(const QString &arg1)
{
    //모든 버튼에서 spinBox값만 바꿔주면 이 함수를 타기때문에, 실제 이미지 조정 이벤트는 이 함수에서만 일어남
    double newScale = arg1.left(arg1.indexOf(tr("%"))).toDouble() / 100;

    if (ui->graphicsView->scene() != nullptr)
    {
        //300이상 가지 않도록 lock
        if ( currentScaleFactor > MAX_SCALE_FACTOR)
            return;

        ui->graphicsView->resetTransform();
        ui->graphicsView->scale(newScale, newScale);

        currentScaleFactor = newScale;
    }
}

void MainWindow::on_plusButton_clicked()
{
    if (ui->graphicsView->scene() != nullptr)
    {
        //300이상 가지 않도록 lock
        if ( currentScaleFactor > MAX_SCALE_FACTOR - 0.2)
            return;

        //클릭수가 +1 된 상태면 +0.2
        agoClickCount = clickCount;
        clickCount++;

        if(agoClickCount < clickCount)
            currentScaleFactor += 0.2;

        ui->spinBox->setValue(static_cast<int>(currentScaleFactor * 100));
    }
}

void MainWindow::on_minusButton_clicked()
{
    if (ui->graphicsView->scene() != nullptr)
    {
        //0이하 가지 않도록 lock
        if ( currentScaleFactor < MIN_SCALE_FACTOR + 0.2)
            return;

        //클릭수가 -1 된 상태면 -0.2
        agoClickCount = clickCount;
        clickCount--;

        if(agoClickCount > clickCount)
            currentScaleFactor -= 0.2;

        ui->spinBox->setValue(static_cast<int>(currentScaleFactor * 100));
    }
}

void MainWindow::on_comboBox_currentTextChanged(const QString &arg1)
{
    double newScale = arg1.left(arg1.indexOf(tr("%"))).toDouble() / 100;

    if (ui->graphicsView->scene() != nullptr)
    {
        currentScaleFactor = newScale;
        ui->spinBox->setValue(static_cast<int>(currentScaleFactor * 100));
    }
}

//graphicView eventFilter
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->graphicsView->viewport())
    {
        if (event->type() == QEvent::Wheel)// 마우스 휠 이벤트 처리 (확대/축소)
        {
            QWheelEvent *wheelEvent = dynamic_cast<QWheelEvent*>(event);
            int delta = wheelEvent->angleDelta().y();
            if (wheelEvent)
            {
                QPoint numPixels = wheelEvent->pixelDelta();
                QPoint numDegrees = wheelEvent->angleDelta() / 8;

                if (!numPixels.isNull())
                {
                    // pixel 단위로 스크롤
                }
                else if (!numDegrees.isNull())
                {
                    // degree 단위로 스크롤
                    if (delta > 0)
                    {
                        if ( currentScaleFactor > MAX_SCALE_FACTOR - 0.01)
                            return true;

                        currentScaleFactor += 0.02;
                        ui->spinBox->setValue(static_cast<int>(currentScaleFactor * 100));
                    }
                    else if (delta < 0)
                    {
                        if ( currentScaleFactor < MIN_SCALE_FACTOR + 0.02)
                            return true;

                        currentScaleFactor -= 0.02;
                        ui->spinBox->setValue(static_cast<int>(currentScaleFactor * 100));
                    }
                }
                return true;
            }
            else
            {
                qDebug() << "Failed to cast to QWheelEvent";
            }
        }
        else if (event->type() == QEvent::MouseButtonPress) // 마우스 클릭 이벤트 처리 (위치 조정을 위한 시작점 기록)
        {
            QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent*>(event);
            if (mouseEvent && mouseEvent->button() == Qt::LeftButton)
            {
                lastPos = mouseEvent->pos();
            }
            else
            {
                qDebug() << "Failed to cast to QMouseEvent or invalid button";
            }
            return true;
        }
        else if (event->type() == QEvent::MouseMove) // 마우스 이동 이벤트 처리 (위치 조정)
        {
            QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent*>(event);
            if (mouseEvent && (mouseEvent->buttons() & Qt::LeftButton))
            {
                QPointF delta = mouseEvent->pos() - lastPos;
                ui->graphicsView->setSceneRect(ui->graphicsView->sceneRect().translated(-delta.x(), -delta.y()));
                lastPos = mouseEvent->pos();
            }
            else
            {
                qDebug() << "Failed to cast to QMouseEvent or invalid button state";
            }
            return true;
        }
    }

    return QMainWindow::eventFilter(obj, event);
}


//function
void MainWindow::loadImage(const QImage &image)
{
    QGraphicsScene* scene = new QGraphicsScene();
    QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(image));
    scene->addItem(item);

    ui->graphicsView->setScene(scene);
    ui->graphicsView->update();

    originalImageSize = image.size();
    fitImage();
    centerViewOnImage();
}

void MainWindow::centerViewOnImage()
{
    // 적절한 이미지 크기를 사용하여 QRectF를 생성
    QRectF imageRect(0, 0, originalImageSize.width(), originalImageSize.height());

    // 뷰의 중심으로 장면을 이동
    ui->graphicsView->centerOn(imageRect.center());

    // 장면의 영역을 새로운 이미지 크기로 설정 (선택 사항)
    ui->graphicsView->setSceneRect(imageRect);
}

void MainWindow::fitImage()
{
    if (ui->graphicsView->scene() != nullptr)
    {
        ui->graphicsView->resetTransform(); // Reset transformations
        ui->graphicsView->fitInView(0, 0, originalImageSize.width(), originalImageSize.height(), Qt::KeepAspectRatio);

        qreal fitScaleX = ui->graphicsView->transform().m11();
        qreal fitScaleY = ui->graphicsView->transform().m22();

        // 스케일 팩터를 현재 스케일 팩터로 설정 qMax 둘중 큰 수를 현재 팩터로
        currentScaleFactor = qMax(fitScaleX, fitScaleY);
        ui->spinBox->setValue(static_cast<int>(currentScaleFactor * 100));
    }
}

