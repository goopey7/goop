#include <goop/App.h>

class EditorApp : public goop::App
{
public:
	void init() final;
	void update(float dt) final;
	void gui() final;
	void render() final;
private:
	bool shouldSpawnHouse = true;
	bool shouldSpawnCow = false;
};
