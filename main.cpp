/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <qtwebengineglobal.h>

#include <QQuickWebEngineProfile>
#include <QQmlContext>
#include <QWebEngineClientCertificateStore>
#include <QFile>
#include <iostream>

using std::cout;
using std::endl;

QQuickWebEngineProfile* createNewProfile(std::string p12Path)
{
    QQuickWebEngineProfile *profile = new QQuickWebEngineProfile();

    auto certs = profile->clientCertificateStore()->certificates();
    cout << "createNewProfile() # of certs in new profile: " << certs.length() << endl;

    // Load cert from file on disk into the client cert store
    QSslKey         key;
    QSslCertificate certificate;
    QFile           file(QString::fromStdString(p12Path));
    if (!file.open(QFile::ReadOnly))
    {
        cout << "createNewProfile() opening cert file FAILED: " << p12Path << endl;
    }
    else
    {
        if (!QSslCertificate::importPkcs12(&file, &key, &certificate, nullptr, "test"))
        {
            cout << "createNewProfile() importing user cert file FAILED: " << p12Path << endl;
        }
        else
        {
            profile->clientCertificateStore()->add(certificate, key);
            cout << "createNewProfile() successfully imported user cert!" << endl;
        }
    }

    // Log state after importing cert, to verify the import was successful
    certs = profile->clientCertificateStore()->certificates();
    cout << "createNewProfile() # of certs after import: " << certs.length() << endl;
    for (const auto& cert : qAsConst(certs))
    {
        cout << "createNewProfile() imported cert:" << endl;
        cout << cert.toText().toStdString() << endl;
    }

    // Set profile name so we can verify in qml we've got the right one
    profile->setObjectName("TestProfile");

    return profile;
}

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("QtExamples");
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QtWebEngine::initialize();
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    /*
     * Arg 1: mode of operation, "profile" or "system"
     * Arg 2: relevant path
     *      for cert mode, path to .p12 file to be imported
     *      for system mode on Linux, the new HOME path - it should contain the .pki folder
     *      for system mode on Windows, this argument isn't used - just provide any string to satisfy the argc check
     */
    if (argc != 3)
    {
        cout << "ERROR, two args required: mode & path" << endl;
        return 1;
    }

    std::string mode = argv[1];
    std::string path = argv[2];

    if (mode == "profile")
    {
        // Add profile to root context before loading main.qml, so it gets picked up by the WebEngineView
        engine.rootContext()->setContextProperty("myProfile", createNewProfile(path));
        cout << "main() created profile" << endl;
    }
    else if (mode == "system")
    {
#ifdef WIN32
    cout << "main() 'system' mode in Windows - nothing to do, since system store is included by default" << endl;
#else
    // Don't use the profile - instead, set the HOME environment variable so we'll use the local nssdb
    qputenv("HOME", path.c_str());
    cout << "main() 'system' mode on Linux - set HOME for nssdb: " << path << endl;
#endif
    }
    else
    {
        cout << "ERROR, unexpected mode argument '" << mode << "'" << endl;
        return 1;
    }

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    return app.exec();
}
