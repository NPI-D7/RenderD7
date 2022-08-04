#include <renderd7/FileSystem.hpp>
#include <3ds.h>
#include <cstring>
//Debugging
#include <memory>
#include <renderd7/Ovl.hpp>
#include <renderd7/Toast.hpp>

const char* RenderD7::FileSystem::GetPhysfsError()
{
    return PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
}

std::string RenderD7::FileSystem::Normalize(const std::string& input)
{
    std::string out;
    bool seenSep = false, isSep = false;

    for (size_t i = 0; i < input.size(); ++i)
    {
        isSep = (input[i] == '/');

        if (!isSep || !seenSep)
            out += input[i];

        seenSep = isSep;
    }

    return out;
}

void RenderD7::FileSystem::Initialize()
{
    RenderD7::FileSystem::savePath = "";
}

int RenderD7::FileSystem::Init(const char* argv)
{
    int res = PHYSFS_init(argv);
    if (res != 1)
    {
        RenderD7::AddOvl(std::make_unique<RenderD7::Toast>("PHYSFS-Error", RenderD7::FileSystem::GetPhysfsError()));
    }
    
    return res;
}

bool RenderD7::FileSystem::SetSource(const char* source)
{
    if (!PHYSFS_isInit())
        RenderD7::AddOvl(std::make_unique<RenderD7::Toast>("PHYSFS-Error", RenderD7::FileSystem::GetPhysfsError()));
        return false;

    std::string searchPath = source;
    if (!PHYSFS_mount(searchPath.c_str(), NULL, 1))
        RenderD7::AddOvl(std::make_unique<RenderD7::Toast>("PHYSFS-Error", RenderD7::FileSystem::GetPhysfsError()));
        return false;

    return true;
}

bool RenderD7::FileSystem::SetIdentity(const char* name, bool append)
{
    if (!PHYSFS_isInit())
        RenderD7::AddOvl(std::make_unique<RenderD7::Toast>("PHYSFS-Error", RenderD7::FileSystem::GetPhysfsError()));
        return false;

    std::string old = RenderD7::FileSystem::savePath;

    RenderD7::FileSystem::savePath = RenderD7::FileSystem::Normalize(RenderD7::FileSystem::GetUserDirectory() + "/save/" + name);
    printf("Save Path set to %s\n", savePath.c_str());

    if (!old.empty())
        PHYSFS_unmount(old.c_str());

    int success = PHYSFS_mount(savePath.c_str(), NULL, append);
    printf("Save Path mounted %d\n", success);

    PHYSFS_setWriteDir(nullptr);

    return true;
}

std::string RenderD7::FileSystem::GetSaveDirectory()
{
    return RenderD7::FileSystem::Normalize(RenderD7::FileSystem::GetUserDirectory() + "/save");
}

bool RenderD7::FileSystem::SetupWriteDirectory()
{
    if (!PHYSFS_isInit())
        RenderD7::AddOvl(std::make_unique<RenderD7::Toast>("PHYSFS-Error", RenderD7::FileSystem::GetPhysfsError()));
        return false;

    if (RenderD7::FileSystem::savePath.empty())
        RenderD7::AddOvl(std::make_unique<RenderD7::Toast>("PHYSFS-Error", "Path is Empty"));
        return false;

    std::string tmpWritePath     = RenderD7::FileSystem::savePath;
    std::string tmpDirectoryPath = RenderD7::FileSystem::savePath;

    if (RenderD7::FileSystem::savePath.find(RenderD7::FileSystem::GetUserDirectory()) == 0)
    {
        tmpWritePath     = RenderD7::FileSystem::GetUserDirectory();
        tmpDirectoryPath = savePath.substr(RenderD7::FileSystem::GetUserDirectory().length());

        /* strip leading '/' characters from the path we want to create */
        size_t startPosition = tmpDirectoryPath.find_first_not_of('/');

        if (startPosition != std::string::npos)
            tmpDirectoryPath = tmpDirectoryPath.substr(startPosition);
    }

    if (!PHYSFS_setWriteDir(tmpWritePath.c_str()))
    {
        printf("Failed to set write dir to %s\n", tmpWritePath.c_str());
        RenderD7::AddOvl(std::make_unique<RenderD7::Toast>("PHYSFS-Error", RenderD7::FormatString("Failed to set write dir to %s\n", tmpWritePath.c_str())));
        return false;
    }

    if (!RenderD7::FileSystem::CreateDirectory(tmpDirectoryPath.c_str()))
    {
        printf("Failed to create dir %s\n", tmpDirectoryPath.c_str());
        /* clear the write directory in case of error */
        RenderD7::AddOvl(std::make_unique<RenderD7::Toast>("PHYSFS-Error", RenderD7::FormatString("Failed to create dir %s\n", tmpDirectoryPath.c_str())));
        PHYSFS_setWriteDir(nullptr);
        return false;
    }

    if (!PHYSFS_setWriteDir(savePath.c_str()))
    {
        printf("Failed to set write dir to %s\n", savePath.c_str());
        RenderD7::AddOvl(std::make_unique<RenderD7::Toast>("PHYSFS-Error", RenderD7::FormatString("Failed to set write dir to %s\n", savePath.c_str())));
        return false;
    }

    if (!PHYSFS_mount(savePath.c_str(), nullptr, 0))
    {
        printf("Failed to mount write dir (%s)\n", RenderD7::FileSystem::GetPhysfsError());
        RenderD7::AddOvl(std::make_unique<RenderD7::Toast>("PHYSFS-Error", RenderD7::FormatString("Failed to mount write dir (%s)\n", RenderD7::FileSystem::GetPhysfsError())));
        /* clear the write directory in case of error */
        PHYSFS_setWriteDir(nullptr);
        return false;
    }

    return true;
}

std::string RenderD7::FileSystem::GetUserDirectory()
{
    return RenderD7::FileSystem::Normalize(PHYSFS_getPrefDir("npi-d7", "renderd7"));
}

bool RenderD7::FileSystem::GetInfo(const char* filename, RenderD7::FileSystem::Info& info)
{
    if (!PHYSFS_isInit())
        RenderD7::AddOvl(std::make_unique<RenderD7::Toast>("PHYSFS-Error", RenderD7::FileSystem::GetPhysfsError()));
        return false;

    PHYSFS_Stat stat = {};

    if (!PHYSFS_stat(filename, &stat))
        RenderD7::AddOvl(std::make_unique<RenderD7::Toast>("PHYSFS-Error", RenderD7::FileSystem::GetPhysfsError()));
        return false;

    info.mod_time = std::min<int64_t>(stat.modtime, RenderD7::FileSystem::MAX_STAMP);
    info.size     = std::min<int64_t>(stat.filesize, RenderD7::FileSystem::MAX_STAMP);

    if (stat.filetype == PHYSFS_FILETYPE_REGULAR)
        info.type = RenderD7::FileSystem::FileType_File;
    else if (stat.filetype == PHYSFS_FILETYPE_DIRECTORY)
        info.type = RenderD7::FileSystem::FileType_Directory;
    else if (stat.filetype == PHYSFS_FILETYPE_SYMLINK)
        info.type = RenderD7::FileSystem::FileType_SymLink;
    else
        info.type = RenderD7::FileSystem::FileType_Other;

    return true;
}

void RenderD7::FileSystem::GetDirectoryItems(const char* path, std::vector<std::string>& items)
{
    if (!PHYSFS_isInit())
        RenderD7::AddOvl(std::make_unique<RenderD7::Toast>("PHYSFS-Error", RenderD7::FileSystem::GetPhysfsError()));
        return;

    char** results = PHYSFS_enumerateFiles(path);

    if (results == nullptr)
        RenderD7::AddOvl(std::make_unique<RenderD7::Toast>("PHYSFS-Error", RenderD7::FileSystem::GetPhysfsError()));
        return;

    for (char** item = results; *item != 0; item++)
        items.push_back(*item);

    PHYSFS_freeList(results);
}

bool RenderD7::FileSystem::OpenFile(File& file, const char* name, FileMode mode)
{
    if (mode == FileMode_Closed)
        RenderD7::AddOvl(std::make_unique<RenderD7::Toast>("PHYSFS-Error", RenderD7::FileSystem::GetPhysfsError()));
        return false;

    if (!PHYSFS_isInit())
        RenderD7::AddOvl(std::make_unique<RenderD7::Toast>("PHYSFS-Error", RenderD7::FileSystem::GetPhysfsError()));
        return false;

    if (file.handle)
        RenderD7::FileSystem::CloseFile(file);

    if (mode == FileMode_Read && !PHYSFS_exists(name))
    {
        printf("Could not open file %s, does not exist.\n", name);
        RenderD7::AddOvl(std::make_unique<RenderD7::Toast>("PHYSFS-Error", RenderD7::FormatString("Could not open file %s, does not exist.\n", name)));
        return false;
    }

    if ((mode == FileMode_Write) &&
        (PHYSFS_getWriteDir() == nullptr && RenderD7::FileSystem::SetupWriteDirectory()))
    {
        printf("Could not set write directory.\n");
        RenderD7::AddOvl(std::make_unique<RenderD7::Toast>("PHYSFS-Error", RenderD7::FormatString("Could not set write directory.\n")));
        return false;
    }

    PHYSFS_getLastErrorCode();

    switch (mode)
    {
        case FileMode_Read:
            file.handle = PHYSFS_openRead(name);
            break;
        case FileMode_Write:
            file.handle = PHYSFS_openWrite(name);
            break;
        default:
            break;
    }

    if (!file.handle)
    {
        const char* error = RenderD7::FileSystem::GetPhysfsError();

        if (error == nullptr)
            error = "unknown error";

        printf("Could not open file %s (%s)\n", name, error);
        RenderD7::AddOvl(std::make_unique<RenderD7::Toast>("PHYSFS-Error", RenderD7::FormatString("Could not open file %s (%s)\n", name, error)));

        return false;
    }

    file.mode = mode;

    return true;
}

bool RenderD7::FileSystem::CloseFile(File& file)
{
    if (file.handle == nullptr || !PHYSFS_close(file.handle))
        return false;

    file.handle = nullptr;

    return true;
}

bool RenderD7::FileSystem::CreateDirectory(const char* name)
{
    if (!PHYSFS_isInit())
        RenderD7::AddOvl(std::make_unique<RenderD7::Toast>("PHYSFS-Error", RenderD7::FileSystem::GetPhysfsError()));
        return false;

    if (PHYSFS_getWriteDir() == nullptr && !RenderD7::FileSystem::SetupWriteDirectory())
        RenderD7::AddOvl(std::make_unique<RenderD7::Toast>("PHYSFS-Error", RenderD7::FileSystem::GetPhysfsError()));
        return false;

    if (!PHYSFS_mkdir(name))
        RenderD7::AddOvl(std::make_unique<RenderD7::Toast>("PHYSFS-Error", RenderD7::FileSystem::GetPhysfsError()));
        return false;

    return true;
}

int64_t RenderD7::FileSystem::ReadFile(File& file, void* destination, int64_t size)
{
    if (!file.handle || file.mode != FileMode_Read)
    {
        printf("File is not opened for reading.\n");
        RenderD7::AddOvl(std::make_unique<RenderD7::Toast>("PHYSFS-Error", "File is not opened for reading.\n"));
        return 0;
    }

    if (size > file.GetSize())
        size = file.GetSize();
    else if (size < 0)
    {
        printf("Invalid read size %lld\n", size);
        RenderD7::AddOvl(std::make_unique<RenderD7::Toast>("PHYSFS-Error", RenderD7::FormatString("Invalid read size %lld\n", size)));
        return 0;
    }

    return PHYSFS_readBytes(file.handle, destination, (PHYSFS_uint64)size);
}

bool RenderD7::FileSystem::WriteFile(File& file, const void* data, int64_t size)
{
    if (!file.handle || file.mode != FileMode_Write)
    {
        printf("File is not opened for writing.\n");
        RenderD7::AddOvl(std::make_unique<RenderD7::Toast>("PHYSFS-Error", "File is not opened for writing.\n"));
        return false;
    }

    int64_t written = PHYSFS_writeBytes(file.handle, data, (PHYSFS_uint64)size);

    if (written != size){
        RenderD7::AddOvl(std::make_unique<RenderD7::Toast>("PHYSFS-Error", RenderD7::FileSystem::GetPhysfsError()));
        return false;
    }

    return true;
}