/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2012 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**************************************************************************/

#include "basetextmark.h"
#include "itexteditor.h"
#include "basetextdocument.h"
#include "texteditorplugin.h"

#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/documentmanager.h>
#include <extensionsystem/pluginmanager.h>

using namespace TextEditor;
using namespace TextEditor::Internal;

BaseTextMarkRegistry::BaseTextMarkRegistry(QObject *parent)
    : QObject(parent)
{
    Core::EditorManager *em = Core::EditorManager::instance();
    connect(em, SIGNAL(editorOpened(Core::IEditor*)),
        SLOT(editorOpened(Core::IEditor*)));

    Core::DocumentManager *dm = Core::DocumentManager::instance();
    connect(dm, SIGNAL(allDocumentsRenamed(QString,QString)),
            this, SLOT(allDocumentsRenamed(QString,QString)));
    connect(dm, SIGNAL(documentRenamed(Core::IDocument*,QString,QString)),
            this, SLOT(documentRenamed(Core::IDocument*,QString,QString)));
}

void BaseTextMarkRegistry::add(BaseTextMark *mark)
{
    m_marks[Utils::FileName::fromString(mark->fileName())].insert(mark);
    Core::EditorManager *em = Core::EditorManager::instance();
    foreach (Core::IEditor *editor, em->editorsForFileName(mark->fileName())) {
        if (ITextEditor *textEditor = qobject_cast<ITextEditor *>(editor)) {
            ITextMarkable *markableInterface = textEditor->markableInterface();
            if (markableInterface->addMark(mark)) {
                break;
            }
        }
    }
}

void BaseTextMarkRegistry::remove(BaseTextMark *mark)
{
    m_marks[Utils::FileName::fromString(mark->fileName())].remove(mark);
}

void BaseTextMarkRegistry::editorOpened(Core::IEditor *editor)
{
    ITextEditor *textEditor = qobject_cast<ITextEditor *>(editor);
    if (!textEditor)
        return;
    if (!m_marks.contains(Utils::FileName::fromString(editor->document()->fileName())))
        return;

    foreach (BaseTextMark *mark, m_marks.value(Utils::FileName::fromString(editor->document()->fileName()))) {
        ITextMarkable *markableInterface = textEditor->markableInterface();
        markableInterface->addMark(mark);
    }
}

void BaseTextMarkRegistry::documentRenamed(Core::IDocument *document, const
                                           QString &oldName, const QString &newName)
{
    TextEditor::BaseTextDocument *baseTextDocument
            = qobject_cast<TextEditor::BaseTextDocument *>(document);
    if (!document)
        return;
    Utils::FileName oldFileName = Utils::FileName::fromString(oldName);
    Utils::FileName newFileName = Utils::FileName::fromString(newName);
    if (!m_marks.contains(oldFileName))
        return;

    QSet<BaseTextMark *> toBeMoved;
    foreach (ITextMark *mark, baseTextDocument->documentMarker()->marks())
        if (BaseTextMark *baseTextMark = dynamic_cast<BaseTextMark *>(mark))
            toBeMoved.insert(baseTextMark);

    m_marks[oldFileName].subtract(toBeMoved);
    m_marks[newFileName].unite(toBeMoved);

    foreach (BaseTextMark *mark, toBeMoved)
        mark->updateFileName(newName);
}

void BaseTextMarkRegistry::allDocumentsRenamed(const QString &oldName, const QString &newName)
{
    Utils::FileName oldFileName = Utils::FileName::fromString(oldName);
    Utils::FileName newFileName = Utils::FileName::fromString(newName);
    if (!m_marks.contains(oldFileName))
        return;

    QSet<BaseTextMark *> oldFileNameMarks = m_marks.value(oldFileName);

    m_marks[newFileName].unite(oldFileNameMarks);
    m_marks[oldFileName].clear();

    foreach (BaseTextMark *mark, oldFileNameMarks)
        mark->updateFileName(newName);
}

BaseTextMark::BaseTextMark(const QString &fileName, int lineNumber)
    : ITextMark(lineNumber), m_fileName(fileName)
{
    Internal::TextEditorPlugin::instance()->baseTextMarkRegistry()->add(this);
}

BaseTextMark::~BaseTextMark()
{
    // oha we are deleted
    Internal::TextEditorPlugin::instance()->baseTextMarkRegistry()->remove(this);
}

void BaseTextMark::updateFileName(const QString &fileName)
{
    m_fileName = fileName;
}
