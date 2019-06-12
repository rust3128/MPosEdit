#include "tanlemodelconnect.h"
#include <QColor>

TanleModelConnect::TanleModelConnect(QObject *parent) : QSqlTableModel(parent)
{

}

QVariant TanleModelConnect::data(const QModelIndex &idx, int role) const
{

    switch (role) {
    case Qt::BackgroundColorRole:
        if(QSqlTableModel::data(this->index(idx.row(),6)).toInt() == 1){
            return QColor(Qt::cyan);
        }
        break;
    case Qt::DisplayRole:
        if(QSqlTableModel::data(this->index(idx.row(),6)) == 1 && idx.column()==6) return "Текущее";
        if(QSqlTableModel::data(this->index(idx.row(),6)) == 0 && idx.column()==6) return "Не используется";
        return QSqlTableModel::data(idx);
        break;
    default:
        break;
    }
    return QVariant();
}
