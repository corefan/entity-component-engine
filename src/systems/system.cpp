// libraries
#include <algorithm>
#include <functional>

// engine
#include <systems/system.hpp>

namespace systems
{
  System::System(std::string name, ComponentType requiredTypes)
    : Printable("System"), Named(name), mRequiredTypes(requiredTypes)
  {
  }

  bool System::hasTypes(const Entity& entity) const
  {
    auto csPtr = entity.getComponents();
    if(!csPtr)
    {
      return true;
    }
    auto components = *csPtr;

    auto allComponentTypes = EMPTY;
    for( auto &c : components )
    {
      allComponentTypes = allComponentTypes | c->getType();
    }

    return ((mRequiredTypes & allComponentTypes) == mRequiredTypes);
  }

  void System::entityAdded(engine::Engine& engine, EntityPtr entity) { }
  void System::entityComponentAdded(engine::Engine& engine, EntityPtr entity, ComponentType newComponent) { }
  void System::entityComponentRemoved(engine::Engine& engine, EntityPtr entity, ComponentType newComponent) { }
  void System::entityRemoved(engine::Engine& engine, EntityPtr entity) { }
  void System::run(engine::Engine& engine) { }

  void System::print(std::ostream& where) const
  {
    printField(where, "name", getName());

    std::vector< std::string > requiredTypes;

    // this loop checks the set bits in the required types bit mask
    int shifts = 0;
    for(int it = static_cast<int>(mRequiredTypes) << 1; (it = it >> 1); shifts++)
    {
      if(it & 1)
      {
        requiredTypes.push_back(ToString(static_cast<ComponentType>(1 << shifts)));
      }
    }

    printVector(where, ", requiredTypes", requiredTypes);
  }
}
