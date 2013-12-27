#include "loaders.h"
#include <iostream>
#include <fstream>

using namespace std;

inline int StrToInt(const string &str)
{
	int i;
	if (sscanf_s(str.c_str(), "%i", &i) == 1)
		return i;
	else return 0;
}

inline vector<string> split_string(const string& str,const string& split_str) 
{
	vector<string> stlv_string;
	string part_string("");
	string::size_type i;
	
	i=0;
	while(i < str.size()) 
	{
		if(split_str.find(str[i]) != string::npos) 
		{
			stlv_string.push_back(part_string);
			part_string="";
			while(split_str.find(str[i]) != string::npos) 
			{
				++i;
			}
		}
		else 
		{
			part_string += str[i];
			++i;
		}
	}
	if (!part_string.empty())
		stlv_string.push_back(part_string);
	return stlv_string;
}

Mesh* ObjLoader::load(const char* file)
{
	ifstream infile(file);
	if(infile.fail())
	{
		cout<<"Error opening file " << file;
		return false;
	}

	Mesh* mesh = new Mesh;

	char current_line[1024];

	while (!infile.eof())
	{
		infile.getline(current_line, 1024);

		switch (current_line[0])
		{		
		case 'v':
			{
				float x, y, z;
				switch(current_line[1])
				{
				case 'n':
					sscanf_s(current_line, "vn %f %f %f", &x,&y,&z);
					mesh->m_n.push_back(Vec3f(x, y, z));
					break;

				case 't':
					//sscanf(current_line, "vt %f %f %f", &x,&y,&z); // z component is not used
					sscanf_s(current_line, "vt %f %f", &x,&y); // z component is not used
					mesh->m_texc.push_back(Vec2f(x, y));
					break;

				default:
					sscanf_s(current_line, "v %f %f %f", &x,&y,&z);
					mesh->m_v.push_back(Vec3f(x, y, z));
					break;
				}
			}
			break;

		case 'f':
			{
				vector<string> faces = split_string(current_line, " \t.\r\n");

				int vnt[3][3] = {{-1, -1, -1},{-1, -1, -1},{-1, -1, -1}};
				string::size_type begin, end;

				for(int i = 0;i < 2;i++)
				{
					//cout<<"f: "<<i<<" "<<faces[i+1]<<endl;
					begin = 0;
					int j = 0;
					do
					{
						end = faces[i+1].find_first_of("/", begin);

						if(begin < end)
						{
							vnt[i][j] = StrToInt(faces[i+1].substr(begin, end-begin))-1;
						}
						else
							vnt[i][j] = -1;
						begin = end+1;
						j++;
					}while(end != string::npos);
				}

				for(unsigned int i = 3;i < faces.size();i++)
				{
					//cout<<"f: "<<i-1<<" "<<faces[i]<<endl;
					begin = 0;
					int j = 0;
					do
					{
						end = faces[i].find_first_of("/", begin);

						if(begin < end)
						{
							vnt[2][j] = StrToInt(faces[i].substr(begin, end-begin))-1;
						}
						else
							vnt[2][j] = -1;
						begin = end+1;
						j++;

					}while(end != string::npos);

					for(int j = 0;j < 3;j++)
					{
						//cout<<"\t"<<vnt[j][0]<<" "<<vnt[j][1]<<" "<<vnt[j][2]<<"\n";
						mesh->m_vi.push_back(vnt[j][0]);

						if(vnt[j][1] != -1) mesh->m_ti.push_back(vnt[j][1]);
						if(vnt[j][2] != -1) mesh->m_ni.push_back(vnt[j][2]);
					}

					memcpy(&vnt[1], &vnt[2], sizeof(int)*3);
				}
			}
			break;
			
		default: break;
		}
	}

	return mesh;
}

//bool ObjLoader::loadMtl(const char* file, STL_VEC<RefPtr<Material> >& mat_set)
//{
//	ifstream infile(file);
//	if(infile.fail())
//	{
//		OUT1("Error opening file " << file);
//		return false;
//	}
//
//	char current_line[1024];
//
//	int line = 0;
//	int cur_m = -1;
//
//	while (!infile.eof())
//	{
//		infile.getline(current_line, 1024);
//		line ++;
//		switch (current_line[0])
//		{
//		case 'n':
//			{
//				STL_VEC<string> tokens = split_string(current_line, " \t\r\n");
//				if(tokens[0] == "newmtl")
//				{
//					Material* mat = new Material;
//					mat_set.push_back(mat);
//
//					OUT1("Material: "<<tokens[1]);
//					mat->m_name = tokens[1]; // no verification
//					cur_m = mat_set.size() - 1;
//				}
//			}
//			break;
//
//		case 'K':
//			{
//				float x, y, z;
//
//				switch(current_line[1])
//				{
//				case 'a':
//					sscanf(current_line, "Ka %f %f %f", &x,&y,&z);					
//					mat_set[cur_m]->m_ambient = Color4f(x, y, z, 1.0f);
//
//					break;
//				case 'd':
//					sscanf(current_line, "Kd %f %f %f", &x,&y,&z); // z component is not used
//					mat_set[cur_m]->m_diffuse = Color4f(x, y, z, 1.0f);
//
//					break;
//				case 's':
//					sscanf(current_line, "Ks %f %f %f", &x,&y,&z);
//					mat_set[cur_m]->m_specular = Color4f(x, y, z, 1.0f);
//
//					break;
//
//				case 'e':
//					sscanf(current_line, "Ke %f %f %f", &x,&y,&z);
//					mat_set[cur_m]->m_emissive = Color4f(x, y, z, 1.0f);
//
//					break;
//
//				default:
//					break;
//				}
//			}
//			break;
//
//		case 'm':
//			{
//
//			}
//			break;
//
//		case 'l':
//			{
//				float power;
//				sscanf(current_line, "l %f", &power);
//				mat_set[cur_m]->m_light_power = power;
//			}
//			break;
//
//		case 'N':
//			{
//				switch(current_line[1])
//				{
//				case 's':
//					{
//						float ns;
//						sscanf(current_line, "Ns %f", &ns);						
//						mat_set[cur_m]->m_shinness = ns;
//					}
//
//					break;
//
//				default:
//					break;
//				}
//			}
//			break;
//
//		default: break;
//		}
//	}
//
//	return true;
//}