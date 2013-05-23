#include "server/serverinterface.h"
#include "scpi/scpiinterface.h"
#include "resource/resource.h"
#include <QDebug>


namespace Server
{
  ServerInterface* ServerInterface::getInstance()
  {
    if(singletonInstance==0)
    {
      singletonInstance=new ServerInterface;
    }
    return singletonInstance;
  }

  void ServerInterface::clientDisconnected()
  {
    if(QObject::sender()!=0)
    {
      Client* cl = static_cast<Server::Client*> (QObject::sender());

      qDebug()<<"Client disconnected:"<<cl->getName();
      clients.removeAll(cl);
      cl->deleteLater();
    }
  }

  void ServerInterface::newClient(Zera::Net::ZeraClient* zcl)
  {
    Client* tmpClient = new Client(zcl);
    clients.append(tmpClient);
    connect(tmpClient,SIGNAL(aboutToDisconnect()),this,SLOT(clientDisconnected()));
    connect(tmpClient,SIGNAL(scpiCommandSent(ProtobufMessage::NetMessage::ScpiCommand)),SCPI::SCPIInterface::getInstance(),SLOT(scpiTransaction(ProtobufMessage::NetMessage::ScpiCommand)));
  }


  ServerInterface::ServerInterface(QObject* parent) :
    QObject(parent),
    m_zServer(Zera::Net::ZeraServer::getInstance())
  {
    connect(m_zServer,SIGNAL(newClientAvailable(Zera::Net::ZeraClient*)),this,SLOT(newClient(Zera::Net::ZeraClient*)));
    m_zServer->startServer(12345); /// @todo Change port
  }

  ServerInterface::~ServerInterface()
  {
    foreach(Client *c, clients)
    {
      c->deleteLater();
    }
  }
  ServerInterface* ServerInterface::singletonInstance=0;



}
