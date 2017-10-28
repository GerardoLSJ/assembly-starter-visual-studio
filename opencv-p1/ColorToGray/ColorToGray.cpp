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
cv::Mat img_blur_asm;

void gaussianBlur() {
	img_original = cv::imread("Lena.jpg", CV_LOAD_IMAGE_COLOR);
	cv::cvtColor(img_original, img_original, cv::COLOR_BGR2GRAY);

	std::cout << "DATA FOR THE ORIGINAL IMAGE:" << std::endl;
	std::cout << "Image width= " << img_original.cols << std::endl;
	std::cout << "Image height= " << img_original.rows << std::endl;
	std::cout << "Number of channels= " << img_original.channels() << std::endl;
	std::cout << "Element size in bytes= " << img_original.elemSize() << std::endl;
	std::cout << "Row size in bytes= " << img_original.step << std::endl;

	img_blur = cv::Mat(img_original.rows, img_original.cols, CV_8UC1); //8 bits unsigned chanel 1 
	img_blur_asm = cv::Mat(img_original.rows, img_original.cols, CV_8UC1); //8 bits unsigned chanel 1 

	int kernel_size = 12;
	int kernel_length = 4 * kernel_size*kernel_size + 4 * kernel_size + 1;
	float* kernel = (float*)malloc(kernel_length * sizeof(float));

	for (int i = 0; i <kernel_length; i++) {
		kernel[i] = float(1.0) / (kernel_length);
	}

	int* pix_addr = (int*)malloc((2 * kernel_size + 1) * (2 * kernel_size + 1) * sizeof(int));


	

	/**
	int i =  kernel_size;
	int j=  kernel_size;
	int k1	= -kernel_size;
	int k2	= -kernel_size;
	//DEBUG
	int contador = 0;
	int contador_j = 0;
	int contador_k1 = 0;
	int contador_k2 = 0;
	//DEBUG

	//ADDRESS
	int pix_x = 0;
	int pix_y = 0;
	int k_x = 0;
	int k_y = 0;
	int parte_1 = 0;
	int parte_2 = 0;
	int parte_3 = 0;
	float suma = 0.0;
	int int_aux = 0;
	float aux = 0.0;
	*/
	int i = kernel_size - 1;
	int j = kernel_size - 1;
	int k1 = -kernel_size - 1;
	int k2 = -kernel_size - 1;

	int pix_x = 0;
	int pix_y = 0;
	int k_x = 0;
	int k_y = 0;
	int pix_aux = 0;
	int offset_data = 0;
	float suma = 0.0;

	__asm {
	main:
		call for_i
		jmp end_loop;// end program
	for_i:
		mov eax, i
		inc eax;//Incrementar i++
		mov i, eax;//Asignar

		call for_j
		;;;;//RETURN
		mov edx, kernel_size
		dec edx
		mov j, edx;//termino la vuelta j

		mov eax, img_original.rows;// para checar el valor sin atar a registro EAX 
		sub eax, kernel_size;// img_original.rows - kernel_size

		mov ecx, i
		cmp eax, ecx;// IF i < img_original.rows - kernel_size
		jnl for_i
		ret;// end call

	for_j:
		mov eax, j;//j++
		inc eax
		mov j, eax

		fldz;//para suma, sto=0.0

		call for_k1
			;;;;//RETURN

		mov ecx, i
		mov edx, j
		mov eax, img_original.cols
		mov ebx, img_blur_asm.data

		imul eax, ecx;// cols*i
		add eax, edx;// (cols*i + j)

		fistp suma;//valor de suma
		xor edx, edx;// porque vamos a usar solo 8 bits
		mov edx, suma
		mov[ebx + eax], dl;// img_blur.data [cols*i + j] = suma (8 bits)

		mov offset_data, 0


		mov ecx, kernel_size;// k1 = -kernel_size
		neg ecx
		dec ecx
		mov k1, ecx

		mov eax, img_original.cols
		mov edx, kernel_size
		sub eax, edx
		mov ecx, j

		cmp eax, ecx;// IF j < img_orig.cols - kernel_size
		jnl for_j
		ret;// end call
	for_k1:
		mov ecx, k1
		inc ecx
		mov k1, ecx;// k1++

		call for_k2
			;;;//RETURN
		mov ecx, kernel_size
		neg ecx
		dec ecx
		mov k2, ecx

		mov ecx, k1
		mov eax, kernel_size

		cmp eax, ecx;// IF k1 <= kernel_size
		jnle for_k1;//Jump if not less or equal
		ret;// end call
	for_k2:
		mov ecx, k2
		inc ecx
		mov k2, ecx;// k2++

		;//int pix_x = i + k1;
		mov eax, i
		mov ebx, k1
		add eax, ebx
		mov pix_x, eax
		;//int pix_y = j + k2;
		mov eax, j
		add eax, ecx
		mov pix_y, eax
		;//int k_x = k1 + kernel_size;
		mov edx, kernel_size
		mov eax, k1
		add eax, edx
		mov k_x, eax
		;//int k_y = k2 + kernel_size;
		mov edx, kernel_size
		mov eax, k2
		add eax, edx
		mov k_y, eax

		;// suma += img_original.data[pix_x * img_original.cols + pix_y] * kernel[k_x*(2 * kernel_size + 1) + k_y]
		;// kernel[k_x*(2 * kernel_size + 1) + k_y]

		mov esi, kernel
		mov edx, offset_data
		add esi, edx;// !!! Direccion dentro de KERNEL FLOAT
		add edx, 4
		mov offset_data, edx

		;// img_original.data[pix_x * img_original.cols + pix_y]
		mov eax, pix_x
		mov ebx, pix_y
		mov ecx, img_original.cols
		imul eax, ecx
		add eax, ebx;// img_original.data[pix_x * img_original.cols + pix_y]

		mov edi, img_original.data
		add edi, eax;// Direccion del valor donde esta el pixel

		fld[esi];// valor de img.data[direccion]
		xor eax, eax
		mov al, [edi];//mov solo 8 bits
		mov pix_aux, eax;//auxiliar para poder cargar en la FLD

		fild[pix_aux];// INT pixel en FLOAT
		fmulp st(1), st(0)
		faddp st(1), st(0);// se queda en ST hasta que lo sacamos en for_j

		mov eax, k2
		mov ebx, kernel_size
		cmp  ebx, eax; IF k2 <= kernel_size
		jnle for_k2
		ret;// end call


	end_loop:
		nop;// end program 

	}
	

	/*
	for (int i = kernel_size; i < img_original.rows - kernel_size; i++) {
		for (int j = kernel_size; j < img_original.cols - kernel_size; j++) {
			float suma = 0.0;

			for (int k1 = -kernel_size; k1 <= kernel_size; k1++) {
				for (int k2 = -kernel_size; k2 <= kernel_size; k2++) {
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
	
*/
	/*
	
			
get_addresses :
		mov ebp, pix_addr
		mov esi, kernel_size
		imul esi, img_original.cols
		add esi, kernel_size
		sub esi, edi
		neg esi

		mov eax, 2
		imul eax, 2
		inc eax
		
		mov edx, 0
		mov ebx, 0
	for_k1:
		mov ecx, 0
	for_k2:
		mov [EBP + edx], ESI
		inc ESI
		add edx, 4
		inc ecx
		cmp ecx, eax
		jl for_k2

		sub ESI, eax
		add ESI, img_original.cols
		inc ebx
		cmp ebx, eax
		jl for_k1



	*/

	//cv::namedWindow("blur Image");
	cv::imshow("Original Image", img_original);
	cv::imshow("img_blur_asm ", img_blur_asm);
	
	/*cv::imshow("blur", img_blur);
	std::cout << "DATA FOR THE ORIGINAL IMAGE:" << std::endl;
	std::cout << "Image width= " << img_blur.cols << std::endl;
	std::cout << "Image height= " << img_blur.rows << std::endl;
	std::cout << "Number of channels= " << img_blur.channels() << std::endl;
	std::cout << "Element size in bytes= " << img_blur.elemSize() << std::endl;
	std::cout << "Row size in bytes= " << img_blur.step << std::endl;

	free(kernel);
	*/
}

void callback_click(int event, int x, int y, int flags, void* userdata)
{
	if (event == cv::EVENT_LBUTTONDOWN)
	{
		std::cout << std::endl;
		std::cout << "Image clicked in (" << x << ", " << y << ")" << std::endl;
		std::cout << "Index of pixel: " << (y * img_original.cols + x) << std::endl;
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
		mov edi, img_original.data; Origen img.data, destino ESI de proposito gral
		mov esi, img_grayscale_asm.data; Direccion destino de img
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

			div bl; Dividendo en AX divisor en BL
			mov[esi], al; cociente en al residuo en AH
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


int main()
{
	//Color2Gray();
	gaussianBlur();

	while (cv::waitKey(15) != 27)
	{

	}
	return 0;


}

