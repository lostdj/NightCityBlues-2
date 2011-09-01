//
// Copyright (c) 2007-2011, 'Madmarx' from the ogre3D forums (ogre3D.org).
// All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of its author nor the names of its contributors
//       may be used to endorse or promote products derived from this software
//       without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// ------------------------------------------------------------------------------//
// ------------------------------------------------------------------------------//
#include "SimpleOgreInit.h"

// I will check for std::exception. If you don't know what exception/try/catch means, you should learn C++ first.
#include <exception>

// These are some files that we need to include to use Ogre3D. Note that you can at the beginnings use directly "Ogre.h", to include lots of commonly used classes.
#include "OgreRoot.h"
#include "OgreRenderSystem.h"
#include "OgreRenderWindow.h"

#include <RenderSystems/Direct3D9/OgreD3D9Plugin.h>
#include <Plugins/OctreeSceneManager/OgreOctreePlugin.h>

#include "OgreConfigDialogImp.h"

#include "EasyDefines.h"

#include "../Config.h"

namespace OgreEasy
{
	// the constructor uses the initialisation list to have a proper state.
	SimpleOgreInit::SimpleOgreInit()
		:mRoot(NULL), mWindow(NULL)
	{
		renderer = NULL;
		sceneManager = NULL;
	}

	// the destructor frees memory allocated by the class. 
	SimpleOgreInit::~SimpleOgreInit()
	{
		mWindow = NULL;
		mRoot.reset();// I was not obliged to do that...

		if(renderer != NULL)
			mRoot->getSingleton().uninstallPlugin(renderer);
		if(sceneManager != NULL)
			mRoot->getSingleton().uninstallPlugin(sceneManager);

		delete renderer;
		delete sceneManager;
	}


	bool SimpleOgreInit::initOgre()
	{
		Ogre::String lLogFileName = "";
		if(!Ogre::LogManager::getSingletonPtr())
		{
			Ogre::LogManager *mLogManager = new Ogre::LogManager();
			mLogManager->createLog(lLogFileName, true, true, true);
		}

		bool result = false;
		// This try/catch will catch potential exception launched by ogre or by my program.
		// Ogre can launch 'Ogre::Exception' for example.
		try
		{
			// STEP 1/ First, we will need to create the Ogre::Root object.
			// It is an object that must be created to use ogre correctly, and delete once we are finished using Ogre.

			// This is the name of an optionnal textual configuration file for the rendersystem.
			// I won't use it.
			Ogre::String lConfigFileName = "";
#if _d_os_win
	DWORD dwRetVal;
	DWORD dwBufSize=4096;    // length of the buffer
	char lpPathBuffer[4096]; // buffer for path

	// Get the temp path.
	dwRetVal = GetTempPathA(dwBufSize, lpPathBuffer);

	if (dwRetVal > dwBufSize)
	{
		std::stringstream ss;
		ss << "GetTempPath failed with error: " << GetLastError() << "\n";
		Ogre::LogManager::getSingleton().logMessage(ss.str());
	}
	else
	{
		std::stringstream ss;
		ss << "GetTempPath returned: " << lpPathBuffer << "\n";
		Ogre::LogManager::getSingleton().logMessage(ss.str());
		lConfigFileName.append(lpPathBuffer);
		lConfigFileName.append("\\NightCityBlues.cfg");
	}
#endif

			// I create the root and I wrap it in an auto_ptr so that it will be automatically released.
			// Now I can even do "throw std::bad_alloc("bad alloc");", the program will release the root smoothly.
			mRoot = std::auto_ptr<Ogre::Root>(
				new Ogre::Root("", lConfigFileName, lLogFileName));

			// STEP 2/ Then we need to load plugins. It means that there are functions that are stored inside dynamic libraries.
			// These libraries are .dll or .so files. Most projects Ogre Project do not need all functions to be usable.
			// That way, only a subset of all function can be loaded. It also means you can create your own plugins if you want.
			// If you want to know more on the subject, you 'll need to dig into a C++ tutorial.
			// Anyway, for our use, we will need to load at least a 'RenderSystem' plugin, which means something to drive opengl or directx.
			// The basic plugins you are the most likely to use are the RenderSystems, the particle FX and the Cgprogram.
			{
				// Ogre uses Ogre::String (which is a typedef) to represent strings.
				// Here I use a typedef. If you don't know what it means, you should learn C++ basics first.
				// Same if you don't know what std::vector is.
				//typedef std::vector<Ogre::String> Strings;
				// Here I list all the plugins I want to load.
				// I let those I don't want to use in comments.
				// Opengl rendersystem is supposed to work everywhere.
				// But in reality a rendersystem may fail on your computer. 
				// It is likely do to bad/old graphic card driver/installation,
				// or too old directx version on windows (try update).
				// Often, when one rendersystem fail, the other at least kind-a-work.
				// I put them in a std::vector, because then I can factorise operations and calls (do a 'for').
				//Strings lPluginNames;
				//lPluginNames.push_back("RenderSystem_GL");
				//lPluginNames.push_back("RenderSystem_Direct3D9");
				//lPluginNames.push_back("Plugin_ParticleFX");
				//lPluginNames.push_back("Plugin_CgProgramManager");
				//lPluginNames.push_back("Plugin_PCZSceneManager");
				//lPluginNames.push_back("Plugin_OctreeZone");
				//lPluginNames.push_back("Plugin_OctreeSceneManager");
				//lPluginNames.push_back("Plugin_BSPSceneManager");

				//{
				//	Strings::iterator lIter = lPluginNames.begin();
				//	Strings::iterator lIterEnd = lPluginNames.end();
				//	for(;lIter != lIterEnd; lIter++)
				//	{
				//		Ogre::String& lPluginName = (*lIter);
				//		bool lIsInDebugMode = OGRE_DEBUG_MODE;
				//		if(lIsInDebugMode)
				//		{
				//			lPluginName.append("_d");
				//		}
				//		mRoot->loadPlugin(lPluginName);
				//	}
				//}
			}

			mRoot->getSingleton().installPlugin(renderer = new Ogre::D3D9Plugin());
			mRoot->getSingleton().installPlugin(sceneManager = new Ogre::OctreePlugin());

			// STEP 3/ Then, we can select from the loaded plugins the unique RenderSystem we want to use.
			{
				// the root provide a method if you want to select 
				// the rendersystem and its options visually (lRoot->showConfigDialog()).
				// in that case, you don't need to set the render system manually

				const Ogre::RenderSystemList& lRenderSystemList = mRoot->getAvailableRenderers();
				if( lRenderSystemList.size() == 0 )
				{
					MWARNING("Sorry, no rendersystem was found.");
					return result;
				}

				Ogre::RenderSystem *lRenderSystem = lRenderSystemList[0];
				mRoot->setRenderSystem(lRenderSystem);
			}

			mRoot->restoreConfig();
			Ogre::ConfigDialog dlg;
			bool ok = dlg.display();
			if(ok)
				mRoot->saveConfig();

			// STEP 4/ When the RenderSystem is selected, we can initialise the Root. The root can be initialised only when a rendersystem has been selected.
			{
				// I can create a window automatically, but I won't do it.
				bool lCreateAWindowAutomatically = false;
				// name of the automatically generated window. empty for me.
				Ogre::String lWindowTitle = "asd";
				// custom capabilities of the rendersystem. It's a feature for advanced use.
				Ogre::String lCustomCapacities = "";
				mRoot->initialise(lCreateAWindowAutomatically, lWindowTitle, lCustomCapacities);
			}

			// STEP 5/ Then we can ask to the RenderSystem to create a window.
			{
				//Ogre::String lWindowTitle = "Hello Ogre World";
				unsigned int width = 800;
				unsigned int height = 600;

				Ogre::ConfigOptionMap mOptions = mRoot->getRenderSystem()->getConfigOptions();
				Ogre::ConfigOptionMap::iterator opt = mOptions.find( "Video Mode" );
				if( opt == mOptions.end() )
					OGRE_EXCEPT( Ogre::Exception::ERR_INTERNAL_ERROR, "Can't find Video Mode option!", "D3D9RenderSystem::initialise" );
				// Now we know that the width starts a 0, so if we can find the end we can parse that out
				Ogre::String::size_type widthEnd = opt->second.currentValue.find(' ');
				// we know that the height starts 3 characters after the width and goes until the next space
				Ogre::String::size_type heightEnd = opt->second.currentValue.find(' ', widthEnd+3);
				// Now we can parse out the values
				width = Ogre::StringConverter::parseInt(opt->second.currentValue.substr(0, widthEnd));
				height = Ogre::StringConverter::parseInt(opt->second.currentValue.substr(widthEnd+3, heightEnd));

				//I don't want to use fullscreen during development.
				bool lFullscreen = false; 
				// This is just an example of parameters that we can put. Check the API for more details.
				Ogre::NameValuePairList lParams;
				// fullscreen antialiasing. (check wikipedia if needed).
				//lParams["FSAA"] = "0"; 
				// vertical synchronisation will prevent some image-tearing, but also
				// will provide smooth framerate in windowed mode.(check wikipedia if needed).
				//lParams["vsync"] = "true";
				mWindow = mRoot->createRenderWindow("", width, height, lFullscreen, &lParams);
			}

			result = true;
		}
		catch(Ogre::Exception &e)
		{
			MWARNING("!!!!Ogre::Exception!!!!" << e.what());
			result = false;
		}
		catch(std::exception &e)
		{
			MWARNING("!!!!std::exception!!!!" << e.what());
			result = false;
		}

		return result;
	}
}
