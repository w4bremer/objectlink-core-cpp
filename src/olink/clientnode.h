#pragma once

#include "core/olink_common.h"
#include "core/types.h"
#include "iclientnode.h"
#include "core/basenode.h"
#include <map>
#include <mutex>
#include <atomic>


namespace ApiGear { namespace ObjectLink {

class ClientNode;
class ClientRegistry;
class IObjectSink;

/** 
 * Client node separates the object sink from a network related implementation, it provides functionality 
 * for sending and receiving messages. Handles incoming messages and decodes them,
 * allows to write messages requested by sinks that are using this client node and codes them.
 * The network implementation should deliver a write function for the node  to allow sending messages
 * see BaseNode::emitWrite and BaseNode::onWrite.
 * A sink that receives a handler call is chosen based on registry entries and objectId retrieved from incoming message.
 * Client node registers itself in registry on creation and removes on destruction, node should not be registered/unregistered manually.
 * Use linkRemote function to link your sink object with a source and use this node for communication. It already takes care of handling 
 * setting and unsetting(for unlink) the node for the object in registry. Set and unset in registry should not be done manually.
 */
class OLINK_EXPORT ClientNode : public BaseNode, public IClientNode, public std::enable_shared_from_this<ClientNode>
{
protected:
    /**
    * Protected constructor. Use create function to make an instance of ClientNode.
    * @param registry. A global registry for client nodes and object sinks
    */
    ClientNode(ClientRegistry& registry);

    /*
    * Protected method to allow assigning the node id from a factory method.
    * @param id. An id obtained on registration in registry.
    */
    void setNodeId(unsigned long id);
public:
    /**
    * Factory method to create a remote node.
    * @return new ClientNode.
    */
    static std::shared_ptr<ClientNode> create(ClientRegistry& registry);

    /* dtor */
    ~ClientNode() override;

    /** IClientNode::linkRemote implementation. */
    void linkRemote(const std::string& objectId) override;
    /** IClientNode::unlinkRemote implementation. */
    void unlinkRemote(const std::string& objectId) override;
    /** IClientNode::invokeRemote implementation. */
    void invokeRemote(const std::string& methodId, const nlohmann::json& args=nlohmann::json{}, InvokeReplyFunc func=nullptr) override;
    /** IClientNode::setRemoteProperty implementation. */
    void setRemoteProperty(const std::string& propertyId, const nlohmann::json& value) override;

     /* The registry in which client is registered*/
    ClientRegistry& registry();

    /* 
    * The id that registry assigned to a node. 
    * This id is used to connect the node with sink object in registry.
    */
    unsigned long getNodeId() const;

protected:
    /** IProtocolListener::handleInit implementation */
    void handleInit(const std::string& objectId, const nlohmann::json& props) override;
    /** IProtocolListener::handlePropertyChange implementation */
    void handlePropertyChange(const std::string& propertyId, const nlohmann::json& value) override;
    /** IProtocolListener::handleInvokeReply implementation */
    void handleInvokeReply(int requestId, const std::string& methodId, const nlohmann::json& value) override;
    /** IProtocolListener::handleSignal implementation */
    void handleSignal(const std::string& signalId, const nlohmann::json& args) override;
    /** IProtocolListener::handleError implementation */
    void handleError(int msgType, int requestId, const std::string& error) override;

    /**
     * Returns a request id for outgoing messages.
     * @return a unique, non negative id.
     */
    int nextRequestId();
private:
    /* The registry in which client is registered and which provides sinks connected with this node*/
    ClientRegistry& m_registry;
    /* Id of this node in registry.*/
    unsigned long m_nodeId;

    /* Value of last request id.*/
    std::atomic<int> m_nextRequestId;
    /** Collection of callbacks for method replies that client is waiting for associated with the id for invocation request message.*/
    std::map<int,InvokeReplyFunc> m_invokesPending;
    std::mutex m_pendingInvokesMutex;
};

} } // ApiGear::ObjectLink
