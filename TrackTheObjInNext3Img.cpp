#include "stdafx.h"
#include <opencv2/opencv.hpp>  
#include <iostream>
#include <string>
#include <vector>
#include "csv.hpp"
#include <stdio.h>
#include <Windows.h>
#include <atlstr.h> 
using namespace std;
#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS

const char *sourceFolder= "\\images\\img\\"; //img�t�H���_
/* the files in the folder
2017-12-01-07h-32m-18s_00001.jpg
2017-12-01-07h-32m-18s_00002.jpg
2017-12-01-07h-32m-18s_00003.jpg
*/


const char *objectFolder ="\\images\\object\\"; // object�t�H���_
/*
2017-12-01-07h-32m-18s_00001_01.jpg
2017-12-01-07h-32m-18s_00001_02.jpg
2017-12-01-07h-32m-18s_00002_01.jpg
2017-12-01-07h-32m-18s_00002_02.jpg
2017-12-01-07h-32m-18s_00003_01.jpg
2017-12-01-07h-32m-18s_00003_02.jpg
*/


const char *input_csv_file ="\\images\\object\\list.csv"; //����CSV�t�@�C������ݒ肷��

/* list.csv
2017-12-01-07h-32m-18s_00001_01.jpg,516,153,62.3
2017-12-01-07h-32m-18s_00001_02.jpg,911,234,50.9
2017-12-01-07h-32m-18s_00002_01.jpg,516,154,61.8
2017-12-01-07h-32m-18s_00002_02.jpg,913,234,60.5
2017-12-01-07h-32m-18s_00003_01.jpg,516,153,57.8
2017-12-01-07h-32m-18s_00003_02.jpg,912,235,60.8
*/

TCHAR currentDir[1000]; //���s�t�H���_
char *output_csv_file; //���ʏo�͗pCSV�t�@�C����
/*
2017-12-01-07h-32m-18s_00001.jpg,2017-12-01-07h-32m-18s_00001_01.jpg,516,1532017-12-01-07h-32m-18s_00002.jpg,2017-12-01-07h-32m-18s_00001_01.jpg,509,1362017-12-01-07h-32m-18s_00003.jpg,2017-12-01-07h-32m-18s_00001_01.jpg,509,1362017-12-01-07h-32m-18s_00001.jpg,2017-12-01-07h-32m-18s_00001_02.jpg,911,2342017-12-01-07h-32m-18s_00002.jpg,2017-12-01-07h-32m-18s_00001_02.jpg,904,2272017-12-01-07h-32m-18s_00003.jpg,2017-12-01-07h-32m-18s_00001_02.jpg,904,2272017-12-01-07h-32m-18s_00002.jpg,2017-12-01-07h-32m-18s_00002_01.jpg,516,1542017-12-01-07h-32m-18s_00003.jpg,2017-12-01-07h-32m-18s_00002_01.jpg,508,1352017-12-01-07h-32m-18s_00002.jpg,2017-12-01-07h-32m-18s_00002_02.jpg,913,2342017-12-01-07h-32m-18s_00003.jpg,2017-12-01-07h-32m-18s_00002_02.jpg,906,2272017-12-01-07h-32m-18s_00003.jpg,2017-12-01-07h-32m-18s_00003_01.jpg,516,153
*/


int imageMaxNum; //�摜�t�@�C��������摜�ԍ����擾����

/*CSV�t�@�C�����𐶐�����*/
char *createCSVFileName();

/*Image�}�b�`����*/
int *matchTemplate(cv::Point &maxLoc,cv::Mat &image_template,char *strSource,char *strTemplate);

/*�摜�t�@�C��������摜�ԍ����擾����*/
int getNum(std::string fileName);

/*�������u������*/
std::string Replace(std::string String1, std::string String2, std::string String3);

int main()
{
	//���s�t�@�C���̃t�H���_���擾
	GetCurrentDirectory(1000, currentDir);

	//���ʏo�͗pCSV�t�@�C�����𐶐�����
	output_csv_file = createCSVFileName();

	//����csv�t�@�E������CSV�f�[�^�擾
	vector<vector<string>> data;
	
	char fileFullName[1000];
	sprintf(fileFullName, "%S%s", currentDir, input_csv_file);
	printf("%s\n", fileFullName);

	Csv objCsv(fileFullName);
	if (!objCsv.getCsv(data)) {
		cout << "[ERROR] �Ǎ��ݎ��s!" << endl;
		return 1;
	}

	std::vector<std::string> files;
	for (int row = 0; row < data.size() ; row++) {
		std::string oneLine;
		vector<string> rec = data[row];  // 1�s
		for (unsigned int col = 0; col < rec.size(); col++) {
			oneLine += rec[col].c_str();  // 1��
			if (col < rec.size() - 1) oneLine += ",";  // �s���ȊO�̓J���}�o��
		}
		files.push_back(oneLine); //���C���[�W
		cout << endl;
	}

	//�ő��ʔԍ����擾����
	std::string maxLine = files.back();
	imageMaxNum = getNum(maxLine);

	char replace1[100];
	char replace2[100];

	std::vector<std::string>::iterator it = files.begin();
	for (; it != files.end(); ++it) {
		std::cout << *it << std::endl;

		std::string s = (*it).c_str();
		std::stringstream ss{ s };

		std::string col1; //��1 file name
		std::string col2; //��2 coordinates X 
		std::string col3; //��3 coordinates Y 

		std::getline(ss, col1, ',');
		std::getline(ss, col2, ',');
		std::getline(ss, col3, ',');

		//Image�}�b�`�����֐�matchTemplate��1�Ԗڂ̃p�����[�^�[�𐶐�����
		cv::Point maxLoc;

		//Image�}�b�`�����֐�matchTemplate�̂S�Ԗڂ̃p�����[�^�[�𐶐�����
		std::string itString1 = col1;
		std::string itString2 = col1;
		std::string itString3 = col1;
		char *strTemplate = new char[itString1.size() + 1];
		strncpy(strTemplate, itString1.c_str(), itString1.size() + 1);

		//Image�}�b�`�����֐�matchTemplate�̂Q�Ԗڂ̃p�����[�^�[�𐶐�����
		cv::Mat image_template = cv::imread(col1, 1);

		/*�Y����ʃ}�b�`����*/
		//Image�}�b�`�����֐�matchTemplate�̂R�Ԗڂ̃p�����[�^�[�𐶐�����@�i�Y����ʂ̃t�@�C�������擾����j
		std::string::size_type posEnd = itString1.find_last_of('_');
		std::cout << itString1.substr(0, posEnd) << std::endl;
		std::string result = itString1.substr(0, posEnd) + ".jpg";
		char* strSource1 = new char[result.size() + 1];
		std::strcpy(strSource1, result.c_str());

		//matchTemplate(maxLoc, image_template, strSource1, strTemplate);

		//���S���W��CSV�t�@�C���֏o�͂���i���W���ɑ��݂��邽�߁A���WX,Y�����̂܂܏o��CSV�t�@�C���֏o�͂���j
		FILE *fp;
		fp = fopen(output_csv_file, "a");
		if (fp == NULL) {
			printf("%s�t�@�C�����J���܂���\n", output_csv_file);
		}
		else {
			fprintf(fp, "%s,%s,%s,%s\r", strSource1, strTemplate, col2, col3);
		}
		fclose(fp);

		//Image�}�b�`�����֐�matchTemplate�̂R�Ԗڂ̃p�����[�^�[�𐶐�����@�i���̉摜�̃t�@�C�������擾����j
		int j = getNum(itString2);
		sprintf(replace1, "%05d", j);
		sprintf(replace2, "%05d", j + 1);
		itString2 = Replace(strSource1, replace1, replace2);
		char* strSource2 = new char[itString2.size() + 1];
		std::strcpy(strSource2, itString2.c_str());
		/*����ʃ}�b�`����*/
		if (j < imageMaxNum) {
			//Image�}�b�`����*/
			matchTemplate(maxLoc, image_template, strSource2, strTemplate);
			cv::imshow("image1", image_template);
		}

		/*���X��ʃ}�b�`����*/
		if (j < (imageMaxNum - 1)) {
			//Image�}�b�`�����֐�matchTemplate�̂R�Ԗڂ̃p�����[�^�[�𐶐�����@�i���X�̉摜�̃t�@�C�������擾����j
			sprintf(replace1, "%05d", j + 1);
			sprintf(replace2, "%05d", j + 2);
			itString3 = Replace(strSource2, replace1, replace2);
			char* strSource3 = new char[itString3.size() + 1];
			std::strcpy(strSource3, itString3.c_str());
			matchTemplate(maxLoc,image_template,strSource3,strTemplate);
			cv::imshow("image1", image_template);
		}

		//if (cv::waitKey(0) == 27) break;
	}

	return 0;
}

/*CSV�t�@�C�����𐶐�����*/
char *createCSVFileName()
{
	time_t timep;
	time(&timep);
	char fileName[64];
	strftime(fileName, sizeof(fileName), "%Y%m%d%H%M%S", localtime(&timep));
	return fileName;
}

/*Image�}�b�`����*/
int *matchTemplate(cv::Point &maxLoc,cv::Mat &image_template,char *strSource,char *strTemplate)
{
	char sourceFile[1000];
	char objectFile[1000];
	
	sprintf(sourceFile, "%S%s%s", currentDir, sourceFolder, strSource);
	printf("%s\n", sourceFile);

	sprintf(objectFile, "%S%s%s", currentDir, objectFolder, strTemplate);
	printf("%s\n", objectFile);

	//source�Atemplate�C���[�W�t�@�C�������[�h���� 
	cv::Mat image_source = cv::imread(sourceFile, cv::IMREAD_GRAYSCALE);
	image_template = cv::imread(objectFile, cv::IMREAD_GRAYSCALE);

	cv::Mat image_matched;

	//template matching  
	cv::matchTemplate(image_source,
		image_template,
		image_matched,
		cv::TM_CCOEFF_NORMED);

	double minVal, maxVal;
	cv::Point minLoc;

	//look for the best matching place
	cv::minMaxLoc(image_matched, &minVal, &maxVal, &minLoc, &maxLoc);

	cv::Mat image_color;
	cv::cvtColor(image_source, image_color, CV_GRAY2BGR);

	//���S���W��CSV�t�@�C���֏o�͂���
	printf("���S���W�F X:%d Y:%d\n", maxLoc.x + image_template.cols / 2, maxLoc.y + image_template.rows / 2);
	FILE *fp;
	fp = fopen(output_csv_file, "a");
	if (fp == NULL) {
		printf("%s�t�@�C�����J���܂���\n", output_csv_file);
	}
	else {
		fprintf(fp, "%s,%s,%d,%d\r", strSource, strTemplate, maxLoc.x + image_template.cols / 2, maxLoc.y + image_template.rows / 2);
	}
	fclose(fp);

	return 0;
}

/*�摜�t�@�C��������摜�ԍ����擾����*/
int getNum(std::string fileName)
{
	std::string::size_type posEnd = fileName.find_last_of('_');
	std::cout << fileName.substr(0, posEnd) << std::endl;
	std::string::size_type posStart = fileName.substr(0, posEnd).find_last_of('_');
	std::cout << fileName.substr(posStart + 1, posEnd - posStart) << std::endl;
	return  std::atoi(fileName.substr(posStart + 1, posEnd - posStart).c_str());;
}

/*�������u������*/
std::string Replace(std::string String1, std::string String2, std::string String3)
{
	std::string::size_type  Pos(String1.find(String2));

	while (Pos != std::string::npos)
	{
		String1.replace(Pos, String2.length(), String3);
		Pos = String1.find(String2, Pos + String3.length());
	}

	return String1;
}


