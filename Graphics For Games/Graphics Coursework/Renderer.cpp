#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent) {
	camera = new Camera();

	//add shaders and textures to our 
	resources.addShader("lightingShader","PerPixelVertex.glsl","reflectFragment.glsl");


}