#include "mainwindow.h"

#include "ui_mainwindow.h"

MainWindow::MainWindow(Neiro &neiro, QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow), neiro(neiro)
{
    ui->setupUi(this);
    ui->qcustomplot->setInteraction(QCP::iRangeDrag, true);
    ui->qcustomplot->setInteraction(QCP::iRangeZoom, true);
    ui->stopBtn->setVisible(false);
    neiro.setRate(ui->learningRate->text().toDouble());
    neiro.setPeriod(ui->period->time().msecsSinceStartOfDay());
    neiro.setScale(ui->scale->value());
    neiro.setTestVariant(ui->testBox->currentIndex());
    setNameSave(windowTitle());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintGraph(QVector<double> &x, QVector<double> &y, QVector<double> &yAsp,
                            double &min, double &max)
{
    ui->qcustomplot->clearGraphs();

    ui->qcustomplot->xAxis->setRange(0.0, x.last());
    ui->qcustomplot->yAxis->setRange(min, max);

    ui->qcustomplot->addGraph();
    ui->qcustomplot->graph(0)->setData(x, y);
    ui->qcustomplot->graph(0)->setPen(QPen(QColor(0, 255, 0, 128), 5.0));

    ui->qcustomplot->addGraph();
    ui->qcustomplot->graph(1)->setData(x, yAsp);
    ui->qcustomplot->graph(1)->setPen(QPen(QColor(0, 0, 255, 128), 5.0));

    ui->qcustomplot->replot();
}

void MainWindow::setNameSave(const QString &title)
{
    setWindowTitle(title);
    neiro.setFileAutoSave(QString(ROOT_PATH "/data/autosave/%1").arg(title));
}

void MainWindow::on_saveBtn_clicked()
{
    neiro.save(QString(ROOT_PATH "/data/save/%1").arg(windowTitle()));
}

void MainWindow::on_saveAtBtn_clicked()
{
    auto path = QFileDialog::getSaveFileName(this, "Сохранить", ROOT_PATH "/data/save");

    if (!path.isEmpty()) {
        neiro.save(path);
        setNameSave(path.split("/").last());
    }
}

void MainWindow::on_testBtn_clicked()
{
    double min = INFINITY;
    double max = -INFINITY;
    QVector<double> x;
    QVector<double> y;
    QVector<double> yAsp;
    double sum = 0.0;
    int n0 = 0;
    int n1 = 0;
    int n2 = 0;
    int correct = 0;
    int wrong = 0;
    double error = 0.0;
    neiro.test(x, y, yAsp, min, max, sum, n0, n1, n2, correct, wrong, error);
    paintGraph(x, y, yAsp, min, max);

    QString str = QString("null %1, long %2, short %3, sum %4, correct %5, wrong %6, error %7")
                      .arg(n0)
                      .arg(n1)
                      .arg(n2)
                      .arg(sum)
                      .arg(correct)
                      .arg(wrong)
                      .arg(error);
    qDebug() << str;
    ui->test->setText(str);
}

void MainWindow::on_startBtn_clicked()
{
    neiro.start();
    qDebug() << "start" << QDateTime::currentDateTime().toString("dd.MM.yyyy_hh:mm");
    ui->startBtn->setVisible(false);
    ui->stopBtn->setVisible(true);
}

void MainWindow::on_stopBtn_clicked()
{
    neiro.stop();
    ui->startBtn->setVisible(true);
    ui->stopBtn->setVisible(false);
}

void MainWindow::on_loadBtn_clicked()
{
    auto path =
        QFileDialog::getOpenFileName(this, "Загрузить сохранение", ROOT_PATH "/data/save");

    if (!path.isEmpty()) {
        double learningRate;
        int period;
        int scale;
        int testVariant;
        QVector<int> vecLayer;
        QVector<int> vecIndex;
        QString filePath;
        QString fileTestPath;
        QString strLayer;
        QString strIndex;

        neiro.load(path, learningRate, period, scale, testVariant, vecLayer, vecIndex, filePath,
                   fileTestPath);

        for (auto &el : vecLayer) {
            strLayer.append(QString("%1 ").arg(el));
        }

        for (auto &el : vecIndex) {
            strIndex.append(QString("%1 ").arg(el));
        }

        ui->sizeEdit->setText(strLayer.left(strLayer.size() - 1));
        ui->indexEdit->setText(strIndex.left(strIndex.size() - 1));
        ui->learningRate->setText(QString::number(learningRate));
        ui->period->setTime(QTime::fromMSecsSinceStartOfDay(period));
        ui->scale->setValue(scale);
        ui->testBox->setCurrentIndex(testVariant);
        ui->pathBtn->setText(filePath.split("/").last());
        ui->testPathBtn->setText(fileTestPath.split("/").last());
        ui->startBtn->setEnabled(true);
        ui->testBtn->setEnabled(true);

        neiro.path(filePath);
        neiro.testPath(fileTestPath);

        setNameSave(path.split("/").last());
    }
}

void MainWindow::on_testPathBtn_clicked()
{
    auto path = QFileDialog::getOpenFileName(this, "Тестовые данные", ROOT_PATH "/data/data");

    if (!path.isEmpty()) {
        neiro.testPath(path);
        ui->testBtn->setEnabled(true);
        ui->testPathBtn->setText(path.split("/").last());
    }
}

void MainWindow::on_pathBtn_clicked()
{
    auto path = QFileDialog::getOpenFileName(this, "Данные", ROOT_PATH "/data/data");

    if (!path.isEmpty()) {
        neiro.path(path);
        ui->startBtn->setEnabled(true);
        ui->pathBtn->setText(path.split("/").last());
    }
}

void MainWindow::on_initBtn_clicked()
{
    QVector<int> vecLayer;
    QVector<int> vecIndex;

    for (auto &el : ui->sizeEdit->text().split(" ")) {
        vecLayer.append(el.toInt());
    }

    for (auto &el : ui->indexEdit->text().split(" ")) {
        vecIndex.append(el.toInt());
    }

    neiro.init(vecLayer);
    neiro.setIndex(vecIndex);
}

void MainWindow::on_learningRate_textChanged(const QString &text)
{
    neiro.setRate(text.toDouble());
}

void MainWindow::on_period_timeChanged(const QTime &time)
{
    neiro.setPeriod(time.msecsSinceStartOfDay());
}

void MainWindow::on_scale_valueChanged(int scale)
{
    neiro.setScale(scale);
}

void MainWindow::on_testBox_currentIndexChanged(int index)
{
    neiro.setTestVariant(index);
}
