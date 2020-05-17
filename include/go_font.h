#ifndef _CUSTOMFONT_H_
#define _CUSTOMFONT_H_

#include <string>
#include "go_image.h"

#include <SDL_ttf.h>


namespace GoSDL {

    class Font {

    public:
        Font();

        void setWindow(Window * parentWindow);
        void setPathAndSize(std::string path, unsigned size);
        void setAll(Window * parentWindow, std::string path, unsigned size);

        int getTextWidth(std::string text);

        GoSDL::Image renderText(std::string text, SDL_Color color = {255, 255, 255, 255});
        GoSDL::Image renderBlock(std::string text, SDL_Color color, unsigned width);

        ~Font();

    private:

        void checkInit();

        GoSDL::Image surfaceToImage (SDL_Surface * tempSurface);

        Window * mParentWindow = NULL;

        std::string mPath;

        unsigned mSize;

        TTF_Font * mFont = NULL;
    };
}


#endif /* _CUSTOMFONT_H_ */
