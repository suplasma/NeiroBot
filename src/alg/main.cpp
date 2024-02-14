#include <QApplication>
#include <QDateTime>
#include <QDebug>

#include <iostream>
#include <thread>

#include <neiro.h>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Neiro neiro;
    MainWindow w(neiro);

    if (a.arguments().size() == 1) {
        w.show();
    } else {
        double learningRate;
        int period;
        int scale;
        int testVariant;
        QVector<int> vecLayer;
        QVector<int> vecIndex;
        QString path;
        QString testPath;

        neiro.load(a.arguments()[1], learningRate, period, scale, testVariant, vecLayer,
                   vecIndex, path, testPath);
        neiro.setFileAutoSave(
            QString(ROOT_PATH "/data/autosave/%1").arg(a.arguments()[1].split("/").last()));
        neiro.setPeriod(period);
        neiro.setScale(scale);
        neiro.setTestVariant(testVariant);
        neiro.setRate(learningRate);
        neiro.path(path);
        neiro.testPath(testPath);
        neiro.start();
        qDebug() << "start" << QDateTime::currentDateTime().toString("dd.MM.yyyy_hh:mm");

        std::thread th([&]() {
            int n;

            forever
            {
                std::cin >> n;

                switch (n) {
                    case 0: exit(0); break;
                    case 1: emit neiro.testAndExitSignal(); break;
                    case 2: emit neiro.testSignal(); break;
                }
            }
        });
        th.detach();
    }

    return a.exec();
}
