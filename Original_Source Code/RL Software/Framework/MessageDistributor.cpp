/////////////////////////////////////////////////////////
//MessageDistributor.pp
//Defines the MessageDistributor, responsible for 
//distributing messages between the agents.
//
//Manuel Madera
//6/26/09
/////////////////////////////////////////////////////////

#include "MessageDistributor.h"
#include "EventDistributor.h"
#include "CommMgr.h"
#include "PackagerFactory.h"
#include "ASynchPackager.h"
#include "Packager.h"
#include "Debug.h"
#include "LogManager.h"

#ifdef PrintMemoryLeak
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif
#endif

using namespace Framework::Communications;
using namespace Framework::Packages;
using namespace Framework::Packagers;
using namespace Framework::Agents;
using namespace Framework::Eventing;
using namespace Framework::Factories;
using namespace Framework::Logging;

namespace Framework
{
	namespace Messaging
	{
		IMessageDistributor *MessageDistributor::instance = 0;

		MessageDistributor::MessageDistributor() : terminate(false), isInitialized(false), regChangePublisher(0), mazeRecvPublisher(0), mazePositionPublisher(0), eventDist(0), log(0), registrationQueue(0), confirmationQueue(0)
		{    
			sem_init(&binSem, 0, 1);
			sem_init(&callbackSem, 0, 1);
			sem_init(&registrationSem, 0, 0);
			sem_init(&confirmationSem, 0, 0);			
		}

		void MessageDistributor::Init(unsigned rank, bool allowSharing)
		{
			if (!this->isInitialized)
			{
				this->shareAgentInfo = allowSharing;

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
				}

				this->rank = rank;
				registrationQueue = new AgentQueue();
				confirmationQueue = new AgentQueue();
				pMyCommMgr = new CommMgr(PackagerType::ASynch_t, rank);

				//Not really used
				agentsManaged[0] = registrationQueue;

				eventDist = EventDistributor::Instance();

				regChangePublisher = new Publisher(eventDist, SubscriptionTypes::REGISTRATION_CHANGE);
				mazeRecvPublisher = new Publisher(eventDist, SubscriptionTypes::MAZE_RECEIVED);
				mazePositionPublisher = new Publisher(eventDist, SubscriptionTypes::MAZE_POSITION_EVENT);

				Callback registrationCallback;
				registrationCallback.callback = &MessageDistributor::DistributorCallback;
				registrationCallback.ownerObject = this;

				Callback mazeCallback;
				mazeCallback.callback = &MessageDistributor::MazePackageCallback;
				mazeCallback.ownerObject = this;

				Callback agentCallback;
				agentCallback.callback = &MessageDistributor::AgentPackageCallback;
				agentCallback.ownerObject = this;

				this->callbackMap[PackageType::REGISTRATION] =	registrationCallback;
				this->callbackMap[PackageType::CONFIRMATION] =	registrationCallback;
				this->callbackMap[PackageType::MAZE] =			mazeCallback;
				this->callbackMap[PackageType::MAZE_POSITION] = agentCallback;
				this->callbackMap[PackageType::ML_AGENT_INFO] = agentCallback;
				
				int rc;
	        
				this->log = LogManager::Instance();

				rc = pthread_create(&this->recvThread, NULL, MessageDistributor::ReceiveThread, (void *)this);
				this->isInitialized = true;
			}
		}

		/*static*/ IMessageDistributor *MessageDistributor::Instance()
		{
			if (instance == 0)
			{
				instance = new MessageDistributor();
			}

			return instance;
		}

		MessageDistributor::~MessageDistributor()
		{    
			terminate = true;           

			if (this->isInitialized)
			{
				delete regChangePublisher;
				regChangePublisher = NULL;

				delete mazeRecvPublisher;
				mazeRecvPublisher = NULL;

				delete mazePositionPublisher;
				mazePositionPublisher = NULL;

				delete registrationQueue;
				registrationQueue = NULL;

				delete confirmationQueue;
				confirmationQueue = NULL;

				delete pMyCommMgr; 
				pMyCommMgr = NULL;

				pthread_cancel(this->recvThread);
				//pthread_join(this->recvThread, NULL);

				//sem_wait(&this->binSem);

				sem_destroy(&this->binSem);
				sem_destroy(&this->callbackSem);
				sem_destroy(&this->registrationSem);
				sem_destroy(&this->confirmationSem);
			}
		}
	     
		void MessageDistributor::Shutdown()
		{
			delete instance;
			instance = NULL;
		}

		void MessageDistributor::Register(IAgent *agent, AgentQueue *agentQ)
		{
			if (!Validator::IsNull(agent, NAME("agent")))
			{
				if (this->isInitialized)
				{
					if (!agent->IsRegistered())
					{
						//Send registration request
						IPackage *registration = new RegistrationPackage(rank, 0);

						//Takes care of the memory clean up for RegistrationPackage
						Send(registration);

						if (!Validator::IsNull(eventDist, NAME("eventDist")))
						{
							eventDist->Subscribe(this, SubscriptionTypes::REGISTRATION_CHANGE, NotificationType::ONETIME);
						}

						sem_wait(&this->registrationSem);
						//Get UI from the main distributor
						RegistrationPackage *reg = NULL;
						
						if (!Validator::IsNull(this->registrationQueue, NAME("this->registrationQueue")))
						{
							reg = dynamic_cast<RegistrationPackage *>(this->registrationQueue->Dequeue());
						}

						if (!Validator::IsNull(reg, NAME("reg")))
						{
							agent->SetAgentUI(reg->GetID());							
						}
						
						delete reg;
						reg = NULL;
					}

					sem_wait(&this->binSem);
					//Add agent to the local table
					if (agentsManaged.find(agent->GetAgentUI()) == agentsManaged.end())
					{
						agentsManaged[agent->GetAgentUI()] = agentQ;
					}
					sem_post(&this->binSem);

#ifdef DebugMessageDistributor
					if (!Validator::IsNull(log, NAME("log")))
					{
						log->Log(&typeid(this), "Registered agent: %d", agent->GetAgentUI());
					}
#endif
				}
				else
				{
#ifdef DebugMessageDistributor
					if (!Validator::IsNull(log, NAME("log")))
					{
						log->Log(&typeid(this), "Error: Attempted to register agent %d before initialization", agent->GetAgentUI());
					}
#endif
				}
			}
		}

		void MessageDistributor::Unregister(Agents::IAgent *agent)
		{			
			bool lockReleased = false;		//Needed to release the semaphore if the agent is not found in the list

			if (!Validator::IsNull(agent, NAME("agent")))
			{
				unsigned agentUI = agent->GetAgentUI();

				if (this->isInitialized)
				{		
					sem_wait(&this->binSem);
					if (!agent->IsTransfered())
					{					
						if (agentsManaged.find(agentUI) != agentsManaged.end())
						{
							lockReleased = true;
							sem_post(&this->binSem);

							//Send unregister request
							IPackage *unreg = new UnregisterPackage(rank, 0, agentUI);
							Send(unreg);

							if (!Validator::IsNull(eventDist, NAME("eventDist")))
							{
								eventDist->Subscribe(this, SubscriptionTypes::REGISTRATION_CHANGE, NotificationType::ONETIME);
							}

							sem_wait(&this->confirmationSem);
							//Wait for confirmation
							if (!Validator::IsNull(this->confirmationQueue, NAME("this->confirmationQueue")))
							{
								ConfirmationPackage *reg = dynamic_cast<ConfirmationPackage *>(this->confirmationQueue->Dequeue());
								
								delete reg;	
								reg = NULL;
							}						
						}

#ifdef DebugMessageDistributor
						if (!Validator::IsNull(log, NAME("log")))
						{
							log->Log(&typeid(this), "Unregistered agent: %d", agent->GetAgentUI());
						}
#endif
					}

					if (lockReleased)
					{
						sem_wait(&this->binSem);
					}

					if (agentsManaged.find(agent->GetAgentUI()) != agentsManaged.end())
					{
						agentsManaged.erase(agentUI);
					}
					sem_post(&this->binSem);
				}
				else
				{
#ifdef DebugMessageDistributor
					if (!Validator::IsNull(log, NAME("log")))
					{
						log->Log(&typeid(this), "Error: Attempted to unregister agent %d before initialization", agent->GetAgentUI());
					}
#endif
				}
			}
		}

		void MessageDistributor::RegisterCallback(Packages::PackageType::Package type, void *ownerObject, MessageCallback callback)
		{
			sem_wait(&this->callbackSem);
			if (this->callbackMap.find(type) == this->callbackMap.end())
			{
				Callback newCallback;
				newCallback.callback = callback;
				newCallback.ownerObject = ownerObject;
				this->callbackMap[type] = newCallback;
			}
			sem_post(&this->callbackSem);
		}

		void MessageDistributor::UnregisterCallback(Packages::PackageType::Package type)
		{
			sem_wait(&this->callbackSem);
			if (this->callbackMap.find(type) != this->callbackMap.end())
			{
				this->callbackMap.erase(type);
			}
			sem_post(&this->callbackSem);
		}

		void MessageDistributor::Broadcast(unsigned senderID, MazePositionPackage *mazePosPackage)
		{
			map<unsigned, AgentQueue *>::iterator iter;

			if (this->isInitialized)
			{
				if (!Validator::IsNull(mazePosPackage, NAME("mazePosPackage")))
				{
					sem_wait(&this->binSem);
					if (mazePosPackage->GetObject() == 'B')
					{
						if (this->shareAgentInfo)
						{
							for (iter = this->agentsManaged.begin(); iter != this->agentsManaged.end(); iter++)
							{
								if ((iter->first != senderID)&&(iter->first != 0))
								{
									if (!Validator::IsNull(iter->second, NAME("iter->second")))
									{
										iter->second->Enqueue(mazePosPackage->Clone());
									}
								}
							}
						}
					}				

#ifdef DebugMessageDistributor
					Position<int> tempPos = mazePosPackage->GetPosition() + this->offset;
					
					if (!Validator::IsNull(log, NAME("log")))
					{
						log->Log(&typeid(this), "Agent: %d, on Node: %d, broadcasted object: %c, on x = %d, y = %d", senderID, this->rank, mazePosPackage->GetObject(), tempPos.GetXCoor(), tempPos.GetYCoor());
					}
#endif

					if (!Validator::IsNull(mazePositionPublisher, NAME("mazePositionPublisher")))
					{
						mazePositionPublisher->Publish(new MazePositionEventPackage(this->rank, 0, new MazePositionPackage(0, 0, mazePosPackage->GetObject(), mazePosPackage->GetPosition() + this->offset)));				
					}

					delete mazePosPackage;
					mazePosPackage = NULL;

					sem_post(&this->binSem);
				}
			}
		}

		void MessageDistributor::SendPackage(IPackage *package)
		{	        
			Send(package);
		}

		void MessageDistributor::Send(IPackage *package)
		{
			if (!Validator::IsNull(package, NAME("package")))
			{
				if (this->isInitialized)
				{
					IPackage *message = new MessagePackage(this->rank, 0, package);

					//Takes care of the memory clean up for the MessagePackage
					if (!Validator::IsNull(this->pMyCommMgr, NAME("this->pMyCommMgr")))
					{
						this->pMyCommMgr->SendPackage(message);
					}
				}
			}
		}

		void MessageDistributor::Notify(const ISubscription *subscription)
		{
			const RegistrationChangeSubscription *regSubscription = static_cast<const RegistrationChangeSubscription *>(subscription);

			if (!Validator::IsNull(regSubscription, NAME("regSubscription")))
			{
				if (regSubscription->GetValue() == PackageType::REGISTRATION)
				{
					if (!Validator::IsNull(this->registrationQueue, NAME("this->registrationQueue")))
					{
						this->registrationQueue->Enqueue(regSubscription->GetData());
					
						sem_post(&this->registrationSem);
					}
				}
				else
				{
					if (!Validator::IsNull(this->confirmationQueue, NAME("this->confirmationQueue")))
					{
						this->confirmationQueue->Enqueue(regSubscription->GetData());
						sem_post(&this->confirmationSem);
					}
				}
			}
		}

		Framework::Position<int> MessageDistributor::GetOffset()
		{
			return this->offset;
		}

		/*static*/ void MessageDistributor::DistributorCallback(void *msgDist, Packages::IPackage *package)
		{
			MessageDistributor *pThis = static_cast<MessageDistributor *>(msgDist);

			//Notify myself that an object registration change just occured.
			if ((!Validator::IsNull(pThis, NAME("pThis")))&&(!Validator::IsNull(package, NAME("package"))))
			{
				if (!Validator::IsNull(pThis->regChangePublisher, NAME("pThis->regChangePublisher")))
				{
					pThis->regChangePublisher->Publish(new RegistrationChangeSubscription(package->GetType(), package));
				}
			}
		}

		/*static*/void MessageDistributor::MazePackageCallback(void *msgDist, Packages::IPackage *package)
		{			
			MessageDistributor *pThis = static_cast<MessageDistributor *>(msgDist);
			MazePackage *tempPtr = static_cast<MazePackage *>(package);

			if ((!Validator::IsNull(pThis, NAME("pThis")))&&(!Validator::IsNull(tempPtr, NAME("tempPtr"))))
			{
				sem_wait(&pThis->binSem);
				pThis->offset.SetXCoor(tempPtr->GetOffset().GetXCoor());
				pThis->offset.SetYCoor(tempPtr->GetOffset().GetYCoor());			

				if (!Validator::IsNull(pThis->mazeRecvPublisher, NAME("pThis->mazeRecvPublisher")))
				{
					pThis->mazeRecvPublisher->Publish(new MazeReceivedSubscription(tempPtr->GetMaze()));
				}

				sem_post(&pThis->binSem); 
			}

			delete package; 
			package = NULL;
		}

		/*static*/ void MessageDistributor::AgentPackageCallback(void *msgDist, Packages::IPackage *package)
		{
			MessageDistributor *pThis = static_cast<MessageDistributor *>(msgDist);

			if ((!Validator::IsNull(pThis, NAME("pThis")))&&(!Validator::IsNull(package, NAME("package"))))
			{
				sem_wait(&pThis->binSem);
				if (pThis->agentsManaged.find(package->GetDestination()) != pThis->agentsManaged.end())
				{
					if (!Validator::IsNull(pThis->agentsManaged[package->GetDestination()], NAME("pThis->agentsManaged[package->GetDestination()]")))
					{
						pThis->agentsManaged[package->GetDestination()]->Enqueue(package);
					}
				}
				else
				{
					delete package; 
					package = NULL;
				}
				sem_post(&pThis->binSem); 
			}
		}

		/*static*/ void *MessageDistributor::ReceiveThread(void *arg)
		{
			MessageDistributor* pThis = static_cast<MessageDistributor *>(arg);
			IPackage *msg = NULL;

			if (!Validator::IsNull(pThis, NAME("pThis")))
			{
				if (pThis->isInitialized)
				{
					while (!pThis->terminate)
					{
						if (!Validator::IsNull(pThis->pMyCommMgr, NAME("pThis->pMyCommMgr")))
						{
							msg = pThis->pMyCommMgr->GetPackage();
						}

						MessagePackage *message = NULL;
						
						if (!Validator::IsNull(msg, NAME("msg")))
						{
							message = dynamic_cast<MessagePackage *>(msg);
						}

						IPackage *package = 0;						

						if (!Validator::IsNull(message, NAME("message")))
						{
							package = message->GetPackage();

							if (!Validator::IsNull(package, NAME("package")))
							{
								sem_wait(&pThis->callbackSem);
								if (pThis->callbackMap.find(package->GetType()) != pThis->callbackMap.end())
								{
#ifdef DebugMessageDistributor
									if (!Validator::IsNull(pThis->log, NAME("pThis->log")))
									{
										pThis->log->Log(&typeid(pThis), "Received package type: %d", package->GetType());
									}
#endif

									pThis->callbackMap[package->GetType()].callback(pThis->callbackMap[package->GetType()].ownerObject, package);
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
								sem_post(&pThis->callbackSem);
							}

							delete message;
							message = NULL;
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