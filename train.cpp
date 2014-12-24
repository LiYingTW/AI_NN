/* =========================================================================

   Summary :
       �����g���� - �ǲ߾����H
   Compiler :
       linux:
           g++ -lrt -o ./Neural3 ./Neural3.cpp
   Usage :
       ./Neural3
   Reference :
      �����g����-���g�������� @ �H�ͥ|�Q�v�}�l�G���v  �l�Ȩ� PIXNET
        http://mogerwu.pixnet.net/blog/post/25518602
   Remark :
   History :
   yyyy.mm.dd Author           Discription
   ---------- ---------------- ---------------------------------------------
   2010.11.10 Yihhann Chang    Translate from the C# source code of Moger Wu
   ---------- ---------------- ---------------------------------------------
   Test:
       ./Neural3
 
========================================================================= */
 
#include <ctype.h>
#include <math.h>   // for exp(), fabs(), sqrt()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>   // for srand()
#include<iostream>
#include<fstream>
#include <vector>


using namespace std;

// ���e�g�������g�{���]���Ҽ{��h������A�\�h�H�ݤ����A�o�@�g�ϥΰ}�C�Ӫ�F
// ���g���A�åB�������Ҽ{�h����������D�A���Ӥ���e���z�ѡC
 
// ������M�N�O�]�p���g���AValue�����g����X�ȡA�p�G���g������J�h�A��
// �P�ɤ]�O��J�ȡA�o�˳]�p�O���F�᭱�p�⪺�{���n�g�C���g����l�ƪ��ɭ�
// �����n�i�D�L�W�@�h�����g���ӼơA�o�˥L�~��ǳƦn���g��}�CSynapse�C��
// �l�ƪ��ɭ����֭ȡ]GateValue)�B���g��]Synapse�^���ϥζüƳ]�w��l�ȡA
// �ھڸg��p�G�ϥΩT�w�ȡA�]�N�O1�P-1�������l�ȡA�b�Y�ǮרҤ��i�ण�e
// �����ġC
//
// �P�ɯ��g���]�]�t�~�t��(diffentValue)�B�֭ȭץ�(fixGateValue)�B���g��
// �ץ�(fixSynapse[])�A�o�˧�Ҧ����ȳ���@�_���Ӥ���e�����F�a�C

int inputMax[25];

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
   
    // �Ѻc��, ����ʺA�t�m���귽
    ~Element() {
        if( Synapse != NULL ) delete Synapse;
        if( fixSynapse != NULL ) delete fixSynapse;
    }
   
};
// end of class Element
 
// ���۸����������󪺳]�p�C
//     Element ***Elements;
// �Τ@�ӤG�����ʺA�}�C���x�s���g���A�}�C�Ĥ@�ӵ��}�N�O���g�h�A�ĤG�ӵ��}
// �N�O�C�ӯ��g�h�����g���C
 
const int ELEMENTS_LENGTH = 3;   // Network::Elements.length, �T�� Layer
class Network {
    public:
    Element ***Elements; 
    double *Standar; 
    double DiffentValue;
 
    // Elements[?].length, �T�� Layer ��������, ��K�]�j���
    int Elements_lengths[ELEMENTS_LENGTH];
 
    // ���F�{����K�A�]�p����ݩʡA�����Ǧ^��J�h�M��X�h�C
    Element ** OutputLayer; // Elements[2];
    Element ** InputLayer;  // Elements[0];
 
    Network(int InputLayerSize, int HiddenLayerSize, int OutputLayerSize) {
        // �ϥΰʺA���覡�ŧi�C�@�h���j�p�A�o�ˤ]����ŦX��ں����[�c�C
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
    }
    // end of Network(int InputLayerSize, int HiddenLayerSize, int OutputLayerSize)
 
    // �Ѻc��, ����ʺA�t�m���귽
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
 
 
    // �]���������ץ��ʧ@�O�N�Ҧ����d�ҳ��p��L�ץ��Ȥ���A�A�@���i��ץ��A��
    // �B�A�ηs�������i��p��A�]�������b�p�⤧�e�N�������ʺA���k�s�A�]�N�O��
    // �U���o�q�{���ح����k�s���ȳ��O�[�`���ȡA�����b�s���g���}�l���ɭԲM�����C
    void ClearValue() {
        DiffentValue = 0;
        for (int l = 0; l < ELEMENTS_LENGTH; l++) {
            for (int e = 0; e < Elements_lengths[l]; e++) {
                Elements[l][e]->Value = 0;
                Elements[l][e]->diffentValue = 0;
                Elements[l][e]->fixGateValue = 0;
                for (int s = 0; s < Elements[l][e]->UpperLayerSize; s++) {
                    Elements[l][e]->fixSynapse[s] = 0;
                }
            }
        }
        // ==== DEBUG ==== TODO : LastHidden ����o�O���Ӫ�, �᭱�]�S�Ψ�
        // for (int h = 0; h < LastHidden.Length; h++) {
        //     LastHidden[h] = 0;
        // }
    }
 
    // �p���������X�ȡA�ݹL�Ѫ����ӥi�H�ݱo���A�ƾǤ�����b���n�K�A���ڷQ��
    // �n��k�A�ɤW�a�C
    void Summation() {
		double outvalue;
        for (int l = 1; l < ELEMENTS_LENGTH; l++) {
            for (int e = 0; e < Elements_lengths[l]; e++) {
                 outvalue = -Elements[l][e]->GateValue;
                for (int s = 0; s < Elements[l][e]->UpperLayerSize; s++) {
                    outvalue += Elements[l - 1][s]->Value *
                        Elements[l][e]->Synapse[s];
                }
			//	printf("-Elements[l][e]->GateValue :%f \toutvalue :%f",-Elements[l][e]->GateValue,outvalue);	
				if(outvalue>0)Elements[l][e]->Value =1;
				else {	Elements[l][e]->Value = 0;	}              
			// Elements[l][e]->Value = (double)(1 / (1 + exp(-outvalue)));
            }
        }
	
    }
 
    // �p������~�t�ȡA�o�̧ڧ⤽��������ءA�@�جO�Ψӭp��ץ������ȥΪ��A
    // �ϥήѤW�Ҽg�������C�ӥt�@�ӬO���H�ݪ��A�P�ɤ]�O��X�Ȱ���K��i�H��
    // �X���̾ڡC���ƻ�n�o�򰵽аѦҳo�@�g�A���M�ܦh�H�ӬݡA�٬O�S���H�i�D
    // �ڵ��סA�ҥH�ڥΤg��k�ѨM�o�Ӱ��D�C
    void CalcDiffent() {
        //output layer
        for (int e = 0; e < Elements_lengths[2]; e++) {
            //���q���ݪ��μзǮt����
            OutputLayer[e]->diffentValue =
                (Standar[e] - OutputLayer[e]->Value) *
                (OutputLayer[e]->Value * (1 - OutputLayer[e]->Value) + 0.01);
            //���H�ݪ��ζǲΤ���
            DiffentValue += fabs(Standar[e] - OutputLayer[e]->Value);
        }
        //hidden layer
        for (int l = ELEMENTS_LENGTH - 2; l > 0; l--) {
            for (int e = 0; e < Elements_lengths[l]; e++) {
                double sumDiff = 0;
                for (int ne = 0; ne < Elements_lengths[l + 1]; ne++) {
                    sumDiff += Elements[l + 1][ne]->Synapse[e] *
                        Elements[l + 1][ne]->diffentValue;
                }
                Elements[l][e]->diffentValue = (Elements[l][e]->Value *
                    (1 - Elements[l][e]->Value)) * sumDiff;
            }
        }
    }
 
    // �o��~�t�Ȥ���N�i�H�̷ӻ~�t�Ȩӱo��ץ��ȡA�]���n�Ҧ����d�ҳ��ǲ߹L
    // �~�i������ץ��A�ҥH�Ҧ����ץ����O�֥[���C�����٬O�Цۦ�ѦҮѤW����
    // �C�ƹ�W�o�@�q�{���i�H��p��~�t�Ȫ��{���X�֡A���}�Ӽg����e���ݱo��
    // �A�p�G�Q�n�į�n�@�I�N�бN���X�֦b�@�_�C
    void CalcFixValue(double LearnSpeed) {
        for (int l = ELEMENTS_LENGTH - 1; l > 0; l--) {
            for (int e = 0; e < Elements_lengths[l]; e++) {
                Elements[l][e]->fixGateValue =
                    -LearnSpeed * Elements[l][e]->diffentValue;
                for (int ue = 0; ue < Elements_lengths[l - 1]; ue++) {
                    Elements[l][e]->fixSynapse[ue] +=
                        LearnSpeed * Elements[l][e]->diffentValue *
                            Elements[l - 1][ue]->Value;
                }
            }
        }
    }
 
    // ��Ҧ����d�ҳ��ǲߧ�������A��M�N�O�n��ڭץ��������e�A�ץ��Ȩ������A
    // �ҥH�ݭn�ǤJ�d�ҭӼƨӶi��B��C
    void FixNetwork(int SampleCount) {
        for (int l = 1; l < ELEMENTS_LENGTH; l++) {
            for (int e = 0; e < Elements_lengths[l]; e++) {
                Elements[l][e]->GateValue +=
                    Elements[l][e]->fixGateValue / sqrt((double)SampleCount);
                for (int s = 0; s < Elements[l][e]->UpperLayerSize; s++) {
                    Elements[l][e]->Synapse[s] +=
                        Elements[l][e]->fixSynapse[s] /
                            sqrt((double)SampleCount);
                }
            }
        }  
    }
	  

	void PrintNetwork(){
		ofstream op("NetWork.txt");
		for(int i=0;i<24;i++){
			op<<inputMax[i]<<" ";
		}
		op<<endl<<Elements_lengths[1]<<endl;
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
 
// �ǲ߾����H��¦���O
// �o�����O�Ӱ�¦���A�]�����J��ƩM�N��Ʃ�i��J�h���ʧ@�A�C�@�Ӯרҳ�
// ���ۦP�A�]���ϥΥ����~�Ӫ�����rabstract�ŧi�o�����O�A�åB�ŧi��Ӥ�
// �kLoadSample�MLoadData�������갵���C
class RobotBase {
    public :
    int SampleCount;
   
    // internal :
    int inputLayerSize, outputLayerSize;
    Network *worknet;
    int noBestCount;
    int learnSamples;
    double bestDiffent; // = 10000;
	double input[800][25];
    // �غc�� ,��P���
    RobotBase(){
        bestDiffent = 10000;
        worknet = NULL;
    }
 
    // �Ѻc��, ����ʺA�t�m���귽
    ~RobotBase(){
        if( worknet != NULL ) delete worknet;
    }
 
    // �µ����禡, ���~�Ӫ̹�@
    virtual void LoadSample() = 0;
    virtual void LoadData(int SampleNo) = 0;
 
    // public delegate void OnCycleFinish(int CycleNo, double BestDiffent, double NewDiffent);
    // public event OnCycleFinish EventCycleFinish;
 
    // public delegate void OnBadLearning(int NoBestCount);
    // public event OnBadLearning EventBadLearning;
 
    // �̫�N�O�̭��n���ǲ߹L�{�F�A�򥻵{���M�e��VB.Net���g�k�@�ˡA�N�O��
    // �i�䤣��̨έȪ����ơj�Ρi�F��w�w����K�סj�ӨM�w�ǲ߬O�_�����C��
    // �L�����ɭԻ~�t�Ƚվ㪺�T�׫ܤp�A�i��ɭP�]�F�X�Q�U�����٥X���ӡA�i
    // �H�Ҽ{�A�W�[�@�ӹ�cycle������C
    virtual void Learning(double LearnSpeed, int HiddenLayerSize,
        int NoBestLimit, double Precision)
    {
        worknet = new Network(inputLayerSize, HiddenLayerSize, outputLayerSize);
        bestDiffent = 10000;
        int cycle = 0;
        noBestCount = 0;
        while ((noBestCount < NoBestLimit) && (bestDiffent > Precision)  ) {
            double newDiffent;
            worknet->ClearValue();
            for (int sampleNo = 0; sampleNo < learnSamples; sampleNo++) {
				LoadData(sampleNo);		
                worknet->Summation();
                worknet->CalcDiffent();
                worknet->CalcFixValue(LearnSpeed);
				
                // ==== DEBUG ====
             /*   int e;
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
			  newDiffent = worknet->DiffentValue / learnSamples;
            if (newDiffent < bestDiffent) {
                bestDiffent = newDiffent;
                noBestCount = 0;
            }
            else {
                noBestCount++;  
            }
            // if (EventBadLearning != null) EventBadLearning(noBestCount);
            
			if(bestDiffent <= Precision)
				worknet->PrintNetwork();
			worknet->FixNetwork(learnSamples);
            // if (EventCycleFinish != null) EventCycleFinish(cycle, bestDiffent, newDiffent);
           
            // ==== DEBUG ====
            printf( "cycle=%d, noBestCount=%d, bestDiffent=%0.8f, newDiffent=%0.8f\n",
                cycle, noBestCount, bestDiffent, newDiffent );

        } 

	
        delete worknet;
        worknet = NULL;
    }
};
// end of class RobotBase
 
// XOR���d��
class RobotXOR : public RobotBase
{
    public :
    // XOR �ǲ߾�
    RobotXOR() {
	
        inputLayerSize = 24;     // ��J�� 24 ��
        outputLayerSize = 1;    // ��X���G1 ��
        learnSamples = 700;       // ��J�զX700 ��
    }
 
    // �]�w��J�˥�, �μзǵ���
    void LoadData( int sampleNo )
    {
		int i; 
		
		for(i=0;i<24;i++){ 
		//	cout<<inputMax[i]<<" ";
			if(inputMax[i]!=0)
			 worknet->InputLayer[i]->Value = input[sampleNo][i]/ inputMax[i];
			else{	
				worknet->InputLayer[i]->Value =  0;
			}
		}	
		worknet->Standar[0] = input[sampleNo][24] - 1;	
		
    }
    // end of void LoadData( int sampleNo )
 
    // �o������ӬO�ΨӨ��N LoadData, ��˥��ƶq�e�j��, �i�H��q�ɮ�/��Ʈw���J
    void LoadSample()
    {	
		int n,i,j,correct=0;
		ifstream inp;
		char file[100];

		inp.open("TraData700",ios::in);		
		if(!inp){	
			cout<<"File Input does not exist!!"<<endl; //input another file name to open another.
		while(!inp){
			cout<<"Please input a valid file name:\t";	
			cin>>file;
			inp.open(file,ios::in);			
		}	
		}	
		//read file	
		memset( inputMax, 0, 25*sizeof(int) );
		for(i=0;i<learnSamples;i++){
			for(j=0;j<25;j++){
				inp>>input[i][j];
				if(input[i][j]>inputMax[j])
					inputMax[j] = input[i][j];
			}
		}	
	}
 
    // �q�`���üh���e�ץi�H�]�m��
    // hiddenLayerSize=(inputLayerSize+outputLayerSize)/2
    // �A���O�\�h�רҨä��ມ���ݨD�A�ӬO�n�ι��տ��~�k�h�D�o�X�A�����üh�e��
    // �C�e��Learning�ŧi���i�H�Q���s�]�˪��A�N�O?�F�o�ӡA��M�A�]�i�H�⥦�]
    // �˦b�@�_�C�p�G�@�Ӿǲ߶g�����X��A��K�פ����N�յ۽վ����üh�e�סC
    void Learning(double LearnSpeed, int HiddenLayerSize, int CycleLimit,
        double Precision)
    {
		LoadSample();
        while (bestDiffent > Precision) {
            // if (EventHiddenLayerChange != null) EventHiddenLayerChange(HiddenLayerSize);
            this->RobotBase::Learning(LearnSpeed, HiddenLayerSize, CycleLimit,
                Precision);
            if (bestDiffent > Precision) {
                HiddenLayerSize = (int)(HiddenLayerSize * 1.2 + 1);
            }
            // ==== DEBUG ====
            printf( "bestDiffent=%0.8f, Precision=%0.8f, HiddenLayerSize=%d\n\n",
                bestDiffent, Precision, HiddenLayerSize );
        }
    }
};
// end of class RobotXOR
 
 
// ===========================================================================
int main()
{
    RobotXOR *Robot;
    srand ( time(NULL) );   /* initialize random seed: */
    printf( "Hello~\n" );
 
    Robot = new RobotXOR;
   
    Robot->Learning(
        1500,    // double LearnSpeed
        10,      // int HiddenLayerSize
        250,      // int CycleLimit
        0.2  // double Precision
    );
    delete Robot;
 
    printf( "Bye~\n" );
	
}

