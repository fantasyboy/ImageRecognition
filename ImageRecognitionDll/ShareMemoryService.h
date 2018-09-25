#pragma once
#include <windows.h>
#include <memory>

#define MAX_IAMGE_SIZE 4*1024*1024
#define GLOBAL_MEMORY_NAME "IAMGE_RECON_DLL"
typedef struct _GAME_IMAGE_DATA
{
	DWORD bInject;
	DWORD nNumberOfBytesToWrite;
	char data[MAX_IAMGE_SIZE];
}GAME_IMAGE_DATA, *PGAME_IMAGE_DATA;


class ShareMemoryService
{
	ShareMemoryService();
	ShareMemoryService(const ShareMemoryService&) = delete;
	void operator=(const ShareMemoryService&) = delete;

	static std::unique_ptr<ShareMemoryService> m_pInstance;
public:
	static ShareMemoryService* GetInstance();
	~ShareMemoryService();

	PGAME_IMAGE_DATA GetSharedMemoryPointer(); //获取共享内存指针
	bool Initialize(); //初始化共享内存
private:
	PGAME_IMAGE_DATA m_pSharedMemory;
	HANDLE m_hMapping;
};

