#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <SDL.h>
#include <SDL_image.h>

#include <string>
#include <memory>
using namespace std;

namespace GoSDL {
    class Window;

    class Image {

    public:

        Image ();
        Image (Window * parentWindow, string path);
        ~Image ();

        Image (const Image & other);
        Image (Image && other);
        Image & operator= (Image&& other);

        void setWindow (Window * parentWindow);
        void setPath (string path);
        bool setWindowAndPath(Window * parentwindow, string path);
        void setTexture (SDL_Texture * texture);

        bool draw (int x, int y, int z,
            double factorX = 1, double factorY = 1, float angle = 0,
            Uint8 alpha = 255, SDL_Color color = {255, 255, 255, 255});

        int getWidth();
        int getHeight();

    private:
        bool loadTexture();

        shared_ptr<SDL_Texture> mTexture;

        struct SDL_Texture_Deleter
        {
            void operator()(SDL_Texture * texture) const
            {
                if (texture)
                {
                    SDL_DestroyTexture(texture);
                }
            }
        };

        Window * mParentWindow = NULL;
        int mWidth, mHeight;

        string mPath;
    };
}

#endif /* _IMAGE_H_ */
