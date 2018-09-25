#pragma once
#include <memory>

#define MAX_IAMGE_SIZE 4*1024*1024
#define GLOBAL_MEMORY_NAME "IAMGE_RECON_DLL"
typedef struct _GAME_IMAGE_DATA
{
	DWORD bInject;
	DWORD nNumberOfBytesToWrite;
	char data[MAX_IAMGE_SIZE];
}GAME_IMAGE_DATA, *PGAME_IMAGE_DATA;

class ShareMemoryClient
{
	ShareMemoryClient();
	ShareMemoryClient(const ShareMemoryClient&)=delete;
	void operator=(const ShareMemoryClient&) = delete;

	static std::unique_ptr<ShareMemoryClient> m_pInstance;
public:
	static ShareMemoryClient* GetInstance();
	~ShareMemoryClient();

	bool Initialize(); //初始化共享内存
	PGAME_IMAGE_DATA GetSharedMemoryPointer();
private:
	PGAME_IMAGE_DATA m_pSharedMemory;
	HANDLE m_hMapping;
};

