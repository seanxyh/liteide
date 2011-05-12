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
// Module: liteeditorfile.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: liteeditorfile.cpp,v 1.0 2011-5-12 visualfc Exp $

#include "liteeditorfile.h"
#include <QFile>
#include <QTextDocument>
#include <QTextCodec>
#include <QMessageBox>

//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

LiteEditorFile::LiteEditorFile(QObject *parent)
    : LiteApi::IFile(parent)
{
    m_codec = QTextCodec::codecForName("UTF-8");
}

QString LiteEditorFile::fileName() const
{
    return m_fileName;
}

bool LiteEditorFile::save(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        return false;
    }
    QString text = m_document->toPlainText();
    if (m_codec) {
        file.write(m_codec->fromUnicode(text));
    } else {
        file.write(text.toLocal8Bit());
    }
    m_document->setModified(false);
    return true;
}

bool LiteEditorFile::reload()
{
    if (m_document->isModified()) {
        QString text = QString(tr("Cancel file %1 modify and reload ?")).arg(m_fileName);
        int ret = QMessageBox::question(0,"Question",text,QMessageBox::Yes|QMessageBox::No);
        if (ret != QMessageBox::Ok) {
            return false;
        }
    }
    bool ret = open(m_fileName,m_mimeType);
    if (ret) {
        emit reloaded();
    }
    return ret;
}

QString LiteEditorFile::mimeType() const
{
    return m_mimeType;
}

void LiteEditorFile::setDocument(QTextDocument *document)
{
    m_document = document;
}

void LiteEditorFile::setTextCodec(QTextCodec *codec)
{
    m_codec = codec;
}

QTextDocument  *LiteEditorFile::document()
{
    return m_document;
}

bool LiteEditorFile::open(const QString &fileName, const QString &mimeType)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return false;
    }
    m_mimeType = mimeType;
    m_fileName = fileName;
    QByteArray text = file.readAll();
    if (m_codec) {
        m_document->setPlainText(m_codec->toUnicode(text));
    } else {
        m_document->setPlainText(text);
    }
    return true;
}
