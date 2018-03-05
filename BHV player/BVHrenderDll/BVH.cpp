/**
***  Reading BVH action file drawing class
***  Copyright (c) 2004-2007, Masaki OSHITA (www.oshita-lab.org)
**/

#include"stdafx.h"
#include <sstream>
#include "BVH.h"
#include<wchar.h>
#include <fstream>
#include <algorithm>
#include <iomanip>


//#include <locale>
//#include <sstream>
//#include <iostream>
#include <locale>
#include <string>
#include <sstream>
#include <iterator>

// Constructor
BVH::BVH()
{
	given_num_frame = 0;
	Clear();
}

// Constructor
BVH::BVH(const char * bvh_file_name)
{
	Clear();
	given_num_frame = 0;
	Load(bvh_file_name);
}


//// Constructor
//BVH::BVH(const WCHAR * bvh_file_name)
//{
//	Clear();
//	given_num_frame = 0;
//	Load(bvh_file_name);
//}

// Destructor
BVH::~BVH()
{
	given_num_frame = 0;
	Clear();
	ClearErroreInfo();
}



// Clear all information
void  BVH::Clear()
{
	for (auto n : channels)
	{
		delete  n;
		n = nullptr;
	}
	vector< Channel* >   empty_ch;
	empty_ch.swap(channels);
	empty_ch.clear();

	for (auto n : joints)
	{
		delete  n;
		n = nullptr;
	}
	vector< Joint* >   empty_j;
	empty_j.swap(joints);
	empty_j.clear();


	map< string, Joint* >   empty_j_i;

	empty_j_i.swap(joint_index);
	empty_j_i.clear();

	delete[] motion;
	motion = nullptr;

	is_load_success = false;
	//if (file_name.size())
	file_name.clear();
	//file_name = "";
	//if (motion_name.size())
	motion_name.clear();
	//motion_name = "";

	num_channel = 0;
	channels.clear();
	joints.clear();

	num_frame = 0;
	interval = 0.0f;

}


void BVH::ClearErroreInfo()
{
	vector<int> ZERO;
	BVH_ERRORS.swap(ZERO);
}


string CStrFirstTok(const string in, const string separators)
{
	const string empty;
	static std::string previous;
	std::string input;

	if (in.empty() && previous.empty())
	{
		return empty;
	}

	if (!in.empty())
	{
		input = in;
	}
	else if (!previous.empty())
	{
		input.clear();
		input.swap(previous);
	}
	else {
		return empty;
	}

	if (separators.empty())
	{// Если нет разделителей
		return input;//Возвращаем строку целиком
	}
	//Хранит позицию первого символа не соответствующего символам разделитетям
	std::string::size_type first = 0;
	//Хранит позицию первого символа, равного одному из символов разделителей
	std::string::size_type last = 0;
	//Находим позицию первого символа не равного ни одному из символов разделителей
	first = input.find_first_not_of(separators, last);
	//Находим позицию первого символа равного одному символов разделителей
	last = input.find_first_of(separators, first);

	if (input != empty && last == input.npos && first != input.npos)
	{// Если в строке последнее слово
		stringstream ss(input);// Отдаем строку в стрим
		input.clear();//Освобождаем строку
		ss >> input;// Получаем чистое слово
		std::string clear;
		previous.clear();//Освобождаем статическую строку
		previous.swap(clear);//Обнуляем буфер
		return input;// Возвращаем полученное слово
	}

	if (input != empty && last != input.npos && first != input.npos)
	{// Если в строке первое или очередное слово
		std::string const token(input, first, last - first); // получаем его
		if (first + last <= input.npos)
			input.erase(input.begin(), input.end() - (input.length() - last - 1));
		previous.clear();//Освобождаем статическую строку
		if (input != empty)
			previous.swap(input);//Сохраняем остаток текста
		return token;
	}
	else
	{
		std::string clear;
		previous.clear();
		previous.swap(clear);
		return empty;
	}
	return empty;
}


void ReString(std::string &aString)
{
	std::string str;
	aString.swap(str);
}

void convertToUpper(std::string &aString)
{
	if (!aString.empty())
		transform(aString.begin(), aString.end(), aString.begin(), ::toupper);
}

float parse_str_double(istringstream istr)
{
	float outDouble = 0.0f;
	istr >> outDouble;

	return outDouble;
}

float parse_str_double(const std::string token)
{

	std::istringstream istr(token);
	istr.imbue(std::locale("C"));

	float outDouble = 0.0f;
	istr >> outDouble;

	istr.str(std::string());
	istr.clear();

	return outDouble;
}

//
//  Load BVH file
//
void  BVH::Load(const char * bvh_file_name)
{
#define  BUFFER_LENGTH  1024*4
	//ifstream  file;
	char line[BUFFER_LENGTH];
	//string control;
	string    token;
	char separater[] = " :,\t";
	char separ('\t');
	vector< Joint * >   joint_stack;
	Joint *   joint = nullptr;
	Joint *   new_joint = nullptr;
	bool is_site(false), endOfFile(false);
	float    x, y, z;
	UINT32  i, j;
	// Initialization
	Clear();

	// Setting of file information (file name/action name)
	file_name = bvh_file_name;
	const char *  mn_first = bvh_file_name;
	const char *  mn_last = bvh_file_name + strlen(bvh_file_name);
	if (strrchr(bvh_file_name, '\\') != NULL)
		mn_first = strrchr(bvh_file_name, '\\') + 1;
	else if (strrchr(bvh_file_name, '/') != NULL)
		mn_first = strrchr(bvh_file_name, '/') + 1;
	if (strrchr(bvh_file_name, '.') != NULL)
		mn_last = strrchr(bvh_file_name, '.');
	if (mn_last < mn_first)
		mn_last = bvh_file_name + strlen(bvh_file_name);
	motion_name.assign(mn_first, mn_last);
	// Opening a file
	FILE* file(nullptr);

	errno_t errFile = fopen_s(&file, bvh_file_name, "r");
	if (file == NULL)
	{
		BVH_ERRORS.push_back(errFile);
		Clear();
		return; // End if file can not be opened
	}

	if (errFile != 0)
	{
		BVH_ERRORS.push_back(errFile);
		errFile = fclose(file);

		if (errFile != 0)
		{
			BVH_ERRORS.push_back(errFile);
			Clear();
			return; // End if file can not be opened
		}
		Clear();
		return; // End if file can not be opened
	}

	token.clear();
	ReString(token);
	// Read motion information
	fgets(line, BUFFER_LENGTH, file);
	if (line == NULL)
	{
		FixError(file);
		errFile = fclose(file);

		if (errFile != 0)
		{
			BVH_ERRORS.push_back(errFile);
			Clear();
			return; // End if file can not be opened
		}
		Clear();
		return; // End if file can not be opened
	}
	//////////////////////////////////
	stringstream ll(line); // line 
	token = CStrFirstTok(line, separater);
	// If it is a blank line, go to the next line
	//		if (!token.empty())  continue;
	// Start of joint block
	if (token != "HIERARCHY")
	{
		BVH_ERRORS.push_back(FDATAERROR);
		Clear();
		errFile = fclose(file);

		if (errFile != 0)
		{
			BVH_ERRORS.push_back(errFile);
			Clear();
			return; // End if file can not be opened
		}
		return;
	}
	token.clear();
	ReString(token);
	fgets(line, BUFFER_LENGTH, file);
	token = CStrFirstTok(line, separater);
	// Start of joint information
	if (token == "ROOT")
	{
		// Create joint data
		new_joint = new Joint();
		new_joint->index = joints.size();
		new_joint->parent = joint;
		new_joint->has_site = false;
		new_joint->offset[0] = 0.0;  new_joint->offset[1] = 0.0;  new_joint->offset[2] = 0.0;
		new_joint->site[0] = 0.0;  new_joint->site[1] = 0.0;  new_joint->site[2] = 0.0;
		joints.push_back(new_joint);
		if (joint)
			joint->children.push_back(new_joint);

		// Read joint name
		//	token = strtok(NULL, "");
		token = CStrFirstTok("", "");
		//	ss.clear();
		ll << token; ll >> token;
		while (token == " ")  ll >> token;

		convertToUpper(token);
		new_joint->name = token;

		// Add to Index
		joint_index[new_joint->name] = new_joint;
		//	continue;
	}
	else
	{
		BVH_ERRORS.push_back(FDATAERROR);
		Clear();
		errFile = fclose(file);
		if (errFile != 0)
		{
			BVH_ERRORS.push_back(errFile);
			Clear();
			return; // End if file can not be opened
		}
		return;
	}
	//////////////////////////////////
	// Read hierarchy information
	while (fgets(line, BUFFER_LENGTH, file)) // Read one line
	{

		token.clear();
		ReString(token);
		// Abnormal termination if it reaches the end of the file
		if (line == NULL)
		{
			FixError(file);
			errFile = fclose(file);

			if (errFile != 0)
			{
				BVH_ERRORS.push_back(errFile);
				Clear();
				return; // End if file can not be opened
			}
			Clear();
			return; // End if file can not be opened
		}
		stringstream ss(line); // line 
		token = CStrFirstTok(line, separater);
		// If it is a blank line, go to the next line
		//		if (!token.empty())  continue;
		// Start of joint block
		if (token == "{")
		{
			// Put the current joint on the stack
			joint_stack.push_back(joint);
			joint = new_joint;
			continue;
		}
		// End joint block
		if (token == "}")
		{
			// Remove the current joint from the stack
			joint = joint_stack.back();
			joint_stack.pop_back();
			is_site = false;
			continue;
		}

		// Start of joint information
		if (token == "JOINT")
		{
			// Create joint data
			new_joint = new Joint();
			new_joint->index = joints.size();
			new_joint->parent = joint;
			new_joint->has_site = false;
			new_joint->offset[0] = 0.0;
			new_joint->offset[1] = 0.0;
			new_joint->offset[2] = 0.0;

			new_joint->site[0] = 0.0;
			new_joint->site[1] = 0.0;
			new_joint->site[2] = 0.0;

			joints.push_back(new_joint);
			if (joint)
				joint->children.push_back(new_joint);

			// Read joint name
			//	token = strtok(NULL, "");
			token = CStrFirstTok("", "");
			//	ss.clear();
			ss << token; ss >> token;
			while (token == " ")
				ss >> token;

			convertToUpper(token);
			new_joint->name = token;

			// Add to Index
			joint_index[new_joint->name] = new_joint;
			continue;
		}

		// Start of terminal information
		if (token == "End")
		{

			new_joint = joint;
			is_site = true;
			continue;
		}

		// Information on joint offset or terminal position
		if (token == "OFFSET")
		{
			// Read coordinate values
			//	token = strtok(NULL, separater);
			token = CStrFirstTok("", separater);
			x = !token.empty() ? strtof(token.c_str(), nullptr) : 0.01f;
			//	x = strtod(token.c_str(), nullptr);
			token = CStrFirstTok("", separater);
			y = !token.empty() ? strtof(token.c_str(), nullptr) : 0.01f;
			//	y = strtod(token.c_str(), nullptr);
			token = CStrFirstTok("", separater);
			z = !token.empty() ? strtof(token.c_str(), nullptr) : 0.01f;
			//	z = strtod(token.c_str(), nullptr);														///////////ERROR////////////
			// Set coordinate values for joint offset
			if (is_site)
			{
				joint->has_site = true;
				joint->site[0] = x;
				joint->site[1] = y;
				joint->site[2] = z;
			}
			else
				// Set the coordinate value at the terminal position
			{
				if (joint == nullptr)
					joint = new Joint();
				joint->offset[0] = x;
				joint->offset[1] = y;
				joint->offset[2] = z;
			}
			continue;
		}

		// Joint channel information
		if (token == "CHANNELS")
		{
			// Read the number of channels
			token = CStrFirstTok("", separater);
			joint->channels.resize(token.c_str() ? atoi(token.c_str()) : 0);

			// Read channel information
			for (i = 0; i < joint->channels.size(); i++)
			{
				// Create channel
				Channel *  channel = new Channel();
				channel->joint = joint;
				channel->index = channels.size();
				channels.push_back(channel);
				joint->channels[i] = channel;

				// Determining the type of channel
				token = CStrFirstTok("", separater);
				if (token == "Xrotation")
					channel->type = X_ROTATION;
				else if (token == "Yrotation")
					channel->type = Y_ROTATION;
				else if (token == "Zrotation")
					channel->type = Z_ROTATION;
				else if (token == "Xposition")
					channel->type = X_POSITION;
				else if (token == "Yposition")
					channel->type = Y_POSITION;
				else if (token == "Zposition")
					channel->type = Z_POSITION;
			}
		}
		// Motion - Transfer to data section

		if (token == "MOTION")
		{
			break;
		}
	}

	if (token != "MOTION")
	{
		BVH_ERRORS.push_back(FDATAERROR);
		Clear();
		errFile = fclose(file);

		if (errFile != 0)
		{
			BVH_ERRORS.push_back(errFile);
			Clear();
			return; // End if file can not be opened
		}
		return;
	}
	// Read motion information
	fgets(line, BUFFER_LENGTH, file);
	if (line == NULL)
	{
		FixError(file);
		errFile = fclose(file);

		if (errFile != 0)
		{
			BVH_ERRORS.push_back(errFile);
			Clear();
			return; // End if file can not be opened
		}
		Clear();
		return; // End if file can not be opened
	}
	token.clear();
	ReString(token);
	token = CStrFirstTok(line, ":");
	if (token != "Frames")
	{
		BVH_ERRORS.push_back(FDATAERROR);
		Clear();
		errFile = fclose(file);

		if (errFile != 0)
		{
			BVH_ERRORS.push_back(errFile);
			Clear();
			return; // End if file can not be opened
		}
		return;
	}
	token = CStrFirstTok("", separater);
	if (token.empty())
	{
		BVH_ERRORS.push_back(FDATAERROR);
		Clear();
		errFile = fclose(file);

		if (errFile != 0)
		{
			BVH_ERRORS.push_back(errFile);
			Clear();
			return; // End if file can not be opened
		}
		return;
	}
	saved_num_frame = num_frame = atoi(token.c_str());


	fgets(line, BUFFER_LENGTH, file);
	if (line == NULL)
	{
		FixError(file);
		errFile = fclose(file);

		if (errFile != 0)
		{
			BVH_ERRORS.push_back(errFile);
			Clear();
			return; // End if file can not be opened
		}
		Clear();
		return; // End if file can not be opened
	}
	token = CStrFirstTok(line, ":");
	if (token != "Frame Time")
	{
		BVH_ERRORS.push_back(FDATAERROR);
		errFile = fclose(file);

		if (errFile != 0)
		{
			BVH_ERRORS.push_back(errFile);
			Clear();
			return; // End if file can not be opened
		}
		Clear();
		return;
	}
	token = CStrFirstTok("", separater);
	if (token == "Time:")
		token = CStrFirstTok("", separater);

	if (token.empty())
	{
		BVH_ERRORS.push_back(FDATAERROR);
		errFile = fclose(file);

		if (errFile != 0)
		{
			BVH_ERRORS.push_back(errFile);
			Clear();
			return; // End if file can not be opened
		}
		Clear();
		return;
	}
	interval = parse_str_double(token);

	num_channel = channels.size();

	if (given_num_frame == 0 || given_num_frame >= num_frame)
	{
		given_num_frame = num_frame;
		snap = 1;
	}
	else if (given_num_frame == 1)
	{
		snap = num_frame;
		num_frame = 1;
	}
	else if (given_num_frame == 2)
	{
		snap = num_frame - 1;
	}
	else if (given_num_frame > 1 && given_num_frame < num_frame)
	{
		if (given_num_frame <= sqrt(num_frame))
			snap = (num_frame - 1) / (given_num_frame - 1);
		else
			snap = num_frame / given_num_frame + 1;

		given_num_frame = (num_frame / snap) + 1;
	}


	motion = new double[given_num_frame * num_channel];



	int k;

	std::istringstream istr("");
	istr.imbue(std::locale("C"));

	// Read motion data
	for (i = 0, k = 0; i < num_frame; i++)
	{
		if (i != 1)
			fgets(line, BUFFER_LENGTH, file);
		if (i == 0)
			fgets(line, BUFFER_LENGTH, file);
		if ((i % snap) == 0 && k < given_num_frame)
		{
			if (line == NULL)
			{
				FixError(file);
				errFile = fclose(file);

				if (errFile != 0)
				{
					BVH_ERRORS.push_back(errFile);
					Clear();
					return; // End if file can not be opened
				}
				Clear();
				return; // End if file can not be opened
			}
			token = CStrFirstTok(line, separater);



			for (j = 0; j < num_channel; j++)
			{
				if (token.empty())
				{
					BVH_ERRORS.push_back(FDATAERROR);
					errFile = fclose(file);

					if (errFile != 0)
					{
						BVH_ERRORS.push_back(errFile);
						Clear();

						istr.str(std::string());
						istr.clear();

						return; // End if file can not be opened
					}
					Clear();

					istr.str(std::string());
					istr.clear();

					return;
				}

				istr.str(token);
				motion[k * num_channel + j] = parse_str_double(std::move(istr));

				token = CStrFirstTok("", separater);
			}
			k++;
		}
	}

	istr.str(std::string());
	istr.clear();

	num_frame = k;
	// Close the file
	//	delete file;


	errFile = fclose(file);

	if (errFile != 0)
	{
		BVH_ERRORS.push_back(errFile);
		Clear();
		return; // End if file can not be opened
	}

	// Load success
	is_load_success = true;


	return;
}

// Load success

//
//  Drawing function of BVH skeleton / posture
//


#include <math.h>


const string prefixLeft = "LEFT";
const string prefixLeft2 = "L";
const string prefixRight = "RIGHT";
const string prefixRight2 = "R";
const string prefixHead = "HEAD";
const string prefixHand = "HAND";
const string prefixHip = "HIPS";
const string prefixHipJoint = "HIPJOINT";
const string prefixChest = "CHEST";

//const string prefixFoot = "FOOT";
const string prefixToes = "TOES";

//const string prefixNeck = "LEG";
//const string prefixNeckHead = "NECK";
//const string prefixNeckHead = "HEAD";
//const string prefixLeg = "LEG";
//const string prefixFoot = "FOOT";
const string prefixCollar = "COLLAR";

const string prefixShoulder = "SHOULDER";	
const string prefixShoulder_1 = "SHLDER"; 
const string prefixShoulder_2 = "SHLDR";

const string prefixElbow = "ELBOW";			
const string prefixElbow_1 = "FOREARM"; 
const string prefixElbow_2 = "LOWARM";

const string prefixKnee = "KNEE";			
const string prefixKnee_1 = "SHIN"; 
const string prefixKnee_2 = "LEG";

const string prefixUpArm = "UPARM";




////вычисляем масштаб отображения

bool BVH::GetScale(double& scale)
{
	if (joints.size() == 0 || joints[0]->children.size() == 0)
	{
		BVH_ERRORS.push_back(FDATAERROR);
		return false;
	}
#if DEBUG
	ofstream myfile_1;
#endif
	Joint * joint[] = { nullptr, nullptr, nullptr };
	double lenght(0.0), layer(0);
	double  k[3]{ 0.0 };
	double  dir_x(0);
	double  dir_y(0);
	double  dir_z(0);
	bool isInit = false;

	for (auto n : joints)
	{
		if (n->name.find(prefixElbow) != string::npos
			|| n->name.find(prefixElbow_1) != string::npos
			|| n->name.find(prefixElbow_2) != string::npos)
		{
			joint[0] = n; 
			k[0] = 5.5;
			isInit = true;
			//break;
		}
		else if (n->name.find(prefixKnee) != string::npos
			|| n->name.find(prefixKnee_1) != string::npos
			|| n->name.find(prefixKnee_2) != string::npos)
		{
			joint[1] = n; 
			k[1] = 8.0;
			isInit = true;
			//break;
		}
		else if (n->name.find(prefixShoulder) != string::npos
			|| n->name.find(prefixShoulder_1) != string::npos
			|| n->name.find(prefixShoulder_2) != string::npos
			|| n->name.find(prefixUpArm) != string::npos)
		{
			joint[2] = n; 
			k[2] = 3.5;
			isInit = true;
			//break;
		}
	}

	if (isInit) {

		if (joint[0] != nullptr)
		{
			dir_x = joint[0]->offset[0];
			dir_y = joint[0]->offset[1];
			dir_z = joint[0]->offset[2];
			layer = k[0];

		}
		else if (joint[1] != nullptr)
		{
			dir_x = joint[1]->offset[0];
			dir_y = joint[1]->offset[1];
			dir_z = joint[1]->offset[2];
			layer = k[1];
		}
		else if (joint[2] != nullptr)
		{
			dir_x = joint[2]->offset[0];
			dir_y = joint[2]->offset[1];
			dir_z = joint[2]->offset[2];
			layer = k[2];
		}

		lenght = sqrt(dir_x*dir_x + dir_y*dir_y + dir_z*dir_z);
	}

	if (lenght > 0.0)
	{
		scale = (float)((SCALE*layer) / lenght);
		if (scale < 0.0)
			scale *= (-1.0);
	}
	else
		scale = 0.02;
#if DEBUG
	myfile_1.open("C:\\Temp\\exampleMy.txt", ios::out | ios::app);
	myfile_1 << "\n";
	myfile_1 << BVH::file_name;
	myfile_1 << std::fixed << std::setprecision(8) << (double)(joints.size());
	for (auto n : joints)
	{
		myfile_1 << n->name;
		myfile_1 << "\n";
	}
	myfile_1 << "\n\n";
	myfile_1.close();
#endif
	//delete[] joint;

	return true;
}

// Draw attitude of specified frame
void  BVH::RenderFigure(const int frame_no, float scale)
{
	//////////////////////////////////////////////

	const double * data(motion + frame_no * num_channel);

	gluLookAt(1.9 + data[0] * scale,
		1.5 + data[1] * scale,
		2.1 + data[2] * scale,
		(data[0] * scale),
		(data[1] * scale),
		(data[2] * scale),
		0.0, 1.0, 0.0);

	/////////////////////////////////////////////
// BVH drawing with skeleton / posture specified
	RenderFigure(joints[0], data, scale);

}

//static const string JointName[] = { "UNKNOWN", "VALID", "VALID_CHECK_DATABASE", "NOTVALID", "NOTVALID_CHECK_DATABASE" };


// Рисование указанного скелета / позы BVH (функция класса)
// Drawing the specified BVH skeleton / posture (class function)
void  BVH::RenderFigure(const Joint * joint, const double * data, float scale)
{

	//if (joint->parent != nullptr && joint->parent->name.find(prefixHand) != string::npos)
	if (joint->name.find(prefixHand) != string::npos || (joint->parent != nullptr && joint->parent->name.find(prefixHead) != string::npos || joint->name.find(prefixToes) != string::npos))
		return;

	glPushMatrix();

	// Apply translation for root joint
	if (joint->parent == NULL)
	{
		glTranslatef((data[0] * scale), (data[1] * scale), (data[2] * scale));
	}
	// In case of a child joint, apply translation from the parent joint
	else
	{
		glTranslatef((joint->offset[0] * scale), (joint->offset[1] * scale), (joint->offset[2] * scale));
	}

	string name;
	if (joint->name.substr(0, prefixLeft.size()) == prefixLeft || joint->name.substr(0, prefixLeft2.size()) == prefixLeft2)
	{
		name = prefixLeft;
		glColor3f(0.33f, 0.9f, 0.33f);
	}
	else if (joint->name.substr(0, prefixRight.size()) == prefixRight || joint->name.substr(0, prefixRight2.size()) == prefixRight2)
	{
		name = prefixRight;
		glColor3f(0.33f, 0.33f, 0.9f);
	}
	else if (joint->name.substr(0, prefixHead.size()) == prefixHead)
	{
		name = prefixHead;
		glColor3f(0.85f, 1.0f, 1.0f);
	}
	else if (joint->name.find(prefixHip) != string::npos)
	{
		name = prefixHip;
		glColor3f(1.0f, 0.97f, 0.4f);
	}
	else
	{
		glColor3f(0.2f, 0.82f, 0.96f);
		if (joint->name.find(prefixChest) != string::npos)
			name = prefixChest;
	}

	// Apply rotation from parent joint (rotation from world coordinates in case of root joint)
	UINT32  i;
	for (i = 0; i < joint->channels.size(); i++)
	{
		Channel *  channel = joint->channels[i];
		if (channel->type == X_ROTATION)
			glRotatef(data[channel->index], 1.0f, 0.0f, 0.0f);
		else if (channel->type == Y_ROTATION)
			glRotatef(data[channel->index], 0.0f, 1.0f, 0.0f);
		else if (channel->type == Z_ROTATION)
			glRotatef(data[channel->index], 0.0f, 0.0f, 1.0f);
	}

	//Draw link
	// draw the link from the origin of the joint coordinate system to the end point
	if (joint->children.size() == 0)
	{
		RenderBone(name, 0.0f, 0.0f, 0.0f, (joint->site[0] * scale), (joint->site[1] * scale), (joint->site[2] * scale), false);
	}
	// Draw a link from the origin of the joint coordinate system to the connection position to the next joint
	else if (joint->children.size() == 1)
	{
		Joint *  child = joint->children[0];
		RenderBone(name, 0.0f, 0.0f, 0.0f, (child->offset[0] * scale), (child->offset[1] * scale), (child->offset[2] * scale));
	}
	// Draw a cylinder from the center point to the connection position to all joints to the connection position to each joint
	else if (joint->children.size() > 1 && joint->name.find(prefixHead) == string::npos)
	{
		// Calculate the center point to the connection point to the origin and all joints
		// Вычисляем центральную точку подключения к исходной точке и всем соединениям
		float  center[3] = { 0.0f, 0.0f, 0.0f };
		for (i = 0; i < joint->children.size(); i++)
		{
			Joint *  child = joint->children[i];
			center[0] += child->offset[0];
			center[1] += child->offset[1];
			center[2] += child->offset[2];
		}
		center[0] /= joint->children.size() + 1;
		center[1] /= joint->children.size() + 1;
		center[2] /= joint->children.size() + 1;

		// Draw a link from the origin to the center point
		//Нарисуйте связь от начала координат до центральной точки
		RenderBone(name, 0.0f, 0.0f, 0.0f, center[0] * scale, center[1] * scale, center[2] * scale);

		//ofstream myfile;
		//myfile.open("C:\\Temp\\example.txt", ios::out | ios::app);
		//myfile << joint->name + "\n";
		//myfile.close();
		// Draw a link from the center point to the next joint position
		for (i = 0; i < joint->children.size(); i++)
		{
			Joint *  child = joint->children[i];

			RenderBone(name, center[0] * scale, center[1] * scale, center[2] * scale,
				(child->offset[0] * scale), (child->offset[1] * scale),
				(child->offset[2] * scale));

		}
		//myfile.close();
	}

	if (joint->name.find(prefixHead) == string::npos && joint->name != prefixHead)
	{
		// Recursive calls to child joints
		for (i = 0; i < joint->children.size(); i++)
		{
			RenderFigure(joint->children[i], data, scale);
		}
	}
	glPopMatrix();
}

// Draw a single link of BVH skeleton (class function)
void  BVH::RenderBone(const string &name, const float x0, const float y0, const float z0, const float x1, const float y1, const float z1, const bool drawJointSphere)
{


	// draw a cylinder connecting the given two points
	// Convert the information of the two end points of the cylinder into information of origin, orientation and length
	GLdouble  dir_x = x1 - x0;
	GLdouble  dir_y = y1 - y0;
	GLdouble  dir_z = z1 - z0;

	const GLdouble  bone_length = sqrt(dir_x*dir_x + dir_y*dir_y + dir_z*dir_z);

	// Drawing parameter setting
	//glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	//glEnable(GL_LINE_SMOOTH);
	//glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	//glEnable(GL_POLYGON_SMOOTH);
	//glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);static

	//glBegin(GL_POLYGON);
	static GLUquadricObj *  quad_obj = NULL;
	if (quad_obj == NULL) {
		// Setup the quadric object
		quad_obj = gluNewQuadric();
		//gluQuadricDrawStyle(quad_obj, GLU_FILL);
		//gluQuadricDrawStyle(quad_obj, GLU_FILL);GLU_POINT| GLU_LINE| GLU_FILL|GLU_SILHOUETTE
		gluQuadricDrawStyle(quad_obj, GLU_FILL);
		gluQuadricNormals(quad_obj, GLU_SMOOTH);
		gluQuadricOrientation(quad_obj, GLU_OUTSIDE);
		gluQuadricTexture(quad_obj, GL_FALSE);
	}

	glPushMatrix();

	// Set translation
	glTranslated(x0, y0, z0);

	// Below, calculate the matrix representing the rotation of the cylinder

	double length = sqrt(dir_x*dir_x + dir_y*dir_y + dir_z*dir_z);
	if (length < 0.0001) {
		dir_x = 0.0;
		dir_y = 0.0;
		dir_z = 1.0;

		length = 1.0;
	}
	dir_x /= length;  dir_y /= length;  dir_z /= length;

	// Set the direction of the y axis to be the reference
	GLdouble up_x = 0.0;
	GLdouble up_y = 1.0;
	GLdouble up_z = 0.0;

	// Calculate x-axis orientation from z-axis and y-axis cross product
	double side_x = up_y * dir_z - up_z * dir_y;
	double side_y = up_z * dir_x - up_x * dir_z;
	double side_z = up_x * dir_y - up_y * dir_x;

	// Normalize x axis to unit vector
	length = sqrt(side_x*side_x + side_y*side_y + side_z*side_z);
	if (length < 0.0001) {
		side_x = 1.0;
		side_y = 0.0;
		side_z = 0.0;

		length = 1.0;
	}
	side_x /= length;  side_y /= length;  side_z /= length;

	// Calculate direction of y axis from cross product of z axis and x axis
	up_x = dir_y * side_z - dir_z * side_y;
	up_y = dir_z * side_x - dir_x * side_z;
	up_z = dir_x * side_y - dir_y * side_x;

	// Set rotation matrix
	GLdouble  m[16] = { side_x, side_y, side_z, 0.0,
		up_x,   up_y,   up_z,   0.0,
		dir_x,  dir_y,  dir_z,  0.0,
		0.0,    0.0,    0.0,    1.0 };

	glMultMatrixd(m);

	// Cylinder setting
	const GLdouble radiusB = 0.045; // Thickness of the cylinder
	const GLdouble radiusT = 0.025; // Thickness of the cylinder
	const GLdouble slices = 8.0; // radial subdivision number of cylinder (default 12)
	const GLdouble stack = 3.0;  // Number of subdivisions of a circular cylinder (default 1)

	if (name == prefixHead)
	{
		// опустить голову чуть ниже
		//glTranslated(x0, y0, z0 - bone_length / 8);

		const auto radius1 = radiusB * 1.8;
		const auto radius2 = radiusB*1.5;

		const auto height = radius1 * 1.75;
		gluCylinder(quad_obj, radius2, radius1, height, slices, stack);

		// Закрываю дыры в цилиндре
		gluCylinder(quad_obj, 0, radius2, 0.0, slices, stack);

		glTranslated(x0, y0, z0 + height);

		gluSphere(quad_obj, radius1, 9, 9);
	}
	else
	{
		// Draw a cylinder
		if (name == prefixChest)
			gluCylinder(quad_obj, radiusT, radiusT, bone_length, slices, stack);
		else
			gluCylinder(quad_obj, radiusB, radiusT, bone_length, slices, stack);

		// Если нарисовать - то выглядит как хвост
		if (name != prefixHip && drawJointSphere)
			gluSphere(quad_obj, 0.04, 9, stack);
	}


	glPopMatrix();
}



// End of BVH.cpp
