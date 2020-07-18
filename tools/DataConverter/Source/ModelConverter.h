#pragma once

class ModelConverter
{
public:
    struct ConvertOptions
    {
    };

public:
    ModelConverter() {}
    ~ModelConverter() {}

    bool Convert(const char *InFilename, const char *OutFilename, const ConvertOptions &Options);
};