#pragma once
#include <QAbstractListModel>
#include <pack/pack.h>

struct History : public pack::Node
{
    struct Entry : public pack::Node
    {
        pack::String endpoint = FIELD("endpoint");
        pack::String method   = FIELD("method");
        pack::String payload  = FIELD("payload");
        pack::Int64  num      = FIELD("num");

        using pack::Node::Node;
        META(Entry, endpoint, method, payload, num);
    };

    pack::ObjectList<Entry> entries = FIELD("entries");

    using pack::Node::Node;
    META(History, entries);
};


class HistoryModel : public QAbstractListModel
{
    Q_OBJECT;

public:
    HistoryModel();
    ~HistoryModel() override;
    int                    rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant               data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

public slots:
    void add(const QString& endpoint, const QString& method, const QString& payload);
    void remove(int index);
private:
    History m_data;
};
