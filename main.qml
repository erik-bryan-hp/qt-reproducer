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

import QtQuick 2.0
import QtQuick.Window 2.0
import QtWebEngine 1.10

Window {
    width: 1024
    height: 750
    visible: true
    WebEngineView {
        anchors.fill: parent

        // Use the profile set up in C++, if available, and log which path we take
        function getProfile() {
            if (typeof(myProfile) === "undefined") {
                console.log("WebEngineView: Not using profile, should use NSSDB");
                return null;
            } else {
                console.log("WebEngineView: Using profile, should use in-memory cert store");
                return myProfile;
            }
        }
        profile: getProfile()

        // Found this website to exercise mTLS here:
        // https://stackoverflow.com/questions/56798331/are-there-any-public-web-services-that-will-check-for-an-mtls-cert-and-response
        url: "https://certauth.cryptomix.com/json/"

        // Log loading status - helps to verify we can see console logging, so lack of onSelectClientCertificate
        // logging indicates that method is never called.
        onLoadingChanged: function(loadRequest) {
            let url = loadRequest.url;

            // Translate load status enum to string for logging
            var status = "UNKNOWN";
            switch (loadRequest.status) {
            case WebEngineView.LoadStartedStatus:
                status = "started";
                break;
            case WebEngineView.LoadStoppedStatus:
                status = "stopped";
                break;
            case WebEngineView.LoadSucceededStatus:
                status = "succeeded";
                break;
            case WebEngineView.LoadFailedStatus:
                status = "failed";
                break;
            }

            // Log profile name to verify we're using the expected profile
            let profileName = (profile && profile.objectName) ? profile.objectName : "null profile";
            console.log("onLoadingChanged(" + profileName + ") Loading " + status + ": " + url);
        }

        onSelectClientCertificate: function(selection) {
            console.log("onSelectClientCertificate() ENTER");

            // log all certs we could choose from
            var selectedCertIndex = -1;
            console.log("onSelectClientCertificate() # of available certs: " + selection.certificates.length + "\n");
            for (var i = 0; i < selection.certificates.length; i++)
            {
                var prefix = "onSelectClientCertificate() certs[" + i + "]";
                var cert = selection.certificates[i];
                console.log(prefix + ".issuer: " + cert.issuer);
                console.log(prefix + ".subject: " + cert.subject);
                console.log(prefix + ".isSelfSigned: " + cert.isSelfSigned + "\n");

                // Select the test cert if it's among the available options
                if (cert.subject === "Test Cert")
                {
                    selectedCertIndex = i;
                    console.log("onSelectClientCertificate() selecting test cert at index " + selectedCertIndex + "\n");
                    cert.select();
                }
            }

            if (selectedCertIndex === -1)
            {
                console.log("onSelectClientCertificate() desired test cert not found! no certs selected");
                selection.selectNone();
            }

            console.log("onSelectClientCertificate() EXIT");
        }
    }
}
