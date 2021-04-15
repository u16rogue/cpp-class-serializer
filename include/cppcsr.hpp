#pragma once

#include <array>
#include <cstdint>
#include <fstream>
#include <ostream>
#include <filesystem>

#define _cppcsr_m_metadataprefix "cppcsr"

#define cppcsr_member(m_type, m_name) \
private: \
	struct \
	{ \
		const char name[sizeof(_cppcsr_m_metadataprefix ## "_" ## #m_type ## "_" ## #m_name)] = { _cppcsr_m_metadataprefix ## "_" ## #m_type ## "_" ## #m_name }; \
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
			for (int mdata_idx = 0; mdata_idx < sizeof(_cppcsr_m_metadataprefix) - 1; mdata_idx++)
				if (src[mdata_idx] != _cppcsr_m_metadataprefix[mdata_idx])
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

	template <class T>
	static bool serialize(T &object_to_serialize, const wchar_t *file_path)
	{
		cppcsr_declare_std_array(T) serialized_buffer = {};
		if (!cppcsr::serialize<T>(object_to_serialize, serialized_buffer))
			return false;

		std::ofstream serialized_stream(file_path, std::ios::binary | std::ios::out);
		if (!serialized_stream.is_open())
			return false;

		bool success_write = !!serialized_stream.write(reinterpret_cast<char *>(serialized_buffer.data()), serialized_buffer.size());
		serialized_stream.close();

		return success_write;
	}

	template <class T>
	static bool deserialize(const wchar_t *file_path, T &destination)
	{
		bool is_success = false;
		char *deserialized_buffer = nullptr;

		char test[40] = { 0 };

		if (!std::filesystem::exists(file_path))
			return false;

		std::ifstream deserialized_stream(file_path, std::ios::binary | std::ios::in);
		if (!deserialized_stream.is_open())
			return false;

		std::size_t object_size = 0;
		if (!deserialized_stream.read(reinterpret_cast<char *>(&object_size), sizeof(std::size_t)))
			goto LBL_CLOSE_STREAM_FAILED;

		if (object_size == 0)
			goto LBL_CLOSE_STREAM_FAILED;
		
		deserialized_buffer = new char[object_size + sizeof(std::size_t)];
		if (!deserialized_buffer)
			goto LBL_CLOSE_STREAM_FAILED;

		*reinterpret_cast<std::size_t*>(deserialized_buffer) = object_size;

		if (!deserialized_stream.read(deserialized_buffer + sizeof(std::size_t), object_size))
			goto LBL_FREE_ALLOC_FAILED;

		if (!cppcsr::deserialize<T>(deserialized_buffer, destination))
			goto LBL_FREE_ALLOC_FAILED;

		is_success = true;
		LBL_FREE_ALLOC_FAILED:
		delete[] deserialized_buffer;
		LBL_CLOSE_STREAM_FAILED:
		deserialized_stream.close();
		return is_success;
	}
}