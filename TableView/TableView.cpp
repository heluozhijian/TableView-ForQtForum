#include "TableView.h"

#include <QDebug>
#include <QElapsedTimer>
#include <QTime>
#include <QHeaderView>

// ========================================== Axis ==========================================
const QString Axis::Text[Axis::Index::Count] = {
    QStringLiteral("A"),
    QStringLiteral("B"),
    QStringLiteral("C"),
    QStringLiteral("D"),
    QStringLiteral("E"),
    QStringLiteral("F"),
    QStringLiteral("G"),
    QStringLiteral("H"),
    QStringLiteral("I"),
    QStringLiteral("J"),
    QStringLiteral("K"),
    QStringLiteral("L"),
    QStringLiteral("M"),
    QStringLiteral("N"),
    QStringLiteral("O"),
    QStringLiteral("P"),
};

// ========================================== TableModel ==========================================
TableModel::TableModel(QObject *parent) : QAbstractTableModel(parent)
{

}

void TableModel::setHorizontalHeaderList(const QStringList &strList)
{
    horizontalHeaderList = strList;
}

void TableModel::setVerticalHeaderList(const QStringList &strList)
{
    verticalHeaderList = strList;
}

void TableModel::initialize(void)
{
    gridVector.clear();
    for (int32_t rowIndex = 0; rowIndex < rowCount(QModelIndex()); rowIndex++) {
        QVector<QString> rowVector;
        rowVector.clear();
        for (int32_t columnIndex = 0; columnIndex < columnCount(QModelIndex()); columnIndex++) {
            rowVector.append(QString("x"));
        }
        gridVector.append(rowVector);
    }

    posCmdList.clear();
    for (int32_t axisIndex = Axis::Index::Min; axisIndex <= Axis::Index::Max; axisIndex++) {
        posCmdList.append(QString::number(0.0, 'f', 2));
    }

    endResetModel();
}

void TableModel::HeaderChange(int32_t first, int32_t last)
{
    emit headerDataChanged(Qt::Horizontal, first, last);
}

int32_t TableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return verticalHeaderList.count();
}

int32_t TableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return horizontalHeaderList.count();
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (role == Qt::TextAlignmentRole) {
        return int32_t(Qt::AlignCenter);
    } else if ((role == Qt::EditRole) || (role == Qt::DisplayRole)) {
        return gridVector[index.row()][index.column()];
    }

    return QVariant();
}

bool TableModel::setData(const QModelIndex &index, const QVariant &value, int32_t role)
{
    if (index.isValid() && ((role == Qt::EditRole) || (role == Qt::DisplayRole))) {
        gridVector[index.row()][index.column()] = value.toString();
        emit dataChanged(index, index);
    }
    return false;
}

QVariant TableModel::headerData(int32_t section, Qt::Orientation orientation, int32_t role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            if ((section >= Axis::Index::Min) && (section <= Axis::Index::Max)) {
                return horizontalHeaderList[section] + "\n" + posCmdList[section];
                // return posCmdList[section];
                // qDebug() << "headerData" << section << QTime::currentTime().toString("hh:mm:ss:zzz");
            } else {
                return horizontalHeaderList[section];
            }
        } else if (orientation == Qt::Vertical) {
            return verticalHeaderList[section];
        } else {
            return QVariant();
        }
    }

    return QVariant();
}

bool TableModel::setHeaderData(int32_t section, Qt::Orientation orientation, const QVariant &value, int32_t role)
{
    Q_UNUSED(orientation);
    if (role == Qt::DisplayRole) {
        // QElapsedTimer timer;
        // timer.start();
        // posCmdList[section] = QString::number(value.value<double>(), 'f', 2);
        posCmdList[section] = QString::number(value.value<double>());
        // qDebug() << "TableModel::setHeaderData" << timer.nsecsElapsed();
        // emit headerDataChanged(orientation, section, section); // replace by HeaderChange
        // qDebug() << "setHeaderData" << section << QTime::currentTime().toString("hh:mm:ss:zzz");
    }

    return true;
}

Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    flags |= Qt::ItemIsEditable;
    return flags;
}

// ==================================== TableView ====================================
TableView::TableView(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("TableView"));
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowState(Qt::WindowMaximized);

    setupUi(100);
    initialize();
}

TableView::~TableView()
{
    delete centralWidget;
    delete gridLayout;
    delete tableView;
    delete tableModel;
}

void TableView::setupUi(int32_t rowCount)
{
    // tableModel
    tableModel = new TableModel(this);

    QStringList strList;

    strList.clear();
    for (int32_t axisIndex = Axis::Index::Min; axisIndex <= Axis::Index::Max; axisIndex++) {
        strList.append(Axis::Text[axisIndex]);
    }
    tableModel->setHorizontalHeaderList(strList);

    strList.clear();
    for (int32_t rowIndex = 0; rowIndex < rowCount; rowIndex++) {
        strList.append(QString().sprintf("%03d", (rowIndex + 1)));
    }
    tableModel->setVerticalHeaderList(strList);

    tableModel->initialize();

    // tableView
    tableView = new QTableView(this);
    tableView->setModel(tableModel);

    tableView->setFrameShape(QFrame::NoFrame);

    tableView->horizontalHeader()->setVisible(true);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    tableView->verticalHeader()->setVisible(true);
    tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    tableView->setSelectionBehavior(QAbstractItemView::SelectItems);
    tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tableView->setAlternatingRowColors(true);

    // gridLayout
    gridLayout = new QGridLayout;
    gridLayout->setSpacing(5);
    gridLayout->setContentsMargins(0, 5, 0, 0); //left top right bottom
    gridLayout->addWidget(tableView, 0, 0);
    gridLayout->setRowStretch(0, 1);
    gridLayout->setColumnStretch(0, 1);

    // centralWidget
    centralWidget = new QWidget;
    centralWidget->setLayout(gridLayout);
    this->setCentralWidget(centralWidget);
}

void TableView::initialize(void)
{
    timerHeaderData = new QTimer();
    timerHeaderData->setInterval(100);
    timerHeaderData->setTimerType(Qt::PreciseTimer);
    connect(timerHeaderData, SIGNAL(timeout()), this, SLOT(onTimerHeaderData()));
    timerHeaderData->start();

    QObject::connect(timerHeaderData, SIGNAL(timeout()), this, SLOT(onTimerHeaderData()));
}

// onTimerHeaderData: triggered by 10Hz timer
void TableView::onTimerHeaderData(void)
{
    bool displayFlag = true; // true: header, false: cells

    static double temp = 0.001;
    temp += 10.24;

    for (int32_t axisIndex = Axis::Index::Min; axisIndex <= Axis::Index::Max; axisIndex++) {
        if (displayFlag) {
            // Option1 - laggly
            tableModel->setHeaderData(axisIndex, Qt::Horizontal, temp, Qt::DisplayRole);
        } else {
            // Option2 - smooth
            tableModel->setData(tableModel->index(2, axisIndex), temp, Qt::DisplayRole);
        }
     }

    // Option1 - laggly
    if (displayFlag) {
        tableModel->HeaderChange(Axis::Index::Min, Axis::Index::Max);
    }
}
