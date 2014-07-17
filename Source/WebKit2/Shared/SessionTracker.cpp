/*
 * Copyright (C) 2014 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "SessionTracker.h"

#include <wtf/MainThread.h>
#include <wtf/NeverDestroyed.h>

using namespace WebCore;

namespace WebKit {

const uint64_t SessionTracker::defaultSessionID;
const uint64_t SessionTracker::legacyPrivateSessionID;

static HashMap<uint64_t, std::unique_ptr<NetworkStorageSession>>& staticSessionMap()
{
    ASSERT(isMainThread());

    static NeverDestroyed<HashMap<uint64_t, std::unique_ptr<NetworkStorageSession>>> map;
    return map.get();
}

static String& identifierBase()
{
    ASSERT(isMainThread());

    static NeverDestroyed<String> base;
    return base;
}

const HashMap<uint64_t, std::unique_ptr<NetworkStorageSession>>& SessionTracker::sessionMap()
{
    return staticSessionMap();
}

const String& SessionTracker::getIdentifierBase()
{
    return identifierBase();
}

NetworkStorageSession* SessionTracker::session(uint64_t sessionID)
{
    if (sessionID == defaultSessionID)
        return &NetworkStorageSession::defaultStorageSession();
    return staticSessionMap().add(sessionID, nullptr).iterator->value.get();
}

void SessionTracker::setSession(uint64_t sessionID, std::unique_ptr<NetworkStorageSession> session)
{
    ASSERT(sessionID != defaultSessionID);
    staticSessionMap().add(sessionID, nullptr).iterator->value = std::move(session);
}

void SessionTracker::destroySession(uint64_t sessionID)
{
    ASSERT(isMainThread());

    staticSessionMap().remove(sessionID);
}

void SessionTracker::setIdentifierBase(const String& identifier)
{
    ASSERT(isMainThread());

    identifierBase() = identifier;
}

} // namespace WebKit
