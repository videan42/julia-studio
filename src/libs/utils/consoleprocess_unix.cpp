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

#include "consoleprocess_p.h"

#include "environment.h"
#include "qtcprocess.h"

#include <QCoreApplication>
#include <QDir>
#include <QSettings>

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

namespace Utils {

ConsoleProcessPrivate::ConsoleProcessPrivate() :
    m_mode(ConsoleProcess::Run),
    m_appPid(0),
    m_stubSocket(0),
    m_tempFile(0),
    m_settings(0)
{
}

ConsoleProcess::ConsoleProcess(QObject *parent)  :
    QObject(parent), d(new ConsoleProcessPrivate)
{
    connect(&d->m_stubServer, SIGNAL(newConnection()), SLOT(stubConnectionAvailable()));

    d->m_process.setProcessChannelMode(QProcess::ForwardedChannels);
    connect(&d->m_process, SIGNAL(finished(int,QProcess::ExitStatus)),
            SLOT(stubExited()));
}

void ConsoleProcess::setSettings(QSettings *settings)
{
    d->m_settings = settings;
}

bool ConsoleProcess::start(const QString &program, const QString &args)
{
    if (isRunning())
        return false;

    QtcProcess::SplitError perr;
    QStringList pargs = QtcProcess::prepareArgs(args, &perr, &d->m_environment, &d->m_workingDir);
    QString pcmd;
    if (perr == QtcProcess::SplitOk) {
        pcmd = program;
    } else {
        if (perr != QtcProcess::FoundMeta) {
            emit processError(tr("Quoting error in command."));
            return false;
        }
        if (d->m_mode == Debug) {
            // FIXME: QTCREATORBUG-2809
            emit processError(tr("Debugging complex shell commands in a terminal"
                                 " is currently not supported."));
            return false;
        }
        pcmd = QLatin1String("/bin/sh");
        pargs << QLatin1String("-c") << (QtcProcess::quoteArg(program) + QLatin1Char(' ') + args);
    }

    QtcProcess::SplitError qerr;
    QStringList xtermArgs = QtcProcess::prepareArgs(terminalEmulator(d->m_settings), &qerr,
                                                    &d->m_environment, &d->m_workingDir);
    if (qerr != QtcProcess::SplitOk) {
        emit processError(qerr == QtcProcess::BadQuoting
                          ? tr("Quoting error in terminal command.")
                          : tr("Terminal command may not be a shell command."));
        return false;
    }

    const QString err = stubServerListen();
    if (!err.isEmpty()) {
        emit processError(msgCommChannelFailed(err));
        return false;
    }

    QStringList env = d->m_environment.toStringList();
    if (!env.isEmpty()) {
        d->m_tempFile = new QTemporaryFile();
        if (!d->m_tempFile->open()) {
            stubServerShutdown();
            emit processError(msgCannotCreateTempFile(d->m_tempFile->errorString()));
            delete d->m_tempFile;
            d->m_tempFile = 0;
            return false;
        }
        QByteArray contents;
        foreach (const QString &var, env) {
            QByteArray l8b = var.toLocal8Bit();
            contents.append(l8b.constData(), l8b.size() + 1);
        }
        if (d->m_tempFile->write(contents) != contents.size() || !d->m_tempFile->flush()) {
            stubServerShutdown();
            emit processError(msgCannotWriteTempFile());
            delete d->m_tempFile;
            d->m_tempFile = 0;
            return false;
        }
    }

    xtermArgs
#ifdef Q_OS_MAC
              << (QCoreApplication::applicationDirPath() + QLatin1String("/../Resources/qtcreator_process_stub"))
#else
              << (QCoreApplication::applicationDirPath() + QLatin1String("/qtcreator_process_stub"))
#endif
              << modeOption(d->m_mode)
              << d->m_stubServer.fullServerName()
              << msgPromptToClose()
              << workingDirectory()
              << (d->m_tempFile ? d->m_tempFile->fileName() : QString())
              << pcmd << pargs;

    QString xterm = xtermArgs.takeFirst();
    d->m_process.start(xterm, xtermArgs);
    if (!d->m_process.waitForStarted()) {
        stubServerShutdown();
        emit processError(tr("Cannot start the terminal emulator '%1'.").arg(xterm));
        delete d->m_tempFile;
        d->m_tempFile = 0;
        return false;
    }
    d->m_executable = program;
    emit wrapperStarted();
    return true;
}

void ConsoleProcess::stop()
{
    if (!isRunning())
        return;
    stubServerShutdown();
    d->m_appPid = 0;
    d->m_process.terminate();
    if (!d->m_process.waitForFinished(1000))
        d->m_process.kill();
    d->m_process.waitForFinished();
}

bool ConsoleProcess::isRunning() const
{
    return d->m_process.state() != QProcess::NotRunning;
}

QString ConsoleProcess::stubServerListen()
{
    // We need to put the socket in a private directory, as some systems simply do not
    // check the file permissions of sockets.
    QString stubFifoDir;
    forever {
        {
            QTemporaryFile tf;
            if (!tf.open())
                return msgCannotCreateTempFile(tf.errorString());
            stubFifoDir = tf.fileName();
        }
        // By now the temp file was deleted again
        d->m_stubServerDir = QFile::encodeName(stubFifoDir);
        if (!::mkdir(d->m_stubServerDir.constData(), 0700))
            break;
        if (errno != EEXIST)
            return msgCannotCreateTempDir(stubFifoDir, QString::fromLocal8Bit(strerror(errno)));
    }
    const QString stubServer  = stubFifoDir + QLatin1String("/stub-socket");
    if (!d->m_stubServer.listen(stubServer)) {
        ::rmdir(d->m_stubServerDir.constData());
        return tr("Cannot create socket '%1': %2").arg(stubServer, d->m_stubServer.errorString());
    }
    return QString();
}

void ConsoleProcess::stubServerShutdown()
{
    delete d->m_stubSocket;
    d->m_stubSocket = 0;
    if (d->m_stubServer.isListening()) {
        d->m_stubServer.close();
        ::rmdir(d->m_stubServerDir.constData());
    }
}

void ConsoleProcess::stubConnectionAvailable()
{
    d->m_stubSocket = d->m_stubServer.nextPendingConnection();
    connect(d->m_stubSocket, SIGNAL(readyRead()), SLOT(readStubOutput()));
}

static QString errorMsg(int code)
{
    return QString::fromLocal8Bit(strerror(code));
}

void ConsoleProcess::readStubOutput()
{
    while (d->m_stubSocket->canReadLine()) {
        QByteArray out = d->m_stubSocket->readLine();
        out.chop(1); // \n
        if (out.startsWith("err:chdir ")) {
            emit processError(msgCannotChangeToWorkDir(workingDirectory(), errorMsg(out.mid(10).toInt())));
        } else if (out.startsWith("err:exec ")) {
            emit processError(msgCannotExecute(d->m_executable, errorMsg(out.mid(9).toInt())));
        } else if (out.startsWith("pid ")) {
            // Will not need it any more
            delete d->m_tempFile;
            d->m_tempFile = 0;

            d->m_appPid = out.mid(4).toInt();
            emit processStarted();
        } else if (out.startsWith("exit ")) {
            d->m_appStatus = QProcess::NormalExit;
            d->m_appCode = out.mid(5).toInt();
            d->m_appPid = 0;
            emit processStopped();
        } else if (out.startsWith("crash ")) {
            d->m_appStatus = QProcess::CrashExit;
            d->m_appCode = out.mid(6).toInt();
            d->m_appPid = 0;
            emit processStopped();
        } else {
            emit processError(msgUnexpectedOutput(out));
            d->m_process.terminate();
            break;
        }
    }
}

void ConsoleProcess::stubExited()
{
    // The stub exit might get noticed before we read the error status.
    if (d->m_stubSocket && d->m_stubSocket->state() == QLocalSocket::ConnectedState)
        d->m_stubSocket->waitForDisconnected();
    stubServerShutdown();
    delete d->m_tempFile;
    d->m_tempFile = 0;
    if (d->m_appPid) {
        d->m_appStatus = QProcess::CrashExit;
        d->m_appCode = -1;
        d->m_appPid = 0;
        emit processStopped(); // Maybe it actually did not, but keep state consistent
    }
    emit wrapperStopped();
}

struct Terminal {
    const char *binary;
    const char *options;
};

static const Terminal knownTerminals[] =
{
    {"xterm", "-e"},
    {"aterm", "-e"},
    {"Eterm", "-e"},
    {"rxvt", "-e"},
    {"urxvt", "-e"},
    {"xfce4-terminal", "-x"},
    {"konsole", "--nofork -e"},
    {"gnome-terminal", "-x"}
};

QString ConsoleProcess::defaultTerminalEmulator()
{
#ifdef Q_OS_MAC
    return QLatin1String("/usr/X11/bin/xterm -e");
#else
    const Environment env = Environment::systemEnvironment();
    const int terminalCount = int(sizeof(knownTerminals) / sizeof(knownTerminals[0]));
    for (int i = 0; i < terminalCount; ++i) {
        QString result = env.searchInPath(QLatin1String(knownTerminals[i].binary));
        if (!result.isEmpty()) {
            result += QLatin1Char(' ');
            result += QLatin1String(knownTerminals[i].options);
            return result;
        }
    }
    return QLatin1String("xterm -e");
#endif
}

QStringList ConsoleProcess::availableTerminalEmulators()
{
#ifdef Q_OS_MAC
    return QStringList(defaultTerminalEmulator());
#else
    QStringList result;
    const Environment env = Environment::systemEnvironment();
    const int terminalCount = int(sizeof(knownTerminals) / sizeof(knownTerminals[0]));
    for (int i = 0; i < terminalCount; ++i) {
        QString terminal = env.searchInPath(QLatin1String(knownTerminals[i].binary));
        if (!terminal.isEmpty()) {
            terminal += QLatin1Char(' ');
            terminal += QLatin1String(knownTerminals[i].options);
            result.push_back(terminal);
        }
    }
    result.sort();
    return result;
#endif
}

QString ConsoleProcess::terminalEmulator(const QSettings *settings)
{
    if (settings) {
        const QString value = settings->value(QLatin1String("General/TerminalEmulator")).toString();
        if (!value.isEmpty())
            return value;
    }
    return defaultTerminalEmulator();
}

void ConsoleProcess::setTerminalEmulator(QSettings *settings, const QString &term)
{
    return settings->setValue(QLatin1String("General/TerminalEmulator"), term);
}
} // namespace Utils
