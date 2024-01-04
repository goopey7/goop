#include <variant>
#include <goop/Scene.h>
#include <goop/Entity.h>
#include <map>

using CustomComponentVariant = std::variant<std::monostate>;
static std::map<std::string, std::function<CustomComponentVariant(entt::entity, goop::Scene*)>> customComponentFactoryMap;

inline void initCustomComponents(goop::Scene* s) {}
inline void updateCustomComponents(goop::Scene* s, float dt) {}
inline void guiCustomComponents(goop::Scene* s) {}
inline void addCustomComponent(const std::string& name, goop::Entity e, goop::Scene* scene) {}
