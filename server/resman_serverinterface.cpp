#include "server/resman_serverinterface.h"
#include "scpi/resman_scpiinterface.h"
#include "resource/resman_resource.h"
#include "server/resman_clientsocket.h"
#include "server/resman_clientmultiton.h"

#include "rmprotobufwrapper.h"

#include <xiqnetserver.h>
#include <xiqnetpeer.h>


#include <QDebug>


namespace ResourceServer
{

  ServerInterface::ServerInterface(SCPI::SCPIInterface *t_scpiInterface, QObject *t_parent) :
    QObject(t_parent),
    m_scpiInterface(t_scpiInterface),
    m_zServer(new XiQNetServer(this)),
    m_defaultWrapper(new RMProtobufWrapper())
  {
    Q_ASSERT(t_scpiInterface != nullptr);
    m_zServer->setDefaultWrapper(m_defaultWrapper);
    connect(m_zServer, &XiQNetServer::sigClientConnected,this, &ServerInterface::newClient);
    m_zServer->startServer(6312);
  }

  ServerInterface::~ServerInterface()
  {
    for(ClientSocket *clientSocket : qAsConst(m_clientSockets))
    {
      delete clientSocket;
    }
    m_clientSockets.clear();
    delete m_defaultWrapper;
  }

  void ServerInterface::clientDisconnected(ClientSocket *t_clientSocket)
  {
    Q_ASSERT(t_clientSocket != nullptr);
    m_clientSockets.removeAll(t_clientSocket);
    delete t_clientSocket;
  }

  void ServerInterface::newClient(XiQNetPeer *t_newClient)
  {
    Q_ASSERT(t_newClient != nullptr);
    ClientSocket* tmpClient = new ClientSocket(t_newClient);
    m_clientSockets.append(tmpClient);
    connect(tmpClient, &ClientSocket::sigAboutToDisconnect, [this, tmpClient](){
      this->clientDisconnected(tmpClient);
    });
    connect(tmpClient, &ClientSocket::sigClientIdentified, this, &ServerInterface::onClientIdentified);
    connect(tmpClient, &ClientSocket::sigClientMultitonDisconnected, m_scpiInterface, &SCPI::SCPIInterface::onClientMultitonDisconnected);
  }

  void ServerInterface::onClientIdentified(ClientMultiton *t_clientMultiton)
  {
    connect(t_clientMultiton, &ClientMultiton::sigScpiTransaction, m_scpiInterface, &SCPI::SCPIInterface::onScpiTransaction);
  }
}
