/****************************************************************************
**
** Copyright (C) 2016 Pelagicore AG
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Pelagicore Application Manager.
**
** $QT_BEGIN_LICENSE:LGPL-QTAS$
** Commercial License Usage
** Licensees holding valid commercial Qt Automotive Suite licenses may use
** this file in accordance with the commercial license agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and The Qt Company.  For
** licensing terms and conditions see https://www.qt.io/terms-conditions.
** For further information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
** SPDX-License-Identifier: LGPL-3.0
**
****************************************************************************/

#include "application.h"
#include "abstractcontainer.h"


AbstractContainer::~AbstractContainer()
{ }

QString AbstractContainer::controlGroup() const
{
    return QString();
}

bool AbstractContainer::setControlGroup(const QString &groupName)
{
    Q_UNUSED(groupName)
    return false;
}

bool AbstractContainer::setProgram(const QString &program)
{
    if (!m_program.isEmpty())
        return false;
    m_program = program;
    return true;
}

void AbstractContainer::setBaseDirectory(const QString &baseDirectory)
{
    m_baseDirectory = baseDirectory;
}

QString AbstractContainer::mapContainerPathToHost(const QString &containerPath) const
{
    return containerPath;
}

QString AbstractContainer::mapHostPathToContainer(const QString &hostPath) const
{
    return hostPath;
}

AbstractContainerProcess *AbstractContainer::process() const
{
    return m_process;
}

AbstractContainer::AbstractContainer(AbstractContainerManager *manager)
    : QObject(manager)
    , m_manager(manager)
{ }

QVariantMap AbstractContainer::configuration() const
{
    if (m_manager)
        return m_manager->configuration();
    return QVariantMap();
}



AbstractContainerManager::AbstractContainerManager(const QString &id, QObject *parent)
    : QObject(parent)
    , m_id(id)
{ }

QString AbstractContainerManager::defaultIdentifier()
{
    return QString();
}

QString AbstractContainerManager::identifier() const
{
    return m_id;
}

bool AbstractContainerManager::supportsQuickLaunch() const
{
    return false;
}

QVariantMap AbstractContainerManager::configuration() const
{
    return m_configuration;
}

void AbstractContainerManager::setConfiguration(const QVariantMap &configuration)
{
    m_configuration = configuration;
}
