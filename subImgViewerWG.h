#ifndef SUBIMGVIEWERWG_H
#define SUBIMGVIEWERWG_H

#include <QWidget>

namespace Ui {
class subImgViewerWG;
}

class subImgViewerWG : public QWidget
{
    Q_OBJECT

public:
    explicit subImgViewerWG(QWidget *parent = nullptr);
    ~subImgViewerWG();

private:
    Ui::subImgViewerWG *ui;
};

#endif // SUBIMGVIEWERWG_H
