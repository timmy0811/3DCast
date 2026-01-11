#pragma once
#include <filesystem>
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

    static std::string FindTextureInDirectory(const std::string& directory, const std::string& filename) {
        // Check if file exists as-is
        std::string fullPath = directory + "/" + filename;
        if (std::filesystem::exists(fullPath)) {
            return fullPath;
        }

        // If no extension, try common texture extensions
        if (filename.find('.') == std::string::npos) {
            const std::vector<std::string> extensions = {".png", ".jpg", ".jpeg", ".tga", ".bmp", ".dds"};
            for (const auto& ext : extensions) {
                std::string pathWithExt = directory + "/" + filename + ext;
                if (std::filesystem::exists(pathWithExt)) {
                    return pathWithExt;
                }
            }
        }

        // Try case-insensitive search
        if (std::filesystem::exists(directory) && std::filesystem::is_directory(directory)) {
            for (const auto& entry : std::filesystem::directory_iterator(directory)) {
                std::string entryName = entry.path().filename().string();
                if (strcasecmp(entryName.c_str(), filename.c_str()) == 0) {
                    return entry.path().string();
                }
            }
        }

        return "";
    }

    static std::string FindTexturePath(const std::string& basePath, const std::string& textureName, bool search = false) {
        std::string filename = textureName;
        const size_t lastSeparator = textureName.find_last_of("/\\");
        if (lastSeparator != std::string::npos) {
            filename = textureName.substr(lastSeparator + 1);
        }

        std::string result = FindTextureInDirectory(basePath, filename);
        if (!result.empty()) {
            return result;
        }

        if (search) {
            std::filesystem::path parentPath = std::filesystem::path(basePath).parent_path();
            std::string texturesDir = (parentPath / "Textures").string();
            result = FindTextureInDirectory(texturesDir, filename);
            if (!result.empty()) {
                return result;
            }
        }

        return basePath + "/" + filename;
    }
}
