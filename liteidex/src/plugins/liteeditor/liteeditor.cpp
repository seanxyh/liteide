/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011 LiteIDE Team. All rights reserved.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** In addition, as a special exception,  that plugins developed for LiteIDE,
** are allowed to remain closed sourced and can be distributed under any license .
** These rights are included in the file LGPL_EXCEPTION.txt in this package.
**
**************************************************************************/
// Module: liteeditor.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: liteeditor.cpp,v 1.0 2011-7-6 visualfc Exp $

#include "liteeditor.h"
#include "liteeditorwidget.h"
#include "liteeditorfile.h"
#include "litecompleter.h"
#include "colorstyle/colorstyle.h"

#include <QFileInfo>
#include <QVBoxLayout>
#include <QToolBar>
#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QTextDocument>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QTextCursor>
#include <QToolTip>
#include <QFileDialog>
#include <QPrinter>
#include <QTextDocumentWriter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QTextCodec>
#include <QDebug>
#include <QPalette>

//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

LiteEditor::LiteEditor(LiteApi::IApplication *app)
    : m_liteApp(app),
      m_extension(new Extension),
      m_completer(0)
{
    m_widget = new QWidget;
    m_editorWidget = new LiteEditorWidget(m_widget);
    m_colorStyleScheme = new ColorStyleScheme(this);
    m_defPalette = m_editorWidget->palette();

    createActions();
    createToolBars();

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(m_toolBar);
    layout->addWidget(m_editorWidget);
    m_widget->setLayout(layout);
    m_file = new LiteEditorFile(m_liteApp,this);
    m_file->setDocument(m_editorWidget->document());
    connect(m_file->document(),SIGNAL(modificationChanged(bool)),this,SIGNAL(modificationChanged(bool)));
    connect(m_file->document(),SIGNAL(contentsChanged()),this,SIGNAL(contentsChanged()));
    connect(m_liteApp->optionManager(),SIGNAL(applyOption(QString)),this,SLOT(applyOption(QString)));

    //applyOption("option/liteeditor");

    m_extension->addObject("LiteApi.ITextEditor",this);
    m_extension->addObject("LiteApi.QToolBar",m_toolBar);
    m_extension->addObject("LiteApi.LiteEditorWidget",m_editorWidget);
    m_extension->addObject("LiteApi.QPlainTextEdit",m_editorWidget);


    findCodecs();
    foreach (QTextCodec *codec, m_codecs) {
        m_codecComboBox->addItem(codec->name(), codec->mibEnum());
    }
    connect(m_codecComboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(codecComboBoxChanged(QString)));
}

LiteEditor::~LiteEditor()
{
    delete m_extension;
    delete m_editorWidget;
    delete m_widget;
}

const ColorStyleScheme *LiteEditor::colorStyleScheme() const
{
    return m_colorStyleScheme;
}

void LiteEditor::setCompleter(LiteCompleter *complter)
{
    if (m_completer) {
        QObject::disconnect(m_completer, 0, m_editorWidget, 0);
    }
    m_completer = complter;
    if (!m_completer) {
        return;
    }
    m_completer->setEditor(m_editorWidget);
    m_editorWidget->setCompleter(m_completer->completer());

    m_extension->addObject("LiteApi.ICompleter",complter);

    connect(m_editorWidget,SIGNAL(completionPrefixChanged(QString)),m_completer,SLOT(completionPrefixChanged(QString)));
    connect(m_completer,SIGNAL(wordCompleted(QString,QStringList)),this,SLOT(updateTip(QString,QStringList)));
}

void LiteEditor::clipbordDataChanged()
{
    QClipboard *clipboard = QApplication::clipboard();
    if (clipboard->mimeData()->hasText() ||
            clipboard->mimeData()->hasHtml()) {
        m_pasteAct->setEnabled(true);
    } else {
        m_pasteAct->setEnabled(false);
    }
}

void LiteEditor::createActions()
{
    m_undoAct = new QAction(QIcon(":/images/undo.png"),tr("Undo"),this);
    m_redoAct = new QAction(QIcon(":/images/redo.png"),tr("Redo"),this);
    m_cutAct = new QAction(QIcon(":/images/cut.png"),tr("Cut"),this);
    m_copyAct = new QAction(QIcon(":/images/copy.png"),tr("Copy"),this);
    m_pasteAct = new QAction(QIcon(":/images/paste.png"),tr("Paste"),this);
    m_lockAct = new QAction(QIcon(":/images/unlock.png"),tr("File is writable"),this);
#ifndef QT_NO_PRINTER
    m_filePrintPdfAct = new QAction(QIcon(":/images/exportpdf.png"),tr("Export PDF"),this);
    m_filePrintAct = new QAction(QIcon(":/images/fileprint.png"),tr("Print Document"),this);
    m_filePrintPreviewAct = new QAction(QIcon(":/images/fileprintpreview.png"),tr("Print Preview Document"),this);
#endif
    m_lockAct->setEnabled(false);

    m_undoAct->setEnabled(false);
    m_redoAct->setEnabled(false);
    m_cutAct->setEnabled(false);
    m_copyAct->setEnabled(false);
    m_pasteAct->setEnabled(false);

    connect(m_editorWidget,SIGNAL(undoAvailable(bool)),m_undoAct,SLOT(setEnabled(bool)));
    connect(m_editorWidget,SIGNAL(redoAvailable(bool)),m_redoAct,SLOT(setEnabled(bool)));
    connect(m_editorWidget,SIGNAL(copyAvailable(bool)),m_cutAct,SLOT(setEnabled(bool)));
    connect(m_editorWidget,SIGNAL(copyAvailable(bool)),m_copyAct,SLOT(setEnabled(bool)));

    connect(m_undoAct,SIGNAL(triggered()),m_editorWidget,SLOT(undo()));
    connect(m_redoAct,SIGNAL(triggered()),m_editorWidget,SLOT(redo()));
    connect(m_cutAct,SIGNAL(triggered()),m_editorWidget,SLOT(cut()));
    connect(m_copyAct,SIGNAL(triggered()),m_editorWidget,SLOT(copy()));
    connect(m_pasteAct,SIGNAL(triggered()),m_editorWidget,SLOT(paste()));
#ifndef QT_NO_PRINTER
    connect(m_filePrintPdfAct,SIGNAL(triggered()),this,SLOT(filePrintPdf()));
    connect(m_filePrintAct,SIGNAL(triggered()),this,SLOT(filePrint()));
    connect(m_filePrintPreviewAct,SIGNAL(triggered()),this,SLOT(filePrintPreview()));
#endif
    QClipboard *clipboard = QApplication::clipboard();
    connect(clipboard,SIGNAL(dataChanged()),this,SLOT(clipbordDataChanged()));
    clipbordDataChanged();
}

void LiteEditor::findCodecs()
 {
     QMap<QString, QTextCodec *> codecMap;
     QRegExp iso8859RegExp("ISO[- ]8859-([0-9]+).*");

     foreach (int mib, QTextCodec::availableMibs()) {
         QTextCodec *codec = QTextCodec::codecForMib(mib);

         QString sortKey = codec->name().toUpper();
         int rank;

         if (sortKey.startsWith("UTF-8")) {
             rank = 1;
         } else if (sortKey.startsWith("UTF-16")) {
             rank = 2;
         } else if (iso8859RegExp.exactMatch(sortKey)) {
             if (iso8859RegExp.cap(1).size() == 1)
                 rank = 3;
             else
                 rank = 4;
         } else {
             rank = 5;
         }
         sortKey.prepend(QChar('0' + rank));

         codecMap.insert(sortKey, codec);
     }
     m_codecs = codecMap.values();
 }


void LiteEditor::createToolBars()
{
    m_toolBar = new QToolBar(tr("editor"),m_widget);
    m_toolBar->setIconSize(QSize(16,16));

    m_toolBar->addAction(m_lockAct);
    m_toolBar->addSeparator();
    m_codecComboBox = new QComboBox;
    m_toolBar->addWidget(m_codecComboBox);
    m_toolBar->addSeparator();

    m_toolBar->addAction(m_cutAct);
    m_toolBar->addAction(m_copyAct);
    m_toolBar->addAction(m_pasteAct);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_undoAct);
    m_toolBar->addAction(m_redoAct);    
    m_toolBar->addSeparator();
#ifndef QT_NO_PRINTER
    m_toolBar->addAction(m_filePrintPdfAct);
    m_toolBar->addAction(m_filePrintPreviewAct);
    m_toolBar->addAction(m_filePrintAct);
    m_toolBar->addSeparator();
#endif

#ifdef LITEEDITOR_FIND
    m_findComboBox = new QComboBox(m_widget);
    m_findComboBox->setEditable(true);
    m_findComboBox->setMinimumWidth(120);
    m_findComboBox->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    m_toolBar->addWidget(m_findComboBox);
    m_toolBar->addSeparator();
    connect(m_findComboBox->lineEdit(),SIGNAL(returnPressed()),this,SLOT(findNextText()));
#endif
    m_tip = new QLabel(m_widget);
    m_tip->setAlignment(Qt::AlignCenter);
    m_tip->setText("...");
    m_toolBar->addWidget(m_tip);
}

#ifdef LITEEDITOR_FIND
void LiteEditor::findNextText()
{
    QString text = m_findComboBox->lineEdit()->text();
    if (text.isEmpty()) {
        return;
    }
    m_editorWidget->find(text);
}
#endif

LiteApi::IExtension *LiteEditor::extension()
{
    return m_extension;
}

QWidget *LiteEditor::widget()
{
    return m_widget;
}

QString LiteEditor::displayName() const
{
    return QFileInfo(m_file->fileName()).fileName();
}

QIcon LiteEditor::icon() const
{
    return QIcon();
}

bool LiteEditor::createNew(const QString &/*contents*/)
{
    return false;
}

bool LiteEditor::open(const QString &fileName,const QString &mimeType)
{
    bool success = m_file->open(fileName,mimeType);
    if (success) {
        m_editorWidget->initLoadDocument();
        m_editorWidget->setReadOnly(m_file->isReadOnly());
        QString codecName = m_file->textCodec();
        for (int i = 0; i < m_codecComboBox->count(); i++) {
            QString text = m_codecComboBox->itemText(i);
            if (codecName == text) {
                m_codecComboBox->setCurrentIndex(i);
                break;
            }
        }
        if (m_file->isReadOnly()) {
            m_lockAct->setIcon(QIcon(":/images/lock.png"));
            m_lockAct->setText("File Is ReadOnly");
        } else {
            m_lockAct->setIcon(QIcon(":/images/unlock.png"));
            m_lockAct->setText("File Is Writable");
        }
    }
    return success;
}

bool LiteEditor::save()
{
    if (m_file->isReadOnly()) {
        return false;
    }
    return m_file->save(m_file->fileName());
}

bool LiteEditor::isReadOnly() const
{
    return m_file->isReadOnly();
}

bool LiteEditor::isModified() const
{
    return m_file->document()->isModified();
}

LiteApi::IFile *LiteEditor::file()
{
    return m_file;
}

int LiteEditor::line() const
{
    return 0;
}

int LiteEditor::column() const
{
    return 0;
}

void LiteEditor::gotoLine(int line, int column)
{
    m_editorWidget->gotoLine(line,column);
}

void LiteEditor::applyOption(QString id)
{
    if (id != "option/liteeditor") {
        return;
    }
    QString fontFamily = m_liteApp->settings()->value("editor/family","Courier").toString();
    int fontSize = m_liteApp->settings()->value("editor/fontsize",12).toInt();
    QFont font = m_editorWidget->font();
    font.setFamily(fontFamily);
    font.setPointSize(fontSize);
    m_editorWidget->setFont(font);
    m_editorWidget->setTabWidth(4);

    QString style = m_liteApp->settings()->value("editor/style","default.xml").toString();
    if (style != m_colorStyle) {
        m_colorStyle = style;
        m_colorStyleScheme->clear();
        QString styleFileName = m_liteApp->resourcePath()+"/colorstyle/"+m_colorStyle;
        bool b = m_colorStyleScheme->load(styleFileName);
        if (b) {
            const ColorStyle *style = m_colorStyleScheme->findStyle("Text");
            const ColorStyle *selection = m_colorStyleScheme->findStyle("Select");
            if (style) {
                QPalette p = m_editorWidget->palette();
                p.setColor(QPalette::Text, style->foregound());
                p.setColor(QPalette::Foreground, style->foregound());
                p.setColor(QPalette::Base, style->background());
                if (selection) {
                    p.setColor(QPalette::Highlight, (selection->background().isValid()?
                                                         selection->background(): QApplication::palette().color(QPalette::Highlight) ));
                    if (selection->foregound().isValid()) {
                        p.setBrush(QPalette::HighlightedText, selection->foregound());
                    }
                }
                p.setBrush(QPalette::Inactive, QPalette::Highlight, p.highlight());
                p.setBrush(QPalette::Inactive, QPalette::HighlightedText, p.highlightedText());
                m_editorWidget->setPalette(p);
            } else {
                m_editorWidget->setPalette(m_defPalette);
            }
            emit colorStyleChanged();
        }
    }
}

void LiteEditor::updateTip(QString func,QStringList args)
{
    if (args.isEmpty()) {
        return;
    }
    m_tip->setToolTip(func+args.join(";"));
    QToolTip::showText(m_toolBar->mapToGlobal(m_tip->pos()),func+args.join(";"),m_tip);
}

void LiteEditor::filePrintPreview()
{
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, m_widget);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)), SLOT(printPreview(QPrinter*)));
    preview.exec();
#endif
}

void LiteEditor::printPreview(QPrinter *printer)
{
#ifdef QT_NO_PRINTER
    Q_UNUSED(printer);
#else
    QPlainTextEdit::LineWrapMode mode = m_editorWidget->lineWrapMode();
    m_editorWidget->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    m_editorWidget->print(printer);
    m_editorWidget->setLineWrapMode(mode);
#endif
}

void LiteEditor::filePrintPdf()
{
#ifndef QT_NO_PRINTER
//! [0]
    QString title;
    if (m_file) {
        title = QFileInfo(m_file->fileName()).baseName();
    }
    QString fileName = QFileDialog::getSaveFileName(m_widget, tr("Export PDF"),
                                                    title, "*.pdf");
    if (!fileName.isEmpty()) {
        if (QFileInfo(fileName).suffix().isEmpty())
            fileName.append(".pdf");
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
        QPlainTextEdit::LineWrapMode mode = m_editorWidget->lineWrapMode();
        m_editorWidget->setLineWrapMode(QPlainTextEdit::WidgetWidth);
        m_editorWidget->print(&printer);
        m_editorWidget->setLineWrapMode(mode);
    }
//! [0]
#endif
}

void LiteEditor::filePrint()
{
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog *dlg = new QPrintDialog(&printer, m_widget);
    if (m_editorWidget->textCursor().hasSelection())
        dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    dlg->setWindowTitle(tr("Print Document"));
    if (dlg->exec() == QDialog::Accepted) {
        QPlainTextEdit::LineWrapMode mode = m_editorWidget->lineWrapMode();
        m_editorWidget->setLineWrapMode(QPlainTextEdit::WidgetWidth);
        m_editorWidget->print(&printer);
        m_editorWidget->setLineWrapMode(mode);
    }
    delete dlg;
#endif
}

void LiteEditor::codecComboBoxChanged(QString codec)
{
    bool success = m_file->reloadByCodec(codec);
    if (success) {
        m_editorWidget->initLoadDocument();
    }
    return;
}
