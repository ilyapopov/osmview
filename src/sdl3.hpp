#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_video.h>
#include <SDL3_ttf/SDL_ttf.h>

namespace SDL3 {

struct Point;
struct FPoint;
struct Rect;
struct FRect;

class Exception;

class SDL;
class Window;
class Renderer;
class Texture;
class Surface;

class SDLTTF;
class Font;


struct Point: public SDL_Point {
    Point operator/(int divisor) const {
        return {x / divisor, y / divisor};
    }
    Point operator-(const Point& other) const {
        return {x - other.x, y - other.y};
    }
    Point operator+(const Point& other) const {
        return {x + other.x, y + other.y};
    }
};

struct FPoint: public SDL_FPoint {
    // FPoint operator/(float divisor) const {
    //     return {x / divisor, y / divisor};
    // }
    FPoint operator-(const FPoint& other) const {
        return {x - other.x, y - other.y};
    }
    FPoint operator+(const FPoint& other) const {
        return {x + other.x, y + other.y};
    }
};
struct FRect: public SDL_FRect {
    FPoint GetCentroid() const {
        return {x + 0.5f * w, y + 0.5f * h};
    }
};
struct Rect: public SDL_Rect {
    // Point GetCentroid() const {
    //     return {x + w/2, y + h/2};
    // }
};


class SDL {
public:
    SDL(SDL_InitFlags flags) {
        SDL_Init(flags);
    }

    ~SDL() {
        SDL_Quit();
    }

    std::pair<Window, Renderer> CreateWindowAndRenderer(const char* title, int width, int height, SDL_WindowFlags window_flags);
};


class Exception : public std::runtime_error {
public:
    explicit Exception(const char* s)
        : runtime_error(s)
    {};
};

template<typename T>
T* verify(T* p) {
    if (!p) {
        throw Exception(SDL_GetError());
    }
    return p;
}

class Window {
    struct deleter {
        void operator()(SDL_Window* p) const {SDL_DestroyWindow(p);}
    };

    std::unique_ptr<SDL_Window, deleter> ptr;

public:
    explicit Window(SDL_Window* p) : ptr(p) {};

    Window(const char* title, int w, int h, SDL_WindowFlags flags)
        : ptr(verify(SDL_CreateWindow(title, w, h, flags)))
    {}

    SDL_Window* get() {
        return ptr.get();
    }

    bool SetFullscreen(bool fullscreen) {
        return SDL_SetWindowFullscreen(get(), fullscreen);
    }
};

struct WholeSurface {};

class Surface {
    struct deleter {
        void operator()(SDL_Surface* p) const {SDL_DestroySurface(p);}
    };

    std::unique_ptr<SDL_Surface, deleter> ptr;

public:
    explicit Surface(SDL_Surface* p) : ptr(p) {};

    explicit Surface(const char* filename)
        : ptr(verify(SDL_LoadSurface(filename)))
    {}
    explicit Surface(const std::string& str)
        : ptr(verify(SDL_LoadSurface(str.c_str())))
    {}

    SDL_Surface* get() {
        return ptr.get();
    }
    const SDL_Surface* get() const {
        return ptr.get();
    }

    bool FillRect(const Rect& rect, uint32_t color) {
        return SDL_FillSurfaceRect(get(), &rect, color);
    }
    bool FillRect(WholeSurface, uint32_t color) {
        return SDL_FillSurfaceRect(get(), nullptr, color);
    }

    Point GetSize() const {
        return Point{get()->w, get()->h};
    }

    Surface(int w, int h, SDL_PixelFormat format)
        : ptr(verify(SDL_CreateSurface(w, h, format)))
    {}

    bool Blit(const Rect& srcrect, Surface& dst, const Rect& dstrect) {
        return SDL_BlitSurface(get(), &srcrect, dst.get(), &dstrect);
    }

    bool Blit(WholeSurface, Surface& dst, const Rect& dstrect) {
        return SDL_BlitSurface(get(), nullptr, dst.get(), &dstrect);
    }

    bool Blit(WholeSurface, Surface& dst, const Point& dst_point) {
        Rect dstrect{dst_point.x, dst_point.y, get()->w, get()->h};
        return SDL_BlitSurface(get(), nullptr, dst.get(), &dstrect);
    }

    uint32_t MapRGB(uint8_t r, uint8_t g, uint8_t b) {
        return SDL_MapSurfaceRGB(get(), r, g, b);
    }
};

class Renderer {
    struct deleter {
        void operator()(SDL_Renderer* p) const {SDL_DestroyRenderer(p);}
    };

    std::unique_ptr<SDL_Renderer, deleter> ptr;

public:
    explicit Renderer(SDL_Renderer* p) : ptr(p) {};

    explicit Renderer(Window& window)
        : ptr(verify(SDL_CreateRenderer(window.get(), nullptr)))
    {}
    SDL_Renderer* get() {
        return ptr.get();
    }

    bool RenderTexture(Texture& texture, const FRect& srcrect, const FRect& dstrect);
    bool RenderTexture(Texture& texture, WholeSurface, const FRect& dstrect);
    bool RenderTexture(Texture& texture, WholeSurface, const FPoint& dst_point);

    bool Clear() {
        return SDL_RenderClear(get());
    }
    bool Present() {
        return SDL_RenderPresent(get());
    }
    Point GetOutputSize() {
        int w, h;
        SDL_GetCurrentRenderOutputSize(get(), &w, &h);
        return {w, h};
    }

    bool SetVSync(int vsync) {
        return SDL_SetRenderVSync(get(), vsync);
    }
};

class Texture {
    struct deleter {
        void operator()(SDL_Texture* p) const {SDL_DestroyTexture(p);}
    };

    std::unique_ptr<SDL_Texture, deleter> ptr;

    explicit Texture(SDL_Texture* p) : ptr(p) {};

public:
    Texture(Renderer& renderer, Surface& surface)
        : ptr(verify(SDL_CreateTextureFromSurface(renderer.get(), surface.get())))
    {}

    SDL_Texture* get() {
        return ptr.get();
    }
    const SDL_Texture* get() const {
        return ptr.get();
    }

    FPoint GetSize() {
        float w, h;
        SDL_GetTextureSize(get(), &w, &h);
        return {w, h};
    }

    int width() const {
        return get()->w;
    }
    int height() const {
        return get()->h;
    }
};

class SDLTTF {
public:
    SDLTTF() {
        TTF_Init();
    }

    ~SDLTTF() {
        TTF_Quit();
    }
};

class Font {
    struct deleter {
        void operator()(TTF_Font* p) const {TTF_CloseFont(p);}
    };

    std::unique_ptr<TTF_Font, deleter> ptr;

    explicit Font(TTF_Font* p) : ptr(p) {};

public:

    Font(const char* filename, float size)
        : ptr(verify(TTF_OpenFont(filename, size)))
    {}

    TTF_Font* get() {
        return ptr.get();
    }

    const TTF_Font* get() const {
        return ptr.get();
    }

    Surface RenderText_Blended(const std::string& str, SDL_Color color) {
        return Surface(TTF_RenderText_Blended(get(), str.c_str(), str.size(), color));
    }

    Surface RenderText_Shaded(const std::string& str, SDL_Color fg, SDL_Color bg) {
        return Surface(TTF_RenderText_Shaded(get(), str.c_str(), str.size(), fg, bg));
    }

    int GetLineSkip() const {
        return TTF_GetFontLineSkip(get());
    }
};

inline std::pair<Window, Renderer> SDL::CreateWindowAndRenderer(const char *title, int width, int height, SDL_WindowFlags window_flags) {
    SDL_Window* p_window;
    SDL_Renderer* p_renderer;
    SDL_CreateWindowAndRenderer(title, width, height, window_flags, &p_window, &p_renderer);
    return {Window(p_window), Renderer(p_renderer)};
}

inline bool Renderer::RenderTexture(Texture &texture, const FRect &srcrect, const FRect &dstrect) {
    return SDL_RenderTexture(get(), texture.get(), &srcrect, &dstrect);
}

inline bool Renderer::RenderTexture(Texture &texture, WholeSurface, const FRect &dstrect) {
    return SDL_RenderTexture(get(), texture.get(), nullptr, &dstrect);
}

inline bool Renderer::RenderTexture(Texture &texture, WholeSurface, const FPoint &dst_point)
{
    FRect dstrect{dst_point.x, dst_point.y, float(texture.width()), float(texture.height())};
    return SDL_RenderTexture(get(), texture.get(), nullptr, &dstrect);
}

} // end namespace SDL3
