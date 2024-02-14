#include "neiro.h"

#include <QDebug>
#include <QFile>
#include <QTime>

Neiro::Neiro() :
    gen(new QRandomGenerator{static_cast<quint32>(QTime::currentTime().msec())}), pers(gen)
{
    timer = new QTimer{};
    timerSave = new QTimer{};

    connect(timer, &QTimer::timeout, this, &Neiro::iteration);
    connect(timerSave, &QTimer::timeout, this, &Neiro::testSlot);
    connect(this, &Neiro::testSignal, this, &Neiro::testSlot);
    connect(this, &Neiro::testAndExitSignal, this, &Neiro::testAndExitSlot);
}

Neiro::~Neiro()
{
    delete gen;
    delete timer;
    delete timerSave;
}

void Neiro::save(const QString &fileName)
{
    pers.saveData(fileName, rate, period, scale, testVariant, mPath, mTestPath, dataIndex);
}

void Neiro::autoSave()
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

    test(x, y, yAsp, min, max, sum, n0, n1, n2, correct, wrong, error);
    auto date = QDateTime::currentDateTime().toString("dd.MM.yyyy_hh:mm");
    pers.saveData(QString("%1-%2").arg(fileAutoSave).arg(date), rate, period, scale,
                  testVariant, mPath, mTestPath, dataIndex);

    qDebug()
        << QString(
               "null %1, long %2, short %3, sum %4, correct %5, wrong %6, error %7, data %8")
               .arg(n0)
               .arg(n1)
               .arg(n2)
               .arg(sum)
               .arg(correct)
               .arg(wrong)
               .arg(error)
               .arg(date);
}

void Neiro::test(QVector<double> &x, QVector<double> &y, QVector<double> &yAsp, double &min,
                 double &max, double &sum, int &n0, int &n1, int &n2, int &correct, int &wrong,
                 double &error)
{
    double sumAsp = 0.0;

    for (int i = 0; i < dataTestSize; i += dataIndexSize) {
        double last = dataTest[i + lastIndex].val;
        double asp = dataTest[i + aspIndex].val;
        double delta = asp - last;
        double test = pers.test(dataTest.mid(i, midSize));

        x.append(i);
        error += abs(asp - test);
        sumAsp += delta * scale;

        if (test > last) {
            sum += delta * scale;
            ++n1;

            if (delta > 0.0) {
                ++correct;
            } else {
                ++wrong;
            }
        } else if (test < last) {
            if (testVariant == 1) {
                sum -= delta * scale;

                if (delta <= 0.0) {
                    ++correct;
                } else {
                    ++wrong;
                }
            }
            ++n2;
        } else {
            ++n0;
        }

        y.append(sum);
        yAsp.append(sumAsp);

        if (max < y.last()) {
            max = y.last();
        }

        if (min > y.last()) {
            min = y.last();
        }

        if (max < yAsp.last()) {
            max = yAsp.last();
        }

        if (min > yAsp.last()) {
            min = yAsp.last();
        }
    }
}

void Neiro::start()
{
    timer->start();
    timerSave->start(period);
}

void Neiro::stop()
{
    timer->stop();
    timerSave->stop();
}

void Neiro::load(const QString fileName, double &learningRate, int &period, int &scale,
                 int &testVariant, QVector<int> &vecLayer, QVector<int> &vecIndex,
                 QString &fileNamePath, QString &fileNameTestPath)
{
    pers.readData(fileName, learningRate, period, scale, testVariant, vecLayer, vecIndex,
                  fileNamePath, fileNameTestPath);
    firstLayerSize = vecLayer.first() - 1;
    lastIndex = firstLayerSize - 1;
    midSize = firstLayerSize + 1;
    setIndex(vecIndex);
}

void Neiro::init(const QVector<int> &vec)
{
    firstLayerSize = vec.first() - 1;
    lastIndex = firstLayerSize - 1;
    midSize = firstLayerSize + 1;
    pers.init(vec);
    dataSize = data.size() - firstLayerSize;
    dataTestSize = dataTest.size() - firstLayerSize;
}

void Neiro::path(const QString &path)
{
    mPath = path;
    readData(path, data);
    dataSize = data.size() - firstLayerSize;
}

void Neiro::testPath(const QString &path)
{
    mTestPath = path;
    readData(path, dataTest);
    dataTestSize = dataTest.size() - firstLayerSize;
}

void Neiro::setRate(double val)
{
    rate = val;
}

void Neiro::setPeriod(int val)
{
    period = val;
}

void Neiro::setScale(int val)
{
    scale = val;
}

void Neiro::setTestVariant(int val)
{
    testVariant = val;
}

void Neiro::setIndex(const QVector<int> &vec)
{
    dataIndex = std::move(vec);
    dataIndexSize = dataIndex.size();
    aspIndex = firstLayerSize + dataIndexSize - 1;
}

void Neiro::setFileAutoSave(const QString &fileName)
{
    fileAutoSave = fileName;
}

void Neiro::testSlot()
{
    stop();
    autoSave();
    start();
}

void Neiro::testAndExitSlot()
{
    stop();
    autoSave();
    exit(0);
}

void Neiro::readData(const QString &path, QVector<Neiron> &data)
{
    data.clear();
    QFile file(path);
    file.open(QFile::ReadOnly);
    while (!file.atEnd()) {
        QStringList split = QString(file.readLine()).split(",");
        for (auto &index : dataIndex) {
            data.append(split[index].toDouble() / scale);
        }
    }
    file.close();
}

void Neiro::iteration()
{
    int i = gen->generate() % dataSize;
    i -= i % dataIndexSize;
    pers.iteration2(data.mid(i, midSize), data[i + aspIndex].val, rate);
}
