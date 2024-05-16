#pragma once

#include <cstdint>
#include <optional>
#include <set>
#include <string>
#include <variant>

namespace lilac::core
{
#define TYPESTR_DEF(type, str) constexpr std::string type##Ty = "lilac." str

    TYPESTR_DEF(Void, "void");

    TYPESTR_DEF(UTF8, "c8");
    TYPESTR_DEF(UTF16, "c16");
    TYPESTR_DEF(UTF32, "c32");

    TYPESTR_DEF(FP16, "fp16");
    TYPESTR_DEF(FP32, "fp32");
    TYPESTR_DEF(FP64, "fp64");

    TYPESTR_DEF(S8, "s8");
    TYPESTR_DEF(S16, "s16");
    TYPESTR_DEF(S32, "s32");
    TYPESTR_DEF(S64, "s64");
    TYPESTR_DEF(U8, "u8");
    TYPESTR_DEF(U16, "u16");
    TYPESTR_DEF(U32, "u32");
    TYPESTR_DEF(U64, "u64");

    TYPESTR_DEF(Bool, "bool");

#undef TYPESTR_DEF

    constexpr std::string SRetName = "%sret";

    constexpr bool IsBuiltinType(const std::string& type)
    {
        const std::set s_Set = {
            VoidTy,
            UTF8Ty, UTF16Ty, UTF32Ty,
            FP16Ty, FP32Ty, FP64Ty,
            S8Ty, S16Ty, S32Ty, S64Ty,
            U8Ty, U16Ty, U32Ty, U64Ty
        };
        return s_Set.contains(type);
    }

    enum HierarchyKind
    {
        HOK_Root,
        HOK_Namespace,

        HOK_Type,
        HOK_Constant,

        HOK_Function,
        HOK_Method,
        HOK_Parameter,

        HOK_Property,
        HOK_Getter,
        HOK_Setter
    };

    struct ConstantData
    {
        bool    Signed;
        uint8_t Size;

        union
        {
            uint64_t ConstantUnsigned;
            int64_t  ConstantSigned;
        };
    };

    struct ParameterData
    {
        ssize_t Index;
        std::string Type;

        union
        {
            struct
            {
                bool Readonly : 1;
                bool Writeonly: 1;
                bool SRet     : 1;
                bool ByVal    : 1;
            };
            uint8_t Flags;
        };
    };

    struct FunctionData
    {
        std::string Type;
    };

    struct Hierarchy
    {
    private:
        void ToString(std::stringstream& ss, std::size_t indent) const;

    public:
        const std::string ActualName;
        const std::string Name;

        const HierarchyKind Kind;
        std::set<Hierarchy> Members;

    private:
        std::variant<ConstantData, FunctionData, ParameterData> m_Trailer;

    public:
        [[nodiscard]]
        Hierarchy* QueryByActualName(const std::string& name);

        ConstantData& GetConstantData();

        FunctionData& GetFunctionData();

        ParameterData& GetParameterData();

        [[nodiscard]]
        const ConstantData& GetConstantData() const;

        [[nodiscard]]
        const FunctionData& GetFunctionData() const;

        [[nodiscard]]
        const ParameterData& GetParameterData() const;

        Hierarchy(HierarchyKind kind, std::string actualName, std::string name);

        [[nodiscard]]
        std::string ToString() const;

        bool operator==(const Hierarchy&) const;

        bool operator!=(const Hierarchy&) const;

        bool operator<(const Hierarchy&) const;

        static std::optional<Hierarchy> CreateFromFile(const std::string& path);
    };
}
