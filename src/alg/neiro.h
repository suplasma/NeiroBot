#pragma once

#include <QRandomGenerator>
#include <QTimer>

#include <pers.h>

class Neiro : public QObject {
    Q_OBJECT
public:
    Neiro();
    ~Neiro();
    void save(const QString &fileName);
    void autoSave();
    void test(QVector<double> &x, QVector<double> &y, QVector<double> &yAsp, double &min,
              double &max, double &sum, int &n0, int &n1, int &n2, int &correct, int &wrong,
              double &error);
    void start();
    void stop();
    void load(const QString fileName, double &learningRate, int &period, int &scale,
              int &testVariant, QVector<int> &vecLayer, QVector<int> &vecIndex,
              QString &fileNamePath, QString &fileNameTestPath);
    void init(const QVector<int> &vec);
    void path(const QString &path);
    void testPath(const QString &path);
    void setRate(double val);
    void setPeriod(int val);
    void setScale(int val);
    void setTestVariant(int val);
    void setIndex(const QVector<int> &vec);
    void setFileAutoSave(const QString &fileName);
    void testSlot();
    void testAndExitSlot();

private:
    QVector<Neiron> data;
    QVector<Neiron> dataTest;
    QRandomGenerator *gen;
    Pers pers;
    double rate;
    int period;
    int firstLayerSize;
    int scale;
    int testVariant;
    int dataSize;
    int dataTestSize;
    int dataIndexSize;
    int lastIndex = firstLayerSize - 1;
    int aspIndex = firstLayerSize + dataIndexSize - 1;
    int midSize = firstLayerSize + 1;
    QVector<int> dataIndex;
    QString fileAutoSave;
    QString mPath;
    QString mTestPath;
    QTimer *timer;
    QTimer *timerSave;

    void iteration();
    void readData(const QString &path, QVector<Neiron> &data);

signals:
    void testSignal();
    void testAndExitSignal();
};
