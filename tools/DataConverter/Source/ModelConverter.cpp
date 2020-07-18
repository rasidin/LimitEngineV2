#include "ModelConverter.h"

#include <Managers/ResourceManager.h>
#include <Factories/ModelFactory.h>
#include <Renderer/Model.h>

#include "Logger.h"
#include "Definitions.h"

bool ModelConverter::Convert(const char *InFilename, const char *OutFilename, const ModelConverter::ConvertOptions &Options)
{
    LOG_SUBLOG << "Convert" | InFilename | " -> " | OutFilename;

    LimitEngine::AutoPointer<LimitEngine::ResourceManager::RESOURCE> loadedResource = LimitEngine::ResourceManager::GetSingleton().GetResourceWithoutRegister(InFilename, LimitEngine::ModelFactory::ID);
    if (LimitEngine::Model *model = static_cast<LimitEngine::Model*>(loadedResource->data)) {
        LimitEngine::ResourceManager::GetSingleton().SaveResource(OutFilename, model);
    }

    return true;
}