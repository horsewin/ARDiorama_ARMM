/*
 * main_TextureTransfer.h
 *
 *  Created on: 2012/05/29
 *      Author: umakatsu
 */

#ifndef MAIN_TEXTURETRANSFER_H_
#define MAIN_TEXTURETRANSFER_H_

namespace TextureTransfer
{
	class ViewingModel;

	class TextureTransfer
	{
	public:
		void DrawString(const char *str,void *font,float x,float y,float z);
		void DrawController();

		void ColorSetting(const double & value, bool harmonic = false);


		//relative OpenGL
		void display();
		void idle();
		void specialkey(int key,int x, int y);
		void keyboard(unsigned char key, int x, int y);
		void mouse(int button, int state, int x, int y);
		void motion(int x, int y);
		void CallbackEntry(void);
		void Init();

		//for rendering to the window
		void DrawModelMonitor(int x, int y, int w, int h, ViewingModel * model, bool isStroke, const int & separationW);
		void DrawTextureMonitor(int x, int y, int w, int h, ViewingModel * model, const int & seprationW);
		void PointsDisplay();
		void TexturePaste(bool color);

	};
}
#endif /* MAIN_TEXTURETRANSFER_H_ */
