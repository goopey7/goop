#!/bin/bash

if [ "$#" -ne 1 ]; then
  echo "Usage: $0 ClassName"
  exit 1
fi

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"  # Get the directory of the script
CLASS_NAME="$1"
HEADER_FILE="${SCRIPT_DIR}/${CLASS_NAME}.h"  # Define the header file path using the script's directory
CPP_FILE="${SCRIPT_DIR}/${CLASS_NAME}.cpp"   # Define the cpp file path using the script's directory

echo "Creating C++ class: ${CLASS_NAME}..."

# Generate header file
cat <<EOF >"${HEADER_FILE}"
#pragma once
#include <goop/Components.h>
#include <imgui.h>

class ${CLASS_NAME} : public goop::CustomComponent
{
public:
    ${CLASS_NAME}(goop::Entity e) : goop::CustomComponent(e) {name = "${CLASS_NAME}";}
    void init();
    void update(float dt);
    void gui();
private:
	void onCollisionEnter(goop::Entity other) final;
	void onCollisionExit(goop::Entity other) final;
};
EOF

echo "Created ${HEADER_FILE}"

# Generate cpp file
cat <<EOF >"${CPP_FILE}"
#include "${CLASS_NAME}.h"

// Gets called when the game starts
void ${CLASS_NAME}::init()
{
	/* do not remove this line */
	CustomComponent::init();

    //...
}

// Gets called every frame
void ${CLASS_NAME}::update(float dt)
{
    //...
}

// Collision callbacks
void ${CLASS_NAME}::onCollisionEnter(goop::Entity other)
{
	// std::cout << "Player started colliding with " << other.getComponent<goop::TagComponent>().tag << std::endl;
	//...
}

void ${CLASS_NAME}::onCollisionExit(goop::Entity other)
{
	// std::cout << "Player stopped colliding with " << other.getComponent<goop::TagComponent>().tag << std::endl;
	//...
}

// Editor GUI - Shown in inspector view
// Refer to ImGui documentation for more info
void ${CLASS_NAME}::gui()
{
	// ImGui::Text("Hello from %s", name.c_str());
    //...
}
EOF

echo "Created ${CPP_FILE}"
echo "C++ class '${CLASS_NAME}' created successfully!"

cd build
cmake ..
