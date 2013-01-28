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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

// This file was generated by qlalr - DO NOT EDIT!
#ifndef QMLJSGRAMMAR_P_H
#define QMLJSGRAMMAR_P_H

#include "qmljsglobal_p.h"
#include <QtCore/qglobal.h>

QT_BEGIN_NAMESPACE

class QML_PARSER_EXPORT QmlJSGrammar
{
public:
  enum VariousConstants {
    EOF_SYMBOL = 0,
    REDUCE_HERE = 101,
    SHIFT_THERE = 100,
    T_AND = 1,
    T_AND_AND = 2,
    T_AND_EQ = 3,
    T_AS = 91,
    T_AUTOMATIC_SEMICOLON = 62,
    T_BREAK = 4,
    T_CASE = 5,
    T_CATCH = 6,
    T_COLON = 7,
    T_COMMA = 8,
    T_COMMENT = 88,
    T_CONST = 84,
    T_CONTINUE = 9,
    T_DEBUGGER = 85,
    T_DEFAULT = 10,
    T_DELETE = 11,
    T_DIVIDE_ = 12,
    T_DIVIDE_EQ = 13,
    T_DO = 14,
    T_DOT = 15,
    T_ELSE = 16,
    T_EQ = 17,
    T_EQ_EQ = 18,
    T_EQ_EQ_EQ = 19,
    T_ERROR = 93,
    T_FALSE = 83,
    T_FEED_JS_EXPRESSION = 97,
    T_FEED_JS_PROGRAM = 99,
    T_FEED_JS_SOURCE_ELEMENT = 98,
    T_FEED_JS_STATEMENT = 96,
    T_FEED_UI_OBJECT_MEMBER = 95,
    T_FEED_UI_PROGRAM = 94,
    T_FINALLY = 20,
    T_FOR = 21,
    T_FUNCTION = 22,
    T_GE = 23,
    T_GT = 24,
    T_GT_GT = 25,
    T_GT_GT_EQ = 26,
    T_GT_GT_GT = 27,
    T_GT_GT_GT_EQ = 28,
    T_IDENTIFIER = 29,
    T_IF = 30,
    T_IMPORT = 90,
    T_IN = 31,
    T_INSTANCEOF = 32,
    T_LBRACE = 33,
    T_LBRACKET = 34,
    T_LE = 35,
    T_LPAREN = 36,
    T_LT = 37,
    T_LT_LT = 38,
    T_LT_LT_EQ = 39,
    T_MINUS = 40,
    T_MINUS_EQ = 41,
    T_MINUS_MINUS = 42,
    T_MULTILINE_STRING_LITERAL = 87,
    T_NEW = 43,
    T_NOT = 44,
    T_NOT_EQ = 45,
    T_NOT_EQ_EQ = 46,
    T_NULL = 81,
    T_NUMERIC_LITERAL = 47,
    T_ON = 92,
    T_OR = 48,
    T_OR_EQ = 49,
    T_OR_OR = 50,
    T_PLUS = 51,
    T_PLUS_EQ = 52,
    T_PLUS_PLUS = 53,
    T_PROPERTY = 66,
    T_PUBLIC = 89,
    T_QUESTION = 54,
    T_RBRACE = 55,
    T_RBRACKET = 56,
    T_READONLY = 68,
    T_REMAINDER = 57,
    T_REMAINDER_EQ = 58,
    T_RESERVED_WORD = 86,
    T_RETURN = 59,
    T_RPAREN = 60,
    T_SEMICOLON = 61,
    T_SIGNAL = 67,
    T_STAR = 63,
    T_STAR_EQ = 64,
    T_STRING_LITERAL = 65,
    T_SWITCH = 69,
    T_THIS = 70,
    T_THROW = 71,
    T_TILDE = 72,
    T_TRUE = 82,
    T_TRY = 73,
    T_TYPEOF = 74,
    T_VAR = 75,
    T_VOID = 76,
    T_WHILE = 77,
    T_WITH = 78,
    T_XOR = 79,
    T_XOR_EQ = 80,

    ACCEPT_STATE = 644,
    RULE_COUNT = 349,
    STATE_COUNT = 645,
    TERMINAL_COUNT = 102,
    NON_TERMINAL_COUNT = 107,

    GOTO_INDEX_OFFSET = 645,
    GOTO_INFO_OFFSET = 2807,
    GOTO_CHECK_OFFSET = 2807
  };

  static const char  *const    spell [];
  static const short             lhs [];
  static const short             rhs [];
  static const short    goto_default [];
  static const short  action_default [];
  static const short    action_index [];
  static const short     action_info [];
  static const short    action_check [];

  static inline int nt_action (int state, int nt)
  {
    const int yyn = action_index [GOTO_INDEX_OFFSET + state] + nt;
    if (yyn < 0 || action_check [GOTO_CHECK_OFFSET + yyn] != nt)
      return goto_default [nt];

    return action_info [GOTO_INFO_OFFSET + yyn];
  }

  static inline int t_action (int state, int token)
  {
    const int yyn = action_index [state] + token;

    if (yyn < 0 || action_check [yyn] != token)
      return - action_default [state];

    return action_info [yyn];
  }
};


QT_END_NAMESPACE
#endif // QMLJSGRAMMAR_P_H

