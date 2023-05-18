#include <cad/CadRegistry.h>

namespace Cad {

RegistryEntry::RegistryEntry() : object(nullptr) {}
RegistryEntry::RegistryEntry(std::unique_ptr<Object> object) : object(std::move(object)) {}

void RegistryEntry::Invalidate()
{
    if (object == nullptr) return;
    Object* ptr = object.release();
    delete ptr;
    object = nullptr;
}
Object& RegistryEntry::GetObject() { return *object; }
bool RegistryEntry::NotValid() const { return object == nullptr; }

RegistryReference Registry::Null() { return std::make_shared<RegistryEntry>(); }

Registry::~Registry()
{
    for (auto& reference : references) {
        reference->Invalidate();
    }

    references.clear();
}

RegistryReference Registry::CreateObject(ObjectBuilder& builder)
{
    auto object = builder.Build();
    auto reference = std::make_shared<RegistryEntry>(std::move(object));
    references.push_back(reference);
    return reference;
}

void Registry::DeleteObject(RegistryReference reference)
{
    if (reference->NotValid()) return;

    reference->Invalidate();

    // Purge invalid references (move to a flush method?)
    references.remove_if([](const RegistryReference& reference) { return reference->NotValid(); });
}

void Registry::DeleteObjects(std::vector<RegistryReference> refs)
{
    for (auto& reference : refs) {
        if (reference->NotValid()) return;

        reference->Invalidate();
    }

    references.remove_if([](const RegistryReference& reference) { return reference->NotValid(); });
}

void Registry::VisitObjects(ObjectVisitor& visitor)
{
    for (auto& reference : references) {
        VisitObject(reference, visitor);
    }
}

void Registry::VisitObject(RegistryReference reference, ObjectVisitor& visitor)
{
    if (reference->NotValid()) return;
    reference->GetObject().Accept(visitor);
}

void Registry::VisitObjects(std::vector<RegistryReference> refs, ObjectVisitor& visitor)
{
    for (auto& reference : refs) {
        VisitObject(reference, visitor);
    }
}

std::vector<RegistryReference> Registry::QueryObjects(ObjectPredicate& predicate)
{
    std::vector<RegistryReference> result;
    for (auto& reference : references) {
        if (predicate.Match(reference->GetObject())) {
            result.push_back(reference);
        }
    }
    return result;
}

unsigned int Registry::Count() const { return references.size(); }
}; // namespace Cad::Core
