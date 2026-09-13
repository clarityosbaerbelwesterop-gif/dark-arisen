#include "UI/AlphaMenuHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
void AAlphaMenuHUD::DrawHUD()
{
    Super::DrawHUD(); if(!Canvas||!GEngine)return;
    UFont* Font=GEngine->GetLargeFont(); if(!Font)return;
    DrawText(TEXT("DARK ARISEN"),FLinearColor::White,96.f,96.f,Font,1.35f,false);
    DrawText(TEXT("ALPHA"),FLinearColor(0.65f,0.65f,0.65f),100.f,156.f,GEngine->GetSmallFont(),1.f,false);
}
