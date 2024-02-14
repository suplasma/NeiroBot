#pragma once

#include <QRandomGenerator>
#include <QVector>

struct Neiron {   //Нейрон
    double val;   //Значение
    double error; //Ошибка

    Neiron() {}
    Neiron(double val) : val(val) {}
    Neiron(double val, double error) : val(val), error(error) {}
};

class Pers {
public:
    Pers(QRandomGenerator *gen);

    void iteration(QVector<Neiron> &&layerVec, double asp,
                   double learningRate); //Старт обучения
    void iteration2(QVector<Neiron> &&layerVec, double asp,
                    double learningRate); //Старт обучения
    void readData(const QString &fileName, double &learningRate, int &period, int &scale,
                  int &testVariant, QVector<int> &vecLayer, QVector<int> &vecIndex,
                  QString &path,
                  QString &testPath); //Читает данные
    void saveData(const QString &fileName, double learningRate, int period, int scale,
                  int testVariant, const QString &path, const QString &testPath,
                  const QVector<int> &vecIndex); //Читает данные
    void init(const QVector<int> &sizes);
    double test(QVector<Neiron> &&layerVec); //Тест

private:
    QRandomGenerator *generator;

    void forwards(const QVector<Neiron> &layerFirst, const QVector<QVector<double>> &weight,
                  QVector<Neiron> &layerSecond); //Работа нейронов
    void findError(QVector<Neiron> &layerFirst, const QVector<QVector<double>> &weight,
                   const QVector<Neiron> &layerSecond); //Расчет ошибки
    void findError2(QVector<Neiron> &layerFirst, const QVector<QVector<double>> &weight,
                    const QVector<Neiron> &layerSecond); //Расчет ошибки
    void fixOutError(QVector<Neiron> &layer,
                     double aspiration); //Расчет ошибки
    void backWards(const QVector<Neiron> &layerFirst, QVector<QVector<double>> &weight,
                   const QVector<Neiron> &layerSecond,
                   double learningRate); //Корректирование весов
    void backWards2(const QVector<Neiron> &layerFirst, QVector<QVector<double>> &weight,
                    const QVector<Neiron> &layerSecond,
                    double learningRate); //Корректирование весов
    double activation(double x);          //Функция активации
    double fluxion(double x);             //Производная

    QVector<QVector<Neiron>> layers; //Слои [слой][нейрон]
    QVector<QVector<QVector<double>>>
        weights; //Веса [слой][нейрон из слоя][нейрон из следующего слоя]
    int weightsSize;
};
