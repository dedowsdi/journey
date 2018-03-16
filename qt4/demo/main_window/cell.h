#ifndef CELL_H
#define CELL_H

#include <QTableWidgetItem>

// QTableWidgetItem is not child of QObject
class Cell : public QTableWidgetItem
{
public:
    Cell();

    QTableWidgetItem *clone() const; // called by QTableWidget when it needs to create a new cel
    void setData(int role, const QVariant &value);
    QVariant data(int role) const;
    void setFormula(const QString &formula);
    QString formula() const;
    void setDirty();

private:
    QVariant value() const;
    QVariant evalExpression(const QString &str, int &pos) const;
    QVariant evalTerm(const QString &str, int &pos) const;
    QVariant evalFactor(const QString &str, int &pos) const;

		// this member variable will be changed in value, value is called in data, which is a const function.
    mutable QVariant cachedValue;
    mutable bool cacheIsDirty;
};

#endif
