#pragma once
#include <string>

#include "nfd.h"
#include "3DCast/Core/Log.h"

namespace Cast::Util
{
    static std::string OpenFileDialogue(const std::string &defaultPath, const std::string &filterTitle, const std::string &filter)
    {
        nfdu8char_t* outPath;
        const nfdu8filteritem_t filters[1] = {{filterTitle.c_str(), filter.c_str()}};
        nfdopendialogu8args_t args = {nullptr};
        args.filterList = filters;
        args.filterCount = 1;
        args.defaultPath = defaultPath.c_str();
        const nfdresult_t result = NFD_OpenDialogU8_With(&outPath, &args);
        if (result == NFD_OKAY)
        {
            std::string outPathStr(outPath);
            LOG_CORE_TRACE("Selected filesystem item: {0}", outPathStr);
            NFD_FreePathU8(outPath);
            return outPathStr;
        }
        else if (result == NFD_CANCEL)
        {
            LOG_CORE_TRACE("Cancelled file dialogue");
        }
        else
        {
            LOG_CORE_ERROR("Error: {0}", NFD_GetError());
        }

        return "";
    }

    static std::string SaveFileDialogue(const std::string &defaultPath, const std::string &defaultName, const std::string &filterTitle, const std::string &filter)
    {
        nfdu8char_t* outPath;
        const nfdu8filteritem_t filters[1] = {{filterTitle.c_str(), filter.c_str()}};
        nfdsavedialogu8args_t args = {nullptr};
        args.filterList = filters;
        args.filterCount = 1;
        args.defaultPath = defaultPath.c_str();
        args.defaultName = defaultName.c_str();
        const nfdresult_t result = NFD_SaveDialogU8_With(&outPath, &args);

        if (result == NFD_OKAY)
        {
            std::string outPathStr(outPath);
            LOG_CORE_TRACE("File saved to: {0}", outPathStr);
            NFD_FreePathU8(outPath);
            return outPathStr;
        }
        else if (result == NFD_CANCEL)
        {
            LOG_CORE_TRACE("Cancelled save dialogue");
        }
        else
        {
            LOG_CORE_ERROR("Error: {0}", NFD_GetError());
        }

        return "";
    }

    static std::string ExtractFilename(const std::string& path, const bool withExtension = true)
    {
        const size_t found = path.find_last_of("/\\");
        std::string filename = path.substr(found + 1);

        if (!withExtension)
        {
            const size_t dot = filename.find_last_of('.');
            if (dot != std::string::npos)
                filename = filename.substr(0, dot);
        }

        return filename;
    }

    static std::string ExtractDirectory(const std::string& path)
    {
        const size_t found = path.find_last_of("/\\");
        if (found != std::string::npos)
            return path.substr(0, found + 1);
        return "";
    }
}
