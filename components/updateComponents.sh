#!/bin/bash

# Get the directory of the script
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Move to the script's directory
cd "$script_dir" || exit

# Find .cpp files  
shopt -s nullglob
cpp_files=("$script_dir"/*.cpp)
shopt -u nullglob

echo "#pragma once" > CustomComponents.h

for file in "${cpp_files[@]}"; do
    filename=$(basename "$file" .cpp)
    echo "#include \"$filename.h\""  
done >> CustomComponents.h

cat <<EOF >>CustomComponents.h
#include <goop/Scene.h>
#include <goop/Entity.h>
#include <map> 
#include <variant>
#include <imgui.h>

using CustomComponentVariant = std::variant<
EOF
if [ ${#cpp_files[@]} -ne 0 ]; then
	for file in "${cpp_files[@]}"; do 
		filename=$(basename "$file" .cpp)
		echo "    $filename,"  
	done >> CustomComponents.h
else
	echo "std::monostate," >> CustomComponents.h
fi

# remove last comma
sed -i '$ s/.$//' CustomComponents.h

cat <<EOF >>CustomComponents.h 
>;

static std::map<std::string, std::function<CustomComponentVariant(entt::entity, goop::Scene*)>> customComponentFactoryMap;

EOF

if [ ${#cpp_files[@]} -ne 0 ]; then
	cat <<EOF >>CustomComponents.h
	#define REGISTER_CUSTOM_COMPONENT(name, type) \\
		static CustomComponentVariant create##type(entt::entity e, goop::Scene* s) \\
		{ \\
			return type(goop::Entity(e, s)); \\
		} \\
		static bool registered##type = []() \\
		{ \\
			customComponentFactoryMap[name] = create##type; \\
			return true; \\
		}()  
EOF

	# Register components
	for file in "${cpp_files[@]}"; do  
		filename=$(basename "$file" .cpp)
		echo "REGISTER_CUSTOM_COMPONENT(\"$filename\", $filename);"   
	done >> CustomComponents.h

fi
cat <<EOF >>CustomComponents.h  
inline void initCustomComponents(goop::Scene* s)
{
EOF
if [ ${#cpp_files[@]} -ne 0 ]; then
	cat <<EOF >>CustomComponents.h
    for (auto& [name, factory] : customComponentFactoryMap) 
    {
        auto variant = factory(entt::null, s);
        std::visit([s](auto& arg)  
        {     
            using T = std::decay_t<decltype(arg)>;
            auto view = s->view<T>();
            for (auto e : view)  
            {        
                goop::Entity(e, s).getComponent<T>().init();
            }
        }, variant);
    }
EOF
fi
cat <<EOF >>CustomComponents.h
}

inline void updateCustomComponents(goop::Scene* s, float dt)  
{
EOF
if [ ${#cpp_files[@]} -ne 0 ]; then
	cat <<EOF >>CustomComponents.h
    for (auto& [name, factory] : customComponentFactoryMap)
    {
        auto variant = factory(entt::null, s);
        std::visit([s, dt](auto& arg) 
        {  
            using T = std::decay_t<decltype(arg)>;
            auto view = s->view<T>();
            for (auto e : view)   
            {         
                goop::Entity(e, s).getComponent<T>().update(dt);
            }
        }, variant);
    }
EOF
fi
cat <<EOF >>CustomComponents.h
}  

inline void guiCustomComponents(goop::Scene* s)
{
EOF
if [ ${#cpp_files[@]} -ne 0 ]; then
cat <<EOF >>CustomComponents.h
    for (auto& [name, factory] : customComponentFactoryMap)  
    {
        auto variant = factory(entt::null, s);
        std::visit([s, &name](auto& arg) 
        {
            using T = std::decay_t<decltype(arg)>;
            auto view = s->view<T>();
            for (auto e : view)
            {     
                ImGui::Text("%s", name.c_str());        
                goop::Entity(e, s).getComponent<T>().gui(); 
            }
        }, variant);
    }
EOF
fi
cat <<EOF >>CustomComponents.h
}   

inline void addCustomComponent(const std::string& name, goop::Entity e, goop::Scene* scene) 
{
EOF
if [ ${#cpp_files[@]} -ne 0 ]; then
cat <<EOF >>CustomComponents.h
    auto variant = customComponentFactoryMap[name](e.getEntity(), scene);
    std::visit([e](auto& arg)   
    {
        using T = std::decay_t<decltype(arg)>;
        e.addComponent<T>(arg);
    }, variant);
EOF
fi
cat <<EOF >>CustomComponents.h
}

inline void saveCustomComponents(goop::Scene* scene, goop::Entity e, nlohmann::json& json)  
{
EOF
if [ ${#cpp_files[@]} -ne 0 ]; then
cat <<EOF >>CustomComponents.h
    for (auto& [n, factory] : customComponentFactoryMap)
    { 
        auto variant = factory(entt::null, nullptr);
        std::visit([&e, &json, &n](auto& arg)
        {
            using T = std::decay_t<decltype(arg)>;
            if (e.hasComponent<T>())  
            {     
                nlohmann::json j;
                j["type"] = n; 
                json.push_back(j);
            } 
        }, variant);
    }
EOF
fi
cat <<EOF >>CustomComponents.h
}
EOF
