/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  ResourceLoader_Windows.cpp
 @brief Resource Loader (Windows)
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/

#ifdef WINDOWS
#include <stdio.h>		// for FILE

#include "Core/Util.h"

namespace LimitEngine {
    class ResourceLoader_Windows : public ResourceLoader
    {
    public:
        ResourceLoader_Windows() {}
        virtual ~ResourceLoader_Windows() {}

        bool IsExist(const char *filename)
        {
            FILE *fp = NULL;
            fopen_s(&fp, filename, "rb");
            bool result = (fp != NULL);
            if (result) {
                fclose(fp);
            }
            return result;
        }
        void* GetResource(const char *filename, size_t *size)
        {
            void *output = NULL;
            FILE *fp = nullptr;
            fopen_s(&fp, filename, "rb");

            if (fp)
            {
                long start_pos = fseek(fp, 0, SEEK_END);
                fpos_t end_pos = 0;
                fgetpos(fp, &end_pos);
                *size = size_t(end_pos) + 1;
                fseek(fp, start_pos, SEEK_SET);
                output = malloc(*size);
                ::memset(output, 0, *size);
                fread(output, *size, 1, fp);
                fclose(fp);
            }

            return output;
        }
    };
}
#endif