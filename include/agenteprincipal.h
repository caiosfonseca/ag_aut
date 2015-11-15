#pragma once

#include <Windows.h>
#include <vector>
#include "Unidade.h"

class AgentePrincipal{
public:
	
	static BOOL DllMain(DWORD  ul_reason_for_call);

	static void *NewAiModule(void* game);

	static void IniciarEmModoTeste();

	static void DormirAteFinalDoTurno();
    
    
    static int mapHeight();
	
	static int mapWidth();

	static bool isWalkable(int x, int y);
    
    

	virtual void onEnd(bool isWinner);
	
	virtual void InicioDePartida();

	virtual void UnidadeCriada(Unidade* unidade);

 
};