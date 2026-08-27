// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Interaction/DarkArisenInteractable.h"

#include "CoreLoopTuning.h"

bool IDarkArisenInteractable::CanInteract_Implementation(AActor* Interactor) const
{
    return Interactor != nullptr;
}

EInteractionClass IDarkArisenInteractable::GetInteractionClass_Implementation() const
{
    return EInteractionClass::Take;
}

float IDarkArisenInteractable::GetInteractionDuration_Implementation() const
{
    return DarkArisen::CoreLoopTuning::DefaultTakeSeconds;
}

FText IDarkArisenInteractable::GetInteractionLabel_Implementation() const
{
    return NSLOCTEXT("DarkArisenInteraction", "DefaultVerb", "Interact");
}

void IDarkArisenInteractable::BeginInteraction_Implementation(AActor* /*Interactor*/)
{
}

void IDarkArisenInteractable::CompleteInteraction_Implementation(AActor* /*Interactor*/)
{
}

void IDarkArisenInteractable::CancelInteraction_Implementation(AActor* /*Interactor*/)
{
}
