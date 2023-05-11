#include "scene.h"
#include "scenerunner.h"
#include "scenereflectcube.h"
#include "scenerefractcube.h"


std::map<std::string, std::string> sceneInfo = {
	{ "reflect-cube", "Reflection with a cube map" },
	{ "refract-cube", "Refraction with a cube map" }
};

int main(int argc, char *argv[])
{

	std::string recipe = SceneRunner::parseCLArgs(argc, argv, sceneInfo);

	SceneRunner runner("Chapter 5 - " + recipe, 1024, 678, 4);
	std::unique_ptr<Scene> scene;
	if( recipe == "reflect-cube") {
		scene = std::unique_ptr<Scene>( new SceneReflectCube() );
	} else if( recipe == "refract-cube" ) {
		scene = std::unique_ptr<Scene>( new SceneRefractCube() );
	}
	else {
		printf("Unknown recipe: %s\n", recipe.c_str());
		exit(EXIT_FAILURE);
	}

    return runner.run(std::move(scene));
}
