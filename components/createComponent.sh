#!/bin/bash

if [ "$#" -ne 1 ]; then
  echo "Usage: $0 ClassName"
  exit 1
fi

CLASS_NAME="$1"
HEADER_FILE="${CLASS_NAME}.h"
CPP_FILE="${CLASS_NAME}.cpp"

echo "Creating C++ class: ${CLASS_NAME}..."

# Generate header file
cat <<EOF >"${HEADER_FILE}"
#pragma once
#include <goop/Components.h>

class ${CLASS_NAME} : public goop::CustomComponent
{
public:
    ${CLASS_NAME}(goop::Entity e) : goop::CustomComponent(e) {}
    void init() final;
    void update(float dt) final;
private:
};
EOF

echo "Created ${HEADER_FILE}"

# Generate cpp file
cat <<EOF >"${CPP_FILE}"
#include "${HEADER_FILE}"

void ${CLASS_NAME}::init()
{
    // Implement initialization
}

void ${CLASS_NAME}::update(float dt)
{
    // Implement update
}
EOF

echo "Created ${CPP_FILE}"
echo "C++ class '${CLASS_NAME}' created successfully!"

