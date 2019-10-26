#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include <QMainWindow>
#include <QWidget>
#include <QLayout>
#include <QAbstractTableModel>
#include <QTableView>
#include <QTimer>

// ==================================== Axis ====================================
class Axis
{
public:
    enum Index : uint8_t {
        Idx00 = 0,
        Idx01,
        Idx02,
        Idx03,
        Idx04,
        Idx05,
        Idx06,
        Idx07,
        Idx08,
        Idx09,
        Idx10,
        Idx11,
        Idx12,
        Idx13,
        Idx14,
        Idx15,
        Min = Idx00,
        Max = Idx15,
        Count
    };

    static const QString Text[Index::Count];
};

// ==================================== CityModel ====================================
class TableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    TableModel(QObject *parent = nullptr);

    void setHorizontalHeaderList(const QStringList &strList);
    void setVerticalHeaderList(const QStringList &strList);
    void initialize(void);
    void HeaderChange(int32_t first, int32_t last);

    int32_t rowCount(const QModelIndex &parent) const;
    int32_t columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int32_t role);
    QVariant headerData(int32_t section, Qt::Orientation orientation, int32_t role) const;
    bool setHeaderData(int32_t section, Qt::Orientation orientation, const QVariant &value, int32_t role);
    Qt::ItemFlags flags(const QModelIndex &index) const;

private:
    QStringList horizontalHeaderList;
    QStringList verticalHeaderList;
    QVector<QVector<QString>> gridVector;
    QStringList posCmdList;
};

// ==================================== TableView ====================================
class TableView : public QMainWindow
{
    Q_OBJECT

public:
    TableView(QWidget *parent = nullptr);
    ~TableView();

private:
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QTableView *tableView;
    TableModel *tableModel;

    QTimer *timerHeaderData;

    void setupUi(int32_t rowCount);
    void initialize(void);

private slots:
    void onTimerHeaderData(void);
};



#endif // TABLEVIEW_H
