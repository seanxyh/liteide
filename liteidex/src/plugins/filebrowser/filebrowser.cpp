#include "filebrowser.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QTreeView>
#include <QHeaderView>
#include <QDirModel>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>
#include <QDebug>

FileBrowser::FileBrowser(LiteApi::IApplication *app, QObject *parent) :
    QObject(parent),
    m_liteApp(app)
{
    m_widget = new QWidget;
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);

    m_fileModel = new QFileSystemModel(this);
    m_fileModel->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    m_fileModel->setRootPath(m_fileModel->myComputer().toString());

    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    m_proxyModel->setDynamicSortFilter(true);
    m_proxyModel->setSourceModel(m_fileModel);
    m_proxyModel->setSortRole(Qt::DisplayRole);
    m_proxyModel->sortColumn();

    m_treeView = new QTreeView;
    m_treeView->setModel(m_proxyModel);

    m_treeView->setRootIsDecorated(true);
    m_treeView->setUniformRowHeights(true);
    m_treeView->setTextElideMode(Qt::ElideNone);
    m_treeView->setAttribute(Qt::WA_MacShowFocusRect, false);

    m_treeView->setHeaderHidden(true);
    m_treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // show horizontal scrollbar
    m_treeView->header()->setResizeMode(QHeaderView::ResizeToContents);
    m_treeView->header()->setStretchLastSection(false);


    int count = m_treeView->header()->count();
    for (int i = 1; i < count; i++) {
        m_treeView->setColumnHidden(i,true);
    }

    m_filterCombo = new QComboBox;
    m_filterCombo->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);
    m_filterCombo->setEditable(true);
    m_filterCombo->addItem("*");
    m_filterCombo->addItem("Makefile;*.go;*.cgo;*.s;*.goc;*.y;*.e64;*.pro");
    m_filterCombo->addItem("*.sh;Makefile;*.go;*.cgo;*.s;*.goc;*.y;*.*.c;*.cpp;*.h;*.hpp;*.e64;*.pro");

    mainLayout->addWidget(m_treeView);
    mainLayout->addWidget(m_filterCombo);
    m_widget->setLayout(mainLayout);


    QDockWidget *dock = m_liteApp->dockManager()->addDock(m_widget,tr("FileBrowser"));
    connect(dock,SIGNAL(visibilityChanged(bool)),this,SLOT(visibilityChanged(bool)));
    connect(m_treeView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(doubleClickedTreeView(QModelIndex)));
    connect(m_filterCombo,SIGNAL(activated(QString)),this,SLOT(activatedFilter(QString)));

    QModelIndex index = m_fileModel->index(QDir::currentPath());
    QModelIndex proxyIndex = m_proxyModel->mapFromSource(index);
    m_treeView->scrollTo(proxyIndex);
    //m_treeView->resizeColumnToContents(0);
}

FileBrowser::~FileBrowser()
{
    delete m_widget;
}

void FileBrowser::visibilityChanged(bool)
{
}

void FileBrowser::doubleClickedTreeView(QModelIndex proxyIndex)
{
    QModelIndex index = m_proxyModel->mapToSource(proxyIndex);
    if (!index.isValid()) {
        return;
    }
    if (m_fileModel->isDir(index)) {
        return;
    }
    QString fileName = m_fileModel->filePath(index);
    if (fileName.isEmpty()) {
        return;
    }
    m_liteApp->fileManager()->openFile(fileName);
}

void FileBrowser::activatedFilter(QString filter)
{
    m_fileModel->setNameFilters(filter.split(";",QString::SkipEmptyParts));
}
