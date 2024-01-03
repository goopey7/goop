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
echo "#include <goop/Components.h>" >>CustomComponents.h
echo "#include <map>" >>CustomComponents.h
echo >>CustomComponents.h
echo "static std::map<std::string, std::function<goop::CustomComponent*(entt::entity, goop::Scene*)>> customComponentMap;" >>CustomComponents.h
echo >>CustomComponents.h
echo "#define REGISTER_CUSTOM_COMPONENT(name, type) \\" >>CustomComponents.h
echo "    static goop::CustomComponent* create##type(entt::entity e, goop::Scene* s) \\" >>CustomComponents.h
echo "    { \\" >>CustomComponents.h
echo "        return new type(goop::Entity(e, s)); \\" >>CustomComponents.h
echo "    } \\" >>CustomComponents.h
echo "    static bool registered##type = []() \\" >>CustomComponents.h
echo "    { \\" >>CustomComponents.h
echo "        customComponentMap[name] = create##type; \\" >>CustomComponents.h
echo "        return true; \\" >>CustomComponents.h
echo "    }()" >>CustomComponents.h
echo >>CustomComponents.h
    exit 0
fi


# Add #includes
for file in "${cpp_files[@]}"; do
    filename=$(basename "$file" .cpp)
    echo "#include \"$filename.h\"" >>CustomComponents.h
done

echo "#include <goop/Components.h>" >>CustomComponents.h
echo "#include <map>" >>CustomComponents.h
echo >>CustomComponents.h
echo "static std::map<std::string, std::function<goop::CustomComponent*(entt::entity, goop::Scene*)>> customComponentMap;" >>CustomComponents.h
echo >>CustomComponents.h
echo "#define REGISTER_CUSTOM_COMPONENT(name, type) \\" >>CustomComponents.h
echo "    static goop::CustomComponent* create##type(entt::entity e, goop::Scene* s) \\" >>CustomComponents.h
echo "    { \\" >>CustomComponents.h
echo "        return new type(goop::Entity(e, s)); \\" >>CustomComponents.h
echo "    } \\" >>CustomComponents.h
echo "    static bool registered##type = []() \\" >>CustomComponents.h
echo "    { \\" >>CustomComponents.h
echo "        customComponentMap[name] = create##type; \\" >>CustomComponents.h
echo "        return true; \\" >>CustomComponents.h
echo "    }()" >>CustomComponents.h
echo >>CustomComponents.h

# Register components
for file in "${cpp_files[@]}"; do
    filename=$(basename "$file" .cpp)
    echo "REGISTER_CUSTOM_COMPONENT(\"$filename\", $filename);" >>CustomComponents.h
done

