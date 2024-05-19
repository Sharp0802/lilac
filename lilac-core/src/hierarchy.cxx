#include "hierarchy.h"

#include <algorithm>
#include <fstream>
#include <memory>
#include <sstream>
#include <stack>
#include <utility>
#include <vector>
#include <filesystem>
#include <llvm/Support/raw_ostream.h>

namespace lilac::core
{
    std::string Replace(std::string str, const std::string& from, const std::string& to)
    {
        size_t start_pos;
        while ((start_pos = str.find(from)) != std::string::npos)
            str.replace(start_pos, from.length(), to);
        return str;
    }

    void Hierarchy::ToString(std::stringstream& ss, const std::size_t indent) const
    {
        const std::string indents(indent, '\t');

        ss << indents << Kind << ',' << Name << ',' << ActualName;
        switch (Kind)
        {
            case HOK_Constant:
                ss << ',' << GetConstantData().Signed << ',' << static_cast<int>(GetConstantData().Size) << ',';
                if (GetConstantData().Signed)
                    ss << GetConstantData().ConstantSigned;
                else
                    ss << GetConstantData().ConstantUnsigned;
                break;

            case HOK_Function:
            case HOK_Method:
                ss << ',' << GetFunctionData().Type;
                break;

            case HOK_Parameter:
                ss
                    << ',' << GetParameterData().Index
                    << ',' << GetParameterData().Type
                    << ',' << static_cast<uint32_t>(GetParameterData().Flags);
                break;

            default:
                break;
        }

        ss << '\n';

        for (const auto& member: Members)
            member.ToString(ss, indent + 1);
    }

    void Hierarchy::QueryBy(const std::function<bool(Hierarchy*)>& p, std::vector<Hierarchy*>& v)
    {
        if (p(this))
            v.push_back(this);
        for (auto& member: Members)
            const_cast<Hierarchy&>(member).QueryBy(p, v);
    }

    void Hierarchy::Bake()
    {
        std::deque<Hierarchy*> queue;
        queue.emplace_front(this);

        while (!queue.empty())
        {
            auto* current = queue.back();
            queue.pop_back();

            for (auto& cmember: current->Members)
            {
                auto& member = const_cast<Hierarchy&>(cmember);
                member.m_ParentCache = current;
                queue.emplace_front(&member);
            }
        }
    }

    Hierarchy* Hierarchy::GetParent() const
    {
        return m_ParentCache;
    }

    Hierarchy* Hierarchy::QueryByActualName(const std::string& name)
    {
        if (ActualName == name)
            return this;
        for (auto&         member: Members)
            if (const auto h = const_cast<Hierarchy&>(member).QueryByActualName(name))
                return h;
        return nullptr;
    }

    std::vector<Hierarchy*> Hierarchy::QueryBy(const std::function<bool(Hierarchy*)>& p)
    {
        std::vector<Hierarchy*> v;
        QueryBy(p, v);
        return v;
    }

    Hierarchy* Hierarchy::FirstBy(const std::function<bool(Hierarchy*)>& p)
    {
        if (p(this))
            return this;
        for (auto&         member: Members)
            if (const auto h = const_cast<Hierarchy&>(member).FirstBy(p))
                return h;
        return nullptr;
    }

    ConstantData& Hierarchy::GetConstantData()
    {
        return std::get<ConstantData>(m_Trailer);
    }

    FunctionData& Hierarchy::GetFunctionData()
    {
        return std::get<FunctionData>(m_Trailer);
    }

    ParameterData& Hierarchy::GetParameterData()
    {
        return std::get<ParameterData>(m_Trailer);
    }

    const ConstantData& Hierarchy::GetConstantData() const
    {
        return std::get<ConstantData>(m_Trailer);
    }

    const FunctionData& Hierarchy::GetFunctionData() const
    {
        return std::get<FunctionData>(m_Trailer);
    }

    const ParameterData& Hierarchy::GetParameterData() const
    {
        return std::get<ParameterData>(m_Trailer);
    }

    Hierarchy::Hierarchy(const HierarchyKind kind, std::string actualName, std::string name)
        : ActualName(std::move(actualName)),
          Name(std::move(name)),
          Kind(kind)
    {
        switch (Kind)
        {
            case HOK_Constant:
                m_Trailer = ConstantData();
                break;
            case HOK_Function:
            case HOK_Method:
                m_Trailer = FunctionData();
                break;
            case HOK_Parameter:
                m_Trailer = ParameterData();
                break;

            default:
                break;
        }
    }

    std::string Hierarchy::ToString() const
    {
        std::stringstream ss;
        ToString(ss, 0);
        return ss.str();
    }

    bool Hierarchy::operator==(const Hierarchy& obj) const
    {
        return ActualName == obj.ActualName;
    }

    bool Hierarchy::operator!=(const Hierarchy& obj) const
    {
        return ActualName != obj.ActualName;
    }

    bool Hierarchy::operator<(const Hierarchy& obj) const
    {
        return ActualName < obj.ActualName;
    }

    std::shared_ptr<Hierarchy> Hierarchy::CreateFromFile(const std::string& path)
    {
        std::ifstream ifs(path);
        if (!ifs)
        {
            llvm::errs()
                << "Couldn't open file '" << path
                << "'. (" << (std::filesystem::exists(path)
                    ? "Possibly permission problem?"
                    : "There is no such file")
                << ")\n";

            return nullptr;
        }

        auto root = std::make_shared<Hierarchy>(HOK_Root, "%root", "%root");

        std::stack<Hierarchy*> stack;
        stack.push(root.get());

        Hierarchy* previous       = nullptr;
        size_t     previousIndent = 0;

        for (std::string token; std::getline(ifs, token);)
        {
            size_t indent = 0;
            for (; token[indent] == '\t'; indent++)
            {
            }
            token = token.erase(0, indent);

            if (indent > previousIndent)
            {
                if (indent > previousIndent + 1)
                {
                    llvm::errs() << "Extranous indentation is not allowed. (line '" << token << "')\n";
                    return nullptr;
                }

                stack.push(previous ? previous : root.get());
                previousIndent = indent;
            }
            else if (indent < previousIndent)
            {
                for (auto i = indent; i < previousIndent; ++i)
                    stack.pop();
                previousIndent = indent;
            }


            std::vector<std::string> tokens;
            tokens.reserve(std::ranges::count(token, ',') + 1);
            std::stringstream ss(token);
            while (std::getline(ss, token, ','))
                tokens.push_back(token);

            auto kind   = static_cast<HierarchyKind>(std::stoi(tokens[0]));
            auto name   = tokens[1];
            auto actual = tokens[2];

            if (kind == HOK_Root)
                continue;

            Hierarchy h{ kind, actual, name };

            constexpr size_t DataOffset = 3;
            switch (kind)
            {
                case HOK_Constant:
                    h.GetConstantData().Signed = std::stoi(tokens[DataOffset]);
                    h.GetConstantData().Size = std::stoi(tokens[DataOffset + 1]);
                    if (h.GetConstantData().Signed)
                        h.GetConstantData().ConstantSigned = std::stoll(tokens[DataOffset + 2]);
                    else
                        h.GetConstantData().ConstantUnsigned = std::stoull(tokens[DataOffset + 2]);
                    break;

                case HOK_Function:
                case HOK_Method:
                    h.GetFunctionData().Type = tokens[DataOffset];
                    break;

                case HOK_Parameter:
                    h.GetParameterData().Index = std::stoll(tokens[DataOffset]);
                    h.GetParameterData().Type  = tokens[DataOffset + 1];
                    h.GetParameterData().Flags = std::stoul(tokens[DataOffset + 2]);
                    break;

                default:
                    break;
            }

            auto [iter, _] = stack.top()->Members.emplace(h);
            previous       = const_cast<Hierarchy*>(&*iter);
        }

        root->Bake();

        return root;
    }
}
