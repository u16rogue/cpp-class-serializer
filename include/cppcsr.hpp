#pragma once

#include <array>
#include <cstdint>
#include <fstream>
#include <ostream>

#define _cppcsr_m_metdataprefix "cppcsr"

#define cppcsr_member(m_type, m_name) \
private: \
	struct \
	{ \
		const char name[sizeof(_cppcsr_m_metdataprefix ## "_" ## #m_type ## "_" ## #m_name)] = { _cppcsr_m_metdataprefix ## "_" ## #m_type ## "_" ## #m_name }; \
		const std::uint8_t size = sizeof(m_type); \
	} cppcsr_metadata_##m_name; \
public: \
	m_type m_name

#define cppcsr_declare_std_array(classT) \
	std::array<std::byte, sizeof(classT) + sizeof(std::size_t)>

namespace cppcsr
{
	template <class T>
	static bool serialize(T &object_to_serialize, std::array<std::byte, sizeof(T) + sizeof(std::size_t)> &destination)
	{
		*reinterpret_cast<std::size_t*>(destination.data()) = sizeof(T); // Set the current class size
		std::memcpy(reinterpret_cast<std::uint8_t*>(destination.data()) + sizeof(std::size_t), &object_to_serialize, sizeof(T)); // Copy the entire object to the array

		return true;
	}

	template <class T>
	static bool deserialize(void *bin_to_deserialize, T &destination)
	{
		if (!bin_to_deserialize)
			return false;

		
		const std::size_t   serialized_size = *reinterpret_cast<std::size_t *>(bin_to_deserialize);
		std::uint8_t       *src             = reinterpret_cast<std::uint8_t *>(bin_to_deserialize) + sizeof(std::size_t); // skip the first sizeof(std::size_t)
		const std::uint8_t *src_end         = src + serialized_size;
		std::uint8_t       *dest            = reinterpret_cast<std::uint8_t *>(&destination);
		const std::size_t   dest_size       = sizeof(T);
		
		while (src < src_end)
		{
			std::size_t  mdata_strlen;
			std::size_t  mdata_typesize;
			void        *mdata_value;

			// Find cppcsr metadata
			for (int mdata_idx = 0; mdata_idx < sizeof(_cppcsr_m_metdataprefix) - 1; mdata_idx++)
				if (src[mdata_idx] != _cppcsr_m_metdataprefix[mdata_idx])
					goto LBL_NEXT_SRC;

			// On metdata match, find the same metadata in destination
			mdata_strlen   = strlen(reinterpret_cast<const char *>(src));

			if (!mdata_strlen)
				goto LBL_NEXT_SRC;

			mdata_typesize = *reinterpret_cast<std::uint8_t*>(src + mdata_strlen + 1); // current source + length of metadata string + null terminator of metadata string
			mdata_value    = reinterpret_cast<void *>(src + mdata_strlen + 1 + sizeof(std::uint8_t)); // current source + length of metadata string + null terminator of metadata string + size metadata
			for (std::size_t dst_mdata_idx = 0; dst_mdata_idx < dest_size - mdata_strlen; dst_mdata_idx++)
			{
				for (std::size_t dst_mdata_str_idx = 0; dst_mdata_str_idx < mdata_strlen + 1; dst_mdata_str_idx++)
				{
					if ((&dest[dst_mdata_idx])[dst_mdata_str_idx] != src[dst_mdata_str_idx])
						goto LBL_NEXT_DST;
				}

				// On destination match, copy from source to destination
				std::memcpy((&dest[dst_mdata_idx]) + mdata_strlen + 1 + sizeof(std::uint8_t), mdata_value, mdata_typesize);
				src += mdata_strlen + 1 + sizeof(std::uint8_t) + mdata_typesize; // Skip the entire block (metadata and value)
				goto LBL_NEXT_BLOCK;
				LBL_NEXT_DST:;
			}

			LBL_NEXT_SRC:
			++src;
			continue;
			LBL_NEXT_BLOCK:;
		}

		return true;
	}
}