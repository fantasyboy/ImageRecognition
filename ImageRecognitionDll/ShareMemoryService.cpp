#include "ShareMemoryService.h"

std::unique_ptr<ShareMemoryService> ShareMemoryService::m_pInstance(new ShareMemoryService);
ShareMemoryService::ShareMemoryService()
{
}


ShareMemoryService::~ShareMemoryService()
{
	if (m_pSharedMemory)
	{
		UnmapViewOfFile(m_pSharedMemory);
	}

	if (m_hMapping)
	{
		CloseHandle(m_hMapping);
	}

}

PGAME_IMAGE_DATA ShareMemoryService::GetSharedMemoryPointer()
{
	return m_pSharedMemory;
}

ShareMemoryService* ShareMemoryService::GetInstance()
{
	return m_pInstance.get();
}

bool ShareMemoryService::Initialize()
{
	m_hMapping = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(GAME_IMAGE_DATA), GLOBAL_MEMORY_NAME);
	if (!m_hMapping)
	{
		return false;
	}

	m_pSharedMemory = (PGAME_IMAGE_DATA)MapViewOfFile(m_hMapping, FILE_MAP_ALL_ACCESS, NULL, NULL, NULL);
	if (!m_pSharedMemory)
	{
		return false;
	}

	memset(m_pSharedMemory, 0, sizeof(GAME_IMAGE_DATA));
	return true;
}
