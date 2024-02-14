#include "pers.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

Pers::Pers(QRandomGenerator *gen) : generator(gen) {}

void Pers::iteration(QVector<Neiron> &&layerVec, double aspiration, double learningRate)
{
    layerVec.last() = 1.0;
    layers.first() = std::move(layerVec);

    for (int i = 0; i < weightsSize; ++i) {
        forwards(layers[i], weights[i], layers[i + 1]);
    }

    fixOutError(layers.last(), aspiration);
    for (int i = weightsSize - 1; i > 0; --i) {
        findError(layers[i], weights[i], layers[i + 1]);
    }

    for (int i = weightsSize - 1; i >= 0; --i) {
        backWards(layers[i], weights[i], layers[i + 1], learningRate);
    }
}

void Pers::iteration2(QVector<Neiron> &&layerVec, double aspiration, double learningRate)
{
    layerVec.last() = 1.0;
    layers.first() = std::move(layerVec);

    for (int i = 0; i < weightsSize; ++i) {
        forwards(layers[i], weights[i], layers[i + 1]);
    }

    fixOutError(layers.last(), aspiration);
    for (int i = weightsSize - 1; i > 0; --i) {
        findError2(layers[i], weights[i], layers[i + 1]);
    }

    for (int i = weightsSize - 1; i >= 0; --i) {
        backWards2(layers[i], weights[i], layers[i + 1], learningRate);
    }
}

void Pers::readData(const QString &fileName, double &learningRate, int &period, int &scale,
                    int &testVariant, QVector<int> &vecLayer, QVector<int> &vecIndex,
                    QString &path, QString &testPath)
{
    QFile file(fileName);
    file.open(QFile::ReadOnly);
    QJsonObject obj = QJsonDocument::fromJson(file.readAll()).object();
    file.close();

    learningRate = obj["learningRate"].toDouble();
    period = obj["period"].toInt();
    scale = obj["scale"].toInt();
    testVariant = obj["testVariant"].toInt();
    path = obj["path"].toString();
    testPath = obj["testPath"].toString();
    QJsonArray sizesJson = obj["sizes"].toArray();
    for (auto &&el : sizesJson) {
        vecLayer.append(el.toInt());
    }

    init(vecLayer);

    QJsonArray indexJson = obj["index"].toArray();
    for (auto &&el : indexJson) {
        vecIndex.append(el.toInt());
    }

    QJsonArray a1 = obj["weights"].toArray();
    for (int i = 0; i < a1.size(); ++i) {
        QJsonArray a2 = a1[i].toArray();
        for (int j = 0; j < a2.size(); ++j) {
            QJsonArray a3 = a2[j].toArray();
            for (int t = 0; t < a3.size(); ++t) {
                weights[i][j][t] = a3[t].toDouble();
            }
        }
    }
}

void Pers::saveData(const QString &fileName, double learningRate, int period, int scale,
                    int testVariant, const QString &path, const QString &testPath,
                    const QVector<int> &vecIndex)
{
    QJsonObject obj;
    QJsonArray sizesJson;
    QJsonArray indexJson;

    QJsonArray a1;
    for (auto layer : weights) {
        QJsonArray a2;
        for (auto neiron : layer) {
            QJsonArray a3;
            for (auto weight : neiron) {
                a3.append(weight);
            }
            a2.append(a3);
        }
        a1.append(a2);
    }

    for (auto &el : layers) {
        sizesJson.append(el.size());
    }

    for (auto &el : vecIndex) {
        indexJson.append(el);
    }

    obj.insert("weights", a1);
    obj.insert("learningRate", learningRate);
    obj.insert("period", period);
    obj.insert("scale", scale);
    obj.insert("testVariant", testVariant);
    obj.insert("sizes", sizesJson);
    obj.insert("index", indexJson);
    obj.insert("path", path);
    obj.insert("testPath", testPath);

    QFile file(fileName);
    file.open(QFile::WriteOnly);
    file.write(QJsonDocument(obj).toJson());
    file.close();
}

void Pers::init(const QVector<int> &sizes)
{
    layers.clear();
    for (int size : sizes) {
        layers.append(QVector<Neiron>());
        for (int i = 0; i < size; ++i) {
            layers.last().append(Neiron());
        }
        layers.last()[size - 1].val = 1;
    }

    weights.clear();
    for (int j = 1; j < sizes.size(); ++j) {
        weights.append(QVector<QVector<double>>());
        for (int i = 0; i < sizes[j - 1]; ++i) {
            weights.last().append(QVector<double>());
            for (int t = 0; t < sizes[j] - 1; ++t) {
                weights.last().last().append(2.0 * generator->generateDouble() - 1.0);
            }
        }
    }

    for (QVector<double> &weigth : weights.last()) {
        weigth.append(2.0 * generator->generateDouble() - 1.0);
    }

    weightsSize = weights.size();
}

double Pers::test(QVector<Neiron> &&layerVec)
{
    layerVec.last() = 1.0;
    layers.first() = std::move(layerVec);

    for (int i = 0; i < weightsSize; ++i) {
        forwards(layers[i], weights[i], layers[i + 1]);
    }

    return layers.last().first().val;
}

void Pers::forwards(const QVector<Neiron> &layerFirst, const QVector<QVector<double>> &weight,
                    QVector<Neiron> &layerSecond)
{
    for (int i2 = 0; i2 < weight.first().size(); ++i2) {
        layerSecond[i2].val = 0.0;
        for (int i1 = 0; i1 < weight.size(); ++i1) {
            layerSecond[i2].val += layerFirst[i1].val * weight[i1][i2];
        }
        layerSecond[i2].val = activation(layerSecond[i2].val);
    }
}

void Pers::findError(QVector<Neiron> &layerFirst, const QVector<QVector<double>> &weight,
                     const QVector<Neiron> &layerSecond)
{
    for (int i1 = 0; i1 < weight.size() - 1; ++i1) {
        layerFirst[i1].error = 0.0;
        for (int i2 = 0; i2 < weight.first().size(); ++i2) {
            layerFirst[i1].error += weight[i1][i2] * layerSecond[i2].error;
        }
    }
}

void Pers::fixOutError(QVector<Neiron> &layer, double aspiration)
{
    layer.first().error = aspiration - layer.first().val;
}

void Pers::backWards2(const QVector<Neiron> &layerFirst, QVector<QVector<double>> &weight,
                      const QVector<Neiron> &layerSecond, double learningRate)
{
    for (int i2 = 0; i2 < weight.first().size(); ++i2) {
        for (int i1 = 0; i1 < weight.size(); ++i1) {
            weight[i1][i2] += learningRate * layerSecond[i2].error * layerFirst[i1].val;
        }
    }
}

double Pers::activation(double x)
{
    return x < 0.0 ? 0.000001 * x : x;
}

double Pers::fluxion(double x)
{
    return x < 0.0 ? 0.000001 : 1.0;
}

void Pers::findError2(QVector<Neiron> &layerFirst, const QVector<QVector<double>> &weight,
                      const QVector<Neiron> &layerSecond)
{
    for (int i1 = 0; i1 < weight.size() - 1; ++i1) {
        layerFirst[i1].error = 0.0;
        for (int i2 = 0; i2 < weight.first().size(); ++i2) {
            layerFirst[i1].error += weight[i1][i2] * layerSecond[i2].error;
        }
        layerFirst[i1].error *= fluxion(layerFirst[i1].val);
    }
}

void Pers::backWards(const QVector<Neiron> &layerFirst, QVector<QVector<double>> &weight,
                     const QVector<Neiron> &layerSecond, double learningRate)
{
    for (int i2 = 0; i2 < weight.first().size(); ++i2) {
        for (int i1 = 0; i1 < weight.size(); ++i1) {
            weight[i1][i2] += learningRate * layerSecond[i2].error *
                              fluxion(layerSecond[i2].val) * layerFirst[i1].val;
        }
    }
}
