// Some copyright should be here...

#pragma once

#include "Modules/ModuleManager.h" // SMODE TECH, Fix UE 4.25

DECLARE_LOG_CATEGORY_EXTERN(SpoutUELog, Log, All);

class FSpoutModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};