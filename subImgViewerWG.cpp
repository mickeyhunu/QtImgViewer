#include "subImgViewerWG.h"
#include "ui_subImgViewerWG.h"

subImgViewerWG::subImgViewerWG(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::subImgViewerWG)
{
    ui->setupUi(this);
}

subImgViewerWG::~subImgViewerWG()
{
    delete ui;
}
