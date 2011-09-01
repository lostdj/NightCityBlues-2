#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_mixer.h>
#include <SDL_image.h>

#include "Base.h"
#include "Config.h"

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
      printf("%s\n", buffer);
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

  // Defaults.
  uint32 screenWidth = _d_app_default_screen_width;
  uint32 screenHeight = _d_app_default_screen_height;
  uint32 soundVolume = _d_app_default_sound_volume;
  char *windowCaption = _d_app_window_caption;

  //
  //App app(screenWidth, screenHeight, soundVolume, windowCaption);
  //app.Init();
  //app.Run();
  //app.Destroy();

  return 0;
}
