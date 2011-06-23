#include "filebrowser.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QTreeView>
#include <QHeaderView>
#include <QDirModel>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>
#include <QToolBar>
#include <QAction>
#include <QFileInfo>
#include <QInputDialog>
#include <QMessageBox>
#include <QMenu>
#include <QProcess>
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

    //create toolbar
    m_toolBar = new QToolBar(m_widget);
    m_toolBar->setIconSize(QSize(16,16));

    m_syncAct = new QAction(QIcon(":/images/sync.png"),tr("Synchronize with editor "),this);
    m_syncAct->setCheckable(true);

    m_filterCombo = new QComboBox;
    m_filterCombo->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    m_filterCombo->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);
    m_filterCombo->setEditable(true);
    m_filterCombo->addItem("*");
    m_filterCombo->addItem("Makefile;*.go;*.cgo;*.s;*.goc;*.y;*.e64;*.pro");
    m_filterCombo->addItem("*.sh;Makefile;*.go;*.cgo;*.s;*.goc;*.y;*.*.c;*.cpp;*.h;*.hpp;*.e64;*.pro");

    m_toolBar->addAction(m_syncAct);
    m_toolBar->addSeparator();
    m_toolBar->addWidget(m_filterCombo);

    //create treeview
    m_treeView = new QTreeView;
    m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);
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
    //hide
    int count = m_treeView->header()->count();
    for (int i = 1; i < count; i++) {
        m_treeView->setColumnHidden(i,true);
    }

    mainLayout->addWidget(m_toolBar);
    mainLayout->addWidget(m_treeView);
    m_widget->setLayout(mainLayout);

    //create menu
    m_fileMenu = new QMenu;//(m_treeView);
    m_folderMenu = new QMenu;//(m_treeView);

    m_openEditorAct = new QAction(tr("Open Editor"),this);
    m_newFileAct = new QAction(tr("New File"),this);
    m_renameFileAct = new QAction(tr("Rename File"),this);
    m_removeFileAct = new QAction(tr("Remove File"),this);

    m_newFolderAct = new QAction(tr("New Folder"),this);
    m_renameFolderAct = new QAction(tr("Rename Folder"),this);
    m_removeFolderAct = new QAction(tr("Remove Folder"),this);

    m_openShellAct = new QAction(tr("Open Terminal Here"),this);

    m_fileMenu->addAction(m_openEditorAct);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_newFileAct);
    m_fileMenu->addAction(m_renameFileAct);
    m_fileMenu->addAction(m_removeFileAct);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_openShellAct);

    m_folderMenu->addAction(m_newFileAct);
    m_folderMenu->addAction(m_newFolderAct);
    m_folderMenu->addAction(m_renameFolderAct);
    m_folderMenu->addAction(m_removeFolderAct);
    m_folderMenu->addSeparator();
    m_folderMenu->addAction(m_openShellAct);

    connect(m_openEditorAct,SIGNAL(triggered()),this,SLOT(openEditor()));
    connect(m_newFileAct,SIGNAL(triggered()),this,SLOT(newFile()));
    connect(m_renameFileAct,SIGNAL(triggered()),this,SLOT(renameFile()));
    connect(m_removeFileAct,SIGNAL(triggered()),this,SLOT(removeFile()));
    connect(m_newFolderAct,SIGNAL(triggered()),this,SLOT(newFolder()));
    connect(m_renameFolderAct,SIGNAL(triggered()),this,SLOT(renameFolder()));
    connect(m_removeFolderAct,SIGNAL(triggered()),this,SLOT(removeFolder()));
    connect(m_openShellAct,SIGNAL(triggered()),this,SLOT(openShell()));


    QDockWidget *dock = m_liteApp->dockManager()->addDock(m_widget,tr("FileBrowser"));
    connect(dock,SIGNAL(visibilityChanged(bool)),this,SLOT(visibilityChanged(bool)));
    connect(m_treeView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(doubleClickedTreeView(QModelIndex)));
    connect(m_filterCombo,SIGNAL(activated(QString)),this,SLOT(activatedFilter(QString)));
    connect(m_syncAct,SIGNAL(triggered(bool)),this,SLOT(syncFileModel(bool)));
    connect(m_liteApp->editorManager(),SIGNAL(currentEditorChanged(LiteApi::IEditor*)),this,SLOT(currentEditorChanged(LiteApi::IEditor*)));
    connect(m_treeView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(treeViewContextMenuRequested(QPoint)));

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

void FileBrowser::currentEditorChanged(LiteApi::IEditor *editor)
{
    if (!m_syncAct->isChecked()) {
        return;
    }
    if (!editor) {
        return;
    }
    LiteApi::IFile *file = editor->file();
    if (!file) {
        return;
    }
    QString fileName = file->fileName();
    if (fileName.isEmpty()) {
        return;
    }
    QString path = QFileInfo(fileName).absolutePath();
    QModelIndex index = m_fileModel->index(path);
    QModelIndex proxyIndex = m_proxyModel->mapFromSource(index);
    m_treeView->scrollTo(proxyIndex,QAbstractItemView::PositionAtCenter);
    m_treeView->expand(proxyIndex);

}

void FileBrowser::syncFileModel(bool b)
{
    if (b == false) {
        return;
    } else {
        currentEditorChanged(m_liteApp->editorManager()->currentEditor());
    }
}

void FileBrowser::treeViewContextMenuRequested(const QPoint &pos)
{
    QModelIndex proxyIndex = m_treeView->indexAt(pos);
    QModelIndex index = m_proxyModel->mapToSource(proxyIndex);
    m_contextIndex = index;
    QFileInfo info = m_fileModel->fileInfo(index);
    showTreeViewContextMenu(m_treeView->mapToGlobal(pos),info);
}

void FileBrowser::showTreeViewContextMenu(const QPoint &globalPos, const QFileInfo &info)
{
    QMenu *contextMenu = 0;
    if (info.isDir()) {
        contextMenu = m_folderMenu;
    } else if (info.isFile()) {
        contextMenu = m_fileMenu;
    }
    if (contextMenu && contextMenu->actions().count() > 0) {
        contextMenu->popup(globalPos);
    }
}

void FileBrowser::openEditor()
{
    QString fileName = m_fileModel->filePath(m_contextIndex);
    if (!fileName.isEmpty()) {
        m_liteApp->fileManager()->openEditor(fileName);
    }
}

static QDir fileInfoToDir(const QFileInfo &info)
{
    QDir dir;
    if (info.isDir()) {
        dir.setPath(info.filePath());
    } else {
        dir = info.dir();
    }
    return dir;
}

void FileBrowser::newFile()
{
    QFileInfo info = m_fileModel->fileInfo(m_contextIndex);
    QDir dir = fileInfoToDir(info);

    QString fileName = QInputDialog::getText(m_liteApp->mainWindow(),tr("Create File"),tr("File Name"));
    if (!fileName.isEmpty()) {
        QString filePath = QFileInfo(dir,fileName).filePath();
        if (QFile::exists(filePath)) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Create File"),
                                     tr("The filename is exists!"));
        } else {
            QFile file(filePath);
            if (file.open(QIODevice::WriteOnly)) {
                file.close();
            } else {
                QMessageBox::information(m_liteApp->mainWindow(),tr("Create File"),
                                         tr("Failed to create the file!"));
            }
        }
    }
}

void FileBrowser::renameFile()
{
    QFileInfo info = m_fileModel->fileInfo(m_contextIndex);
    if (!info.isFile()) {
        return;
    }
    QString fileName = QInputDialog::getText(m_liteApp->mainWindow(),
                                             tr("Rename File"),tr("File Name"));
    if (!fileName.isEmpty()) {
        QDir dir = fileInfoToDir(info);
        if (!QFile::rename(info.filePath(),QFileInfo(dir,fileName).filePath())) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Rename File"),
                                     tr("Failed to rename the file!"));
        }
    }
}

void FileBrowser::removeFile()
{
    QFileInfo info = m_fileModel->fileInfo(m_contextIndex);
    if (!info.isFile()) {
        return;
    }

    int ret = QMessageBox::question(m_liteApp->mainWindow(),tr("Remove File"),
                          tr("Confirm remove the file and continue"),
                          QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        if (!QFile::remove(info.filePath())) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Remove File"),
                                     tr("Failed to remove the file!"));
        }
    }
}

void FileBrowser::newFolder()
{
    QFileInfo info = m_fileModel->fileInfo(m_contextIndex);

    QString folderName = QInputDialog::getText(m_liteApp->mainWindow(),tr("Create Folder"),tr("Folder Name"));
    if (!folderName.isEmpty()) {
        QDir dir = fileInfoToDir(info);
        if (!dir.mkpath(folderName)) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Create Folder"),
                                     tr("Failed to create the folder!"));
        }
    }
}

void FileBrowser::renameFolder()
{
    QFileInfo info = m_fileModel->fileInfo(m_contextIndex);
    if (!info.isDir()) {
        return;
    }

    QString folderName = QInputDialog::getText(m_liteApp->mainWindow(),tr("Rename Folder"),tr("Folder Name"));
    if (!folderName.isEmpty()) {
        QDir dir = fileInfoToDir(info);
        dir.cdUp();
        if (!dir.rename(info.fileName(),folderName)) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Rename Folder"),
                                     tr("Failed to rename the folder!"));
        }
    }
}

void FileBrowser::removeFolder()
{
    QFileInfo info = m_fileModel->fileInfo(m_contextIndex);
    if (!info.isDir()) {
        return;
    }

    int ret = QMessageBox::warning(m_liteApp->mainWindow(),tr("Remove Folder"),
                          tr("Confirm remove the foler and continue"),
                          QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        if (!m_fileModel->rmdir(m_contextIndex)) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Remove Folder"),
                                     tr("Failed to remove the folder!"));
        }
    }
}

void FileBrowser::openShell()
{
    QFileInfo info = m_fileModel->fileInfo(m_contextIndex);
    QDir dir = fileInfoToDir(info);
#if defined(Q_OS_WIN)
    //QString shell = "cmd.exe";
    QString shell = "c:\\MinGW\\msys\\1.0\\msys.bat";
#elif defined(Q_OS_MAC)
    QString shell = "/usr/X11R6/bin/xterm"
#else
    QString shell = "/usr/bin/xterm";
#endif
    QStringList args;
    QString work = dir.path();
    QProcess::startDetached(shell,args,work);
}
