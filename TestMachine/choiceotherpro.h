#ifndef CHOICEOTHERPRO_H
#define CHOICEOTHERPRO_H

#include <QDialog>

namespace Ui {
    class choiceOtherPro;
}

class choiceOtherPro : public QDialog
{
    Q_OBJECT

public:
    explicit choiceOtherPro(QWidget *parent = 0);
    ~choiceOtherPro();

private slots:
    void on_pushButton_sc_clicked();

    void on_pushButton_kaoji_clicked();

    void on_pushButton_guide_clicked();
signals:
    void endPro(QString);

private:
    Ui::choiceOtherPro *ui;
};

#endif // CHOICEOTHERPRO_H
