#include "Healthbar.h"
#include "Components/ProgressBar.h"
			
void UHealthbar::SetHealthPercent(float Percent)
{
	if (HealthProgress)
	{
		HealthProgress->SetPercent(Percent);	
	}
}