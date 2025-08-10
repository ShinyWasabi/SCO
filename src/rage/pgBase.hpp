#pragma once

namespace rage
{
	class pgBase
	{
	public:
		virtual void ShutdownClass() = 0;
		virtual void SetHandleIndex(std::uint32_t index) = 0;
		virtual void Validate(std::uint32_t arg) = 0;
		virtual void InitClass(int arg) = 0;
		virtual std::uint32_t GetHandleIndex() const
		{
			return 0;
		}

		std::uintptr_t LookupMetaData(std::uint32_t index) const
		{
			std::uintptr_t current = reinterpret_cast<std::uintptr_t>(this);

			std::uintptr_t nextPage = *reinterpret_cast<std::uintptr_t*>(current + 8);
			if (nextPage == 0)
				return 0;

			std::uint8_t pageStatus = *reinterpret_cast<std::uint8_t*>(nextPage + 11);
			if (pageStatus == 0)
				return 0;

			std::uintptr_t* metadataStart = reinterpret_cast<std::uintptr_t*>(nextPage + 16);
			std::uintptr_t metadataCount = *reinterpret_cast<std::uintptr_t*>(nextPage + 24);

			for (std::uintptr_t i = 0; i < metadataCount; ++i)
			{
				std::uintptr_t currentMeta = *(metadataStart + i);

				std::uint32_t metaIndex = *reinterpret_cast<std::uint32_t*>(currentMeta + 8);
				if (metaIndex == index)
				{
					return currentMeta;
				}
			}

			return 0;
		}

		bool HasPageMap() const
		{
			std::uintptr_t nextPage = *reinterpret_cast<std::uintptr_t*>(reinterpret_cast<std::uintptr_t>(this) + 8);
			if (nextPage == 0)
				return false;

			std::uint8_t pageStatus = *reinterpret_cast<std::uint8_t*>(nextPage + 11);
			if (pageStatus == 0)
				return false;

			nextPage = 0;
			bool hasPageMap = (nextPage != 0);

			return hasPageMap;
		}

		bool MapContainsPointer(void* ptr) const
		{
			std::uintptr_t basePage = reinterpret_cast<std::uintptr_t>(this);
			std::uintptr_t nextPage = *reinterpret_cast<std::uintptr_t*>(basePage + 8);
			if (nextPage == 0)
				return false;

			std::uint8_t pageStatus = *reinterpret_cast<std::uint8_t*>(nextPage + 11);
			if (pageStatus == 0)
				return false;

			std::uintptr_t* data = reinterpret_cast<std::uintptr_t*>(nextPage + 16);
			std::uintptr_t count = *reinterpret_cast<std::uintptr_t*>(nextPage + 8);

			for (std::uintptr_t i = 0; i < count; ++i)
			{
				std::uintptr_t elementPage = *(data + i);
				if (elementPage >= reinterpret_cast<std::uintptr_t>(ptr) && elementPage < reinterpret_cast<std::uintptr_t>(ptr) + 24)
				{
					return true;
				}
			}

			return false;
		}

	private:
		std::uint32_t m_MapSize;
	};
}