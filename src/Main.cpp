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
#include <OgreEntity.h>
#include <OgreSubEntity.h>
#include <OgreMaterial.h>
#include <OgrePass.h>

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

		static uint64 getCounter()
		{
			#if _d_os_win
				static LARGE_INTEGER freq;
				static LARGE_INTEGER ticks;

				if(!freq.QuadPart)
					::QueryPerformanceFrequency(&freq);

				::QueryPerformanceCounter(&ticks);

				return (uint64)(ticks.QuadPart * (uint64)1000 / freq.QuadPart);
			#elif _d_posix
				static timespec t;
				::clock_gettime(CLOCK_MONOTONIC, &t);

				return ((t.tv_sec * Time::MIL_IN_SECOND) + (t.tv_nsec / Time::NA_IN_MIL));
			#endif
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

		float64 GetFade()
		{
			return fade;
		}

		Fade& SetFade(float64 fade)
		{
			this->fade = fade;

			return *this;
		}

	private:
		bool fadeIn;
		float64 fade;
		uint32 sleep;
};

//
//
//
class Random
{
	public:
		static Random RND;

		Random()
		{
			reSeed();
		}

		Random(uint64 seed)
		{
			setSeed(seed);
		}

		uint64 getNewSeed() const
		{
			return TimeMgr::getCounter();
		}

		void reSeed()
		{
			setSeed(getNewSeed());
		}

		void setSeed(uint64 seed)
		{
			c = 362436;
			i = 15;

			if(!seed)
				seed = getNewSeed();

			uint32 j = (uint32)(seed & 0xFFFFFFFF);

			for(int32 i = 0; i < 16; i++)
			{
				j ^= j << 13;
				j ^= j >> 17;
				j ^= j << 5;
				q[i] = j;
			}

			q[15] = (uint32)(seed ^ (seed >> 32));
		}

		uint32 rnd()
		{
			uint32 x, r = 0xFFFFFFFE;
			uint64 t, a = 487198574;

			i = (i + 1) & 15;
			t = a * q[i] + c;
			c = (uint32)(t >> 32);
			x = (uint32)(t + c);
			if(x < c)
			{
				x++;
				c++;
			}

			return (q[i] = r - x);
		}

		bool getBool()
		{
			return getInt(0, 1) ? true : false;
		}

		int32 getInt()
		{
			return static_cast<int32>(rnd());
		}

		int32 getInt(uint32 max)
		{
			return static_cast<int32>(rnd() / MAGIC * max);
		}

		int32 getInt(int32 min, int32 max)
		{
			if(min > max)
				min = max - 1;

			return (min + static_cast<int32>(rnd() / MAGIC * (max - min)));
		}

		float64 getFloat()
		{
			return (rnd() / MAGIC);
		}

	private:
		static const float64 MAGIC;

		uint32 c, i;
		uint32 q[16];
};

Random Random::RND;
const float64 Random::MAGIC = 4294967296.0;

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
				_d_log_info("------------------------");
				_d_log_info("*-*-* Init.");

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

				//
				scene = new OgreMaxScene();
				scene->Load("scene.scene", mWindow, OgreMaxScene::NO_OPTIONS);

				//
				//Ogre::Light* lLight = scene->GetSceneManager()->createLight();
				//lLight->setType(Ogre::Light::LT_POINT);
				//lLight->setDiffuseColour(1, 1, 1);
				//lLight->setSpecularColour(1.0f, 1.0f, 1.0f);// color of 'reflected' light
				//Ogre::SceneNode* lLightSceneNode = scene->GetRootNode()->createChildSceneNode();
				//lLightSceneNode->attachObject(lLight);

				//
				Ogre::SceneManager *mgr = scene->GetSceneManager();
				Ogre::Camera *cam = mgr->getCamera("Camera001");
				Ogre::Viewport *vp = mWindow->addViewport(cam);
				vp->setAutoUpdated(true);
				mWindow->setActive(true);
				mWindow->setAutoUpdated(false);
				mRoot->clearEventTimes();

				//
				Ogre::MaterialPtr m;

				// Building lights.
				{
					m = scene->GetSceneManager()->getEntity("bl-01-01")->getSubEntity(0)->getMaterial()->clone("bl-01");
					m->getTechnique(0)->getPass(0)->setDiffuse(1, 1, 1, 1);
					m->getTechnique(0)->getPass(0)->setAmbient(1, _d_app_nc_building_lights_green_blue, _d_app_nc_building_lights_green_blue);
					scene->GetSceneManager()->getEntity("bl-01-01")->getSubEntity(0)->setMaterial(m);
					scene->GetSceneManager()->getEntity("bl-01-02")->getSubEntity(0)->setMaterial(m);

					m = scene->GetSceneManager()->getEntity("bl-02-01")->getSubEntity(0)->getMaterial()->clone("bl-02");
					m->getTechnique(0)->getPass(0)->setDiffuse(1, 1, 1, 1);
					m->getTechnique(0)->getPass(0)->setAmbient(1, _d_app_nc_building_lights_green_blue, _d_app_nc_building_lights_green_blue);
					scene->GetSceneManager()->getEntity("bl-02-01")->getSubEntity(0)->setMaterial(m);
					scene->GetSceneManager()->getEntity("bl-02-02")->getSubEntity(0)->setMaterial(m);

					m = scene->GetSceneManager()->getEntity("bl-03-01")->getSubEntity(0)->getMaterial()->clone("bl-03");
					m->getTechnique(0)->getPass(0)->setDiffuse(1, 1, 1, 1);
					m->getTechnique(0)->getPass(0)->setAmbient(1, _d_app_nc_building_lights_green_blue, _d_app_nc_building_lights_green_blue);
					scene->GetSceneManager()->getEntity("bl-03-01")->getSubEntity(0)->setMaterial(m);
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

			return true;
		}

		void Run()
		{
			//
			try
			{
				_d_log_info("------------------------");
				_d_log_info("*-*-* Main loop init.");

				//
				#if _d_release
					if(Mix_PlayMusic(blues, -1) == -1)
						_d_log_fatal("Mix_PlayMusic(): " << SDL_GetError());
				#endif

				// Building lights.
				Ogre::Pass &bl01mat = *Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().getByName("bl-01"))->getTechnique(0)->getPass(0);
				Ogre::Pass &bl02mat = *Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().getByName("bl-02"))->getTechnique(0)->getPass(0);
				Ogre::Pass &bl03mat = *Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().getByName("bl-03"))->getTechnique(0)->getPass(0);

				Fade bl01fade = Fade(Random::RND.getInt(_d_app_nc_building_lights_fade_min, _d_app_nc_building_lights_fade_max), 0, Random::RND.getBool()).SetFade(Random::RND.getFloat());
				Fade bl02fade = Fade(Random::RND.getInt(_d_app_nc_building_lights_fade_min, _d_app_nc_building_lights_fade_max), 0, Random::RND.getBool()).SetFade(Random::RND.getFloat());
				Fade bl03fade = Fade(Random::RND.getInt(_d_app_nc_building_lights_fade_min, _d_app_nc_building_lights_fade_max), 0, Random::RND.getBool()).SetFade(Random::RND.getFloat());

				//
				TimeMgr::Time prevTime = TimeMgr::GetTicks();

				//
				_d_log_info("------------------------");
				_d_log_info("*-*-* Main loop.");

				//
				while(!mWindow->isClosed())
				{
					//
					const TimeMgr::Time currentTime = TimeMgr::GetTicks();
					const TimeMgr::Time elapsedTime = currentTime - prevTime;

					//
					bl01mat.setAmbient(bl01fade.Calc(prevTime, currentTime), _d_app_nc_building_lights_green_blue, _d_app_nc_building_lights_green_blue);
					bl02mat.setAmbient(bl02fade.Calc(prevTime, currentTime), _d_app_nc_building_lights_green_blue, _d_app_nc_building_lights_green_blue);
					bl03mat.setAmbient(bl03fade.Calc(prevTime, currentTime), _d_app_nc_building_lights_green_blue, _d_app_nc_building_lights_green_blue);

					//
					mWindow->update(false);
					mWindow->swapBuffers(true);
					mRoot->renderOneFrame();

					//
					Ogre::WindowEventUtilities::messagePump();

					//
					SDL_Delay(1);

					//
					prevTime = currentTime;
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

	//OgreEasy::waitForUser();

	return 0;
}
