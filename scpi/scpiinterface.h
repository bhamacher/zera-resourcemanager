#ifndef SCPIINTERFACE_H
#define SCPIINTERFACE_H

#include <QObject>
#include <QMutex>
#include <QList>
#include <scpi.h>

#include "delegate.h"
#include "catalog.h"

namespace Server
{
  class Client;
}


namespace Application
{
  class Resource;
}

/**
  @brief The SCPI namespace holds all the SCPI related classes and interfaces
  */
namespace SCPI
{
  /**
    @brief this encapsulates redundant entries for the add / remove parameters
    */
  namespace AddParams
  {
    /**
      @brief This enum allows parameter reordering and will provide legacy support if changed
      */
    enum Parameters
    {
      type=0,
      name,
      amount,
      description,
      providerSocketDesc
    };
  }
  /**
    @brief this encapsulates redundant entries for the add / remove parameters
    */
  namespace RemoveParams
  {
    /**
      @brief This enum allows parameter reordering and will provide legacy support if changed
      */
    enum Parameters
    {
      type=0,
      name,
      providerSocketDesc
    };
  }

  /**
    @brief this will encapsulate redundant entries for the occupy / free parameters
    */
  namespace OccupyParams
  {
    /**
      @brief This enum allows parameter reordering and will provide legacy support if changed
      */
    enum Parameters
    {
      amount=0,
      providerSocketDesc
    };
  }
  /**
    @brief this will encapsulate redundant entries for the occupy / free parameters
    */
  namespace FreeParams
  {
    /**
      @brief This enum allows parameter reordering and will provide legacy support if changed
      */
    enum Parameters
    {
      providerSocketDesc=0
    };
  }

  class ResourceObject;

  /**
    @brief The interface between this application and the SCPI library
    */
  class SCPIInterface : public QObject
  {
    Q_OBJECT
  protected:
    /**
      @b default constructor, private due to Singleton
      */
    SCPIInterface(QObject* parent=0);

  public:
    /**
      @b returns the singletonInstance
      */
    static SCPIInterface* getInstance();
    /**
      @b For debugging, returns the cSCPI::model
      */
    QStandardItemModel* getModel();
    /**
      @b uses the ResourceManager to list Application::Resource names of a given type
      */
    const QString listResourceByType(const QString &type);
    /**
      @b Lists types which have a Catalog command
      */
    const QString listTypes();



  signals:
    /**
      @b This signal will inform the Resourcemanager of new resources, it will also call SCPIInterface::resourceAdd()
      */
    void resourceAdded(Application::Resource* res);


  public slots:
    /**
      @b The clients SCPI data is handled here
      */
    void scpiTransaction(const QString &commands);
    /**
      @b Adds a SCPI::ResourceObject to the SCPI tree

      Also adds a SCPI::Catalog if the resource's type is a new type
      */
    void resourceAdd(Application::Resource * res);
    /**
      @b Removes a SCPI::ResourceObject from the SCPI tree

      @todo Also remove the SCPI::Catalog if the refcounter hits 0 and remove the node of the type
      */
    void resourceRemove(Application::Resource * res, int clientSocketDesc);
    /**
      @b Occupies an Application::Resource with the given client and amount
      */
    void resourceOccupy(Application::Resource * res, int clientSocketDesc, int amount);
    /**
      @b Frees an Application::Resource with the given client
      */
    void resourceFree(Application::Resource * res, int clientSocketDesc, int amount);

  private:
    /**
      @b represents the RESOURCE:ADD in the SCPI tree
      */
    Delegate* addResource;
    /**
      @b represents the RESOURCE:REMOVE in the SCPI tree
      */
    Delegate* removeResource;
    /**
      @b represents the RESOURCE:(TYPE):CATALOG in the SCPI tree
      */
    Delegate* catalogType;

    /**
      @b SCPI library interaction is held here
      */
    cSCPI* scpiInstance;
    /**
      @b Singleton instance, no other instances of this object are possible
      */
    static SCPIInterface* singletonInstance;

    /**
      @b used for thread safety in scpiTransaction()
      */
    QMutex scpiMutex;

    /**
      @b The list that holds the SCPI::Catalog for each catalogType
      */
    QList<Catalog *> catalogList;
    /**
      @b Holds the cSCPIObjects for the tree
      */
    QList<ResourceObject *> resourceList;


    /**
      @b Disables copying due to Singleton pattern
      */
    Q_DISABLE_COPY(SCPIInterface)
  };
}
#endif // SCPIINTERFACE_H
