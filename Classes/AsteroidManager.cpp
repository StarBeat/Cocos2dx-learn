#include "AsteroidManager.h"

AsteroidManager* ISingle< AsteroidManager>::instance = new AsteroidManager();

AsteroidManager::AsteroidManager()
{
}

AsteroidManager::~AsteroidManager()
{
}
