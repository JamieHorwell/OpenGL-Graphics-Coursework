#include "Portal.h"
#include "Renderer.h"
#include "../../nclgl/Matrix3.h"

Portal::Portal(Renderer* renderer)
{
	this->renderer = renderer;
	CulmativeTimer = 4;
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
	
}


int Portal::portal_intersection(Vector3 pos1, Vector3 pos2, SceneNode * portal, float msec)
{
	CulmativeTimer += msec;
	//cameraviews arent in same position (camera has moved)
	if (pos1 != pos2) {
		
			Vector3 p0 = portal->GetTransform() * portal->GetMesh()->getVertice(0);
			Vector3 p1 = portal->GetTransform() * portal->GetMesh()->getVertice(1);
			Vector3 p2 = portal->GetTransform() * portal->GetMesh()->getVertice(2);
			Vector3 p3 = portal->GetTransform() * portal->GetMesh()->getVertice(3);
	
			Matrix3 temp;
			temp.SetColumn(0,Vector3(pos1.x - pos2.x, pos1.y - pos2.y,pos1.z - pos2.z));
			temp.SetColumn(1, Vector3(p1.x - p0.x, p1.y - p0.y, p1.z - p0.z));
			temp.SetColumn(2, Vector3(p2.x - p0.x, p2.y - p0.y, p2.z - p0.z));

			/*temp.SetRow(0, Vector3(pos1.x - pos2.x, pos1.y - pos2.y, pos1.z - pos2.z));
			temp.SetRow(1, Vector3(p1.x - p0.x, p1.y - p0.y, p1.z - p0.z));
			temp.SetRow(2, Vector3(p2.x - p0.x, p2.y - p0.y, p2.z - p0.z));*/


			temp = Matrix3::Inverse(temp);
			Vector3 tuv = temp * Vector3(pos1.x - p0.x, pos1.y - p0.y, pos1.z - p0.z);
			
			//intersection with the plane
			if (fabs(tuv.x) >= 0.000006 && fabs(tuv.x) <= 20.16) {
				if (fabs(tuv.y) >= 0.000006 && fabs(tuv.y) <= 40.16 && fabs(tuv.z) >= 0.000006 && fabs(tuv.z) <= 40.16 ) {
					if (CulmativeTimer > minTime) {

						CulmativeTimer = 0;
						return 1;
					}
				
					
				}

			}

			//now check for intersection with other triangle
			temp.SetColumn(0, Vector3(pos1.x - pos2.x, pos1.y - pos2.y, pos1.z - pos2.z));
			temp.SetColumn(1, Vector3(p2.x - p1.x, p2.y - p1.y, p2.z - p1.z));
			temp.SetColumn(2, Vector3(p3.x - p1.x, p3.y - p1.y, p3.z - p1.z));
			temp = Matrix3::Inverse(temp);

			if (fabs(tuv.x) >= 0.000006 && fabs(tuv.x) <= 20.16) {
				if (fabs(tuv.y) >= 0.000006 && fabs(tuv.y) <= 40.16 && fabs(tuv.z) >= 0.000006 && fabs(tuv.z) <= 40.16) {
					if (CulmativeTimer > minTime) {
						CulmativeTimer = 0;
						return 1;
					}
				}

			}
			
	}

	return 0;
}

void Portal::renderFromPortalView(SceneNode * portalSrc, SceneNode * portalDest, bool reflection, SceneRender sceneToRender)
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
	glStencilOp(GL_INCR, GL_KEEP, GL_KEEP);  


	//4. draw our portal, which will fill inside of portal frame with 1s
	renderer->SetCurrentShader(renderer->getResources()->getShader("basicShader"));
	renderer->UpdateShaderMatrices();
	glUniformMatrix4fv(glGetUniformLocation(renderer->getCurrentShader()->GetProgram(), "modelMatrix"), 1, false, (float*)&(portalSrc->GetTransform()*Matrix4::Scale(portalSrc->GetModelScale())));
	portalSrc->Draw();

	if (reflection) {
		renderer->getReflectManager()->resetCamera(renderer->getCam(), renderer->getViewMatObj());
	}
	//5. now get viewmatrix from portal
	Matrix4 tempViewHolder = renderer->getViewMatrix();
	//std::cout << "VIEWMATRIX AT PORTAL RENDERING" << tempViewHolder << "\n\n";
	renderer->setViewMatrix(getPortalView(renderer->getViewMatrix(), portalSrc, portalDest));
	renderer->UpdateShaderMatrices();

	//6. enable color and depth drawing, disable writing to stencil buffer
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);
	glStencilMask(0x00);

	//7. stencil function to GL_LEQUAL, only draw where stencil has been incremented, so where the portal frame is
	glStencilFunc(GL_EQUAL, 1, 0xff);

	//8. draw rest of scene from portalView ( as weve changed viewmatrix in OGLRenderer
	switch (sceneToRender) {
	case SceneRender::Scene1:
		renderer->RenderScene1(false);
		break;
	case SceneRender::Scene2:
		renderer->RenderScene2(false);
		break;
	}
	

	


	//reset view 
	renderer->setViewMatrix(tempViewHolder); 

	if (reflection) {
		renderer->getReflectManager()->cameraReflectionPos(renderer->getCam(), renderer->getViewMatObj());
	}

	//9. disable stencil test, color buffer drawing, enable depth drawing
	glDisable(GL_STENCIL_TEST);
	glStencilMask(0x00);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_TRUE);

	//10. clear depth buffer
	glClear(GL_DEPTH_BUFFER_BIT);
	//11. draw quad to depth buffer
	renderer->SetCurrentShader(renderer->getResources()->getShader("basicShader"));
	renderer->UpdateShaderMatrices();
	glUniformMatrix4fv(glGetUniformLocation(renderer->getCurrentShader()->GetProgram(), "modelMatrix"), 1, false, (float*)&(portalSrc->GetTransform()*Matrix4::Scale(portalSrc->GetModelScale())));
	portalSrc->Draw();
	glUseProgram(0);

	//12. enable color buffer
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	renderer->UpdateShaderMatrices();
}

void Portal::teleport(Matrix4 prevCam, Matrix4 currentCam)
{

}
