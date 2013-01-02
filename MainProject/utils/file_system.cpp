#include "stdafx.h"
#include "file_system.h"

std::wstring CFileSystem::repositoryDir;
std::wstring CFileSystem::logsDir;
std::wstring CFileSystem::appDir;

CFileSystem::CFileSystem()
{
    wchar        szAppPath[MAX_PATH] = L"";

    ::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);

    appDir = szAppPath;
    appDir = appDir.substr(0, appDir.rfind(L"\\"));

}

CFileSystem::~CFileSystem()
{

}    


void CFileSystem::SetRepositoryDir(const std::wstring& dir)
{
    repositoryDir = dir;
}

void CFileSystem::SetLogsDir(const std::wstring& dir)
{
    logsDir = dir;
}

std::wstring CFileSystem::GetRepositoryDir()
{
    return repositoryDir;
}

std::wstring CFileSystem::GetLogsDir()
{
    return logsDir;
}

std::wstring CFileSystem::GetAppDir()
{
    return appDir;
}