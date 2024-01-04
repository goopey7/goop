#!/bin/bash

# Get the directory of the script
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Move to the script's directory
cd "$script_dir" || exit

# Find .cpp files
shopt -s nullglob
cpp_files=("$script_dir"/*.cpp)
shopt -u nullglob

# Start by creating or clearing CustomComponents.h
echo -n >CustomComponents.h

# Check if there are no .cpp files
if [ ${#cpp_files[@]} -eq 0 ]; then
echo "No .cpp files found in the directory."
echo "#include <variant>" >>CustomComponents.h
echo "#include <goop/Scene.h>" >>CustomComponents.h
echo "#include <goop/Entity.h>" >>CustomComponents.h
echo "#include <map>" >>CustomComponents.h
echo >>CustomComponents.h
echo "using CustomComponentVariant = std::variant<std::monostate>;" >>CustomComponents.h
echo "static std::map<std::string, std::function<CustomComponentVariant(entt::entity, goop::Scene*)>> customComponentFactoryMap;" >>CustomComponents.h
echo >>CustomComponents.h
echo "inline void initCustomComponents(goop::Scene* s) {}" >> CustomComponents.h
echo "inline void updateCustomComponents(goop::Scene* s, float dt) {}" >> CustomComponents.h
echo "inline void guiCustomComponents(goop::Scene* s) {}" >> CustomComponents.h
echo "inline void addCustomComponent(const std::string& name, goop::Entity e, goop::Scene* scene) {}" >> CustomComponents.h

    exit 0
fi


# Add #includes
for file in "${cpp_files[@]}"; do
    filename=$(basename "$file" .cpp)
    echo "#include \"$filename.h\"" >>CustomComponents.h
done

echo "#include <goop/Scene.h>" >>CustomComponents.h
echo "#include <map>" >>CustomComponents.h
echo "#include <variant>" >>CustomComponents.h
echo >>CustomComponents.h
echo "using CustomComponentVariant = std::variant<" >>CustomComponents.h

for file in "${cpp_files[@]}"; do
	filename=$(basename "$file" .cpp)
	echo "    $filename," >>CustomComponents.h
done

# remove last comma
sed -i '$ s/.$//' CustomComponents.h

echo ">;" >>CustomComponents.h

echo "static std::map<std::string, std::function<CustomComponentVariant(entt::entity, goop::Scene*)>> customComponentFactoryMap;" >>CustomComponents.h
echo >>CustomComponents.h
echo "#define REGISTER_CUSTOM_COMPONENT(name, type) \\" >>CustomComponents.h
echo "    static CustomComponentVariant create##type(entt::entity e, goop::Scene* s) \\" >>CustomComponents.h
echo "    { \\" >>CustomComponents.h
echo "        return type(goop::Entity(e, s)); \\" >>CustomComponents.h
echo "    } \\" >>CustomComponents.h
echo "    static bool registered##type = []() \\" >>CustomComponents.h
echo "    { \\" >>CustomComponents.h
echo "        customComponentFactoryMap[name] = create##type; \\" >>CustomComponents.h
echo "        return true; \\" >>CustomComponents.h
echo "    }()" >>CustomComponents.h
echo >>CustomComponents.h

# Register components
for file in "${cpp_files[@]}"; do
    filename=$(basename "$file" .cpp)
    echo "REGISTER_CUSTOM_COMPONENT(\"$filename\", $filename);" >>CustomComponents.h
done

echo "inline void initCustomComponents(goop::Scene* s)" >> CustomComponents.h
echo "{" >> CustomComponents.h
echo "    for (auto& [name, factory] : customComponentFactoryMap)" >> CustomComponents.h
echo "    {" >> CustomComponents.h
echo " auto variant = factory(entt::null, s);" >> CustomComponents.h
echo " std::visit([s](auto& arg)" >> CustomComponents.h
echo " {" >> CustomComponents.h
echo "     using T = std::decay_t<decltype(arg)>;" >> CustomComponents.h
echo "     auto view = s->view<T>();" >> CustomComponents.h
echo "     for (auto e : view)" >> CustomComponents.h
echo "     {" >> CustomComponents.h
echo "         goop::Entity(e, s).getComponent<T>().init();" >> CustomComponents.h
echo "     }" >> CustomComponents.h
echo " }, variant);" >> CustomComponents.h
echo "    }" >> CustomComponents.h
echo "}" >> CustomComponents.h

echo "inline void updateCustomComponents(goop::Scene* s, float dt)" >> CustomComponents.h
echo "{" >> CustomComponents.h
echo "    for (auto& [name, factory] : customComponentFactoryMap)" >> CustomComponents.h
echo "    {" >> CustomComponents.h
echo " auto variant = factory(entt::null, s);" >> CustomComponents.h
echo " std::visit([s, dt](auto& arg)" >> CustomComponents.h
echo " {" >> CustomComponents.h
echo "     using T = std::decay_t<decltype(arg)>;" >> CustomComponents.h
echo "     auto view = s->view<T>();" >> CustomComponents.h
echo "     for (auto e : view)" >> CustomComponents.h
echo "     {" >> CustomComponents.h
echo "         goop::Entity(e, s).getComponent<T>().update(dt);" >> CustomComponents.h
echo "     }" >> CustomComponents.h
echo " }, variant);" >> CustomComponents.h
echo "    }" >> CustomComponents.h
echo "}" >> CustomComponents.h

echo "inline void guiCustomComponents(goop::Scene* s)" >> CustomComponents.h
echo "{" >> CustomComponents.h
echo "    for (auto& [name, factory] : customComponentFactoryMap)" >> CustomComponents.h
echo "    {" >> CustomComponents.h
echo " auto variant = factory(entt::null, s);" >> CustomComponents.h
echo " std::visit([s](auto& arg)" >> CustomComponents.h
echo " {" >> CustomComponents.h
echo "     using T = std::decay_t<decltype(arg)>;" >> CustomComponents.h
echo "     auto view = s->view<T>();" >> CustomComponents.h
echo "     for (auto e : view)" >> CustomComponents.h
echo "     {" >> CustomComponents.h
echo "         goop::Entity(e, s).getComponent<T>().gui();" >> CustomComponents.h
echo "     }" >> CustomComponents.h
echo " }, variant);" >> CustomComponents.h
echo "    }" >> CustomComponents.h
echo "}" >> CustomComponents.h

echo "inline void addCustomComponent(const std::string& name, goop::Entity e, goop::Scene* scene)" >> CustomComponents.h
echo "{" >> CustomComponents.h
echo "    auto variant = customComponentFactoryMap[name](e.getEntity(), scene);" >> CustomComponents.h
echo "    std::visit([e](auto& arg)" >> CustomComponents.h
echo "    {" >> CustomComponents.h
echo "        using T = std::decay_t<decltype(arg)>;" >> CustomComponents.h
echo "        e.addComponent<T>(arg);" >> CustomComponents.h
echo "    }, variant);" >> CustomComponents.h
echo "}" >> CustomComponents.h
