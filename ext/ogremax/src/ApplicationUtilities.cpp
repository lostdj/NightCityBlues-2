/*
 * OgreMax Sample Viewer and Scene Loader - Ogre3D-based viewer and code for loading and displaying .scene files
 * Copyright 2011 AND Entertainment
 *
 * This code is available under the OgreMax Free License:
 *   -You may use this code for any purpose, commercial or non-commercial.
 *   -If distributing derived works (that use this source code) in binary or source code form, 
 *    you must give the following credit in your work's end-user documentation: 
 *        "Portions of this work provided by OgreMax (www.ogremax.com)"
 *
 * AND Entertainment assumes no responsibility for any harm caused by using this code.
 * 
 * The OgreMax Sample Viewer and Scene Loader were released at www.ogremax.com 
 */


//Includes---------------------------------------------------------------------
#include "ApplicationUtilities.hpp"
#include <OgreEntity.h>

using namespace Ogre;
using namespace OgreMax;


//Local functions--------------------------------------------------------------
template<class _Func>
void for_each_movable(const::Ogre::SceneNode* node, _Func func)
{
	std::queue<const Ogre::SceneNode*> sceneNodes;
	sceneNodes.push(node);

	//Iterate over the nodes
	while (sceneNodes.size())
	{
		//Pop from the front
		const Ogre::SceneNode* curNode = sceneNodes.front(); 
        sceneNodes.pop();

		//Visit attached objects
		Ogre::SceneNode::ConstObjectIterator objectIter = curNode->getAttachedObjectIterator();
		while (objectIter.hasMoreElements())
			func(objectIter.getNext());
		
		//Recurse below
		Ogre::SceneNode::ConstChildNodeIterator nodei = curNode->getChildIterator();
		while (nodei.hasMoreElements())
		{
			const Ogre::SceneNode* childNode = static_cast<const Ogre::SceneNode*>(nodei.getNext());
			if (childNode)
				sceneNodes.push(childNode);
		}
	}
}

class ComputeAABB
{
public:
	AxisAlignedBox compute(const SceneNode* node)
	{
		this->aabb.setNull(); 

		for_each_movable(node, std::bind1st(std::mem_fun(&ComputeAABB::visit), this));

		return this->aabb;
	}

private:
	void visit(MovableObject* movable)
	{
		if (movable->getMovableType() == "Entity" ||
            movable->getMovableType() == "BillboardSet")
        {
            this->aabb.merge(movable->getWorldBoundingBox(true));
        }
	}

	AxisAlignedBox aabb;
};

static Sphere aabbToSphere(const AxisAlignedBox& aabb)
{
	if (!aabb.isNull())
	{
		// Compute sphere
		float radius = -1.0f;
		const Vector3* corners = aabb.getAllCorners();
		for(unsigned int i = 0; i < 6; ++i)
		{
			float distance = corners[i].distance(aabb.getCenter());
			if (distance>radius)
			{
				radius = distance;
			}
		}
		return Sphere(aabb.getCenter(), radius);
	}
    else
	    return Sphere(Vector3::ZERO, -1.0f);
}

static void ComputeWorldExtents(const SceneNode* node, AxisAlignedBox& aabb, Sphere& sphere)
{
    ComputeAABB extents;
	aabb = extents.compute(node);
	
    sphere = aabbToSphere(aabb);
}

static void View(Camera* camera, const AxisAlignedBox& aabb, const Sphere& sphere)
{
	float nearClip = (sphere.getRadius() > 1)? 1 : 0.05;
	float farClip = sphere.getRadius()*10000.0f;

	camera->setNearClipDistance(nearClip);
	camera->setFarClipDistance(farClip);

	// tan (fov/2.0) = r/d => d = r/tan(fov/2.0)
	float distance = sphere.getRadius()/Math::Tan(camera->getFOVy()/2.0f);

	camera->setPosition(sphere.getCenter() - (camera->getDirection().normalisedCopy()*distance));
}


//Implementation---------------------------------------------------------------
void ApplicationUtilities::CopyCameraSettings(Camera* destinationCamera, const Camera* sourceCamera)
{
    destinationCamera->setPosition(sourceCamera->getRealPosition());
    destinationCamera->setOrientation(sourceCamera->getRealOrientation());
    destinationCamera->setNearClipDistance(sourceCamera->getNearClipDistance());
    destinationCamera->setFarClipDistance(sourceCamera->getFarClipDistance());
    destinationCamera->setProjectionType(sourceCamera->getProjectionType());
    destinationCamera->setFOVy(sourceCamera->getFOVy());
    destinationCamera->setOrthoWindow(sourceCamera->getOrthoWindowWidth(), sourceCamera->getOrthoWindowHeight());
}

void ApplicationUtilities::SetBestCameraPosition(Camera* camera, SceneNode* node)
{
    AxisAlignedBox aabb;
    Sphere sphere;
    ComputeWorldExtents(node, aabb, sphere);
    View(camera, aabb, sphere);
}

const RenderSystemList& ApplicationUtilities::GetRenderSystems()
{
#if OGRE_VERSION_MAJOR == 1 && OGRE_VERSION_MINOR < 7
    return *Root::getSingleton().getAvailableRenderers();
#else
    return Root::getSingleton().getAvailableRenderers();
#endif
}