#pragma once



class CFileSystem
{
    static std::wstring     repositoryDir;
    static std::wstring     logsDir;
    static std::wstring     appDir;

public:
    CFileSystem();
    ~CFileSystem();

    void SetRepositoryDir(const std::wstring& dir);
    void SetLogsDir(const std::wstring& dir);
    
    std::wstring GetRepositoryDir();
    std::wstring GetLogsDir();
    std::wstring GetAppDir();

};