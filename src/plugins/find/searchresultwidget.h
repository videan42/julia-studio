/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#ifndef SEARCHRESULTWIDGET_H
#define SEARCHRESULTWIDGET_H

#include "searchresultwindow.h"

#include <coreplugin/infobar.h>

#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include <QWidget>

namespace Find {
namespace Internal {

class SearchResultTreeView;

class SearchResultWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SearchResultWidget(QWidget *parent = 0);
    ~SearchResultWidget();

    void setInfo(const QString &label, const QString &toolTip, const QString &term);

    void addResult(const QString &fileName, int lineNumber, const QString &lineText,
                   int searchTermStart, int searchTermLength, const QVariant &userData = QVariant());
    void addResults(const QList<SearchResultItem> &items, SearchResult::AddMode mode);

    int count() const;

    QString dontAskAgainGroup() const;
    void setDontAskAgainGroup(const QString &group);

    void setTextToReplace(const QString &textToReplace);
    QString textToReplace() const;
    void setShowReplaceUI(bool visible);

    bool hasFocusInternally() const;
    void setFocusInternally();
    bool canFocusInternally() const;

    void notifyVisibilityChanged(bool visible);

    void setTextEditorFont(const QFont &font);

    void setAutoExpandResults(bool expand);
    void expandAll();
    void collapseAll();

    void goToNext();
    void goToPrevious();

    void restart();

    void setSearchAgainSupported(bool supported);
    void setSearchAgainEnabled(bool enabled);

public slots:
    void finishSearch(bool canceled);
    void sendRequestPopup();

signals:
    void activated(const Find::SearchResultItem &item);
    void replaceButtonClicked(const QString &replaceText, const QList<Find::SearchResultItem> &checkedItems);
    void searchAgainRequested();
    void cancelled();
    void paused(bool paused);
    void restarted();
    void visibilityChanged(bool visible);
    void requestPopup(bool focus);

    void navigateStateChanged();

private slots:
    void hideNoUndoWarning();
    void continueAfterSizeWarning();
    void cancelAfterSizeWarning();
    void handleJumpToSearchResult(const SearchResultItem &item);
    void handleReplaceButton();
    void cancel();
    void searchAgain();

private:
    bool showWarningMessage() const;
    void setShowWarningMessage(bool showWarningMessage);
    QList<SearchResultItem> checkedItems() const;
    void updateMatchesFoundLabel();

    SearchResultTreeView *m_searchResultTreeView;
    int m_count;
    bool m_sizeWarningActive;
    bool m_sizeWarningOverridden;
    QString m_dontAskAgainGroup;
    QFrame *m_messageWidget;
    Core::InfoBar m_infoBar;
    Core::InfoBarDisplay m_infoBarDisplay;
    bool m_isShowingReplaceUI;
    QLabel *m_replaceLabel;
    QLineEdit *m_replaceTextEdit;
    QToolButton *m_replaceButton;
    QToolButton *m_searchAgainButton;
    bool m_searchAgainSupported;
    QWidget *m_descriptionContainer;
    QLabel *m_label;
    QLabel *m_searchTerm;
    QToolButton *m_cancelButton;
    QLabel *m_matchesFoundLabel;
};

} // Internal
} // Find

#endif // SEARCHRESULTWIDGET_H
