#pragma once

/**
***	Reading and drawing class of BVH action file
***	Copyright (c) 2004-2007, Masaki OSHITA (www.oshita-lab.org)
**/


//#ifndef  _BVH_H_
//#define  _BVH_H_

//#include<TCHAR.H>
#include "stdafx.h"
#include <vector>
#include <map>
#include <string>
//#include <sstream>

template <typename T>
std::string toString(T val)
{
	std::ostringstream oss;
	oss << val;
	return oss.str();
}

template<typename T>
T fromString(const std::string& s)
{
	std::istringstream iss(s);
	T res;
	iss >> res;
	return res;
}

using namespace  std;

#define NONERROR	0		// ��� ������
#define FDATAERROR	100		//�� ������������ ���������� ����� BVH
#define RAFTEREND	101		//������� ������ �� ������ �����
#define NULLPSTR	102		//� ���������� ������� ������� ��������� �� ������
#define ZEROFRSIZE	103		//� ���������� ������� ������ ����a ������ �������
#define NEGATIVEFR	104		//� ���������� ��������� ������������� ���������� ������
#define NOTBVHMEM	105		//������������ ������ ��� ��������� ��� BVH ������
#define GTINITERR	106		//������ ������������� ����� GLUT
#define GTCRTWIND	107		//������ �������� ���� GLUT
#define HWINDERR	108		//�� ������� �������� ���������� ���� GLUT
#define NOTSGFRAM	109		//������������ ������ ��� ������ ������ �����
#define NOTMTFRAM	110		//������������ ������ ��� ������ ������ ������
#define INVALBTMP	111		//��������� ������ ������� ����� ������ ����
#define EMPTYFNAME	112		//���  ����� - ������ ������
#define HDCNULL		113		//������� ������� ��������� HDC �� ��������� �����������
#define XYZERROR	114		// ���������� ������ �� X Y Z ��� �����

#define SCALE		0.065f	// ������� �����������


//
// Motion data in BVH format
//
class  BVH
{
public:
	/* Internal structure */

	// type of channel
	enum  ChannelEnum
	{
		X_ROTATION, Y_ROTATION, Z_ROTATION,
		X_POSITION, Y_POSITION, Z_POSITION
	};

	struct  Joint;

	// Channel information
	struct  Channel
	{
		// Corresponding joint
		Joint *     joint;

		// type of channel
		ChannelEnum type;

		// Channel number
		size_t    index;
	};

	// Joint information
	struct  Joint
	{
		// Joint name
		string name;
		// Joint number
		size_t    index;

		// joint hierarchy (parent joint)
		Joint *     parent;
		// joint hierarchy (child joint)
		vector< Joint * >    children;

		// connection position
		double offset[3];

		// flag as to whether or not to have end position information
		bool   has_site;
		// end position
		double site[3];

		// Axis of rotation
		vector< Channel * >  channels;
	};



private:
	// flag indicating whether the load was successful
	bool   is_load_success;

	/* File information */
	string     file_name;   // file name
	string     motion_name; // action name

							/* Information on hierarchical structure */
	size_t    num_channel;					// number of channels
	vector< Channel * > channels;			// channel information [channel number]
	vector< Joint * >   joints;				// joint information [part number]

	map< string, Joint * >   joint_index;	// index from joint name to joint information

											/////////////////////////////////////////////////////////////////////////////////////////
	vector<int> BVH_ERRORS;

	void FixError(FILE* afile)
	{
		if (feof(afile))
		{
			BVH_ERRORS.push_back(RAFTEREND);
		}
		else
			BVH_ERRORS.push_back(ferror(afile));
	}

	//////////////////////////////////////////////////////////////////////////////////////////

	/* Information on motion data */
	UINT32    saved_num_frame, num_frame, given_num_frame, snap;		// number of frames
	float     interval;    // time interval between frames
	double *   motion;		// [frame number] [channel number]

public:
	// constructor / destructor  WCHAR
	BVH();
	BVH(const char * bvh_file_name);
	BVH(const WCHAR * bvh_file_name);

	~BVH();

	// Clear all information
	void  Clear();
	void ClearErroreInfo();

	// Load BVH file
	void  Load(const char * bvh_file_name);
	//void  Load(const WCHAR * bvh_file_name);
	void SetNumFrame(int n) { given_num_frame = n; }
public:
	/* Data access function */
	void AddErrorInformation(int Err)
	{
		BVH_ERRORS.push_back(Err);
	}

	int GetNumberError() const
	{
		return BVH_ERRORS.size();
	}

	int GetFirstError(int i)
	{
		if (BVH_ERRORS.size() <= i)
			return 0;

		return BVH_ERRORS[i];
	}

	// Get whether the load was successful
	bool  IsLoadSuccess() const { return is_load_success; }

	// Get file information
	const string &  GetFileName() const { return file_name; }
	const string &  GetMotionName() const { return motion_name; }

	// Retrieve hierarchical structure information
	const size_t  GetNumJoint() const { return  joints.size(); }
	const Joint *   GetJoint(int no) const { return  joints[no]; }
	const size_t  GetNumChannel() const { return  channels.size(); }
	const Channel * GetChannel(int no) const { return  channels[no]; }

	const Joint *   GetJoint(const string & j) const {
		map< string, Joint * >::const_iterator  i = joint_index.find(j);
		return  (i != joint_index.end()) ? (*i).second : NULL;
	}
	const Joint *   GetJoint(const char * j) const {
		map< string, Joint * >::const_iterator  i = joint_index.find(j);
		return  (i != joint_index.end()) ? (*i).second : NULL;
	}

	// Acquire information on motion data	saved_num_frame
	int     GetNumFrame() const { return  num_frame; }
	int		GetSavedNumFrame() const { return saved_num_frame; }
	float  GetInterval() const { return  interval; }
	double  GetMotion(int f, int c) const { return  motion[f*num_channel + c]; }

public:
	/* Drawing function of attitude */

	bool GetScale(double& scale);

	// draw attitude of specified frame
	void  RenderFigure(const int frame_no, float scale = 1.0f);

	// Draw the specified BVH skeleton / posture (class function)
	static void  RenderFigure(const Joint * root, const double * data, float scale = 1.0f);

	// draw one link of BVH skeleton (class function)
	static void  RenderBone(const string &name, float x0, float y0, float z0, float x1, float y1, float z1, bool drawJointSphere = true);
};



