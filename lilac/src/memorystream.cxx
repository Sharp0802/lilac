#include "memorystream.h"

#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <cstdio>
#include <limits>

namespace lilac
{
    static std::unique_ptr<uint8_t, void(*)(uint8_t*)> MakeUnique(uint8_t* p)
    {
        return { p, [](uint8_t* p) { free(p); } };
    }

    MemoryStream::MemoryStream()
        : m_Handle(MakeUnique(static_cast<uint8_t*>(malloc(BUFSIZ)))),
          m_Index(0),
          m_Size(0),
          m_Capability(BUFSIZ)
    {
        if (m_Handle == nullptr)
            throw std::runtime_error("Couldn't allocate memory");
    }

    void MemoryStream::Write(const uint8_t* src, const size_t size)
    {
        if (m_Index + size > m_Capability)
        {
            const auto cap = std::max(m_Index + m_Index / 2, m_Index + size);
            const auto tmp = realloc(m_Handle.get(), cap);
            if (tmp == nullptr)
                throw std::runtime_error("Couldn't reallocate memory");

            auto tmpUnique = MakeUnique(static_cast<uint8_t*>(tmp));
            m_Handle.swap(tmpUnique);
            tmpUnique.get_deleter() = [](uint8_t*)
            {
            }; // do NOT dispose original pointer

            m_Capability = cap;
        }
        memcpy(m_Handle.get() + m_Index, src, size);
        m_Index += size;
        m_Size = std::max(m_Size, m_Index);
    }

    void MemoryStream::Write(const MemoryStream& stream)
    {
        Write(stream.m_Handle.get() + stream.m_Index, stream.m_Size - stream.m_Index);
    }

    void MemoryStream::Seek(const size_t index)
    {
        m_Index = std::min(index, m_Size);
    }

    void MemoryStream::CopyTo(std::ostream stream) const
    {
        static_assert(sizeof(size_t) >= sizeof(std::streamsize));

        constexpr auto mask = static_cast<size_t>(std::numeric_limits<std::streamsize>::max());
        for (
            size_t i = 0, size = m_Size - m_Index;
            size > 0;
            i += mask, size -= std::min(mask, size))
        {
            stream.write(
                reinterpret_cast<char*>(m_Handle.get() + m_Index + i),
                static_cast<std::streamsize>(std::min(mask, size - i))
            );
        }
    }
}
