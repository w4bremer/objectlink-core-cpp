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
#pragma once

#include "core/node.h"
#include <set>

namespace ApiGear { namespace ObjectLink {

class RemoteNode;
class RemoteRegistry;


// passed into source object
// distribute signals/propetty changes
class IRemoteNode {
public:
    virtual ~IRemoteNode();
    virtual void notifyPropertyChange(std::string name, json value) = 0;
    virtual void notifySignal(std::string name, json args) = 0;
};

// impemented by source object
// called from object link
class IObjectSource {
public:
    virtual ~IObjectSource();
    virtual std::string olinkObjectName() = 0;
    virtual json olinkInvoke(std::string name, json args) = 0;
    virtual void olinkSetProperty(std::string name, json value) = 0;
    virtual void olinkLinked(std::string name, IRemoteNode* node) = 0;
    virtual void olinkUnlinked(std::string name) = 0;
    virtual json olinkCollectProperties() = 0;
};


struct SourceToNodesEntry {
    SourceToNodesEntry()
        : source(nullptr)
    {}
    IObjectSource *source;
    std::set<RemoteNode*> nodes;
};

class RemoteNode: public BaseNode, public IRemoteNode {
public:
    RemoteNode();
    virtual ~RemoteNode() override;
    void writePropertyChange(std::string name, json value);
    IObjectSource* getObjectSource(std::string name);
    RemoteRegistry &remoteRegistry();
public: // source registry
    void addObjectSource(IObjectSource *source);
    void removeObjectSource(IObjectSource *source);
public: // IMessagesListener interface
    void handleLink(std::string name) override;
    void handleUnlink(std::string name) override;
    void handleSetProperty(std::string name, json value) override;
    void handleInvoke(int requestId, std::string name, json args) override;

public: // IObjectSourceNode interface
    void notifyPropertyChange(std::string name, json value) override;
    void notifySignal(std::string name, json args) override;
};

class RemoteRegistry: public Base {
private:
    RemoteRegistry();
public:
    static RemoteRegistry& get();
    void addObjectSource(IObjectSource *source);
    void removeObjectSource(IObjectSource *source);
    IObjectSource* getObjectSource(std::string name);
    std::set<RemoteNode*> getRemoteNodes(std::string name);
    void attachRemoteNode(RemoteNode *node);
    void detachRemoteNode(RemoteNode* node);
    void linkRemoteNode(std::string name, RemoteNode *node);
    void unlinkRemoteNode(std::string name, RemoteNode *node);
private:
    std::map<std::string, SourceToNodesEntry> m_entries;
};

} } // Apigear::ObjectLink


