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
#include<Eigen\Dense>
#include<typeinfo>
#define A 0
#define C 1
#define T 2
#define G 3
using namespace std;
using namespace Eigen;

typedef struct node
{
	string data;
	node *next;
}node;
typedef node *LinkList;

LinkList findfile(string path,string mode)
{
	cout<<"Loading files..."<<endl;
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
	cout<<"Complete."<<endl;
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
	cout<<"Reading files..."<<endl;
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
	cout<<"Complete."<<endl;
	return txt_list;
}

typedef struct sample_node
{
	int position, label, result;
	string name, sequence;
	long double score;
	VectorXd code;
	sample_node *next;
}sample_node;
typedef sample_node *sample_LinkList;

void encoding_seq_list(sample_LinkList seq_list)
{
	sample_node *seq_p;
	seq_p = seq_list->next;
	VectorXd code(36);
	while(seq_p!=NULL)
	{
		for(int i = 0; i < 36; ++i)
		{
			code[i] = 0;
		}
		for(int i = 0; i < 9; ++i)
		{
			switch(seq_p->sequence[i])
			{
				case 'A':
					code[4*i + A] = 1;
					break;
				case 'C':
					code[4*i + C] = 1;
					break;
				case 'T':
					code[4*i + T] = 1;
					break;
				case 'G':
					code[4*i + G] = 1;
					break;
			}
		}
		seq_p->code = code;
		seq_p = seq_p->next;
	}
	
}

sample_LinkList read_sample_sequences(LinkList file_list, LinkList txt_list)
{
	cout<<"Reading sequences..."<<endl;
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
		bool exist = 1;
		//cout<<"index_pos_pos  "<<index_pos_pos<<endl;

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
   			if(exist) seq_p->next->label = 1; else seq_p->next->label = -1;
			seq_p->next->position = index;
			seq_p->next->name = file_p->data;
			//cout<<i<<"  "<<seq_p->next->data<<"  "<<endl;
			seq_p = seq_p->next;
		}


		//select all negative sites
		/*
		for(int i=3; i < sequence_length; ++i)
		{
			bool exist = 0;
			for(int j = 0; j < index_pos_pos; ++j)
			{
				if(i == position_positive[j]) exist = 1;
			}
			seq_p->next = new(sample_node);
			seq_p->next->sequence = whole_sequence.substr(i-3, 9);
			transform(seq_p->next->sequence.begin(), seq_p->next->sequence.end(), seq_p->next->sequence.begin(), ::toupper);
   			if(exist) seq_p->next->label = 1; else seq_p->next->label = -1;
			seq_p->next->position = i;
			seq_p->next->name = file_p->data;
			//cout<<i<<"  "<<seq_p->next->data<<"  "<<endl;
			seq_p = seq_p->next;
		}
		*/
		delete []position_positive;
		txt_p = txt_p->next;
		file_p = file_p->next;
	}
	seq_p->next = NULL;
	encoding_seq_list(seq_list);
	cout<<"Complete."<<endl;
	return seq_list;
}

void write_in_Sn_Sp(const long double Sn_list[],const long double Sp_list[], int length)
{
	ofstream outfile(".\\SVM_Sn_Sp.txt");
	outfile<<"Sn\tSp"<<endl;
	for(int i = 0; i < length; ++i)
	{
		outfile<<Sn_list[i]<<"\t"<<Sp_list[i]<<endl;
	}
	outfile.close();
}

class SVM
{
		VectorXd W, alpha, E, train_y;
		MatrixXd train_x;
		int train_size, test_size;
		long double b, c, epsilon;
		long double Sn, Sp;
	public:
		void init_train_data(sample_LinkList code_list, long double c_);
		int select_first(int index1);
		int select_second(int first);
		void smo();
		long double clip_alpha(long double a1, long double a2, long double a2_new_unc, long double y1, long double y2);
		void update_E();
		void update_W();
		long double gx(int index);
		void get_model(string path);
		void predict(sample_LinkList seq_list);
		void draw_Sn_Sp(sample_LinkList train_seq_list, sample_LinkList test_seq_list);
		void calculate_Sn_and_Sp(sample_LinkList seq_list, long double thr);
};

void SVM::init_train_data(sample_LinkList code_list, long double c_)
{
	cout<<"Initializing SVM..."<<endl;
	sample_node *code_p;
	code_p = code_list->next;
	b = 0;
	c = c_;
	epsilon = 0.0001;
	int size = 0;
	while(code_p!=NULL)
	{
		size += 1;
		code_p = code_p->next;
	}
 	train_size = size;
 	cout<<"	Training sample:"<<size<<endl;
	VectorXd tW(36), talpha(size), tE(size), ty(size);
	for(int i = 0; i < 36; ++i)
	{
		tW[i] = 0;
	}
	MatrixXd tX = MatrixXd::Zero(36, size);
	code_p = code_list->next;
	int num = 0;
	while(code_p!=NULL)
	{
		for(int i = 0; i < 36; ++i)
		{
			tX(i, num) = code_p->code[i];
		}
		VectorXd ttx = tX.col(num);
		talpha[num] = 0;
		ty[num] = code_p->label;
		tE[num] = tW.dot(ttx) + b - ty[num];
		num += 1;
		code_p = code_p->next;
	}
	train_x = tX;
	alpha = talpha;
	E = tE;
	train_y = ty;
	W = tW;
	cout<<"Complete."<<endl;
}

long double SVM::gx(int index)
{
	long double gxi = b;
	VectorXd xi;
	xi = train_x.col(index);
	for(int i = 0; i < train_size; ++i)
	{
		VectorXd xj;
		xj = train_x.col(i);
		gxi += alpha[i] * train_y[i] * xi.dot(xj);
	}
	return gxi;
}

int SVM::select_first(int index1)
{
	int num;
	long double diff = 0;
	for(int i = 0; i < train_size; ++i)
	{
		if(i == index1) continue;
		VectorXd x = train_x.col(i);
		long double gxi = gx(i);
		if(0 < alpha[i] < c && train_y[i] * gxi != 1)
		{
			if (abs(1 - train_y[i] * gxi > diff))
			{
				diff = abs(1 - train_y[i] * gxi);
				num = i;
			}
		}
	}
	if(diff != 0)
	{
		return num;
	}
 	for(int i = 0; i < train_size; ++i)
	{
		if(i == index1) continue;
		VectorXd x = train_x.col(i);
		long double gxi = gx(i);
		if((alpha[i] == 0 && train_y[i] * gxi < 1) || (0 < alpha[i] && alpha[i] < c && train_y[i] * gxi != 1) || (alpha[i] == c && train_y[i] * gxi > 1))
		{
			if (abs(1 - train_y[i] * gxi) > diff)
			{
				diff = abs(1 - train_y[i] * gxi);
				num = i;
			}
		}
	}
	if(diff == 0)
	{
		return -1;
	}
	return num;
}

int SVM::select_second(int first)
{
	int num;
	long double diff = 0;
	for(int i = 0; i < train_size; ++i)
	{
		if(0 < alpha[i] && alpha[i] < c && train_y[i])
		{
			if(abs(E[i] - E[first]) > diff)
			{
				diff = abs(E[i] - E[first]);
				num = i;
			}
		}
	}
	if(diff != 0)
	{
		return num;
	}
	for(int i = 0; i < train_size; ++i)
	{
		if(abs(E[i] - E[first]) > diff)
		{
			diff = abs(E[i] - E[first]);
			num = i;
		}
	}
	if(diff == 0)
	{
		return -1;
	}
	return num;
}

long double SVM::clip_alpha(long double a1, long double a2, long double a2_new_unc, long double y1, long double y2)
{
	long double zero = 0;
	if(y1 != y2)
	{
		long double L = max(zero, a2 - a1);
		long double H = min(c, c + a2 - a1);
		if(a2_new_unc < L) return L;
		if(a2_new_unc > H) return H;
		if(L <= a2_new_unc && a2_new_unc <= H) return a2_new_unc;
	}
	else
	{
		long double L = max(zero, a2 + a1 -c);
		long double H = min(c, a2 + a1);
		if(a2_new_unc < L) return L;
		if(a2_new_unc > H) return H;
		if(L <= a2_new_unc && a2_new_unc <= H) return a2_new_unc;
	}
}

void SVM::update_E()
{
	//cout<<"		Updating E..."<<endl;
	for(int i = 0; i < train_size; ++i)
	{
		//cout<<"			Updating E"<<i<<"..."<<endl;
		VectorXd xi;
		xi = train_x.col(i);
		long double E_new = b - train_y[i];
		for(int j = 0; j < train_size; ++j)
		{
			VectorXd xj;
			xj = train_x.col(j);
			E_new += train_y[j] * alpha[j] * xi.dot(xj);
		}
		E[i] = E_new;
	}
	//cout<<"		Complete."<<endl;
}

void SVM::update_W()
{
	VectorXd new_W(36);
	for(int i = 0; i < 36; ++i)
	{
		new_W[i] = 0;
	}
	for(int i = 0; i < train_size; ++i)
	{
		VectorXd x;
		x = train_x.col(i);
		new_W += alpha[i] * train_y[i] * x;
	}
	W = new_W;
	for(int i = 0; i < 9; ++i)
	{
		for(int j = 0; j < 4; ++j)
		{
			cout<<W[4*i + j]<<",";
		}
		cout<<endl;
	}
}

void SVM::smo()
{
	cout<<"Training SVM..."<<endl;
	int last_index = -1;
	int training_time = 1;
	//int last_index2;
	while(true)
	{
		cout<<"	Training_time:"<<training_time<<endl;
		int index1, index2;
		//cout<<"		Selecting fisrt"<<endl;
		index1 = select_first(last_index);
		cout<<"		index1:"<<index1<<endl;
		if(index1 < 0)
		{
			cout<<"Training Complete."<<endl;
			break;
		}
		//cout<<"		Selecting Second"<<endl;
		index2 = select_second(index1);
		cout<<"		index2:"<<index2<<endl;
		if(index2 < 0) //or (index2 == last_index2 and index1 == last_index))
		{
   			//cout<<"Can not find proper a2."<<endl;
			//last_index = index1;
			//last_index2 = -1;
			//continue;
			break;
		}
		last_index = -1;
		//last_index2 = index2;
		long double a1 = alpha[index1];
		long double a2 = alpha[index2];
		VectorXd x1, x2;
		x1 = train_x.col(index1);
		x2 = train_x.col(index2);
		long double y1, y2, E1, E2;
		y1 = train_y[index1];
		y2 = train_y[index2];
		E1 = E[index1];
		E2 = E[index2];
		//cout<<"		Updating a1, a2..."<<endl;
		long double eta;
		eta = x1.dot(x1) + x2.dot(x2) - 2*x1.dot(x2);
		if(eta == 0)
		{
			last_index = index1;
			continue;
		}
		long double a2_new_unc = a2 + y2 * (E1 - E2) / eta;
		cout<<"		a2_new_unc:"<<a2_new_unc<<endl;
		long double a2_new = clip_alpha(a1, a2, a2_new_unc, y1, y2);
		long double a1_new = a1 + y1 * y2 * (a2 - a2_new);
		cout<<"		a1_new:"<<a1_new<<" a2_new:"<<a2_new<<endl;
		alpha[index1] = a1_new;
		alpha[index2] = a2_new;
		//cout<<"		Updating b..."<<endl;
		if(0 < a1_new && a1_new < c)
		{
			long double b1_new, b2_new, b_new;
			b1_new = - E1 - y1 * x1.dot(x1) * (a1_new - a1) - y2 * x2.dot(x1) * (a2_new - a2) + b;
			b2_new = - E2 - y1 * x1.dot(x2) * (a1_new - a1) - y2 * x2.dot(x2) * (a2_new - a2) + b;
			cout<<"		b1_new:"<<b1_new<<" b2_new:"<<b2_new<<endl;
			if(b1_new == b2_new) b_new = b1_new;
			else b_new = (b1_new + b2_new) / 2;
			b = b_new;
		}
		if(a1_new == 0 && a2_new == 0)
		{
			last_index = index1;
		}
		cout<<"		b_new:"<<b<<endl;
		update_E();
		update_W();
		training_time += 1;
	}
	cout<<"Complete."<<endl;
}

void SVM::get_model(string path)
{
	cout<<"Loading files..."<<endl;
	ifstream infile;
	string path_W = "W.txt";
	string newpath = path + "\\" + path_W;
	char filename[80];
	strcpy(filename, newpath.c_str());
	//cout<<newpath<<endl;
	infile.open(filename);
	if (!infile)
  	{
		cout<<"Failed to open the file: "<<newpath<<endl;
		exit(1);
	}
	else
	{
		string lines;
		char line[500];
		while(infile.getline(line, 500, '\n'))
		{
			lines = lines + line;
		}
		string content = lines;
		VectorXd tW(36);
		for(int i = 0; i < 9; ++i)
		{
			for(int j = 0; j < 4; ++j)
			{
				int pos_comma;
				pos_comma = content.find(',');
				long double Wi = 0;
				//cout<<content.substr(0, pos_table - 1).c_str()<<endl;
				Wi += atof(content.substr(0, pos_comma - 1).c_str());
				content = content.substr(pos_comma + 1);
				//cout<<content<<endl;
				tW[4*i + j] = Wi;
			}
		}
		W = tW;
		cout<<W<<endl;
	}
	string path_b = "b.txt";
	string newpath1 = path + "\\" + path_b;
	char filename1[80];
	strcpy(filename1, newpath1.c_str());
	ifstream infile1;
	infile1.open(filename1);
	if (!infile1)
  	{
		cout<<"Failed to open the file: "<<newpath1<<endl;
		exit(1);
	}
	else
	{
		string lines;
		char line[500];
		while(infile1.getline(line, 500, '\n'))
		{
			lines = lines + line;
		}
		double tb;
		tb = atof(lines.c_str());
		b = tb;
	}
	cout<<b<<endl<<endl;
}

void SVM::predict(sample_LinkList seq_list)
{
	sample_node *seq_p;
	seq_p = seq_list->next;
	cout<<"Predicting..."<<endl;
	while(seq_p != NULL)
	{
		seq_p->score = W.dot(seq_p->code) + b;
		//cout<<seq_p->score<<endl;
		if(seq_p->score > 0) seq_p->result = 1;
		else seq_p->result = -1;
		seq_p = seq_p->next;
	}
	//cout<<endl<<endl;
}

void SVM::calculate_Sn_and_Sp(sample_LinkList seq_list, long double thr)
{
	long double threshold;
	threshold = thr;
	sample_node *seq_p;
	seq_p = seq_list->next;
	
	long double TP, FP, TN, FN;
	TP = FP = TN = FN = 0;
	while(seq_p!=NULL)
	{
		switch(int(seq_p->label))
		{
			case -1:
				if(seq_p->score >= threshold)
				{
					++FP;
					seq_p->result = 1;
				}
				else
				{
					++TN;
					seq_p->result = -1;
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
					seq_p->result = -1;
				}
				break;
		}
		seq_p = seq_p->next;
	}
	//cout<<"TN:"<<TN<<" FP:"<<FP<<" TP:"<<TP<<" FN:"<<FN<<endl;
	Sp = TN / (TN + FP);
	Sn = TP / (TP + FN);
	cout<<"threshold:"<<thr<<endl;
	cout<<"Sn:"<<Sn<<" Sp:"<<Sp<<endl;
}

void SVM::draw_Sn_Sp(sample_LinkList train_seq_list, sample_LinkList test_seq_list)
{
	predict(train_seq_list);
	predict(test_seq_list);
	sample_node *train_p, *test_p;
	train_p = train_seq_list->next;
	test_p = test_seq_list->next;
	int n = 0;
	while(train_p!=NULL)
	{
		if(train_p->label == -1) ++n;
		train_p = train_p->next;
	}
	train_p = train_seq_list->next;
	long double *neg_scores = new long double[n];
	int index = 0;
	while(train_p!=NULL)
	{
		if(train_p->label == -1)
		{
			neg_scores[index] = train_p->score;
			//cout<<train_p->score<<endl;
			++index;
		}
		train_p = train_p->next;
	}
	sort(neg_scores, neg_scores + n);
	long double Sn_array[1000], Sp_array[1000];
	for(long double portion = 0; portion <= 1; portion += 0.001)
	{
		long double threshold = neg_scores[int(portion*n)];
		//cout<<"portion:"<<portion<<endl;
		//cout<<neg_scores[int(portion*n)]<<endl;
		calculate_Sn_and_Sp(test_seq_list, threshold);
		cout<<"Sn:"<<Sn<<" Sp:"<<Sp<<endl;
		Sn_array[int(portion/0.001)] = Sn;
		cout<<"Sn_array["<<int(portion/0.001)<<"]:"<<Sn_array[int(portion/0.001)]<<endl;
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
	training_sequence_list=read_sample_sequences(training_txtfile_list, training_txtcontent_list);
	SVM SVM1;
	SVM1.init_train_data(training_sequence_list, 1);
	SVM1.smo();

	//SVM1.get_model(".\\svm_model1");

	LinkList testing_txtfile_list, testing_txtcontent_list;
	sample_LinkList testing_sequence_list;
	testing_txtfile_list=findfile(".\\training and testing dataset\\Testing Set", "\\*.txt");
	testing_txtcontent_list=read_files(".\\training and testing dataset\\Testing Set", testing_txtfile_list);
	testing_sequence_list=read_sample_sequences(testing_txtfile_list, testing_txtcontent_list);
	SVM1.draw_Sn_Sp(training_sequence_list, testing_sequence_list);
}
