#ifndef TANLEMODELCONNECT_H
#define TANLEMODELCONNECT_H

#include <QObject>
#include <QSqlTableModel>

class TanleModelConnect : public QSqlTableModel
{
    Q_OBJECT
public:
    explicit TanleModelConnect(QObject *parent = 0);
    QVariant data(const QModelIndex &idx, int role) const;
};

#endif // TANLEMODELCONNECT_H
