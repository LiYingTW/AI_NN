#include <ctype.h>
#include <math.h>   // for exp(), fabs(), sqrt()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>   // for srand()
#include<iostream>
#include<fstream>
#include <vector>
#include <sstream>

using namespace std;

int inputMax[25];
vector<vector<int> > data;
vector<int> data_num;
int **input; 


class Element
{
    public :
    double Value;
    double GateValue;
    double *Synapse;
 
    // internal :
    double diffentValue;
    double fixGateValue;
    double *fixSynapse;
    int UpperLayerSize; // the length of Synapse and fixSynapse
 
    public :
 
    Element(int upperLayerSize) {
        int s;
 
        UpperLayerSize = upperLayerSize;
        Synapse = new double[UpperLayerSize];
        fixSynapse = new double[UpperLayerSize];
 
        if (UpperLayerSize > 0) {
            GateValue = ( (double)rand() / RAND_MAX ) * 2 - 1;
            for ( s = 0; s < UpperLayerSize; s++) {
                Synapse[s] = ( (double)rand() / RAND_MAX ) * 2 - 1;
            }
        }
    }
   
    // 解構式, 釋放動態配置的資源
    ~Element() {
        if( Synapse != NULL ) delete Synapse;
        if( fixSynapse != NULL ) delete fixSynapse;
    }
   
};
// end of class Element
 
// 接著解釋網路元件的設計。
//     Element ***Elements;
// 用一個二維的動態陣列來儲存神經元，陣列第一個註腳就是神經層，第二個註腳
// 就是每個神經層的神經元。
const int ELEMENTS_LENGTH = 3;   // Network::Elements.length, 三種 Layer

class Network {
    public:
    Element ***Elements;
    double *Standar;
    double DiffentValue;

	
	
    // Elements[?].length, 三種 Layer 的元素數, 方便跑迴圈用
    int Elements_lengths[ELEMENTS_LENGTH];
 
    // 為了程式方便，設計兩個屬性，直接傳回輸入層和輸出層。
    Element ** OutputLayer; // Elements[2];
    Element ** InputLayer;  // Elements[0];
 
    Network(int InputLayerSize, int OutputLayerSize){
		int HiddenLayerSize;
		ifstream inp;
		inp.open("NetWork.txt",ios::in);		
		if(!inp){	
			cout<<"File Input does not exist!!"<<endl; //input another file name to open another.
			exit(-1); 
		}	
		for(int i=0; i<24;i++){
			inp>>inputMax[i];
		//	cout<<inputMax[i]<<" ";
		}
	//	cout<<endl;
		inp>>HiddenLayerSize;
		//cout<<HiddenLayerSize<<endl;
		input = new int *[(int)data.size()];
		for(int i=0;i<(int)data.size();i++){
			input[i]=new int [25];
		}
		
	
        // 使用動態的方式宣告每一層的大小，這樣也比較符合實際網路架構。
        Elements = new Element **[3];
        Elements[0] = new Element *[InputLayerSize];
        Elements[1] = new Element *[HiddenLayerSize];
        Elements[2] = new Element *[OutputLayerSize];
 
        OutputLayer = Elements[2];
        InputLayer = Elements[0];
        Elements_lengths[0] = InputLayerSize;
        Elements_lengths[1] = HiddenLayerSize;
        Elements_lengths[2] = OutputLayerSize;
 
        Standar = new double[OutputLayerSize];
 
        int upperLayerSize = 0;
        for (int l = 0; l < ELEMENTS_LENGTH; l++) {
            for (int e = 0; e < Elements_lengths[l]; e++) {
                Elements[l][e] = new Element(upperLayerSize);
            }
            upperLayerSize = Elements_lengths[l];
        }
		int i,j;
		for(i=0;i<HiddenLayerSize;i++){
			for(j=0;j<25;j++){
				if(j==0){
					inp>>Elements[1][i]->GateValue;
			//		cout<<Elements[1][i]->GateValue<<" ";
				}
				else{
					inp>>Elements[1][i]->Synapse[j-1];
			//		cout<<Elements[1][i]->Synapse[j-1]<<" ";
				}
				
			}
		//	cout<<endl;
		}
		inp>>Elements[2][0]->GateValue;
	//	cout<<Elements[2][0]->GateValue<<" ";
		for(i=0;i<HiddenLayerSize;i++){
			inp>>Elements[2][0]->Synapse[i];
		//	cout<<Elements[2][0]->Synapse[i]<<" ";
		}
	//	cout<<endl;
    }
	
    // end of Network(int InputLayerSize, int HiddenLayerSize, int OutputLayerSize)
 
    // 解構式, 釋放動態配置的資源
    ~Network() {
        if( Elements != NULL ) {
            for (int l = 0; l < ELEMENTS_LENGTH; l++) {
                if( Elements[l] != NULL ) {
                    for (int e = 0; e < Elements_lengths[l]; e++) {
                        if( Elements[l][e] != NULL ) delete Elements[l][e];
                    }
                    delete Elements[l];
                }
            }
            delete Elements;
        }
        if( Standar != NULL ) delete Standar;
    }
    // end of ~Network()

	 void Summation() { 
		for (int l = 1; l < ELEMENTS_LENGTH; l++) {
            for (int e = 0; e < Elements_lengths[l]; e++) {
			//	cout<<"Elements_lengths["<<l<<"]"<<Elements_lengths[l]<<"["<<e<<"]\t";
                double outvalue = -Elements[l][e]->GateValue;
			//	cout<<"outvalue("<<-Elements[l][e]->GateValue<<")\t";
                for (int s = 0; s < Elements[l][e]->UpperLayerSize; s++) {
                    outvalue += Elements[l - 1][s]->Value *
                        Elements[l][e]->Synapse[s];
			//			cout<<Elements[l - 1][s]->Value << "*" << Elements[l][e]->Synapse[s]<<"+";
                }
				
			//	printf("Elements[%d][%d]->GateValue :%f \toutvalue :%f",l,e,Elements[l][e]->GateValue,outvalue);
				if(outvalue>0){
					Elements[l][e]->Value =1;
				} 
				else {	
					Elements[l][e]->Value = 0;		
				}
                //Elements[l][e]->Value = (double)(1 / (1 + exp(-outvalue)));
            }
        }
    }
   void LoadData( int sampleNo )
    {
		for(int i=0;i<24;i++){ 
			if(inputMax[i]!=0)
		//	 InputLayer[i]->Value = input[sampleNo][i]/ inputMax[i];
			Elements[0][i]->Value = (double)input[sampleNo][i]/ (double) inputMax[i];
			else{	
				Elements[0][i]->Value= 0;
			//	InputLayer[i]->Value =  0;
			}
		}	
		Standar[0] = input[sampleNo][24] - 1;	
		
    }
	void LoadTestData(int learnSamples) 
    {	
		int i,j=0;
		ifstream inp;
		char file[100];

		inp.open("TestData100",ios::in);		
		if(!inp){	 
			cout<<"File Input does not exist!!"<<endl; //input another file name to open another.
			while(!inp){
				cout<<"Please input a valid file name:\t";	
				cin>>file;
				inp.open(file,ios::in);			
			}	
		}	
		//read file	
		/*	test
		for(i=0;i<learnSamples;i++){
			for(j=0;j<25;j++){
				inp>>input[i][j];
			//	if(input[i][j]>inputMax[j])
			//		inputMax[j] = input[i][j];
			}
		}	
		*/
		//dynamic
		for( int i = 0 ; i < (int)data.size() ; i++ )	{
			for( int j = 0 ; j < (int)data[i].size() ; j++ ){
			//data[i][j] = 1;
				input[i][j]=data[i][j];
			}
		}
	}
	
    
 	void PrintNetwork(){
		ofstream op("Output2.txt");
		op<<Elements_lengths[1]<<endl;
		 for (int l = 1; l < ELEMENTS_LENGTH; l++) {
			
            for (int e = 0; e < Elements_lengths[l]; e++) {
					op<<Elements[l][e]->GateValue<<" ";
				//	printf("Elements[%d][%d]-GateValue :%f\tWeight",l,e,Elements[l][e]->GateValue);
                for (int s = 0; s < Elements[l][e]->UpperLayerSize; s++) {
					op<< Elements[l][e]->Synapse[s]<<" ";
				//	printf("%d:%f , ",s,Elements[l][e]->Synapse[s]);
                }
				op<<endl;
			//	printf("\n");
            }
        }
		 
		 
	}
   
};
// end of class Network


int ReadTestFile(){
	
	string str_buf;
	fstream fin;
	
	fin.open( "input.csv" , fstream::in );
	
    if( !fin.is_open() )
    {
        cout << "Can't open the file : TraData700.csv" << endl;
		return -1;
    }

    for(int i=0; getline(fin,str_buf); i++)
    {
        data_num.clear();
        for(size_t j=0; j < str_buf.length() ; j++ )
        {
            if( str_buf[j] == ',' )
            {
                str_buf[j] = ' ';
            }
        }
        stringstream ss(str_buf);
        int tmp;
        for(int j=0; ss >> tmp ; j++ )
        {
            data_num.push_back(tmp);
        }
        data.push_back(data_num);
    }

    fin.close();

    // use method sample
/*	cout<<(int)data.size()<<endl;
	for( int i = 0 ; i < (int)data.size() ; i++ )
	{
		for( int j = 0 ; j < (int)data[i].size() ; j++ )
		{
			//data[i][j] = 1;
			cout<<data[i][j]<<" ";
		}
		cout<<endl;
	}
	return 0;*/
}

// ===========================================================================
int main()
{ 
	srand ( time(NULL) );   /* initialize random seed: */
	const int learnSamples = 700; 
	ReadTestFile();
	
	int testSamples=(int)data.size();
	
	Network *worknet;

    worknet = new Network(24,1);
	int correct=0;
	worknet->LoadTestData(learnSamples);
//	worknet->PrintNetwork();
	
	ofstream op("output.csv");
    for (int sampleNo = 0; sampleNo < testSamples; sampleNo++) {
		worknet->LoadData(sampleNo);
		worknet->Summation();
	 
	//	if(worknet->Standar[0]== worknet->OutputLayer[0]->Value )correct++;
	//	cout<< ( worknet->OutputLayer[0]->Value + 1)<<endl;
		op<<( worknet->OutputLayer[0]->Value + 1)<<endl;
                // ==== DEBUG ====
				
        /*        int e;
				printf( "Input %d :( " ,sampleNo);
                for( e = 0; e < worknet->Elements_lengths[0]; e++)
                    printf( "%.4f,", worknet->InputLayer[e]->Value );
                printf( "), Standard:( " );
                for( e = 0; e < worknet->Elements_lengths[2]; e++)
                    printf( "%.0f ", worknet->Standar[e] );
                printf( "), Output:( " );
                for( e = 0; e < worknet->Elements_lengths[2]; e++)
                    printf( "%f ", worknet->OutputLayer[e]->Value );
                printf( ")\n" );
			*/
    }
	
//	printf("Correct rate: %.4f%%\n",((double)correct / (double)testSamples)*100);
		 
}
