#include "MeuAgentePrincipal.h"
#include <AgentePrincipal.h>
#include <Windows.h>
#include <BWAPI\Position.h>
#include <BWAPI\UnitType.h>

//blackboard!
bool GameOver = false;
Unidade* amigoDaVez = NULL;
bool isPoolDone = false;
bool readyToPool = true;
bool canBuild10thWorker = true;
bool canTrainOverlord = false;
bool canEndTheWorld = false;
int drones = 0;

void workerAI (Unidade* u){
	double distance = 99999;
	Unidade* mineralPerto = NULL;
	std::set<Unidade*> minerais = u->getMinerals();
	for(std::set<Unidade*>::iterator it = minerais.begin(); it != minerais.end(); it++){
		if(u->getDistance(*it) < distance){
			distance = u->getDistance(*it);
			mineralPerto = *it;
		}
	}
	if(mineralPerto != NULL) u->rightClick(mineralPerto);
}

void odebrechtAI(Unidade* u){
	printf("I AM THE ODEBRECH\n");
	while(true){
		BWAPI::TilePosition tp = u->getTilePosition();
		if (u->getType().isWorker()){
			std::set<Unidade*> units = u->getAllyUnits();
			for(std::set<Unidade*>::iterator it = units.begin(); it != units.end(); it++){
				if((*it)->getType() == BWAPI::UnitTypes::Zerg_Hatchery){
					tp = (*it)->getTilePosition();
					break;
				}
			}
			int limite = 0;
			int adi = 6;

			while(!(u)->build(tp, BWAPI::UnitTypes::Zerg_Spawning_Pool)){
				if(((u->minerals()&0xF)>>2) < 2)
					tp = BWAPI::TilePosition(tp.x(), tp.y()+adi);
				else
					tp = BWAPI::TilePosition(tp.x()+adi, tp.y());
				tp.makeValid();
				limite++;
				if(limite > 50){
					break;
				}
				adi = -adi + (adi > 0 ? -2 : +2);
			}
			//printf("Fora\n");
		}else{
			printf("BREAK");
			break;
		}
	}
}

void headQuartersAI (Unidade* u){
	if (u->minerals() < 50){
		return;
	}
	//if (u->minerals() < 200 && isPoolDone && u->supplyTotal() < 18) continue;
	if (readyToPool) {
		if (u->supplyUsed() >= 18 && !isPoolDone && u->minerals() >= 200){
			std::set<Unidade*> units = u->getAllyUnits();
			for(std::set<Unidade*>::iterator it = units.begin(); it != units.end(); it++){
				if((*it)->getType() == BWAPI::UnitTypes::Zerg_Drone){
					printf("Ready To Pool\n");
					odebrechtAI(*it);
					readyToPool = false;
					break;
				}
			}
		}else if(u->supplyUsed() < 18){
			printf("Creating Drone!!\n");
			u->train(BWAPI::UnitTypes::Zerg_Drone);
		}
	}else if (isPoolDone && canBuild10thWorker) {
		//printf("Creating 10th Drone!!\n");
		if (1) {
			/*if(u->isTraining())
			{
				printf("Done Drone\n");
				canBuild10thWorker = false;
				canTrainOverlord = true;
			}*/
			//Sleep(2000);
			std::set<Unidade*> units = u->getLarva();
			
			printf("size units: %d\n", units.size());
			for(std::set<Unidade*>::iterator it = units.begin(); it != units.end(); it++){
				if((*it)->morph(BWAPI::UnitTypes::Zerg_Drone)){
					printf("i am morphing\n");
					canTrainOverlord = true;
					while((*it)->isMorphing()) printf("i am still morphng\n");
					if((*it)->getType() == BWAPI::UnitTypes::Zerg_Drone)
					{
						printf("I am now a Drone\n");
					}else if((*it)->getType() == BWAPI::UnitTypes::Zerg_Larva)printf("I am still a larva, how come?");
				}else 
				{
					printf("im not morphing.\n");
					continue;
				}
				
				printf("done drone\n");
				
				//cantrainoverlord = true;
				break;
			}
			canBuild10thWorker = false;
			printf("I am leaving 10th");
		}
	}else if (canTrainOverlord){
		if (u->train(BWAPI::UnitTypes::Zerg_Overlord)){
			while (u->isTraining() ){
			}
			printf("Done Overlord");
			canTrainOverlord = false;
			//canEndTheWorld = true;
		}
	}else if(canEndTheWorld){
		u->train(BWAPI::UnitTypes::Zerg_Zergling);
	}

	//if((u->supplyTotal() - u->supplyUsed() < 5)){
	// //botar no "blackboard" para alguem construir predios de supply
	// std::set<Unidade*> amigos = u->getAllyUnits();
	// for(std::set<Unidade*>::iterator it = amigos.begin(); it != amigos.end(); it++){
	//  if((*it)->getType().isWorker()){
	//    amigoDaVez = *it;
	//    break;
	//  }
	// }
	//}//Lembrar que ha varias threads rodando em paralelo. O erro presente neste metodo (qual?) nao resulta em crash do jogo, mas outros poderiam.
}

void zerglingAI (Unidade* u){
	printf("ATAQUE ATAQUE\n");
	//u->attack(*(u->getEnemyUnits().begin())); //ataca uma unidade inimiga aleatoria. Assume que existe uma.
	////cuidado com bugs como este. O codigo acima daria crash de null pointer no exato momento que o time inimigo
	////nao possuisse mais unidades, antes da partida de fato acabar.
}

void poolAI (Unidade* u) {
}

DWORD WINAPI threadMorphing(LPVOID param)
{
	Unidade *u = (Unidade*) param;
	while(!(u->getType() == BWAPI::UnitTypes::Zerg_Drone))
	{
		printf("Still a worker");
	}
	return 0;

}

DWORD WINAPI threadAgente(LPVOID param){

	Unidade* u = (Unidade*) param;
	//exemplos de metodos uteis para construir predios
	bool x = u->hasPower(3,4,50,60);
	u->isBuildable(50,50);
	u->isBuildable(BWAPI::TilePosition(3,5));

	int heig = AgentePrincipal::mapHeight();
	int wid = AgentePrincipal::mapWidth();
	//A classe Agente Principal ainda tem o metodo AgentePrincipal bool isWalkable(int x, int y).

	while(true){
		//Se houve algum problema (ex: o jogo foi fechado) ou a unidade estah morta, finalizar a thread
		if(GameOver || u == NULL || !u->exists()) return 0;
		//Enquanto a unidade ainda nao terminou de ser construida ou o seu comando ainda nao foi
		//processado (ou seja, continua no mesmo turno), aguardar e poupar processamento
		if(!u->isCompleted()){
			Sleep(500);
			continue;
		}
		if(!u->checkNovoTurno()){
			Sleep(10);
			continue;
		}
		//Inserir o codigo de voces a partir daqui//
		if(u->isIdle()){ //nao ta fazendo nada, fazer algo util
			//if(u == amigoDaVez) AIConstrutora(u);
			//else if(u->getType().isWorker()) AITrabalhador(u);
			//else if(u->getType().canProduce()) AICentroComando(u);
			//else AIGuerreiro(u);

			bool xuxuBeleza = u->getType().isWorker();

			if(u->getType().isWorker()) workerAI(u);
			else if (u->getType() == BWAPI::UnitTypes::Zerg_Hatchery) headQuartersAI(u);
			else if (u->getType() == BWAPI::UnitTypes::Zerg_Spawning_Pool){
				if(!isPoolDone){
					isPoolDone = true;
				}
			}
			else if (u->getType() == BWAPI::UnitTypes::Zerg_Zergling) zerglingAI(u);

		}else if(u->getType().isWorker()){
			/*printf("XSCHUPA ARI %d", readyToPool);
			readyToPool++;*/
		}//construir msm q estivesse fazendo algo
		Sleep(10);//Sempre dormir pelo menos 10ms no final do loop, pois uma iteracao da thread é muito mais rápida do que um turno do bwapi.
	}
}

void MeuAgentePrincipal::InicioDePartida(){
	//Inicializar estruturas de dados necessarias, ou outras rotinas de inicio do seu programa. Cuidado com concorrencia,
	//em alguns casos pode ser recomendavel que seja feito antes do while na criacao de uma nova thread.
	GameOver = false;
}

void MeuAgentePrincipal::onEnd(bool isWinner){  
	//sinalizar e aguardar o tempo necessario para todas as threads se terminarem antes do jogo sair, evitando erros.
	GameOver = true;
	Sleep(550);
}

void MeuAgentePrincipal::UnidadeCriada(Unidade* unidade){
	//Uma nova unidade sua foi criada (isto inclui as do inicio da partida). Implemente aqui como quer tratar ela.
	BWAPI::UnitType tipo = unidade->getType();
	std::set<Unidade*> larvas = unidade->getLarva();
	
	if (unidade->getType() == BWAPI::UnitTypes::Zerg_Larva)
	{
		drones++;
		printf("larvas = %d\n",drones);	
		
	}
	
	if(drones == 10) canTrainOverlord = true;

	if (tipo == BWAPI::UnitTypes::Zerg_Spawning_Pool){
		printf("Pool Done");
		isPoolDone = true;
	}

	//Nao desperdicar threads com predios que nao fazem nada
	if(tipo != BWAPI::UnitTypes::Protoss_Pylon && tipo != BWAPI::UnitTypes::Protoss_Assimilator){
		CreateThread(NULL,0,threadAgente,(void*)unidade,0,NULL);
	}
	else{
		printf("XSCHUPA ARI");
		//FODASE
	}
}

/*
Os outros eventos nao existem numa arquitetura orientada a Agentes Autonomos, pois eram relacionados ao Player do Broodwar
de maneira generica, nao sendo especificamente ligados a alguma unidade do jogador. Se desejado, seus comportamentos podem
ser simulados através de técnicas ou estruturas de comunicação dos agentes, como por exemplo Blackboards.
*/