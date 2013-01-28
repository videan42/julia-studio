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

#include "imagecontainer.h"

namespace QmlDesigner {

ImageContainer::ImageContainer()
    : m_instanceId(-1)
{
}

ImageContainer::ImageContainer(qint32 instanceId, const QImage &image)
    : m_image(image), m_instanceId(instanceId)
{
}

qint32 ImageContainer::instanceId() const
{
    return m_instanceId;
}

QImage ImageContainer::image() const
{
    return m_image;
}

QDataStream &operator<<(QDataStream &out, const ImageContainer &container)
{
    out << container.instanceId();

    const QImage image = container.image();
    const QByteArray data(reinterpret_cast<const char*>(image.constBits()), image.byteCount());

    out << qint32(image.bytesPerLine());
    out << image.size();
    out << qint32(image.format());
    out << qint32(image.byteCount());
    out.writeRawData(reinterpret_cast<const char*>(image.constBits()), image.byteCount());

    return out;
}

QDataStream &operator>>(QDataStream &in, ImageContainer &container)
{

    qint32 byteSize;
    qint32 bytesPerLine;
    QSize imageSize;
    qint32 format;

    in >> container.m_instanceId;

    in >> bytesPerLine;
    in >> imageSize;
    in >> format;
    in >> byteSize;

    container.m_image = QImage(imageSize, QImage::Format(format));
    in.readRawData(reinterpret_cast<char*>(container.m_image.bits()), byteSize);

    return in;
}

} // namespace QmlDesigner
