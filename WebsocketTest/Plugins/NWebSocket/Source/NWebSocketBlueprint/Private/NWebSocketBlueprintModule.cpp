
#include "NWebSocketBlueprintModule.h"
#include "WebSocketsModule.h"
#include "IWebSocket.h"

DEFINE_LOG_CATEGORY(LogNWebSocketBlueprint);

#define LOCTEXT_NAMESPACE "FNWebSocketBlueprintModule"

void FNWebSocketBlueprintModule::StartupModule()
{
	const FName WebSocketsModuleName = TEXT("WebSockets");

	if (!FModuleManager::Get().IsModuleLoaded(WebSocketsModuleName))
	{
		FWebSocketsModule& Module = FModuleManager::LoadModuleChecked<FWebSocketsModule>(WebSocketsModuleName);
	}
}

void FNWebSocketBlueprintModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FNWebSocketBlueprintModule, NWebSocketBlueprint)