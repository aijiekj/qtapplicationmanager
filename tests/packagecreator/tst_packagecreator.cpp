/****************************************************************************
**
** Copyright (C) 2016 Pelagicore AG
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Pelagicore Application Manager.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT-QTAS$
** Commercial License Usage
** Licensees holding valid commercial Qt Automotive Suite licenses may use
** this file in accordance with the commercial license agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and The Qt Company.  For
** licensing terms and conditions see https://www.qt.io/terms-conditions.
** For further information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest>

#include "global.h"
#include "installationreport.h"
#include "utilities.h"
#include "packagecreator.h"

#include "../error-checking.h"

QT_USE_NAMESPACE_AM

class tst_PackageCreator : public QObject
{
    Q_OBJECT

public:
    tst_PackageCreator();

private slots:
    void initTestCase();

    void createAndVerify_data();
    void createAndVerify();

private:
    QDir m_baseDir;
    bool m_tarAvailable = false;
};

tst_PackageCreator::tst_PackageCreator()
    : m_baseDir(qSL(AM_TESTDATA_DIR))
{ }

void tst_PackageCreator::initTestCase()
{
    // check if tar command is available at all
    QProcess tar;
    tar.start(qSL("tar"), { qSL("--version") });
    m_tarAvailable = tar.waitForStarted(3000)
            && tar.waitForFinished(3000)
            && (tar.exitStatus() == QProcess::NormalExit);

    QVERIFY(checkCorrectLocale());
}

void tst_PackageCreator::createAndVerify_data()
{
    QTest::addColumn<QStringList>("files");
    QTest::addColumn<bool>("expectedSuccess");
    QTest::addColumn<QString>("errorString");

    QTest::newRow("basic") << QStringList { qSL("testfile") } << true << QString();
    QTest::newRow("no-such-file") << QStringList { qSL("tastfile") } << false << qSL("~file not found: .*");
}

void tst_PackageCreator::createAndVerify()
{
    QFETCH(QStringList, files);
    QFETCH(bool, expectedSuccess);
    QFETCH(QString, errorString);

    QTemporaryFile output;
    QVERIFY(output.open());

    InstallationReport report(qSL("com.pelagicore.test"));
    report.addFiles(files);

    PackageCreator creator(m_baseDir, &output, report);
    bool result = creator.create();
    output.close();

    if (expectedSuccess) {
        QVERIFY2(result, qPrintable(creator.errorString()));
    } else {
        QVERIFY(creator.errorCode() != Error::None);
        QVERIFY(creator.errorCode() != Error::Canceled);
        QVERIFY(!creator.wasCanceled());

        AM_CHECK_ERRORSTRING(creator.errorString(), errorString);
        return;
    }

    // check the tar listing
    if (!m_tarAvailable)
        QSKIP("No tar command found in PATH - skipping the verification part of the test!");

    QProcess tar;
    tar.start(qSL("tar"), { qSL("-tzf"), output.fileName() });
    QVERIFY2(tar.waitForStarted(3000) &&
             tar.waitForFinished(3000) &&
             (tar.exitStatus() == QProcess::NormalExit) &&
             (tar.exitCode() == 0), qPrintable(tar.errorString()));

    QStringList expectedContents = files;
    expectedContents.sort();
    expectedContents.prepend(qSL("--PACKAGE-HEADER--"));
    expectedContents.append(qSL("--PACKAGE-FOOTER--"));
    QCOMPARE(expectedContents, QString::fromLocal8Bit(tar.readAllStandardOutput()).split(qL1C('\n'), QString::SkipEmptyParts));

    // check the contents of the files

    foreach (const QString &file, files) {
        QFile src(m_baseDir.absoluteFilePath(file));
        QVERIFY2(src.open(QFile::ReadOnly), qPrintable(src.errorString()));
        QByteArray data = src.readAll();

        tar.start(qSL("tar"), { qSL("-xzOf"), output.fileName(), file });
        QVERIFY2(tar.waitForStarted(3000) &&
                 tar.waitForFinished(3000) &&
                 (tar.exitStatus() == QProcess::NormalExit) &&
                 (tar.exitCode() == 0), qPrintable(tar.errorString()));

        QCOMPARE(tar.readAllStandardOutput(), data);
    }
}

int main(int argc, char *argv[])
{
    ensureCorrectLocale();
    QCoreApplication app(argc, argv);
    app.setAttribute(Qt::AA_Use96Dpi, true);
    tst_PackageCreator tc;
    QTEST_SET_MAIN_SOURCE_PATH
    return QTest::qExec(&tc, argc, argv);
}

#include "tst_packagecreator.moc"
