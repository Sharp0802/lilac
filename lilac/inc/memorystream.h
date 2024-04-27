#ifndef MEMORYSTREAM_H
#define MEMORYSTREAM_H

#include <cstdint>
#include <memory>

namespace lilac
{
    class MemoryStream
    {
        std::unique_ptr<uint8_t, void(*)(uint8_t*)> m_Handle;
        size_t m_Index;
        size_t m_Size;
        size_t m_Capability;

    public:
        MemoryStream();

        void Write(const uint8_t* src, size_t size);

        void Write(const MemoryStream& stream);

        void Seek(size_t index);

        void CopyTo(std::ostream stream) const;
    };
}

#endif //MEMORYSTREAM_H
