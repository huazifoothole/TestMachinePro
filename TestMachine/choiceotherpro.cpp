#include "choiceotherpro.h"
#include "ui_choiceotherpro.h"

choiceOtherPro::choiceOtherPro(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::choiceOtherPro)
{
    ui->setupUi(this);
}

choiceOtherPro::~choiceOtherPro()
{
    delete ui;
}

void choiceOtherPro::on_pushButton_sc_clicked()
{
     emit endPro("sc");
     delete ui;
}

void choiceOtherPro::on_pushButton_kaoji_clicked()
{
    emit endPro("kaoji");
     delete ui;
}

void choiceOtherPro::on_pushButton_guide_clicked()
{
    emit endPro("LATECH");
     delete ui;
}
