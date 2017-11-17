#include "Portal.h"
#include "Renderer.h"


Portal::Portal()
{
}


Portal::~Portal()
{
}


Matrix4 Portal::getPortalView(Matrix4 originalView, SceneNode * portalSrc, SceneNode * portalDest)
{
	//modelview matrix of portal
	Matrix4 mv = originalView * portalSrc->GetTransform();

	//need to invert portalDest
	Matrix4 portalCam = mv * Matrix4::Rotation(180, Vector3(0, 1, 0)) *  Matrix4::Inverse(portalDest->GetTransform());

	return portalCam;
	//return cameraStart;
}


int Portal::portal_intersection(Vector3 pos1, Vector3 pos2, SceneNode * portal)
{
	//cameraviews arent in same position
	if (pos1 != pos2) {
		for (int i = 0; i < 2; ++i) {

		}

	}

	return 0;
}

void Portal::renderFromPortalView(SceneNode * portalSrc, SceneNode * portalDest)
{
	//1. disable color and depth draw, enable writing to stencil buffer
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);
	glStencilMask(0xFF);
	glClear(GL_STENCIL_BUFFER_BIT);
	glEnable(GL_STENCIL_TEST);


	//2. stencil test always fails on every pixel drawn
	glStencilFunc(GL_NEVER, 0, 0xFF);

	//3. increment stencil value on stencil fail
	glStencilOp(GL_INCR, GL_KEEP, GL_KEEP);  // increment stencil value on stencil fail


											 //4. draw our portal, which will fill inside of portal frame with 1s
	renderer->SetCurrentShader(renderer->getResources()->getShader("basicShader"));
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)&(portalQuad->GetTransform()*Matrix4::Scale(portalQuad->GetModelScale())));
	portalQuad->Draw();


	//5. now get viewmatrix from portal
	Matrix4 tempViewHolder = viewMatrix;
	viewMatrix = getPortalView(viewMatrix, portalQuad, portalQuad2);
	UpdateShaderMatrices();

	//6. enable color and depth drawing, disable writing to stencil buffer
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);
	glStencilMask(0x00);

	//7. stencil function to GL_LEQUAL, only draw where stencil has been incremented, so where the portal frame is
	glStencilFunc(GL_LEQUAL, 1, 0xff);

	//8. draw rest of scene from portalView ( as weve changed viewmatrix in OGLRenderer
	DrawSkybox();
	DrawHeightmap();
	//DrawPortal();
	DrawWater();

	//reset view 
	viewMatrix = tempViewHolder;

	//9. disable stencil test, color buffer drawing, enable depth drawing
	glDisable(GL_STENCIL_TEST);
	glStencilMask(0x00);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_TRUE);

	//10. clear depth buffer
	glClear(GL_DEPTH_BUFFER_BIT);
	//11. draw quad to depth buffer
	SetCurrentShader(lightShader);
	UpdateShaderMatrices();
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)&(portalQuad->GetTransform()*Matrix4::Scale(portalQuad->GetModelScale())));
	portalQuad->Draw();
	glUseProgram(0);

	//12. enable color buffer
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	UpdateShaderMatrices();

}
