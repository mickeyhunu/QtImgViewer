#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qgraphicsview.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_selectButton_clicked();
    void on_comboBox_currentTextChanged(const QString &arg1);

    void on_plusButton_clicked();

    void on_minusButton_clicked();

    void on_spinBox_textChanged(const QString &arg1);

    void on_fitButton_clicked();

    void on_basicButton_clicked();

    void on_selectFathButton_clicked();

    void on_listView_clicked(const QModelIndex &index);

    void on_centerButton_clicked();

private:
    Ui::MainWindow *ui;
    void setUi();

    bool eventFilter(QObject *obj, QEvent *event);

    void loadImage(const QImage &image);
    void centerViewOnImage();
    void fitImage();

    QSize originalImageSize;
    double currentScaleFactor = 1;

    int clickCount = 0;
    int agoClickCount = 0;

    QString folderPath;

    QPoint lastPos = QPoint(0, 0);

    const double MAX_SCALE_FACTOR = 2.99;
    const double MIN_SCALE_FACTOR = 0.01;
};
#endif // MAINWINDOW_H
