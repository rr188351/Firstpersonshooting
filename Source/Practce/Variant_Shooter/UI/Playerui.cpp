#include "Playerui.h"

void UPlayerui::UpdateScore(int32 InTeam1Score, int32 InTeam2Score, int32 InTeam1Destroyed, int32 InTeam2Destroyed)
{
	Team1Score = InTeam1Score;
	Team2Score = InTeam2Score;
	Team1Destroyed = InTeam1Destroyed;
	Team2Destroyed = InTeam2Destroyed;
}