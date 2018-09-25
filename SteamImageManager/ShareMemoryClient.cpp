#include "stdafx.h"
#include "ShareMemoryClient.h"

std::unique_ptr<ShareMemoryClient> ShareMemoryClient::m_pInstance(new ShareMemoryClient);
ShareMemoryClient::ShareMemoryClient()
{
}


ShareMemoryClient::~ShareMemoryClient()
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

ShareMemoryClient* ShareMemoryClient::GetInstance()
{
	return m_pInstance.get();
}

bool ShareMemoryClient::Initialize()
{
	m_hMapping = OpenFileMappingA(FILE_MAP_WRITE, false, GLOBAL_MEMORY_NAME);
	if (!m_hMapping)
	{
		return false;
	}

	m_pSharedMemory = (PGAME_IMAGE_DATA)MapViewOfFile(m_hMapping,FILE_MAP_WRITE, NULL, NULL, NULL);
	if (!m_pSharedMemory)
	{
		return false;
	}


	//m_pSharedMemory->bInject = 100;
	return true;
}

PGAME_IMAGE_DATA ShareMemoryClient::GetSharedMemoryPointer()
{
	return m_pSharedMemory;
}
