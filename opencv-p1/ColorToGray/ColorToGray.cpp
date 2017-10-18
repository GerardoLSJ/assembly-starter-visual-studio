// ColorToGray.cpp: define el punto de entrada de la aplicación de consola.
//

#include "stdafx.h"
#include "opencv2\opencv.hpp"
#include <iostream>

cv::Mat img_original;
cv::Mat img_grayscale;
cv::Mat img_grayscale_cpp;
cv::Mat img_grayscale_asm;
cv::Mat img_blur;

void callback_click(int event, int x, int y, int flags, void* userdata)
{
	if (event == cv::EVENT_LBUTTONDOWN)
	{
		std::cout << std::endl;
		std::cout << "Image clicked in (" << x << ", " << y << ")" << std::endl;
		std::cout << "Index of pixel: "<< (y * img_original.cols + x)<< std::endl;
		int idx_b = (y*img_original.step + x*img_original.channels());
		int idx_g = idx_b + 1;
		int idx_r = idx_b + 2;
		std::cout << "Indices of BGR for this pixel: " << idx_b << ", " << idx_g << ", " << idx_r << std::endl;
		std::cout << "BGR values for this pixel: ";
		std::cout << (int)img_original.data[idx_b] << ", " << (int)img_original.data[idx_g] << ", " << (int)img_original.data[idx_r] << std::endl;
	} 
}

int Color2Gray() {
	img_original = cv::imread("Cats.jpg", CV_LOAD_IMAGE_COLOR);
	img_grayscale = cv::Mat(img_original.rows, img_original.cols, CV_8UC1);
	img_grayscale_cpp = cv::Mat(img_original.rows, img_original.cols, CV_8UC1);
	img_grayscale_asm = cv::Mat(img_original.rows, img_original.cols, CV_8UC1);
	cv::namedWindow("Original Image");
	cv::imshow("Original Image", img_original);
	cv::setMouseCallback("Original Image", callback_click, NULL);

	std::cout << "DATA FOR THE ORIGINAL IMAGE:" << std::endl;
	std::cout << "Image width= " << img_original.cols << std::endl;
	std::cout << "Image height= " << img_original.rows << std::endl;
	std::cout << "Number of channels= " << img_original.channels() << std::endl;
	std::cout << "Element size in bytes= " << img_original.elemSize() << std::endl;
	std::cout << "Row size in bytes= " << img_original.step << std::endl;

	std::cout << "DATA FOR THE GRAYSCALE IMAGE:" << std::endl;
	std::cout << "Image width= " << img_grayscale.cols << std::endl;
	std::cout << "Image height= " << img_grayscale.rows << std::endl;
	std::cout << "Number of channels= " << img_grayscale.channels() << std::endl;
	std::cout << "Element size in bytes= " << img_grayscale.elemSize() << std::endl;
	std::cout << "Row size in bytes= " << img_grayscale.step << std::endl;

	long init_count;
	long end_count;


	int init_count_asm = cv::getTickCount();
	__asm {
		mov edi, img_original.data					; Origen img.data, destino ESI de proposito gral
		mov esi, img_grayscale_asm.data				; Direccion destino de img
		mov ecx, img_original.cols
		imul ecx, img_original.rows
		imul ecx, 3
		add ecx, edi
		mov bl, 3
		xor edx, edx

		for_loop :
		xor eax, eax
			mov al, [edi]
			mov dl, [edi + 1]
			add ax, dx
			mov dl, [edi + 2]
			add ax, dx

			div bl								; Dividendo en AX divisor en BL
			mov[esi], al						; cociente en al residuo en AH
			add edi, 3
			inc esi
			cmp edi, ecx
			jl for_loop


	}
	int end_count_asm = cv::getTickCount();
	std::cout << "Elapsed time [s] with asm: " << (end_count_asm - init_count_asm) / cv::getTickFrequency() << std::endl;



	int init_count_cpp = cv::getTickCount();
	for (int i = 0; i < img_original.rows * img_original.cols; i++) {
		int b = img_original.data[3 * i];
		int g = img_original.data[3 * i + 1];
		int r = img_original.data[3 * i + 2];
		img_grayscale_cpp.data[i] = (b + g + r) / 3;
	}
	int end_count_cpp = cv::getTickCount();
	std::cout << "Elapsed time [s] with CPP: " << (end_count_cpp - init_count_cpp) / cv::getTickFrequency() << std::endl;


	init_count = cv::getTickCount();
	cv::cvtColor(img_original, img_grayscale, cv::COLOR_BGR2GRAY);
	end_count = cv::getTickCount();
	std::cout << "Elapsed time [s] with OpenCV: " << (end_count - init_count) / cv::getTickFrequency() << std::endl;
	cv::imshow("Grayscale Image", img_grayscale);
	cv::imshow("CPP Image", img_grayscale_cpp);
	cv::imshow("ASM Image", img_grayscale_asm);

}

void gaussianBlur() {
	img_original = cv::imread("Lena.jpg", CV_LOAD_IMAGE_COLOR);
	cv::cvtColor(img_original, img_original, cv::COLOR_BGR2GRAY);
	
	std::cout << "DATA FOR THE ORIGINAL IMAGE:" << std::endl;
	std::cout << "Image width= " << img_original.cols << std::endl;
	std::cout << "Image height= " << img_original.rows << std::endl;
	std::cout << "Number of channels= " << img_original.channels() << std::endl;
	std::cout << "Element size in bytes= " << img_original.elemSize() << std::endl;
	std::cout << "Row size in bytes= " << img_original.step << std::endl;

	img_blur 	 = cv::Mat(img_original.rows, img_original.cols, CV_8UC1); //8 bits unsigned chanel 1 
	int kernel_size = 15;
	int kernel_len = 4 * kernel_size*kernel_size + 4 * kernel_size + 1;
	float* kernel = (float*)malloc(kernel_len*sizeof(float));

	for (int i = 0; i <kernel_len; i++) {
		kernel[i] = float(1.0) / (kernel_len);
	}


	for (int i = kernel_size; i < img_original.rows - kernel_size; i++) {
		for (int j = kernel_size; j < img_original.cols - kernel_size; j++) {
			float suma = 0.0;
			
			for (int k1 = -kernel_size; k1 <= kernel_size; k1++){
				for (int k2 = -kernel_size; k2 <= kernel_size; k2++){
					int pix_x = i + k1;
					int pix_y = j + k2;
					int k_x = k1 + kernel_size;
					int k_y = k2 + kernel_size;

					suma += img_original.data[pix_x * img_original.cols + pix_y] * kernel[k_x*(2 * kernel_size + 1) + k_y];


				}
			}

			img_blur.data[i * img_original.cols + j] = (int)suma;


		}
	}

	//cv::namedWindow("blur Image");
	cv::imshow("Original Image", img_original);
	cv::imshow("blur", img_blur);
	std::cout << "DATA FOR THE ORIGINAL IMAGE:" << std::endl;
	std::cout << "Image width= " << img_blur.cols << std::endl;
	std::cout << "Image height= " << img_blur.rows << std::endl;
	std::cout << "Number of channels= " << img_blur.channels() << std::endl;
	std::cout << "Element size in bytes= " << img_blur.elemSize() << std::endl;
	std::cout << "Row size in bytes= " << img_blur.step << std::endl;

	//free(kernel);
}
int main()
{
	//Color2Gray();
	gaussianBlur();

	while (cv::waitKey(15) != 27)
	{

	}
    return 0;
}

