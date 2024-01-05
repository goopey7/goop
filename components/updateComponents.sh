#!/bin/bash

# Get the directory of the script
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Move to the script's directory
cd "$script_dir" || exit

# Find .cpp files  
shopt -s nullglob
cpp_files=("$script_dir"/*.cpp)
shopt -u nullglob

# Calculate hash of CustomComponents.h if it exists
existing_hash=""
if [ -e CustomComponents.h ]; then
    existing_hash=$(sha256sum CustomComponents.h | awk '{ print $1 }')
fi

# Function to calculate hash for a file
calculate_hash() {
    local file="$1"
    sha256sum "$file" | awk '{ print $1 }'
}

# Concatenate all .cpp file names together
file_names_concatenated=$(printf "%s" "${cpp_files[@]##*/}")

# Calculate the hash
updated_hash=$(echo -n "$file_names_concatenated" | sha256sum | awk '{ print $1 }')

# Get existing hash from first line comment
existing_hash_comment=$(head -n 1 CustomComponents.h)
existing_hash=$(echo "$existing_hash_comment" | sed 's/^\/\/\(.*\)$/\1/')

needs_update=false
if [ "$existing_hash" != "$updated_hash" ]; then
    needs_update=true
fi

if ! $needs_update; then
	echo "CustomComponents.h is up to date"
	exit 0
fi

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

inline void guiCustomComponents(goop::Entity e)
{
EOF
if [ ${#cpp_files[@]} -ne 0 ]; then
cat <<EOF >>CustomComponents.h
    for (auto& [name, factory] : customComponentFactoryMap)  
    {
        auto variant = factory(entt::null, nullptr);
        std::visit([e, &name](auto& arg) 
        {
			goop::Scene* s = e.getScene();
            using T = std::decay_t<decltype(arg)>;
            auto view = s->view<T>();
            for (auto en : view)
            {     
				if (e.getEntity() != en) continue;
                ImGui::Text("%s", name.c_str());        
                goop::Entity(en, s).getComponent<T>().gui(); 
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

sed -i "1s/.*/\/\/$updated_hash/" CustomComponents.h
echo "CustomComponents.h updated"
