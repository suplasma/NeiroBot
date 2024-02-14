#pragma once

#include <QMainWindow>

#include <neiro.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(Neiro &neiro, QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_saveBtn_clicked();
    void on_saveAtBtn_clicked();
    void on_testBtn_clicked();
    void on_startBtn_clicked();
    void on_stopBtn_clicked();
    void on_loadBtn_clicked();
    void on_testPathBtn_clicked();
    void on_pathBtn_clicked();
    void on_initBtn_clicked();
    void on_learningRate_textChanged(const QString &text);
    void on_period_timeChanged(const QTime &time);
    void on_scale_valueChanged(int scale);
    void on_testBox_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;
    Neiro &neiro;

    void paintGraph(QVector<double> &x, QVector<double> &y, QVector<double> &yAsp, double &min,
                    double &max);
    void setNameSave(const QString &title);
};
