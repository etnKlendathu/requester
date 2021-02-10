#include "history.h"
#include <QDebug>
#include <QDir>
#include <QStandardPaths>

enum Roles
{
    EndPoint = Qt::UserRole + 1,
    Method,
    Payload,
    Num
};

HistoryModel::HistoryModel()
{
    auto locs = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
    if (!locs.empty()) {
        pack::yaml::deserializeFile((locs[0] + "/history.yaml").toStdString(), m_data);
    }
}

HistoryModel::~HistoryModel()
{
}

int HistoryModel::rowCount(const QModelIndex& parent) const
{
    return m_data.entries.size();
}

QVariant HistoryModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_data.entries.size()) {
        return {};
    }

    auto it = m_data.entries[index.row()];
    switch (role) {
        case EndPoint:
            return QString::fromStdString(it.endpoint);
        case Method:
            return QString::fromStdString(it.method);
        case Payload:
            return QString::fromStdString(it.payload);
        case Num:
            return QString::number(it.num.value());
    }
    return {};
}

QHash<int, QByteArray> HistoryModel::roleNames() const
{
    return {{EndPoint, "endpoint"}, {Method, "method"}, {Payload, "payload"}, {Num, "num"}};
}

void HistoryModel::add(const QString& endpoint, const QString& method, const QString& payload)
{
    int index = m_data.entries.findIndex([&](const auto& it) {
        return it.endpoint == endpoint.toStdString() && it.method == method.toStdString();
    });

    std::string pload = payload.toStdString();
    if (method == "GET" || method == "DELETE") {
        pload.clear();
    }

    bool save = false;

    if (index < 0) {
        emit beginInsertRows({}, m_data.entries.size() + 1, m_data.entries.size() + 1);

        History::Entry& ent = m_data.entries.append();
        ent.endpoint        = endpoint.toStdString();
        ent.method          = method.toStdString();
        ent.payload         = pload;

        emit endInsertRows();

        emit layoutAboutToBeChanged();
        emit layoutChanged();
        save = true;
    } else {
        int num = 0;
        for(const auto& it: m_data.entries) {
            if (it.endpoint == endpoint.toStdString()) {
                if (it.num > num) {
                    num = it.num;
                }
            }
        }
        History::Entry ent = m_data.entries[index];
        if (ent.payload.value() != pload) {
            qInfo() << ent.endpoint.value().c_str();
            emit beginInsertRows({}, m_data.entries.size() + 1, m_data.entries.size() + 1);

            History::Entry& copy = m_data.entries.append();
            copy.endpoint        = ent.endpoint.value();
            copy.method          = ent.method;
            copy.num             = num + 1;
            copy.payload         = pload;
            save                 = true;
            qInfo() << copy.endpoint.value().c_str();

            emit endInsertRows();

            emit layoutAboutToBeChanged();
            emit layoutChanged();
        }
    }

    if (save) {
        auto locs = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
        if (!locs.empty()) {
            QDir().mkpath(locs[0]);
            pack::yaml::serializeFile((locs[0] + "/history.yaml").toStdString(), m_data);
        }
        emit dataChanged(createIndex(0, 0), createIndex(m_data.entries.size(), 0));
    }
}

void HistoryModel::remove(int index)
{
    emit beginRemoveRows({}, index, index);
    int count = 0;
    m_data.entries.remove([&](const auto& it){
        return count++ == index;
    });
    emit endRemoveRows();
    emit layoutAboutToBeChanged();
    emit layoutChanged();
}

