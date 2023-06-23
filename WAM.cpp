#include<iostream>
#include<vector>
#include<io.h>
#include<fstream>
#include<string.h>
#include<stdlib.h>
#include<algorithm>
#include<cmath>
#include<cstdlib>
#include<ctime>
#define A 0
#define C 1
#define T 2
#define G 3
#define AA 0
#define AC 1
#define AT 2
#define AG 3
#define CA 4
#define CC 5
#define CT 6
#define CG 7
#define TA 8
#define TC 9
#define TT 10
#define TG 11
#define GA 12
#define GC 13
#define GT 14
#define GG 15
#define minus_2 0
#define minus_1 1
#define zero 2
#define plus_1 3
#define plus_2 4
#define plus_3 5
#define plus_4 6
#define plus_5 7
using namespace std;

typedef struct node
{
	string data;
	node *next;
}node;
typedef node *LinkList;

LinkList findfile(string path,string mode)
{
	_finddata_t file;
	intptr_t HANDLE;
	string Onepath = path + mode;
	HANDLE = _findfirst(Onepath.c_str(), &file);
	if (HANDLE == -1L)
	{
		cout << "can not match the folder path" << endl;
		system("pause");
	}
	node *name_list, *p;
	name_list = new(node);
	p = name_list;
	do {
		if (file.attrib & _A_SUBDIR)
		{
			if ((strcmp(file.name, ".") != 0) && (strcmp(file.name, "..") != 0))
			{
				string newPath = path +"\\" + file.name;
				findfile(newPath,mode);
			}
		}
		else
		{
			p->next = new(node);
			p->next->data = file.name;
			p = p->next;
		//cout << file.name << " " << endl;
		}
	} while (_findnext(HANDLE, &file) == 0);
	p->next = NULL;
	_findclose(HANDLE);
	return name_list;
}

void display(LinkList L)
{
    node *p;
    p=L->next;
    while (p!=NULL)
    {
        cout<<p->data;
        p=p->next;
		cout<<endl;
    }
    printf("\n");
}

LinkList read_files(string relative_path, LinkList path)
{
	node *path_p, *txt_list, *txt_p;
	txt_list = new(node);
	txt_p = txt_list;
	path_p = path->next;
	while(path_p != NULL)
	{
  		ifstream infile;
		string newpath = relative_path + "\\" + path_p->data;
		char filename[80];
		strcpy(filename, newpath.c_str());
		//cout<<newpath<<endl;
		infile.open(filename);
  		if (!infile)
		{
			cout<<"Failed to open the file: "<<path_p->data<<endl;
			exit(1);
		}
		string lines;
		char line[500];
		while(infile.getline(line, 500, '\n'))
		{
			lines = lines + line;
		}
		txt_p->next = new(node);
		txt_p->next->data = lines;
		txt_p = txt_p->next;
  		infile.close();
  		path_p = path_p->next;
	}
	txt_p->next = NULL;
	return txt_list;
}

typedef struct sample_node
{
	int position, label,result;
	string name, sequence;
	long double score;
	sample_node *next;
}sample_node;
typedef sample_node *sample_LinkList;

sample_LinkList read_sample_sequences(LinkList file_list, LinkList txt_list)
{
	node *txt_p, *file_p;
	sample_node *seq_list, *seq_p;
	file_p = file_list->next;
	txt_p = txt_list->next;
	seq_list = new(sample_node);
	seq_p = seq_list;
	while(txt_p!=NULL)
	{
		int pos_left_bracket = txt_p->data.find('(');
		int pos_right_bracket = txt_p->data.find(')');
		int pos_zero;
		string CDSjoins = txt_p->data.substr(pos_left_bracket+1, pos_right_bracket-pos_left_bracket-1);
		string whole_sequence = txt_p->data.substr(pos_right_bracket+1);
		int *position_positive = new int[100];
		int index_pos_pos = 0;
		//cout<<CDSjoins<<endl<<txt_p->data<<endl;
		while(CDSjoins.find('.')!=-1)
		{
			int pos_comma = CDSjoins.find(',');
			int pos_double_period = CDSjoins.find('.');
			if(pos_comma != -1)
			{
				pos_zero = atoi(CDSjoins.substr(pos_double_period+2, pos_comma-1).c_str());
				CDSjoins = CDSjoins.substr(pos_comma+1);
			}
			else
			{
				pos_zero = atoi(CDSjoins.substr(pos_double_period+2).c_str());
				CDSjoins = CDSjoins.substr(pos_double_period+2);
			}
			position_positive[index_pos_pos] = pos_zero;
			//cout<<pos_zero<<endl;
			index_pos_pos += 1;
		}
		/*
		for(int j=0; j < index_pos_pos;++j)
		{
			cout<<position_positive[j]<<endl;
		}
		*/
		int sequence_length = strlen(whole_sequence.c_str())-5;
		int index = 0;
		
		//shuffle select same amount negative samples as positive samples
		//bool exist = 1;
		//cout<<"index_pos_pos  "<<index_pos_pos<<endl;
		/*
		for(int i=0; i < index_pos_pos; ++i)
		{
			if(!i && exist)//first one not positive
			{
				//cout<<"1"<<endl;
				index = rand()%(position_positive[i]-3) + 3;
				//index = position_positive[i] - 5;
				--i;
				exist = 0;
			}
			else if(exist && i==index_pos_pos-1)//last one not positive
			{
				//cout<<"2"<<endl;
				//index = rand()%(sequence_length-position_positive[i]) + position_positive[i] + 1;
				//exist = 0;
				break;
			}
			else if(exist)//if previous one is positive, get a negative one
			{
				//cout<<"3"<<endl;
				index = rand()%(position_positive[i+1]-position_positive[i]) + position_positive[i] + 1;
				//index = position_positive[i] + 5;
				--i;
				exist = 0;
			}
			else//if previous one is negative, get a positive one
			{
				//cout<<"4"<<endl;
				index = position_positive[i];
				exist = 1;
			}
			seq_p->next = new(sample_node);
			seq_p->next->sequence = whole_sequence.substr(index-3, 9);
			transform(seq_p->next->sequence.begin(), seq_p->next->sequence.end(), seq_p->next->sequence.begin(), ::toupper);
   			if(exist) seq_p->next->label = 1; else seq_p->next->label = 0;
			seq_p->next->position = index;
			seq_p->next->name = file_p->data;
			//cout<<i<<"  "<<seq_p->next->data<<"  "<<endl;
			seq_p = seq_p->next;
		}
		*/
		
		//select all negative sites
		for(int i=3; i < sequence_length; ++i)
		{
			bool exist = 0;
			for(int j = 0; j < index_pos_pos; ++j)
			{
				if(i == position_positive[j]) exist = 1;
			}
			seq_p->next = new(sample_node);
			seq_p->next->sequence = whole_sequence.substr(i-3, 9);
			cout<<seq_p->next->sequence<<endl;
			transform(seq_p->next->sequence.begin(), seq_p->next->sequence.end(), seq_p->next->sequence.begin(), ::toupper);
   			if(exist) seq_p->next->label = 1; else seq_p->next->label = 0;
			seq_p->next->position = i;
			seq_p->next->name = file_p->data;
			//cout<<i<<"  "<<seq_p->next->data<<"  "<<endl;
			seq_p = seq_p->next;
		}
		delete []position_positive;
		txt_p = txt_p->next;
		file_p = file_p->next;
	}
	seq_p->next = NULL;
	return seq_list;
}

void write_in_Sn_Sp(const long double Sn_list[],const long double Sp_list[], int length)
{
	ofstream outfile(".\\WAM_Sn_Sp.txt");
	outfile<<"Sn\tSp"<<endl;
	for(int i = 0; i < length; ++i)
	{
		outfile<<Sn_list[i]<<"\t"<<Sp_list[i]<<endl;
	}
	outfile.close();
}

class WAM
{
		long double pos_WAM_minus_3[4], pos_WAM_array[8][16], neg_WAM_minus_3[4], neg_WAM_array[8][16];
		long double highest_threshold, high_threshold, medium_threshold, low_threshold;
		long double Sn, Sp;
    public:
		void create_WAM(sample_LinkList seq_list);
		void process_training_data(sample_LinkList seq_list);
		void calculate_threshold(sample_LinkList seq_list);
		long double calculate_Sn_and_Sp(sample_LinkList seq_list, char threshold, long double thr);
		void predict(sample_LinkList seq_list);
		void display_result(sample_LinkList seq_list);
		void draw_Sn_Sp(sample_LinkList train_seq_list, sample_LinkList test_seq_list);
};

void WAM::create_WAM(sample_LinkList seq_list)
{
	sample_node *seq_p;
	seq_p = seq_list->next;
	long double minus_3_A, minus_3_C, minus_3_T, minus_3_G, count;
	//positive
	minus_3_A=minus_3_C=minus_3_T=minus_3_G=count=0;
	while(seq_p!=NULL)
	{
		if(!seq_p->label)
		{
			seq_p = seq_p->next;
			continue;
		}
		switch(seq_p->sequence[0])
		{
			case 'A':
				minus_3_A += 1;break;
			case 'C':
				minus_3_C += 1;break;
			case 'T':
				minus_3_T += 1;break;
			case 'G':
				minus_3_G += 1;break;
		}
		seq_p = seq_p->next;
		count += 1;
	}
	pos_WAM_minus_3[A] = minus_3_A/count;
	pos_WAM_minus_3[C] = minus_3_C/count;
	pos_WAM_minus_3[T] = minus_3_T/count;
	pos_WAM_minus_3[G] = minus_3_G/count;
 	/*
	for(int i=0; i<4; ++i)
	{
		cout<<pos_WAM_minus_3[i]<<endl;
	}
	//*/
	for(int i=0; i<8; ++i)
	{
		long double nAA, nAC, nAT, nAG, nCA, nCC, nCT, nCG, nTA, nTC, nTT, nTG, nGA, nGC, nGT, nGG;
		nAA=nAC=nAT=nAG=nCA=nCC=nCT=nCG=nTA=nTC=nTT=nTG=nGA=nGC=nGT=nGG=0;
		seq_p = seq_list->next;
		while(seq_p!=NULL)
		{
			if(!seq_p->label)
			{
				seq_p = seq_p->next;
				continue;
			}
			switch(seq_p->sequence[i])
			{
   				case 'A':
   					switch(seq_p->sequence[i+1])
   					{
						case 'A':
							nAA += 1;break;
						case 'C':
							nAC += 1;break;
						case 'T':
							nAT += 1;break;
						case 'G':
							nAG += 1;break;
					}
					break;
				case 'C':
   					switch(seq_p->sequence[i+1])
   					{
						case 'A':
							nCA += 1;break;
						case 'C':
							nCC += 1;break;
						case 'T':
							nCT += 1;break;
						case 'G':
							nCG += 1;break;
					}
					break;
				case 'T':
   					switch(seq_p->sequence[i+1])
   					{
						case 'A':
							nTA += 1;break;
						case 'C':
							nTC += 1;break;
						case 'T':
							nTT += 1;break;
						case 'G':
							nTG += 1;break;
					}
					break;
				case 'G':
   					switch(seq_p->sequence[i+1])
   					{
						case 'A':
							nGA += 1;break;
						case 'C':
							nGC += 1;break;
						case 'T':
							nGT += 1;break;
						case 'G':
							nGG += 1;break;
					}
					break;
			}
			seq_p = seq_p->next;
		}
		pos_WAM_array[i][AA] = nAA/count;
		pos_WAM_array[i][AC] = nAC/count;
		pos_WAM_array[i][AT] = nAT/count;
		pos_WAM_array[i][AG] = nAG/count;
		pos_WAM_array[i][CA] = nCA/count;
		pos_WAM_array[i][CC] = nCC/count;
		pos_WAM_array[i][CT] = nCT/count;
		pos_WAM_array[i][CG] = nCG/count;
		pos_WAM_array[i][TA] = nTA/count;
		pos_WAM_array[i][TC] = nTC/count;
		pos_WAM_array[i][TT] = nTT/count;
		pos_WAM_array[i][TG] = nTG/count;
		pos_WAM_array[i][GA] = nGA/count;
		pos_WAM_array[i][GC] = nGC/count;
		pos_WAM_array[i][GT] = nGT/count;
		pos_WAM_array[i][GG] = nGG/count;
	}
	/*
	for(int i=0; i<8; ++i)
 	{
		for(int j=0; j<16; ++j)
		{
			cout<<i<<"  "<<j<<"  "<<pos_WAM_array[i][j]<<endl;
		}
	}
	//*/
	//negative
	minus_3_A=minus_3_C=minus_3_T=minus_3_G=count=0;
	seq_p = seq_list->next;
	while(seq_p!=NULL)
	{
		if(seq_p->label)
		{
			seq_p = seq_p->next;
			continue;
		}
		switch(seq_p->sequence[0])
		{
			case 'A':
				minus_3_A += 1;break;
			case 'C':
				minus_3_C += 1;break;
			case 'T':
				minus_3_T += 1;break;
			case 'G':
				minus_3_G += 1;break;
		}
		seq_p = seq_p->next;
		count += 1;
	}
	neg_WAM_minus_3[A] = minus_3_A/count;
	neg_WAM_minus_3[C] = minus_3_C/count;
	neg_WAM_minus_3[T] = minus_3_T/count;
	neg_WAM_minus_3[G] = minus_3_G/count;
	/*
	for(int i=0; i<4; ++i)
	{
		cout<<neg_WAM_minus_3[i]<<endl;
	}
	//*/
	for(int i=0; i<8; ++i)
	{
		long double nAA, nAC, nAT, nAG, nCA, nCC, nCT, nCG, nTA, nTC, nTT, nTG, nGA, nGC, nGT, nGG;
		nAA=nAC=nAT=nAG=nCA=nCC=nCT=nCG=nTA=nTC=nTT=nTG=nGA=nGC=nGT=nGG=0;
		seq_p = seq_list->next;
		while(seq_p!=NULL)
		{
			if(seq_p->label)
			{
				seq_p = seq_p->next;
				continue;
			}
			switch(seq_p->sequence[i])
			{
   				case 'A':
   					switch(seq_p->sequence[i+1])
   					{
						case 'A':
							nAA += 1;break;
						case 'C':
							nAC += 1;break;
						case 'T':
							nAT += 1;break;
						case 'G':
							nAG += 1;break;
					}
					break;
				case 'C':
   					switch(seq_p->sequence[i+1])
   					{
						case 'A':
							nCA += 1;break;
						case 'C':
							nCC += 1;break;
						case 'T':
							nCT += 1;break;
						case 'G':
							nCG += 1;break;
					}
					break;
				case 'T':
   					switch(seq_p->sequence[i+1])
   					{
						case 'A':
							nTA += 1;break;
						case 'C':
							nTC += 1;break;
						case 'T':
							nTT += 1;break;
						case 'G':
							nTG += 1;break;
					}
					break;
				case 'G':
   					switch(seq_p->sequence[i+1])
   					{
						case 'A':
							nGA += 1;break;
						case 'C':
							nGC += 1;break;
						case 'T':
							nGT += 1;break;
						case 'G':
							nGG += 1;break;
					}
					break;
			}
			seq_p = seq_p->next;
		}
		neg_WAM_array[i][AA] = nAA/count;
		neg_WAM_array[i][AC] = nAC/count;
		neg_WAM_array[i][AT] = nAT/count;
		neg_WAM_array[i][AG] = nAG/count;
		neg_WAM_array[i][CA] = nCA/count;
		neg_WAM_array[i][CC] = nCC/count;
		neg_WAM_array[i][CT] = nCT/count;
		neg_WAM_array[i][CG] = nCG/count;
		neg_WAM_array[i][TA] = nTA/count;
		neg_WAM_array[i][TC] = nTC/count;
		neg_WAM_array[i][TT] = nTT/count;
		neg_WAM_array[i][TG] = nTG/count;
		neg_WAM_array[i][GA] = nGA/count;
		neg_WAM_array[i][GC] = nGC/count;
		neg_WAM_array[i][GT] = nGT/count;
		neg_WAM_array[i][GG] = nGG/count;
	}
	/*
	for(int i=0; i<8; ++i)
 	{
		for(int j=0; j<16; ++j)
		{
			cout<<i<<"  "<<j<<"  "<<neg_WAM_array[i][j]<<endl;
		}
	}
	//*/
}

void WAM::process_training_data(sample_LinkList seq_list)
{
	sample_node *seq_p;
	seq_p = seq_list->next;
	while(seq_p!=NULL)
	{
		long double score;
		score = log(pos_WAM_minus_3[seq_p->sequence[0]]/neg_WAM_minus_3[seq_p->sequence[0]]);
		for(int i=0; i<8; ++i)
		{
			long double pos_WAM_value, neg_WAM_value;
			switch(seq_p->sequence[i])
			{
				case 'A':
					switch(seq_p->sequence[i+1])
					{
						case 'A':
							pos_WAM_value = pos_WAM_array[i][AA];
							neg_WAM_value = neg_WAM_array[i][AA];
							break;
						case 'C':
							pos_WAM_value = pos_WAM_array[i][AC];
							neg_WAM_value = neg_WAM_array[i][AC];
							break;
						case 'T':
							pos_WAM_value = pos_WAM_array[i][AT];
							neg_WAM_value = neg_WAM_array[i][AT];
							break;
						case 'G':
							pos_WAM_value = pos_WAM_array[i][AG];
							neg_WAM_value = neg_WAM_array[i][AG];
							break;
					}
					break;
				case 'C':
					switch(seq_p->sequence[i+1])
					{
						case 'A':
							pos_WAM_value = pos_WAM_array[i][CA];
							neg_WAM_value = neg_WAM_array[i][CA];
							break;
						case 'C':
							pos_WAM_value = pos_WAM_array[i][CC];
							neg_WAM_value = neg_WAM_array[i][CC];
							break;
						case 'T':
							pos_WAM_value = pos_WAM_array[i][CT];
							neg_WAM_value = neg_WAM_array[i][CT];
							break;
						case 'G':
							pos_WAM_value = pos_WAM_array[i][CG];
							neg_WAM_value = neg_WAM_array[i][CG];
							break;
					}
					break;
				case 'T':
					switch(seq_p->sequence[i+1])
					{
						case 'A':
							pos_WAM_value = pos_WAM_array[i][TA];
							neg_WAM_value = neg_WAM_array[i][TA];
							break;
						case 'C':
							pos_WAM_value = pos_WAM_array[i][TC];
							neg_WAM_value = neg_WAM_array[i][TC];
							break;
						case 'T':
							pos_WAM_value = pos_WAM_array[i][TT];
							neg_WAM_value = neg_WAM_array[i][TT];
							break;
						case 'G':
							pos_WAM_value = pos_WAM_array[i][TG];
							neg_WAM_value = neg_WAM_array[i][TG];
							break;
					}
					break;
				case 'G':
					switch(seq_p->sequence[i+1])
					{
						case 'A':
							pos_WAM_value = pos_WAM_array[i][GA];
							neg_WAM_value = neg_WAM_array[i][GA];
							break;
						case 'C':
							pos_WAM_value = pos_WAM_array[i][GC];
							neg_WAM_value = neg_WAM_array[i][GC];
							break;
						case 'T':
							pos_WAM_value = pos_WAM_array[i][GT];
							neg_WAM_value = neg_WAM_array[i][GT];
							break;
						case 'G':
							pos_WAM_value = pos_WAM_array[i][GG];
							neg_WAM_value = neg_WAM_array[i][GG];
							break;
					}
					break;
			}
			score += log(pos_WAM_value/neg_WAM_value);
		}
		//if(score < -9999) score = -9999;
		seq_p->score = score;
		//if(!seq_p->label) cout<<seq_p->name<<"  "<<seq_p->position<<"  "<<seq_p->label<<"  "<<seq_p->score<<endl;
		seq_p = seq_p->next;
	}
}

void WAM::calculate_threshold(sample_LinkList seq_list)
{
	sample_node *seq_p;
	seq_p = seq_list->next;
	int n = 0;
	while(seq_p!=NULL)
	{
		if(!seq_p->label) ++n;
		seq_p = seq_p->next;
	}
	seq_p = seq_list->next;
	long double *neg_scores = new long double[n];
	int index = 0;
	while(seq_p!=NULL)
	{
		if(!seq_p->label)
		{
			neg_scores[index] = seq_p->score;
			++index;
		}
		seq_p = seq_p->next;
	}
	/*
	for(int i=0; i<n; ++i)
	{
		cout<<neg_scores[i]<<endl;
	}
	*/
	sort(neg_scores, neg_scores + n);
	highest_threshold = neg_scores[n-1];
	high_threshold = neg_scores[int(0.9*n)];
	medium_threshold = neg_scores[int(0.5*n)];
	low_threshold = neg_scores[int(0.2*n)];
	delete []neg_scores;
	cout<<"----------After Training----------"<<endl;
	/*
	calculate_Sn_and_Sp(seq_list, 'O');
	calculate_Sn_and_Sp(seq_list, 'H');
	calculate_Sn_and_Sp(seq_list, 'M');
	calculate_Sn_and_Sp(seq_list, 'L');
	*/
}

long double WAM::calculate_Sn_and_Sp(sample_LinkList seq_list, char mode, long double thr)
{
	long double threshold;
	string which_threshold;
	if(thr==0)
	{
		switch(mode)
		{
			case 'O':
				which_threshold = "highest_threshold";
				threshold = highest_threshold;
				break;
			case 'H':
				which_threshold = "high_threshold";
				threshold = high_threshold;
				break;
			case 'M':
				which_threshold = "medium_threshold";
				threshold = medium_threshold;
				break;
			case 'L':
				which_threshold = "low_threshold";
				threshold = low_threshold;
				break;
		}
	}
	else
	{
		threshold = thr;
	}
	//long double Sn, Sp;
	sample_node *seq_p;
	seq_p = seq_list->next;
	/*
	while(seq_p!=NULL)
	{
		cout<<seq_p->name<<" "<<seq_p->position<<" "<<seq_p->label<<" "<<seq_p->score<<" "<<seq_p->result<<endl;
		seq_p = seq_p->next;
	}
	seq_p = seq_list->next;
	*/
	long double TP, FP, TN, FN;
	TP = FP = TN = FN = 0;
	while(seq_p!=NULL)
	{
		switch(int(seq_p->label))
		{
			case 0:
				if(seq_p->score >= threshold)
				{
					++FP;
					seq_p->result = 1;
				}
				else
				{
					++TN;
					seq_p->result = 0;
				}
				break;
			case 1:
				if(seq_p->score >= threshold)
				{
					++TP;
					seq_p->result = 1;
				}
				else
				{
					++FN;
					seq_p->result = 0;
				}
				break;
		}
		seq_p = seq_p->next;
	}
	/*
	cout<<"TP:  "<<TP<<"  FN:  "<<FN<<"  TN:  "<<TN<<"  FP:  "<<FP<<endl;
	cout<<"Thersholdtype: "<<which_threshold;
	//cout.precision(50);
	cout<<" == "<<threshold;
	*/
	Sp = TN / (TN + FP);
	Sn = TP / (TP + FN);
	/*
	//cout.precision(50);
	cout<<"  Sn: "<< Sn;
	//cout.precision(50);
	cout<<"  Sp: "<<Sp<<endl;
	*/
	return Sn, Sp;
}

void WAM::predict(sample_LinkList seq_list)
{
	sample_node *seq_p;
	seq_p = seq_list->next;
	while(seq_p!=NULL)
	{
		long double score;
		score = log(pos_WAM_minus_3[seq_p->sequence[0]]/neg_WAM_minus_3[seq_p->sequence[0]]);
		for(int i=0; i<8; ++i)
		{
			long double pos_WAM_value, neg_WAM_value;
			switch(seq_p->sequence[i])
			{
				case 'A':
					switch(seq_p->sequence[i+1])
					{
						case 'A':
							pos_WAM_value = pos_WAM_array[i][AA];
							neg_WAM_value = neg_WAM_array[i][AA];
							break;
						case 'C':
							pos_WAM_value = pos_WAM_array[i][AC];
							neg_WAM_value = neg_WAM_array[i][AC];
							break;
						case 'T':
							pos_WAM_value = pos_WAM_array[i][AT];
							neg_WAM_value = neg_WAM_array[i][AT];
							break;
						case 'G':
							pos_WAM_value = pos_WAM_array[i][AG];
							neg_WAM_value = neg_WAM_array[i][AG];
							break;
					}
					break;
				case 'C':
					switch(seq_p->sequence[i+1])
					{
						case 'A':
							pos_WAM_value = pos_WAM_array[i][CA];
							neg_WAM_value = neg_WAM_array[i][CA];
							break;
						case 'C':
							pos_WAM_value = pos_WAM_array[i][CC];
							neg_WAM_value = neg_WAM_array[i][CC];
							break;
						case 'T':
							pos_WAM_value = pos_WAM_array[i][CT];
							neg_WAM_value = neg_WAM_array[i][CT];
							break;
						case 'G':
							pos_WAM_value = pos_WAM_array[i][CG];
							neg_WAM_value = neg_WAM_array[i][CG];
							break;
					}
					break;
				case 'T':
					switch(seq_p->sequence[i+1])
					{
						case 'A':
							pos_WAM_value = pos_WAM_array[i][TA];
							neg_WAM_value = neg_WAM_array[i][TA];
							break;
						case 'C':
							pos_WAM_value = pos_WAM_array[i][TC];
							neg_WAM_value = neg_WAM_array[i][TC];
							break;
						case 'T':
							pos_WAM_value = pos_WAM_array[i][TT];
							neg_WAM_value = neg_WAM_array[i][TT];
							break;
						case 'G':
							pos_WAM_value = pos_WAM_array[i][TG];
							neg_WAM_value = neg_WAM_array[i][TG];
							break;
					}
					break;
				case 'G':
					switch(seq_p->sequence[i+1])
					{
						case 'A':
							pos_WAM_value = pos_WAM_array[i][GA];
							neg_WAM_value = neg_WAM_array[i][GA];
							break;
						case 'C':
							pos_WAM_value = pos_WAM_array[i][GC];
							neg_WAM_value = neg_WAM_array[i][GC];
							break;
						case 'T':
							pos_WAM_value = pos_WAM_array[i][GT];
							neg_WAM_value = neg_WAM_array[i][GT];
							break;
						case 'G':
							pos_WAM_value = pos_WAM_array[i][GG];
							neg_WAM_value = neg_WAM_array[i][GG];
							break;
					}
					break;
			}
			score += log(pos_WAM_value/neg_WAM_value);
		}
		seq_p->score = score;
		//if(seq_p->label) cout<<seq_p->name<<"  "<<seq_p->position<<"  "<<seq_p->label<<"  "<<seq_p->score<<endl;
		seq_p = seq_p->next;
	}
	cout<<"----------After Predicting----------"<<endl;
	/*
	calculate_Sn_and_Sp(seq_list, 'O');
	//display_result(seq_list);
	//cout<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl;
	calculate_Sn_and_Sp(seq_list, 'H');
	//display_result(seq_list);
	//cout<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl;
	calculate_Sn_and_Sp(seq_list, 'M');
	//display_result(seq_list);
	//cout<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl;
	calculate_Sn_and_Sp(seq_list, 'L');
	//display_result(seq_list);
	//*/
}

void WAM::display_result(sample_LinkList seq_list)
{
	sample_node *seq_p;
	seq_p = seq_list->next;
	while(seq_p!=NULL)
	{
		if(seq_p->result)
		{
			cout<<seq_p->name<<" "<<seq_p->position<<" "<<seq_p->score<<endl;
		}
		seq_p = seq_p->next;
	}
}

void WAM::draw_Sn_Sp(sample_LinkList train_seq_list, sample_LinkList test_seq_list)
{
	sample_node *train_p, *test_p;
	train_p = train_seq_list->next;
	test_p = test_seq_list->next;
	int n = 0;
	while(train_p!=NULL)
	{
		if(!train_p->label) ++n;
		train_p = train_p->next;
	}
	train_p = train_seq_list->next;
	long double *neg_scores = new long double[n];
	int index = 0;
	while(train_p!=NULL)
	{
		if(!train_p->label)
		{
			neg_scores[index] = train_p->score;
			++index;
		}
		train_p = train_p->next;
	}
	sort(neg_scores, neg_scores + n);
	long double Sn_array[1000], Sp_array[1000];
	for(long double portion = 0; portion <= 1; portion += 0.001)
	{
		long double threshold = neg_scores[int(portion*n)];
		Sn, Sp = calculate_Sn_and_Sp(test_seq_list, 'N', threshold);
		Sn_array[int(portion/0.001)] = Sn;
		Sp_array[int(portion/0.001)] = Sp;
	}
	delete []neg_scores;
	write_in_Sn_Sp(Sn_array, Sp_array, 1000);
}

int main()
{
	LinkList training_txtfile_list, training_txtcontent_list;
	sample_LinkList training_sequence_list;
	training_txtfile_list=findfile(".\\training and testing dataset\\Training Set", "\\*.txt");
	training_txtcontent_list=read_files(".\\training and testing dataset\\Training Set", training_txtfile_list);
	//display(txtcontent_list);
	training_sequence_list=read_sample_sequences(training_txtfile_list, training_txtcontent_list);
	
	WAM WAM1;
	WAM1.create_WAM(training_sequence_list);
	WAM1.process_training_data(training_sequence_list);
	//WAM1.calculate_threshold(training_sequence_list);
	
	LinkList testing_txtfile_list, testing_txtcontent_list;
	sample_LinkList testing_sequence_list;
	testing_txtfile_list=findfile(".\\training and testing dataset\\Testing Set", "\\*.txt");
	testing_txtcontent_list=read_files(".\\training and testing dataset\\Testing Set", testing_txtfile_list);
	testing_sequence_list=read_sample_sequences(testing_txtfile_list, testing_txtcontent_list);
	WAM1.predict(testing_sequence_list);
	WAM1.draw_Sn_Sp(training_sequence_list, testing_sequence_list);
}
