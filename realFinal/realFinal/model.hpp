#include<vector>
#include<string>
#include<fstream>
#include<iostream>

using namespace std;

class CPoint2f {
public:
	vector<float> d = vector<float>{ 0, 0 };
};

class CPoint3f {
public:
	vector<float> d = vector<float>{ 0, 0, 0 };
};

class CPoint3i {
public:
	vector<int> d = vector<int>{ 0, 0, 0 };
};

class CFace {
public:
	vector <CPoint3i> v_pairs;
};

class CObj {
public:
	string name;
	vector <CPoint3f> v;
	vector <CPoint2f> vt;
	vector <CPoint3f> vn;
	vector <CFace> f;
};

class CModel {
public:
	vector <CObj> objs;

	vector <float> my_strtok_f(char* str, char* delimeter) {
		vector <float> v;
		char* context;
		char* tok = strtok_s(str, delimeter, &context);

		while (tok != NULL) {
			v.push_back(atof(tok));
			tok = strtok_s(context, delimeter, &context);
		}

		return v;
	}

	vector <string> my_strtok_s(char* str, char* delimeter) {
		vector <string> v;
		char* context;
		char* tok = strtok_s(str, delimeter, &context);

		while (tok != NULL) {
			v.push_back(tok);
			tok = strtok_s(context, delimeter, &context);
		}

		return v;
	}

	vector <int> my_strtok_i(char* str, char* delimeter) {
		vector <int> v;
		char* context;
		char* tok = strtok_s(str, delimeter, &context);

		while (tok != NULL) {
			v.push_back(atoi(tok));
			tok = strtok_s(context, delimeter, &context);
		}

		return v;
	}

	void loadObj(ifstream& fin) {
		string line;
		CObj obj_tmp;
		int cnt = 0;
		//이전 obj까지의 누적 벡터 수
		int cnt_prev_vertex = 0;
		int cnt_prev_texture = 0;
		int cnt_prev_normal = 0;

		while (getline(fin, line)) {
			int len = line.length();
			vector <float> vf;
			vector <string> s;
			vector <int> vi;
			CPoint3f p3;
			CPoint2f p2;
			CPoint3i p3i;

			if (line[0] == 'o' && line[1] == ' ') {
				obj_tmp.name = line.substr(2, len - 2);
				objs.push_back(obj_tmp);
				if (cnt > 0) {
					cnt_prev_vertex += objs[cnt - 1].v.size();
					cnt_prev_texture += objs[cnt - 1].vt.size();
					cnt_prev_normal += objs[cnt - 1].vn.size();
				}
				cnt += 1;
			}

			if (line[0] == 'v' && line[1] == ' ') {
				vf = my_strtok_f((char*)line.substr(2, len - 2).c_str(), (char*)" ");
				p3.d = { vf[0], vf[1], vf[2] };
				objs[cnt - 1].v.push_back(p3);
			}

			else if (line[0] == 'v' && line[1] == 't') {
				vf = my_strtok_f((char*)line.substr(3, len - 2).c_str(), (char*)" ");
				p2.d = { vf[0], vf[1] };
				objs[cnt - 1].vt.push_back(p2);
			}

			else if (line[0] == 'v' && line[1] == 'n') {
				vf = my_strtok_f((char*)line.substr(3, len - 2).c_str(), (char*)" ");
				p3.d = { vf[0], vf[1], vf[2] };
				objs[cnt - 1].vn.push_back(p3);
			}

			else if (line[0] == 'f' && line[1] == ' ') {
				s = my_strtok_s((char*)line.substr(2, len - 2).c_str(), (char*)" ");
				int nVertexes = s.size();
				CFace face_tmp;
				for (int i = 0; i < nVertexes; i++) {
					vi = my_strtok_i((char*)s[i].c_str(), (char*)"/");
					p3i.d = { vi[0] - cnt_prev_vertex, vi[1] - cnt_prev_texture, vi[2] - cnt_prev_normal };
					face_tmp.v_pairs.push_back(p3i);
				}

				objs[cnt - 1].f.push_back(face_tmp);
			}
		}
	}

};
