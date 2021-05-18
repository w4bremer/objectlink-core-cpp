/*
* MIT License
*
* Copyright (c) 2021 ApiGear
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#include "qclientio.h"
#include "olink/sinklink.h"

using namespace ApiGear::ObjectLink;

QSinkLink::QSinkLink(const QString& name, QWebSocket *socket, QObject *parent)
    : QObject(parent)
    , m_socket(socket ? socket : new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this))
    , m_link(name.toStdString())
{
    m_link.onLog(m_logger.logFunc());
    connect(m_socket, &QWebSocket::connected, this, &QSinkLink::onConnected);
    connect(m_socket, &QWebSocket::disconnected, this, &QSinkLink::onDisconnected);
    connect(m_socket, &QWebSocket::textMessageReceived, this, &QSinkLink::handleTextMessage);
    WriteMessageFunc func = [this](std::string msg) {
        m_protocol << msg;
        processMessages();
    };
    m_link.onWrite(func);
}

QSinkLink::~QSinkLink()
{
}


void QSinkLink::connectToHost(QUrl url)
{
    m_socket->open(QUrl(url));
}

SinkNode *QSinkLink::sinkNode()
{
    return m_link.sinkNode();
}

SinkLink &QSinkLink::sinkLink()
{
    return m_link;
}

void QSinkLink::link(const QString &name)
{
    m_link.link(name.toStdString());
}


void QSinkLink::onConnected()
{
    qDebug() << "socket connected";
    processMessages();
}

void QSinkLink::onDisconnected()
{
    qDebug() << "socket disconnected";
}

void QSinkLink::handleTextMessage(const QString &message)
{
    m_link.handleMessage(message.toStdString());
}


void QSinkLink::processMessages()
{
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        while(!m_protocol.isEmpty()) {
            // if we are using JSON we need to use txt message
            // otherwise binary messages
            //    m_socket->sendBinaryMessage(QByteArray::fromStdString(message));
            const QString& msg = QString::fromStdString(m_protocol.dequeue());
            qDebug() << "write message to socket" << msg;
            m_socket->sendTextMessage(msg);
        }
    }

}


const QString &QSinkLink::name() const
{
    return m_name;
}
