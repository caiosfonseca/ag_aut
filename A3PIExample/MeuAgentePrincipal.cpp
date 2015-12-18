#include "MeuAgentePrincipal.h"
#include <AgentePrincipal.h>
#include <Windows.h>
#include <BWAPI\Position.h>
#include <BWAPI\UnitType.h>
#include <stdlib.h>




//blackboard!
bool GameOver = false;
Unidade* amigoDaVez = NULL;
bool isPoolDone = false;
bool readyToPool = true;
bool canBuild10thWorker = true;

bool canEndTheWorld = false;

int zealots=0;


//Using
int probes = 0;
bool firstIteration = true;
bool canMakeFirstPylon = false;
bool secondIteration = false;
bool canMakeFirstGateway = false;
bool canMakeSecondGateway = false;
bool thirdIteration = false;
bool built = false;
bool pylonDone = false;
bool gatDone = false;
bool gatDoneSecond = false;
bool fourthIteration = false;
bool canZealot = false;
bool fifthIteration = false;
bool pylonInfinito = false;
bool topSide = false;
bool lock = false;
std::set<Unidade*> newMimnerals;
bool canAttack = false;


BWAPI::Position* p;
BWAPI::Position nexusPos;
int steps = 0;
bool end = false;
BWAPI::Position enembyBase;
std::set<BWAPI::Position> newPositions;
std::set<Unidade*> squad;





void scoutAI(Unidade* u){
	
	//u->move(BWAPI::Position(2048,2048));
	
	//printf("I AM THE SCOUT\n");
	
	BWAPI::Position np;
	
		int x = rand()%70;
		int y = rand()%70;
		BWAPI::Position pp = u->getPosition();
		if(steps >= 50)
		{
			//steps = 0;
			u->move(nexusPos);
			end = true;
			printf("Steps end\n");
			return;
		}
		
		if(topSide){
			if(BWAPI::Position(u->getPosition().x() + x + 70,u->getPosition().y() + y + 20).isValid()){
				np = BWAPI::Position(u->getPosition().x() + x + 70,u->getPosition().y() + y + 20);
			}else{
				u->move(nexusPos);
				return;
			}
				
			
			if(u->getPosition().hasPath(np) ){
				u->move(np);
				Sleep(500);
				steps++;
				
			}else{
				u->move(BWAPI::Position(u->getPosition().x()+ 70,u->getPosition().y() - 20));
				steps++;
				
			}
			std::set<Unidade*> minerais = u->getMinerals();
			for(std::set<Unidade*>::iterator it = minerais.begin(); it != minerais.end(); it++){
				newMimnerals.insert((*it));
				newPositions.insert((*it)->getPosition());
				for(std::set<BWAPI::Position>::iterator it = newPositions.begin(); it != newPositions.end(); it++){
					printf("Mineral position (%d,%d)\n", it->x(),it->y());
				}	
				
					
			}



			if(u->getTilePosition().y()> 50){
				u->move(nexusPos);
				Sleep(1000);
				steps = 0;
				return;
			}
		}else{
			if(BWAPI::Position(u->getPosition().x() - x - 100,u->getPosition().y() - y - 20).isValid())
				np = BWAPI::Position(u->getPosition().x() - x - 100,u->getPosition().y() - y - 20);
			else{
				u->move(nexusPos);
				return;
			}
			if(u->getPosition().hasPath(np)){
				u->move(np);
				steps++;
				Sleep(500);
			}else{
				u->move(BWAPI::Position(u->getPosition().x()- 120,u->getPosition().y() ));
				steps++;
			}
			
			std::set<Unidade*> minerais = u->getMinerals();
			for(std::set<Unidade*>::iterator it = minerais.begin(); it != minerais.end(); it++){
				newPositions.insert((*it)->getPosition());
				for(std::set<BWAPI::Position>::iterator it = newPositions.begin(); it != newPositions.end(); it++){
					printf("Mineral position (%d,%d)\n", it->x(),it->y());
				}	
				
					
			}
			
			if(u->getTilePosition().y()< 18){
				u->move(nexusPos);
				Sleep(1000);
				steps= 0;
				return;
			}
		}

}

int workerAI (Unidade* u,int choice){
	double distance = 99999;
	Unidade* mineralPerto = NULL;
	std::set<Unidade*> minerais = u->getMinerals();
	for(std::set<Unidade*>::iterator it = minerais.begin(); it != minerais.end(); it++){
		if(u->getDistance(*it) < distance){
			distance = u->getDistance(*it);
			mineralPerto = *it;
			newMimnerals.insert(*it);
		}
	}
	if(mineralPerto != NULL) u->rightClick(mineralPerto);
	else
	{
		//printf("Minerals novos\n");
		int dsitance = 99999;
		BWAPI::Position mineralPerto =  BWAPI::Position(377, 999);
		if( !newPositions.empty()){
			std::set<BWAPI::Position>::iterator it = newPositions.begin();
			std::advance(it, choice);
			mineralPerto = *it;
		}
		std::set<Unidade*> minerais = u->getMinerals();
		if(mineralPerto.isValid())
		{
			//u->move(mineralPerto->getPosition());
			printf("Pegando mineral na posicao (%d,%d)\n",mineralPerto.x(),mineralPerto.y());
			
			u->move(mineralPerto);
			printf("choice = %d\n",choice); 
			if( !(newPositions.size() == choice + 1)) 
				choice++;
			else if(newPositions.size() == choice - 1){
				u->attack(enembyBase);
			}
			//newPositions.erase(mineralPerto);


		}else
		{
			
			printf("Tamanho da lista = %d\n",newMimnerals.size());
			u->rightClick(mineralPerto);
		}
		
	
	}

	return choice;
}

void odebrechtAI(Unidade* u){
	//printf("I AM THE ODEBRECH\n");
	while(true){
		BWAPI::TilePosition tp = u->getTilePosition();
		if (u->getType().isWorker()){
			
			std::set<Unidade*> units = u->getAllyUnits();

			for(std::set<Unidade*>::iterator it = units.begin(); it != units.end(); it++){
				if((*it)->getType() == BWAPI::UnitTypes::Protoss_Nexus && canMakeFirstPylon){
					tp = (*it)->getTilePosition();
					break;
				}

				else if((*it)->getType() == BWAPI::UnitTypes::Protoss_Pylon && !canMakeFirstPylon){
					tp = (*it)->getTilePosition();
					break;
				}
			}

			int limite = 0;
			int adi = 6;
			
			if (canMakeFirstPylon){
				
				//tp = u->getTilePosition();
				while(!(u)->build(tp, BWAPI::UnitTypes::Protoss_Pylon)){
					if(((u->minerals()&0xF)>>2) < 2)
						tp = BWAPI::TilePosition(tp.x(), tp.y()+adi);
					else
						tp = BWAPI::TilePosition(tp.x()+adi, tp.y());
					tp.makeValid();
					limite++;
					if(limite > 50){
						//printf("limite\n");
						break;
					}
					adi = -adi + (adi > 0 ? -2 : +2);
					//printf("FODAM- SE \n");
					
				}
				//printf("puto safado\n");
				
				
				//printf("ola cara\n");
				break;
				

				

			} else if (canMakeFirstGateway){
				//printf("gateway");
				
				while(!(u)->build(tp, BWAPI::UnitTypes::Protoss_Gateway)){
					if(((u->minerals()&0xF)>>2) < 2)
						tp = BWAPI::TilePosition(tp.x(), tp.y()+adi);
					else
						tp = BWAPI::TilePosition(tp.x()+adi, tp.y());
					tp.makeValid();
					limite++;
					if(limite > 50){
						//printf("limite\n");
						break;
					}
					adi = -adi + (adi > 0 ? -2 : +2);
					
				}
				break;
			} else if (canMakeSecondGateway){
				//printf("second gateway\n");
				
				while(!(u)->build(tp, BWAPI::UnitTypes::Protoss_Gateway)){
					if(((u->minerals()&0xF)>>2) < 2)
						tp = BWAPI::TilePosition(tp.x(), tp.y()+adi);
					else
						tp = BWAPI::TilePosition(tp.x()+adi, tp.y());
					tp.makeValid();
					limite++;
					if(limite > 50){
						//printf("limite\n");
						break;
					}
					adi = -adi + (adi > 0 ? -2 : +2);
					
				}
				break;
			}else {
				//printf("OI\n");
				while(!(u)->build(tp, BWAPI::UnitTypes::Protoss_Pylon)){
						if(((u->minerals()&0xF)>>2) < 2)
							tp = BWAPI::TilePosition(tp.x(), tp.y()+adi);
						else
							tp = BWAPI::TilePosition(tp.x()+adi, tp.y());
						tp.makeValid();
						limite++;
						if(limite > 50){
						//	printf("limite\n");
							break;
						}
						adi = -adi + (adi > 0 ? -2 : +2);
					
				}
				break;
			
			}
			//printf("Fora\n");
		}else{
			printf("BREAK\n");
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
		if (u->supplyUsed() >= 16 && !isPoolDone && u->minerals() >= 200){
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
					//canTrainOverlord = true;
					Sleep(30);
					while((*it)->isMorphing()) printf("i am still morphng\n");
					if((*it)->getType() == BWAPI::UnitTypes::Zerg_Drone)
					{
						printf("I am now a Drone\n");
					}else if((*it)->getType() == BWAPI::UnitTypes::Zerg_Larva)printf("I am still a larva, how come?\n");
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
			printf("I am leaving 10th\n");
		}
	}else if (0){
		std::set<Unidade*> units = u->getLarva();
		std::set<Unidade*>::iterator it = units.begin();

		if ((*it)->morph(BWAPI::UnitTypes::Zerg_Overlord)){
			Sleep(500);
			while ((*it)->isMorphing() ){
				printf("morphing to Overlord\n");
			}
			printf("Done Overlord");
			//canTrainOverlord = false;
			canEndTheWorld = true;
		}
	}else if(canEndTheWorld){

		u->train(BWAPI::UnitTypes::Zerg_Zergling);
		
		canEndTheWorld = false;
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

void zealotAI (Unidade* u){
	//printf("ATAQUE ATAQUE\n");
	if(topSide){
		
		u->move(enembyBase);
		Unidade* enemy;
		if(!u->getEnemyUnits().empty()){
			u->attack(*(u->getEnemyUnits().begin()));
			
		}else{ 
			int min = -30;
			int max = 30;
			int output = min + (rand() % (int)(max - min + 1));
			if(u->getPosition()== enembyBase){
				u->move(BWAPI::Position(u->getPosition().x() + output, u->getPosition().y() + output));
			}
			return;
		}
		/*
		BWAPI::TilePosition tempTile = u->getTilePosition();
		int dis = u->getDistance(enembyBase)/5;
		BWAPI::Position nextPos = BWAPI::Position(u->getPosition().x()+dis,u->getPosition().y() + dis);
		
		if(u->hasPath(nextPos )  ){
			u->move(nextPos );
			Sleep(500);
		}
		else{
			if( !u->getEnemyUnits().empty()){
				std::set<Unidade*> units = u->getEnemyUnits();
				for(std::set<Unidade*>::iterator it = units.begin(); it != units.end(); it++){
					if((*it)->getType() == BWAPI::UnitTypes::Protoss_Zealot){
						u->attack((*it));
						break;
					}else if((*it)->getType() == BWAPI::UnitTypes::Protoss_Pylon){
						u->attack((*it));
						break;
					}else {
						u->attack((*it));
					}
					
				}
			}
		}
		*/
	}else{
		
		u->move(enembyBase);
		Unidade* enemy;
		
		if(!u->getEnemyUnits().empty())
			u->attack(*(u->getEnemyUnits().begin()));
		else{
			int min = -30;
			int max = 30;
			int output = min + (rand() % (int)(max - min + 1));
			u->move(BWAPI::Position(u->getPosition().x() + output, u->getPosition().y() + output));
			return;
		}
	}
	//u->attack(*(u->getEnemyUnits().begin())); //ataca uma unidade inimiga aleatoria. Assume que existe uma.
	////cuidado com bugs como este. O codigo acima daria crash de null pointer no exato momento que o time inimigo
	////nao possuisse mais unidades, antes da partida de fato acabar.
}

void callODB(Unidade *u) {
	std::set<Unidade*> units = u->getAllyUnits();
	for(std::set<Unidade*>::iterator it = units.begin(); it != units.end(); it++){
		if((*it)->getType() == BWAPI::UnitTypes::Protoss_Probe){
			odebrechtAI(*it);
			break;
		}
	}
}




/*
void squads(Unidade* u){
	std::set<Unidade*> myZealots = u->getAllyUnits();
	for(std::set<Unidade*>::iterator it = myZealots.begin(); it!=myZealots.end(); it++){
		if((*it)->getType() == BWAPI::UnitTypes::Protoss_Zealot){
			squad.insert(*it);
		}
	}

}
*/

void trainZealot(Unidade *u)
{
	if(canZealot)
		u->train(BWAPI::UnitTypes::Protoss_Zealot);
}







void nexusAI (Unidade* u) {
	if (u->minerals() < 50){
		return;
	}

	if (firstIteration) {
		int currentSupply = u->supplyUsed();
		while (firstIteration && currentSupply < 16) {
			u->train(BWAPI::UnitTypes::Protoss_Probe);
			currentSupply = u->supplyUsed();
		}
		firstIteration = false;
		canMakeFirstPylon = true;
	}

	if (canMakeFirstPylon && u->minerals() >= 100 && !pylonDone) {
		callODB(u);
	}
	//printf("Second iteration %d\n", secondIteration);
	if(secondIteration){
		u->train(BWAPI::UnitTypes::Protoss_Probe);
		secondIteration = false;
		canMakeFirstGateway = true;
		
	}
	
	if(canMakeFirstGateway && u->minerals() >= 160 && !gatDone){
		callODB(u);
	}

	if(thirdIteration)
	{
		u->train(BWAPI::UnitTypes::Protoss_Probe);
		canMakeSecondGateway = true;
		thirdIteration = false;
	}

	if(canMakeSecondGateway && u->minerals() >= 160 && !gatDoneSecond){
		callODB(u);
	}

	if(fourthIteration)
	{
		u->train(BWAPI::UnitTypes::Protoss_Probe);
		canZealot = true;
		fourthIteration = false;
		fifthIteration = true;
		
	}

	if(fifthIteration)
	{
		if((u->supplyTotal() - u->supplyUsed() ) < 8){
				callODB(u);
		}
		int z = 0;
		std::set<Unidade*> units = u->getAllyUnits();
		for(std::set<Unidade*>::iterator it = units.begin(); it != units.end(); it++){
			if((*it)->getType() == BWAPI::UnitTypes::Protoss_Zealot){
				z++;
			}
		}
	
		if(z>=3)
			canAttack = true;
		Sleep(100);


	}




		
	
}

void zealotPatrol(Unidade* u){
	std::set<Unidade*> units = u->getEnemyUnits();
	if(!units.empty()){
		for(std::set<Unidade*>::iterator it = units.begin(); it != units.end(); it++){
		u->attack(*it);
	}
	}
	
	zealotAI(u);

}

int retDistance(Unidade* u){
	int distance = 0;
	std::set<Unidade*> units = u->getAllyUnits();
	for(std::set<Unidade*>::iterator it = units.begin(); it != units.end(); it++){
		if((*it)->getType() == BWAPI::UnitTypes::Protoss_Nexus){
			distance = u->getDistance((*it));
			break;
		}
	}

	return distance;

}

DWORD WINAPI threadAgente(LPVOID param){

	Unidade* u = (Unidade*) param;
	//exemplos de metodos uteis para construir predios
	bool x = u->hasPower(3,4,50,60);
	u->isBuildable(50,50);
	u->isBuildable(BWAPI::TilePosition(3,5));
	//int distance = retDistance(u)/5;
	
	int heig = AgentePrincipal::mapHeight();
	int wid = AgentePrincipal::mapWidth();
	//A classe Agente Principal ainda tem o metodo AgentePrincipal bool isWalkable(int x, int y).
	bool scout = false;
	int choice = 0;
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

			
			if(u->getType().isWorker()){
				
				
				if(lock || scout){
					lock = false;
					scout = true;
					if(end){
						scout = false;
						steps = 0;
					}else 
						scoutAI(u);
				}else{
					choice = workerAI(u,choice);
					
				}
					
			} 
			else if (u->getType() == BWAPI::UnitTypes::Protoss_Nexus) nexusAI(u);
			else if (u->getType() == BWAPI::UnitTypes::Protoss_Gateway) trainZealot(u);
			//else if (u->getType() == BWAPI::UnitTypes::Protoss_Gateway) headQuartersAI(u);
			else if (u->getType() == BWAPI::UnitTypes::Protoss_Zealot) zealotPatrol(u);
		}
		Sleep(10);
		
	}
}

void MeuAgentePrincipal::InicioDePartida(){
	//Inicializar estruturas de dados necessarias, ou outras rotinas de inicio do seu programa. Cuidado com concorrencia,
	//em alguns casos pode ser recomendavel que seja feito antes do while na criacao de uma nova thread.
	GameOver = false;
	p = new BWAPI::Position();
}

void MeuAgentePrincipal::onEnd(bool isWinner){  
	//sinalizar e aguardar o tempo necessario para todas as threads se terminarem antes do jogo sair, evitando erros.
	GameOver = true;
	Sleep(550);
}

void MeuAgentePrincipal::UnidadeCriada(Unidade* unidade){
	//Uma nova unidade sua foi criada (isto inclui as do inicio da partida). Implemente aqui como quer tratar ela.
	BWAPI::UnitType tipo = unidade->getType();
	
	if (unidade->getType() == BWAPI::UnitTypes::Protoss_Probe) {
		probes++;
		printf("Probes = %d\n",probes);	
		if(probes == 10){
			lock = true;

		}

		
	}
	
	

	if (unidade->getType() == BWAPI::UnitTypes::Protoss_Pylon) {
		if(pylonInfinito) {
			printf("Pylon infinito\n");
		}else {
			pylonDone = true;
			secondIteration = true;
			canMakeFirstPylon = false;
			pylonInfinito = true;
		}
	}

	if (unidade->getType() == BWAPI::UnitTypes::Protoss_Gateway) {
		
		if(!canMakeSecondGateway)
		{
			printf("First Pylon completed!\n");
			gatDone = true;
			thirdIteration = true;
			canMakeFirstGateway = false;
		}else
		{
			printf("Second Pylon completed!\n");
			gatDoneSecond = true;
			fourthIteration = true;
			canMakeSecondGateway = false;
		
		}
		
		
	}
	
	
	if(tipo == BWAPI::UnitTypes::Protoss_Zealot) {
		printf("I am a zealot.\n");
		zealots++;
	}

	if (tipo == BWAPI::UnitTypes::Protoss_Pylon){
		if (canMakeFirstPylon){
			printf("First Pylon completed!");
		}		
	}


	if (unidade->getType() == BWAPI::UnitTypes::Protoss_Nexus) {
		printf("(%d,%d)\n",unidade->getPosition().x(),unidade->getPosition().y() );
		printf("(%d,%d)\n", unidade->getTilePosition().x(),unidade->getTilePosition().y());
		nexusPos = unidade->getPosition();
		
		if(unidade->getTilePosition().y() < 32){
			topSide = true;
			enembyBase = BWAPI::Position(BWAPI::TilePosition(62,62));
		}
		else{
			topSide = false;
			enembyBase = BWAPI::Position(320,320);
		}
	}

	//Nao desperdicar threads com predios que nao fazem nada
	if(tipo != BWAPI::UnitTypes::Protoss_Pylon && tipo != BWAPI::UnitTypes::Protoss_Assimilator){
		CreateThread(NULL,0,threadAgente,(void*)unidade,0,NULL);
	}
	else{
		printf("SCHUPA ARI\n");
		//FODA-SE
	}
}