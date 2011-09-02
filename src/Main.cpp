//
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_mixer.h>
#include <SDL_image.h>

//
#define OGRE_STATIC_LIB
#include <OgreRoot.h>
#include <OgreRenderWindow.h>
#include <OgreWindowEventUtilities.h>
#include <OgreMaterialManager.h>

// Mad Marx Tutorials.
#include "ogreapp/EasyDefines.h"
#include "ogreapp/SimpleOgreInit.h"

#include "OgreMaxScene.hpp"
using namespace OgreMax;

//
#include "Base.h"
#include "Config.h"

//
#include <signal.h>
#if _d_os_win
	#include <windows.h>
	#include "resource.h"
#endif
#include <cmath>

//
//
//
class Log
{
	public:
		Log()
		{
			used = 0;
		}

		~Log()
		{
			//printf("%s\n", buffer);
			Ogre::LogManager::getSingleton().logMessage(buffer);
		}

		Log& operator <<(bool arg)
		{
			print(arg ? "true" : "false");
			
			return *this;
		}

		Log& operator <<(int8 arg)
		{
			print("%d", arg);

			return *this;
		}

		Log& operator <<(uint8 arg)
		{
			print("%d", arg);

			return *this;
		}

		Log& operator <<(int16 arg)
		{
			print("%d", arg);

			return *this;
		}

		Log& operator <<(uint16 arg)
		{
			print("%d", arg);

			return *this;
		}

		Log& operator <<(int32 arg)
		{
			print("%d", arg);

			return *this;
		}

		Log& operator <<(uint32 arg)
		{
			print("%d", arg);

			return *this;
		}

		Log& operator <<(int64 arg)
		{
			print("%d", arg);

			return *this;
		}

		Log& operator <<(uint64 arg)
		{
			print("%d", arg);

			return *this;
		}

		Log& operator <<(float64 arg)
		{
			print("%f", arg);

			return *this;
		}

		Log& operator <<(const char *arg)
		{
			print(arg);

			return *this;
		}

		void print(const char *fmt, ...)
		{
			va_list args;
			va_start(args, fmt);

			#if _d_cc_msc && _d_cc_msc_major > 8
				used += ::vsprintf_s(&buffer[used], _d_log_buffer_length - used, fmt, args);
			#else
				char b[_d_log_buffer_length];
				int c = vsprintf(b, fmt, args);

				if(c > _d_log_buffer_length - used)
					b[c] = '\0';

				used += sprintf(&buffer[used], b);
			#endif

			va_end(args);
		}

		const char* GetBuffer() const
		{
			return buffer;
		}

	private:
		char buffer[_d_log_buffer_length];
		int used;
};

#if _d_enable_log_info
	#define _d_log_info(__args) \
		{ \
			Log() << "Info: " << __args; \
		}
#endif

#if _d_enable_log_warn
	#define _d_log_warn(__args) \
		{ \
			Log() << "Warn: " << __args; \
		}
#endif

#if _d_enable_log_err
	#define _d_log_err(__args) \
		{ \
			Log() << "Err: " << __args; \
		}
#endif

#if _d_os_win
	#define _d_log_fatal(__args) \
		{ \
			Log l; \
			l << "Fatal: " << __args; \
			MessageBoxA( \
				null, \
				l.GetBuffer(), \
				"Fatal", \
				MB_ICONERROR | MB_OK | MB_DEFBUTTON1); \
			::exit(1); \
		}
#else
	#define _d_log_fatal(__args) \
		{ \
			Log() << "Fatal: " << __args; \
			::exit(1); \
		}
#endif

//
//
//
template<typename T> class Vector
{
	public:
		T x;
		T y;

		Vector()
			: x(0), y(0)
		{
			;
		}

		Vector(T x, T y)
			: x(x), y(y)
		{
			;
		}

		Vector(const Vector<T> &v)
			: x(v.GetX()), y(v.GetY())
		{
			;
		}

		T GetX() const
		{
			return x;
		}

		T GetY() const
		{
			return y;
		}

		Vector<T>& Set(T x, T y)
		{
			this->x = x;
			this->y = y;

			return *this;
		}

		Vector<T>& Set(const Vector<T> &v)
		{
			x = v.GetX();
			y = v.GetY();

			return *this;
		}

		T GetDistance(const Vector<T> &to) const
		{
			return ::sqrt(GetDistanceSquared(to));
		}

		T GetDistanceSquared(const Vector<T> &to) const
		{
			T dx, dy;
			dx = x - to.GetX();;
			dy = y - to.GetY();

			return (dx * dx) + (dy * dy);
		}

		T GetLength() const
		{
			return ::sqrt(GetLengthSquared());
		}

		T GetLengthSquared() const
		{
			return (x * x) + (y * y);
		}

		Vector<T>& Add(const Vector<T> &v)
		{
			Set(x + v.GetX(), y + v.GetY());

			return *this;
		}

		Vector<T>& Sub(const Vector<T> &v)
		{
			Set(x - v.GetX(), y - v.GetY());

			return *this;
		}

		Vector<T>& Mul(const T to)
		{
			Set(x * to, y * to);

			return *this;
		}

		Vector<T>& Mul(const Vector<T> &v)
		{
			Set(x * v.GetX(), y * v.GetY());

			return *this;
		}
};

typedef Vector<GLdouble> Vector2d;

//
//
//
template<typename T> class Line
{
	public:
		Vector<T> origin;
		Vector<T> direction;

		static Line<T> Make(const Vector<T> &begin, const Vector<T> &end)
		{
			Vector<T> origin(begin);
			Vector<T> direction(end);
			direction.Sub(begin);

			return Line(origin, direction);
		}

		Line()
		{
			;
		}

		Line(const Vector<T> &origin, const Vector<T> &direction)
		{
			this->origin.Set(origin);
			this->direction.Set(direction);
		}

		Line(const Line<T> &copy)
		{
			this->origin.Set(copy.GetOrigin());
			this->direction.Set(copy.GetDirection());
		}

		Vector<T> GetOrigin() const
		{
			return origin;
		}

		Vector<T> GetDirection() const
		{
			return direction;
		}

		T GetLength() const
		{
			return direction.GetLength();
		}

		Line<T>& AddLength(T length)
		{
			if(length != 0)
			{
				length = T(1) + (length / GetLength());
				direction.Set(direction.x * length, direction.y * length);
			}

			return *this;
		}

		Line<T>& SetLength(T length)
		{
			const T old = GetLength();
			length = length - old;
			if(length == 0)
				return *this;

			length = T(1) + (length / old);
			direction.Set(direction.x * length, direction.y * length);

			return *this;
		}

		Vector<T> GetAbsDirection()
		{
			Vector<T> v(origin);
			v.Add(direction);

			return v;
		}
};

typedef Line<GLdouble> Line2d;

//
//
//
class TimeMgr
{
	public:
		typedef uint32 Time;

		static Time GetTicks()
		{
			// "Initialization". Taking in account application loading time,
			// as SDL_GetTicks() returns time since SDL init.
			static Time t = SDL_GetTicks();

			return SDL_GetTicks() - t;
		}
};

//
//
//
class Fade
{
	public:
		const uint32 FADE_MILLIS;
		const uint32 SLEEP_MILLIS;

		Fade()
			: FADE_MILLIS(0), SLEEP_MILLIS(0)
		{
			;
		}

		Fade(uint32 fadeMillis, uint32 sleepMillis = 0, bool fadeIn = true)
			: FADE_MILLIS(fadeMillis), SLEEP_MILLIS(sleepMillis),
				fadeIn(true), fade(fadeIn ? 0 : 1), sleep(0)
		{
			;
		}

		//Fade(uint32 fadeMillis, uint32 sleepMillis = 0, bool fadeIn = true, float64 fade = 0)
		//  : FADE_MILLIS(fadeMillis), SLEEP_MILLIS(sleepMillis),
		//    fadeIn(true), fade(fadeIn ? 0 : 1), sleep(0)
		//{
		//  ;
		//}

		float64 Calc(uint32 prevTime, uint32 currentTime)
		{
			const uint32 elapsedTime = currentTime - prevTime;

			sleep += elapsedTime;
			if(sleep >= SLEEP_MILLIS)
			{
				float64 fadeTick = (double)elapsedTime / (double)FADE_MILLIS;
				if(fadeTick > FADE_MILLIS)
				{
					// TODO: !
				}

				fade += fadeIn ? fadeTick : -fadeTick;
				if(fade >= 1)
				{
					fadeIn = false;

					if(fade > 1)
						fade += 1 - fade;
				}
				elif(fade <= 0)
				{
					fadeIn = true;
					sleep = 0;

					if(fade < 0)
						fade = -fade;
				}

				if(fade < 0 || fade > 1)
					_d_log_fatal("Fade: " << fade);

				return fade;
			}
		}

	private:
		bool fadeIn;
		float64 fade;
		uint32 sleep;
};

//
//
//
class App : public OgreEasy::SimpleOgreInit
{
	public:
		App(Ogre::String logFileName, Ogre::String cfgFileName, Ogre::String windowCaption)
			: SimpleOgreInit(logFileName, cfgFileName, windowCaption)
		{
			scene = null;
		};

		bool Init()
		{
			try
			{
				//
				if(!initOgre())
				{
					_d_log_fatal("Impossible to init Ogre correctly.");
					return false;
				}

				//
				if(SDL_Init(SDL_INIT_AUDIO) < 0)
					_d_log_fatal("Failed to initialize SDL: " << SDL_GetError());

				//
				if(Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 4096) < 0)
					_d_log_fatal("Failed to initialize audio: " << SDL_GetError());

				//
				SDL_RWops *rw = LoadResource(_d_app_res_blues);
				if(!rw)
					_d_log_fatal(_d_file_line << ": " << SDL_GetError());
				//blues = Mix_LoadMUS("blues.mp3");
				blues = Mix_LoadMUS_RW(rw);
				if(!blues)
					_d_log_fatal(_d_file_line << ": " << SDL_GetError());
				//SDL_FreeRW(rw); // Commented out. Required by streamer.

				//
				atexit(SDL_Quit);

				//
				Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(1);
				Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_NONE);
				//Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::FT_MIN, Ogre::FO_NONE);
				//Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::FT_MAG, Ogre::FO_NONE);

				//
				scene = new OgreMaxScene();
				scene->Load("scene.scene", mWindow, OgreMaxScene::NO_OPTIONS);

				//
				Ogre::SceneManager *mgr = scene->GetSceneManager();
				Ogre::Camera *cam = mgr->getCamera("Camera001");
				Ogre::Viewport *vp = mWindow->addViewport(cam);
				vp->setAutoUpdated(true);
				mWindow->setActive(true);
				mWindow->setAutoUpdated(false);
				mRoot->clearEventTimes();
			}
			catch(Ogre::Exception &e)
			{
				_d_log_fatal("Ogre::Exception:\r\n" << e.what());
			}
			catch(std::exception &e)
			{
				_d_log_fatal("std::exception:\r\n" << e.what());
			}

			return true;
		}

		void Run()
		{
			//
			try
			{
				//
				if(Mix_PlayMusic(blues, -1) == -1)
					_d_log_fatal("Mix_PlayMusic(): " << SDL_GetError());

				//
				;

				//
				while(!mWindow->isClosed())
				{
					mWindow->update(false);
					mWindow->swapBuffers(true);
					mRoot->renderOneFrame();

					Ogre::WindowEventUtilities::messagePump();
				}
			}
			catch(Ogre::Exception &e)
			{
				_d_log_fatal("Ogre::Exception:\r\n" << e.what());
			}
			catch(std::exception &e)
			{
				_d_log_fatal("std::exception:\r\n" << e.what());
			}
		}

		void Destroy()
		{
			mWindow->removeAllViewports();

			scene->Destroy();
			scene->GetRootNode()->removeAndDestroyAllChildren();

			//Ogre::ResourceGroupManager::getSingleton().destroyResourceGroup(lNameOfResourceGroup);
		}

		#if _d_os_win
			SDL_RWops* LoadResource(int resourceId)
			{
				HRSRC resRef = FindResourceA(null, MAKEINTRESOURCEA(resourceId), "FOO");
				if(!resRef)
					_d_log_fatal(_d_file_line << ", resourceId: " << resourceId);

				HGLOBAL resPtr = ::LoadResource(null, resRef);
				if(!resPtr)
					_d_log_fatal(_d_file_line << ", resourceId: " << resourceId);

				void *resData = LockResource(resPtr);
				if(!resData)
					_d_log_fatal(_d_file_line << ", resourceId: " << resourceId);

				SDL_RWops *rw = SDL_RWFromMem(resData, SizeofResource(null, resRef));

				UnlockResource(resRef);

				return rw;
			}
		#endif

		Mix_Music *blues;

		OgreMaxScene *scene;
};

//
//
//
void SignalHandlerFpe(int s)
{
	_d_log_fatal("Caught " << _d_funcname);
}

void SignalHandlerIll(int s)
{
	_d_log_fatal("Caught " << _d_funcname);
}

void SignalHandlerSeg(int s)
{
	_d_log_fatal("Caught " << _d_funcname);
}

int main(int argc, char **argv)
{
	//
	signal(SIGFPE, SignalHandlerFpe);
	signal(SIGILL, SignalHandlerIll);
	signal(SIGSEGV, SignalHandlerSeg);

	//
	App app(_d_app_log_file_name, _d_app_cfg_file_name, _d_app_window_caption);
	app.Init();
	app.Run();
	app.Destroy();

	OgreEasy::waitForUser();

	return 0;
}
