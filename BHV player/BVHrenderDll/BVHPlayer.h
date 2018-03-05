#pragma once

#pragma once

#include"stdafx.h"
#include"BVH.h"




class CBVHRender
{
protected:
	class CBVHPlayer : BVH
	{
	public:

		CBVHPlayer()
		{
			win_width = 512;
			win_height = 512;
			bvh = nullptr;
			VarToStart();
		}

		~CBVHPlayer() {
			VarToStart();
			delete bvh;
		}

		int     drag_mouse_r; // flag indicating whether the right button is being dragged (1: dragging, 0: not dragging)
		int     drag_mouse_l; // flag as to whether the left button is being dragged (1: dragging, 0: not dragging)
		int     drag_mouse_m; // Flag as to whether middle button is dragging (1: dragging, 0: not dragging)
		int     last_mouse_x, last_mouse_y; // the coordinates of the last recorded mouse cursor

											// Variable for camera rotation
		float	camera_yaw; // rotation angle about Y axis
		float	camera_pitch;  // rotation angle about X axis
		float	camera_distance; // center distance from camera


		// BVH motion data
		BVH *  bvh;

		// Flag indicating whether animation is in progress
		bool   on_animation;
		bool   is_load_success;

		// animation playback time
		float  animation_time;
		int		frame_no;
		HFONT oldFont, newFont;

		int win_width;
		int win_height;


		void VarToStart(void)
		{
			// Variable for mouse drag
			drag_mouse_r = 0; // flag indicating whether the right button is being dragged (1: dragging, 0: not dragging)
			drag_mouse_l = 0; // flag as to whether the left button is being dragged (1: dragging, 0: not dragging)
			drag_mouse_m = 0; // Flag as to whether middle button is dragging (1: dragging, 0: not dragging)


							  // Variable for camera rotation
			camera_yaw = 0.0f; // rotation angle about Y axis
			camera_pitch = -20.0f;  // rotation angle about X axis
			camera_distance = 5.0f; // center distance from camera
									// Window size

			frame_no = 0;

			// Flag indicating whether animation is in progress
			on_animation = true;
			is_load_success = false;
			// animation playback time
			animation_time = 0.0f;


		}

		//
		// Callback function called when changing window size
		//
		void  reshape(int w, int h)
		{
			// Set the area to draw in the window (draw in the whole window here)
			glViewport(0, 0, w, h);

			// Set transformation matrix from camera coordinate system to screen coordinate system
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(45, (double)w / h, 1, 500);

			// record window size (for text rendering process)
			win_width = w;
			win_height = h;
			//UpdateWindow(hWnd);
		}


		// Callback function called at idle
		//
		void idle(void)
		{
			// Calculate the current frame number
			if (bvh)
			{
				frame_no++;
				if (frame_no % bvh->GetNumFrame() == 0)
				{
					animation_time = 0.0f;
					frame_no = 0;
				}
			}
			else
				frame_no = 0;

		}

		void  motion(int mx, int my)
		{
			// Rotate the viewpoint while dragging the right button
			if (drag_mouse_r)
			{
				// Rotate the viewpoint according to the difference between the previous mouse coordinates and the current mouse coordinates

				// Rotate around the Y axis according to the lateral movement of the mouse
				camera_yaw -= (float)(mx - last_mouse_x) * 1.0f;
				if (camera_yaw < 0.0)
					camera_yaw += 360.0;
				else if (camera_yaw > 360.0)
					camera_yaw -= 360.0;

				// rotate around the X axis according to the vertical movement of the mouse
				camera_pitch -= (float)(my - last_mouse_y) * 1.0f;
				if (camera_pitch < -90.0)
					camera_pitch = -90.0;
				else if (camera_pitch > 90.0)
					camera_pitch = 90.0;
			}
			// Change the distance between the viewpoint and the camera while dragging the left button
			if (drag_mouse_l)
			{
				// Rotate the viewpoint according to the difference between the previous mouse coordinates and the current mouse coordinates

				// Move distance according to vertical movement of mouse
				camera_distance += (float)(my - last_mouse_y) * 0.2f;
				if (camera_distance < 2.0)
					camera_distance = 2.0;
			}

			// record the current mouse coordinates
			last_mouse_x = mx;
			last_mouse_y = my;

			// redraw
			//glutPostRedisplay();!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		}

		void  initEnvironment(void)
		{

			// Create a light source
			float  light0_position[] = { 10.0f, 10.0f, 10.0f, 1.0f };
			float  light0_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
			float  light0_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			float  light0_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
			glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
			glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
			glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
			glEnable(GL_LIGHT0);

			// Enable light source calculation
			glEnable(GL_LIGHTING);

			// Activate object color information
			glEnable(GL_COLOR_MATERIAL);

			// Enable Z test
			glEnable(GL_DEPTH_TEST);

			// Enable back removal
			glCullFace(GL_BACK);
			glEnable(GL_CULL_FACE);

			// Set the background color
			glClearColor(0.2f, 0.2f, 0.2f, 0.0f);

		}



		void  display(const float scale) const
		{
			// Clear screen
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			// Set transformation matrix (model coordinate system to camera coordinate system)
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			//glTranslatef(0.0, 0.0, -camera_distance);
			//glRotatef(-camera_pitch, 1.0, 0.0, 0.0);
			//glRotatef(-camera_yaw, 0.0, 1.0, 0.0);
			//glTranslatef(0.0, -0.5, 0.0);
			/*gluLookAt(1.0, 1.5, 2.0,
				0.0, 0.6, 0.0,
				0.0, 1.0, 0.0);*/
				// Reset light source position
			float  light0_position[] = { 10.0, 10.0, 10.0, 1.0 };
			glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

			// Draw the ground
			//float  size = 1.5f;
			//int  num_x = 10, num_z = 10;
			//double  ox, oz;
			//glBegin(GL_QUADS);
			//glNormal3d(0.0, 1.0, 0.0);
			//ox = -(num_x * size) / 2;
			//for (int x = 0; x<num_x; x++, ox += size)
			//{
			//	oz = -(num_z * size) / 2;
			//	for (int z = 0; z<num_z; z++, oz += size)
			//	{
			//		if (((x + z) % 2) == 0)
			//			glColor3f(1.0, 1.0, 1.0);
			//		else
			//			glColor3f(0.8f, 0.8f, 0.8f);
			//		glVertex3d(ox, 0.0, oz);
			//		glVertex3d(ox, 0.0, oz + size);
			//		glVertex3d(ox + size, 0.0, oz + size);
			//		glVertex3d(ox + size, 0.0, oz);
			//	}
			//}
			//glEnd();

			// draw a character
			glColor3f(1.0f, 0.0f, 0.0f);
			if (bvh)
			{
				bvh->RenderFigure(frame_no, scale);
			}
			glFlush();


			//// display time and frame number
			//char  message[64];
			//if (bvh)
			//	sprintf_s(message, "%.2f (%d)", animation_time, frame_no);
			//else
			//	sprintf_s(message, "Press 'L' key to Load a BVH file");
			//drawMessage(sizeof(message), message);
			//	InvalidateRect(hWnd, NULL, TRUE);
		}
	};
private:
	int frame_numb;
	float interval;
	vector<int> BVH_PLAYER_ERROR;
	// The function reverce all strings position in a frame
	void copyPixelsAndSwapString(const int& numbPixelsInStr_X, const int& numbStrings_Y,
		const int& frameNumber, GLuint* bufSource, GLuint* bufReceiver)
	{
		int y_rev(numbStrings_Y - 1);
		int framePoz(numbPixelsInStr_X * numbStrings_Y * frameNumber);
		for (int y = y_rev; y >= 0; y--)
			for (int x = 0; x < numbPixelsInStr_X; x++)
				*(bufReceiver + ((y_rev - y)*numbPixelsInStr_X) + x + framePoz)
				= *(bufSource + (y * numbPixelsInStr_X) + x);

	}
public:
	vector<HBITMAP> ALLBITMAPS;

	CBVHRender() {
		frame_numb = 0;
		interval = 0.0f;
	}
	~CBVHRender() {}

	const HBITMAP * GetBitmap(int& frames_amount, /*int frame_number,*/
		int frame_Width = 0, int frame_Hight = 0,
		HDC hScreenDC = NULL, LPSTR file_name = NULL)
	{
		//ѕроверка переданных параметров указатель на им€ файла
		if (file_name == NULL)
		{
			frames_amount = 0;
			BVH_PLAYER_ERROR.push_back(NULLPSTR);
			return nullptr;
		}
		string control(file_name);
		if (control.empty())
		{
			frames_amount = 0;
			BVH_PLAYER_ERROR.push_back(EMPTYFNAME);
			return nullptr;
		}
		//ѕроверка переданных параметров размеров окна
		if (frame_Width < 1 || frame_Hight < 1)
		{
			frames_amount = 0;
			BVH_PLAYER_ERROR.push_back(ZEROFRSIZE);
			return nullptr;
		}
		// ѕроверка запрошенных кадров на отрицательное значение
		if (frames_amount < 0)
		{
			frames_amount = 0;
			BVH_PLAYER_ERROR.push_back(NEGATIVEFR);
			return nullptr;
		}
		// ѕроверка переданного HDC
		if (hScreenDC == NULL)
		{
			//hScreenDC->unused;
			frames_amount = 0;
			BVH_PLAYER_ERROR.push_back(HDCNULL);
			return nullptr;
		}


		CBVHPlayer BVHPlayer;
		GLuint*  buffSingleFrame(nullptr);
		GLuint*  buffXreflectFrame(nullptr);
		HGLRC hGLRC(NULL); // —сылка на OpenGL через который передадим параметры
		//for//vvvvvvvvvvvv//OpenGL//vvvvvvvvvvvvvv//for//
		PIXELFORMATDESCRIPTOR pfd = {
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_SUPPORT_OPENGL |
			PFD_DRAW_TO_WINDOW |
			PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA,
			32, //cRedShift
			0, 0, 0, 0, 0, 0,
			0,
			0,
			0,
			0, 0, 0, 0,
			16,	//cAccumGreenBits
			0,
			0,
			PFD_MAIN_PLANE,
			0,
			0, 0, 0,
		};
		//////////////////////////////////////////////
		//	hDC = GetDC(hWnd);

		const int pixelFormat = ChoosePixelFormat(hScreenDC, &pfd);
		SetPixelFormat(hScreenDC, pixelFormat, &pfd);
		hGLRC = wglCreateContext(hScreenDC);
		wglMakeCurrent(hScreenDC, hGLRC);
		///////////////////////////////////////////////
		//for//^^^^^^^^^^//OpenGL//^^^^^^^^^^^^^^^//for//
		BVHPlayer.bvh = new BVH();
		if (BVHPlayer.bvh == 0x0)
		{
			BVH_PLAYER_ERROR.push_back(NOTBVHMEM);
			return nullptr;
		}
		if (frames_amount)
		{
			BVHPlayer.bvh->SetNumFrame(frames_amount);
			BVHPlayer.bvh->Load(file_name);
			frames_amount = BVHPlayer.bvh->GetNumFrame();
		}
		else
		{
			BVHPlayer.bvh->Load(file_name);
			frames_amount = BVHPlayer.bvh->GetNumFrame();
		}

		if (BVHPlayer.bvh->IsLoadSuccess())
		{
			//glEnable(GL_LINE_SMOOTH);
			//glEnable(GL_BLEND);
			//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			//glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
			//glEnable(GL_LINE_SMOOTH);
			//glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
			//glEnable(GL_POLYGON_SMOOTH);
			//glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

			interval = BVHPlayer.bvh->GetInterval();
			frame_numb = BVHPlayer.bvh->GetSavedNumFrame();
			if (!buffSingleFrame)
				buffSingleFrame = new GLuint[frame_Width * frame_Hight];
			if (!buffXreflectFrame)
				buffXreflectFrame = new GLuint[frame_Width * frame_Hight];

			int numFrame = BVHPlayer.bvh->GetNumFrame();
			glReadBuffer(GL_BACK_LEFT);
			glPixelStorei(GL_PACK_ALIGNMENT, 1);
			BVHPlayer.reshape(frame_Width, frame_Hight);

			double scale;
			if (BVHPlayer.bvh->GetScale(scale))
			{
				while (numFrame > 0)
				{
					numFrame--;

					BVHPlayer.initEnvironment();
					BVHPlayer.display(scale);
					// Display the screen drawn in the back buffer in the front buffer
					//SwapBuffers(hScreenDC);
					if (BVHPlayer.bvh->GetNumberError() == 0)
					{
						glReadPixels(0, 0, frame_Width, frame_Hight, GL_BGRA,
							GL_UNSIGNED_INT_8_8_8_8_REV, buffSingleFrame);
						copyPixelsAndSwapString(frame_Width, frame_Hight, 0, buffSingleFrame, buffXreflectFrame);
						ALLBITMAPS.push_back(CreateBitmap(frame_Width, frame_Hight, 1, 32, buffXreflectFrame));
					}
					BVHPlayer.idle();
				}
				if (BVHPlayer.bvh->GetNumFrame() > ALLBITMAPS.size())
				{
					frames_amount = ALLBITMAPS.size();
				}
			}
		}

		const int err = BVHPlayer.bvh->GetNumberError();
		if (err)
		{
			for (int i = 0; i < err; i++)
				BVH_PLAYER_ERROR.push_back(BVHPlayer.bvh->GetFirstError(i));
		}

		delete BVHPlayer.bvh;
		BVHPlayer.bvh = nullptr;
		// удал€ем созданный выше
		// контекст воспроизведени€ OpenGL
		if (hGLRC)
		{
			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(hGLRC);
		}
		if (buffSingleFrame)
		{
			delete[] buffSingleFrame;
			buffSingleFrame = nullptr;
		}

		if (buffXreflectFrame)
		{
			delete[] buffXreflectFrame;
			buffXreflectFrame = nullptr;
		}


		if (ALLBITMAPS.begin() == ALLBITMAPS.end())
			return nullptr;
		else
			return ALLBITMAPS.data();
	}

	float GetInterval() const
	{
		return interval;
	}

	int GetNumFrame() const
	{
		return frame_numb;
	}

	int GetNumbErr() const
	{
		return BVH_PLAYER_ERROR.size();
	}


	int* GetPtrErr()
	{
		return BVH_PLAYER_ERROR.data();
	}

	void FreeMemory()
	{
		vector<HBITMAP> EMPTY;
		int n(ALLBITMAPS.size());
		if (n)
			for (auto hB : ALLBITMAPS)
			{
				DeleteObject(hB);
			}
		EMPTY.swap(ALLBITMAPS);
	}
};

