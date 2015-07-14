/////////////////////////////////////////////////////////
//MainMessageDistributor.h
//Defines the MainMessageDistributor, who's responsible
//for distributing message and storing global data.
//
//Manuel Madera
//6/26/09
/////////////////////////////////////////////////////////

#include "MainMessageDistributor.h"
#include "CommMgr.h"
#include <fstream>
#include "PackagerFactory.h"
#include "ASynchPackager.h"
#include "Debug.h"
#include "LogManager.h"

#ifdef PrintMemoryLeak
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif
#endif

using namespace std;
using namespace Framework::Communications;
using namespace Framework::Packages;
using namespace Framework::Packagers;
using namespace Framework::Factories;
using namespace Framework::Logging;

namespace Framework
{
	namespace Messaging
	{
		RegistryID::RegistryID() : currID(1)
		{
		}

		RegistryID::~RegistryID()
		{
		}

		unsigned RegistryID::GetID()
		{
			return currID++;
		}

		void RegistryID::FreeID(unsigned id)
		{
		}

		MainMessageDistributor::MainMessageDistributor(unsigned rank, unsigned numNodes) 
			: myRank(rank), terminate(false), totalNumNode(numNodes), terminatedNodesCount(0), pMyCommMgr(0)
		{
			int rc;			

			sem_init(&binSem, 0, 1);

			PackagerFactory *packageFactory = PackagerFactory::Instance();

			if (!Validator::IsNull(packageFactory, NAME("packageFactory")))
			{
				packageFactory->RegisterPackager(PackagerType::ASynch_t, ASynchPackager::CreatePackager);
				packageFactory->RegisterPackager(PackagerType::Synch_t, Packager::CreatePackager);

				packageFactory->RegisterPackage(PackageType::NONE, EmptyPackage::CreatePackage);
				packageFactory->RegisterPackage(PackageType::REGISTRATION, RegistrationPackage::CreatePackage);
				packageFactory->RegisterPackage(PackageType::UNREGISTER, UnregisterPackage::CreatePackage);
				packageFactory->RegisterPackage(PackageType::CONFIRMATION, ConfirmationPackage::CreatePackage);
				packageFactory->RegisterPackage(PackageType::TERMINATE, TerminatePackage::CreatePackage);
				packageFactory->RegisterPackage(PackageType::MESSAGE, MessagePackage::CreatePackage);
				packageFactory->RegisterPackage(PackageType::MAZE_POSITION, MazePositionPackage::CreatePackage);
				packageFactory->RegisterPackage(PackageType::GET_ML_AGENT_INFO, GetMLAgentInfoPackage::CreatePackage);
				packageFactory->RegisterPackage(PackageType::ML_AGENT_INFO, MLAgentInfoPackage::CreatePackage);
				packageFactory->RegisterPackage(PackageType::MAZE, MazePackage::CreatePackage);
				packageFactory->RegisterPackage(PackageType::MAZE_CAN_MOVE, MazeCanMovePackage::CreatePackage);
				packageFactory->RegisterPackage(PackageType::MAZE_CAN_MOVE_REPLY, MazeCanMoveReplyPackage::CreatePackage);
				packageFactory->RegisterPackage(PackageType::MAZE_AGENT, MazeAgentPackage::CreatePackage);
				packageFactory->RegisterPackage(PackageType::NEW_MAZE_AGENT, NewMazeAgentPackage::CreatePackage);
				packageFactory->RegisterPackage(PackageType::GET_OBJ_ON, GetObjectOnPackage::CreatePackage);
				packageFactory->RegisterPackage(PackageType::POSITION, PositionPackage::CreatePackage);
			}

			pMyCommMgr = new CommMgr(PackagerType::ASynch_t, rank);

			Callback registrationCallback;
			registrationCallback.callback = &MainMessageDistributor::DistributorCallback;
			registrationCallback.ownerObject = this;

			this->callbackMap[PackageType::REGISTRATION]		= registrationCallback;
			this->callbackMap[PackageType::UNREGISTER]			= registrationCallback;
			this->callbackMap[PackageType::GET_ML_AGENT_INFO]	= registrationCallback;
			this->callbackMap[PackageType::TERMINATE]			= registrationCallback;
			this->callbackMap[PackageType::POSITION]			= registrationCallback;
			this->callbackMap[PackageType::MAZE_CAN_MOVE]		= registrationCallback;
			this->callbackMap[PackageType::MAZE_CAN_MOVE_REPLY]	= registrationCallback;
			this->callbackMap[PackageType::MAZE_AGENT]			= registrationCallback;

			this->log = LogManager::Instance();

			rc = pthread_create(&this->mainThread, NULL, MainMessageDistributor::MainThread, (void *)this);
		}

		MainMessageDistributor::~MainMessageDistributor()
		{
			//sem_wait(&this->binSem);

			sem_destroy(&this->binSem);

			agentsRegistry.clear();
			callbackMap.clear();
			//terminate = true;
			delete pMyCommMgr;
			pMyCommMgr = NULL;

			//pthread_join(this->mainThread, NULL); 
		}

		void MainMessageDistributor::Terminate()
		{
			terminate = true;
			//delete pMyCommMgr; pMyCommMgr = NULL;
		}

		void MainMessageDistributor::Wait()
		{
			pthread_join(this->mainThread, NULL);
		}

		void MainMessageDistributor::Register(Agents::IAgent *agent, Agents::AgentQueue *agentQ)
		{
			//Do nothing.
		}

		void MainMessageDistributor::Unregister(Agents::IAgent *agent)
		{
			//Do nothing.
		}

		void MainMessageDistributor::RegisterCallback(Packages::PackageType::Package type, void *ownerObject, MessageCallback callback)
		{
			sem_wait(&this->binSem);
			if (this->callbackMap.find(type) == this->callbackMap.end())
			{
				Callback newCallback;
				newCallback.callback = callback;
				newCallback.ownerObject = ownerObject;
				this->callbackMap[type] = newCallback;
			}
			sem_post(&this->binSem);
		}

		void MainMessageDistributor::UnregisterCallback(Packages::PackageType::Package type)
		{
			sem_wait(&this->binSem);
			if (this->callbackMap.find(type) != this->callbackMap.end())
			{
				this->callbackMap.erase(type);
			}
			sem_post(&this->binSem);
		}
		
		void MainMessageDistributor::SendPackage(Packages::IPackage *package)
		{
			if (!Validator::IsNull(package, NAME("package")))
			{
				Send(package, package->GetDestination());
			}
		}

		void MainMessageDistributor::Send(IPackage *package, unsigned dest)
		{
			if (!Validator::IsNull(package, NAME("package")))
			{
				IPackage *message = new MessagePackage(this->myRank, dest, package);

				if (!Validator::IsNull(this->pMyCommMgr, NAME("this->pMyCommMgr")))
				{
					this->pMyCommMgr->SendPackage(message);
				}
			}
		}

		void MainMessageDistributor::Broadcast(unsigned senderID, Framework::Packages::MazePositionPackage *mazePosPackage)
		{
			//Do nothing.
		}

		/*static*/ void MainMessageDistributor::DistributorCallback(void *ownerObject, Packages::IPackage *msg)
		{
			MainMessageDistributor* pThis = static_cast<MainMessageDistributor *>(ownerObject);

			if (!Validator::IsNull(pThis, NAME("pThis")))
			{
				MessagePackage *message = dynamic_cast<MessagePackage *>(msg);
				IPackage *package = 0;

				if (!Validator::IsNull(message, NAME("message")))
				{
					package = message->GetPackage();

					if (!Validator::IsNull(package, NAME("package")))
					{
						if (package->GetType() == PackageType::REGISTRATION)
						{
							RegistrationPackage *registration = dynamic_cast<RegistrationPackage *>(package);

							if (!Validator::IsNull(registration, NAME("registration")))
							{
								registration->SetID(pThis->registryID.GetID());
								unsigned dest = message->GetSource();
								registration->SetDestination(0);
								registration->SetSource(pThis->myRank);

								//Add to the list
								pThis->agentsRegistry[registration->GetID()] = dest;

								pThis->Send(registration, dest);					
							}
						}
						else if (package->GetType() == PackageType::UNREGISTER)
						{
							UnregisterPackage *registration = dynamic_cast<UnregisterPackage *>(package);

							if (!Validator::IsNull(registration, NAME("registration")))
							{
								pThis->registryID.FreeID(registration->GetID());      
								unsigned dest = message->GetSource();
								pThis->Send(new ConfirmationPackage(pThis->myRank, 0), dest);

								if (pThis->agentsRegistry.find(registration->GetID()) != pThis->agentsRegistry.end())
								{
									pThis->agentsRegistry.erase(registration->GetID());
								}								                   
							}
							
							delete registration; 
							registration = NULL;
						}
						else if (package->GetType() == PackageType::GET_ML_AGENT_INFO)
						{
							//GetMLAgentInfoPackage *infoPackage = dynamic_cast<GetMLAgentInfoPackage *>(package);

							//if (infoPackage !Validator::IsNull(package, NAME("package"))
							//{
							//	pThis->Send(new MLAgentInfoPackage(0, infoPackage->GetSource(), pThis->mlAgentsInfo[message->GetSource()].front()),
							//				message->GetSource());
							//	pThis->mlAgentsInfo[message->GetSource()].pop_front();
							//	delete infoPackage;
							//}
						}
						else if (package->GetType() == PackageType::TERMINATE)
						{
							pThis->terminatedNodesCount++;

							if (pThis->terminatedNodesCount == (pThis->totalNumNode - 1))
							{
								pThis->terminate = true;
							}

							delete package;
							package = NULL;
						}
						else if (package->GetType() == PackageType::POSITION)
						{
							if (pThis->agentsRegistry.find(package->GetDestination()) != pThis->agentsRegistry.end())
							{
								pThis->Send(package, pThis->agentsRegistry[package->GetDestination()]);
							}
							else
							{
								delete package; 
								package = NULL;
							}
						}
						else if (package->GetType() == PackageType::MAZE_CAN_MOVE)
						{
							MazeCanMovePackage *movePackage = dynamic_cast<MazeCanMovePackage *>(package);

							if (!Validator::IsNull(movePackage, NAME("movePackage")))
							{
								for (unsigned i = 1; i < pThis->totalNumNode; i++)
								{
									if (i != movePackage->GetSource())
									{
										pThis->Send(new MazeCanMovePackage(movePackage->GetSource(), movePackage->GetDestination(), movePackage->GetPos(), movePackage->GetAgentId()), i);
									}
								}								
							}
							
							delete movePackage;
							movePackage = NULL;
						}				
						else if (package->GetType() == PackageType::MAZE_CAN_MOVE_REPLY)
						{
							pThis->Send(package, package->GetDestination());
						}
						else if (package->GetType() == PackageType::MAZE_AGENT)
						{
							pThis->agentsRegistry[package->GetSource()] = message->GetSource();

							pThis->Send(package, package->GetDestination());					
						}
					}
				}
			}

			delete msg;
			msg = NULL;
		}

		/*static*/ void *MainMessageDistributor::MainThread(void *arg)
		{
			MainMessageDistributor* pThis = static_cast<MainMessageDistributor *>(arg);

			if (!Validator::IsNull(pThis, NAME("pThis")))
			{
				IPackage *msg = NULL;

				while (!pThis->terminate)
				{
					if (!Validator::IsNull(pThis->pMyCommMgr, NAME("pThis->pMyCommMgr")))
					{
						msg = pThis->pMyCommMgr->GetPackage();
					}

					MessagePackage *message = dynamic_cast<MessagePackage *>(msg);
					IPackage *package = 0;

					if (!Validator::IsNull(message, NAME("message")))
					{
						sem_wait(&pThis->binSem);
						if (pThis->callbackMap.find(message->GetInnerPackageType()) != pThis->callbackMap.end())
						{
#ifdef DebugMessageDistributor
							if (!Validator::IsNull(pThis->log, NAME("pThis->log")))
							{
								pThis->log->Log(&typeid(pThis), "Received package type: %d", message->GetInnerPackageType());
							}
#endif

							pThis->callbackMap[message->GetInnerPackageType()].callback(pThis->callbackMap[message->GetInnerPackageType()].ownerObject, message);
						}
						else
						{
							package = message->GetPackage();

							if (!Validator::IsNull(package, NAME("package")))
							{
								if (pThis->agentsRegistry.find(package->GetDestination()) != pThis->agentsRegistry.end())
								{
#ifdef DebugMessageDistributor
									if (!Validator::IsNull(pThis->log, NAME("pThis->log")))
									{
										pThis->log->Log(&typeid(pThis), "Sending package type: %d to %d from %d", package->GetType(), package->GetDestination(), package->GetSource());
									}
#endif

									pThis->Send(package, pThis->agentsRegistry[package->GetDestination()]);
								}
								else
								{
									if (!Validator::IsNull(pThis->log, NAME("pThis->log")))
									{
										pThis->log->Log(&typeid(pThis), "Warning: unhandled package.  PackageType %d", package->GetType());
									}

									delete package;
									package = NULL;
								}
							}
							else
							{
								if (!Validator::IsNull(pThis->log, NAME("pThis->log")))
								{
									pThis->log->Log(&typeid(pThis), "Warning: null package received.");
								}

								delete message;
								message = NULL;
							}
						}
						sem_post(&pThis->binSem);
					}
					else
					{
						if (!Validator::IsNull(msg, NAME("msg")))
						{
							if (!Validator::IsNull(pThis->log, NAME("pThis->log")))
							{
								pThis->log->Log(&typeid(pThis), "Warning: package was not of type Message.  PackageType %d", msg->GetType());
							}							
						}
						
						delete msg;
						msg = NULL;
					}
				}
			}

			#ifdef LINUX
				pthread_exit(NULL);
			#endif

	#ifdef WIN32
			return NULL;
	#endif
		}
	}
}