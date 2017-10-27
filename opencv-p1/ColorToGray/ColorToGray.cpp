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

void gaussianBlur() {
	img_original = cv::imread("ColorTest.jpg", CV_LOAD_IMAGE_COLOR);
	cv::cvtColor(img_original, img_original, cv::COLOR_BGR2GRAY);

	std::cout << "DATA FOR THE ORIGINAL IMAGE:" << std::endl;
	std::cout << "Image width= " << img_original.cols << std::endl;
	std::cout << "Image height= " << img_original.rows << std::endl;
	std::cout << "Number of channels= " << img_original.channels() << std::endl;
	std::cout << "Element size in bytes= " << img_original.elemSize() << std::endl;
	std::cout << "Row size in bytes= " << img_original.step << std::endl;

	img_blur = cv::Mat(img_original.rows, img_original.cols, CV_8UC1); //8 bits unsigned chanel 1 
	int kernel_size = 2;
	int kernel_length = 4 * kernel_size*kernel_size + 4 * kernel_size + 1;
	float* kernel = (float*)malloc(kernel_length * sizeof(float));

	for (int i = 0; i <kernel_length; i++) {
		kernel[i] = float(1.0) / (kernel_length);
	}

	int* pix_addr = (int*)malloc((2 * kernel_size + 1) * (2 * kernel_size + 1) * sizeof(int));


	
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


	__asm {
	main:


	for_i:
		mov ecx, kernel_size
		mov j, ecx
		; // IF i = 0, jmp 	

		mov eax, i
		mov ebx, img_original.rows	 ;// para checar el valor sin comprometer ebx
		sub ebx, kernel_size
		cmp eax, ebx
		je end_loop
		inc eax			;//Incrementar i++
		mov i, eax		;//Asignar

		mov ecx, contador
		inc ecx
		mov contador, ecx
	
	for_j:
		mov ecx, kernel_size
		neg ecx
		mov k1, ecx
		
		//xor eax, eax
		mov eax, j
		mov ebx, img_original.cols
		sub ebx, kernel_size
		cmp eax, ebx
		je for_i
		inc eax			;//j++
		mov j, eax

		mov ecx, contador_j
		inc ecx
		mov contador_j, ecx
		
		



	;//for (int k1 = -kernel_size; k1 <= kernel_size; k1++) {
	for_k1:
		mov ecx, kernel_size
		neg ecx
		mov k2, ecx

		mov eax, k1
		mov ebx, kernel_size
		cmp eax, ebx
		je for_j

		inc eax			;//j++
		mov k1, eax

		mov ecx, contador_k1
		inc ecx
		mov contador_k1, ecx


	for_k2:
		mov eax, k2
		mov ebx, kernel_size
		cmp eax, ebx
		je for_k1

		inc eax			;//j++
		mov k2, eax

		;//int pix_x = i + k1;
		mov eax, i
		add eax, k1
		mov pix_x, eax
		;//int pix_y = j + k2;
		mov eax, j
		add eax, k2
		mov pix_y, eax
		;//int k_x = k1 + kernel_size;
		mov eax, k1
		add eax, kernel_size
		mov k_x, eax
		;//int k_y = k2 + kernel_size;
		mov eax, k2
		add eax, kernel_size
		mov k_y, eax
		;//int parte_1 = pix_x * img_original.cols + pix_y
		mov eax, pix_x
		mov ebx, img_original.cols
		imul eax,ebx
		add eax, pix_y
		mov parte_1, eax
		;//int parte_2 = k_x*(2 * kernel_size + 1) + k_y
		mov eax, kernel_size
		imul eax, 2
		add eax, 1
		mov ebx, k_x
		imul eax, ebx
		
		imul eax, 8

		add eax, k_y

	

		mov parte_2, eax
		;//int parte_3 = img_original.data[parte_1] * kernel[parte_2] <<<---- KERNEL float
	
		mov ebx, img_original.data
		add ebx, parte_1
		xor ecx, ecx
		mov cl, [ebx]
		mov int_aux, ecx
		fild int_aux		; Load 32 - bit DWORD value at top of stack to ST(0)

		mov ebx,kernel
		add ebx, parte_2
		mov eax, [ebx]
		fld [ebx];//LOAD kernel[PARTE_2]

		xor edx, edx
		add eax, parte_1
		lea ebx, kernel
		add ebx, parte_2
		
		ffree st(0)
		ffree st(1)

		jmp for_k2
		

	end_loop:
		mov eax, img_original.rows
		mov ebx, contador
		;//add ebx, 4
		mov ecx, img_original.cols
		mov edx, contador_j
		mov edi, 4
		imul edi,img_original.rows
		;//add edx, edi
		xor edi, edi

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

	/*
	void gaussianBlurASM()
{
	img_original = cv::imread("Lena.jpg", CV_LOAD_IMAGE_COLOR);
	cv::cvtColor(img_original, img_original, cv::COLOR_BGR2GRAY);

	std::cout << "DATA FOR THE ORIGINAL IMAGE:" << std::endl;
	std::cout << "Image width= " << img_original.cols << std::endl;
	std::cout << "Image height= " << img_original.rows << std::endl;
	std::cout << "Number of channels= " << img_original.channels() << std::endl;
	std::cout << "Element size in bytes= " << img_original.elemSize() << std::endl;
	std::cout << "Row size in bytes= " << img_original.step << std::endl;

	img_blur_asm = cv::Mat(img_original.rows, img_original.cols, CV_8UC1); //8 bits unsigned chanel 1 
	int kernel_size = 2;
	int kernel_length = 4 * kernel_size*kernel_size + 4 * kernel_size + 1;
	float* kernel = (float*)malloc(kernel_length * sizeof(float));

	int * pix_addr = (int*)malloc(kernel_length * sizeof(int)); //guarda las direcciones de memoria de la matriz kernel

	for (int i = 0; i <kernel_length; i++) {
		kernel[i] = float(1.0) / (kernel_length);
		std::cout << "ki:" << kernel[i] <<std::endl;
		
	}
	
	int i = kernel_size-1;
	int j = kernel_size-1;
	int k1 = -kernel_size-1;
	int k2 = -kernel_size-1;

	int pix_x = 0;
	int pix_y = 0;
	int k_x = 0;
	int k_y = 0;
	int pix_aux = 0;
	int ki = 0;
	float suma = 0.0;

	__asm
	{
		main:

			CALL for_i

			JMP fin

		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

		for_i:; recorre filas i

			MOV ecx, i; se carga i se incrementa y se regresa
			INC ecx
			MOV i, ecx

			CALL for_j

			MOV eax, img_original.rows; se cargan los datos para realizar la comparacion
			MOV edx, kernel_size
			SUB eax, edx

			; se reinicia j
			MOV edx, kernel_size
			DEC edx
			MOV j, edx
			MOV ecx, i

					; CMP i < img_original.rows - kernel_size
			CMP eax, ecx
			JNL for_i

			ret

		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

		  for_j:; recorre cols j

			  MOV ecx, j; se carga j se incrementa y se regresa
			  INC ecx
			  MOV j, ecx

			  FLDZ; se carga un 0 a la pila de float para suma = 0

			  CALL for_k1

			  MOV ecx, i; se carga i, j, cols, y la direccion de img_blur
			  MOV edx, j
			  MOV eax, img_original.cols
			  MOV ebx, img_blur_asm.data

			  IMUL eax, ecx; se hace la multuplicacion de cols * i
			  ADD eax, edx; se suma j a la multiplicacion

			  ; cargar de st0 a suma
			  FISTP suma
			  XOR edx, edx
			  MOV edx, suma

			  MOV[ebx + eax], dl; se guarda suma a la dirccion en img_blur
			  ; img_blur.data[i * img_original.cols + j] = (int)suma
			  MOV ki,0
			  ; se reinicia k1
			  MOV ecx, kernel_size
			  NEG ecx
			  DEC ecx
			  MOV k1, ecx

			  MOV eax, img_original.cols; se cargan los datos para realizar la comparacion
			  MOV edx, kernel_size
			  SUB eax, edx
			  MOV ecx, j

			  ; CMP j < img_original.cols - kernel_size
			  CMP eax, ecx
			  JNL for_j

			  ret

		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

			for_k1:

				MOV ecx, k1; se carga k1 se incrementa y se regresa
				INC ecx
				MOV k1, ecx

				CALL for_k2

				; se reinicia k2
				MOV ecx, kernel_size
				NEG ecx
				DEC ecx
				MOV k2, ecx

				MOV ecx, k1
				MOV eax, kernel_size
					
					; CMP k1 <= kernel_size
				CMP eax,ecx
				JNLE for_k1

				ret

		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

			for_k2:

				MOV ecx, k2; se carga k2 se incrementa y se regresa
				INC ecx
				MOV k2, ecx

				; pix_x = i + k1
				MOV eax, i
				MOV ebx, k1

				ADD eax, ebx
				MOV pix_x, eax

				; pix_y = j + k2;
				MOV eax, j

				ADD eax, ecx
				MOV pix_y, eax

				; k_x = k1 + kernel_size;
				MOV edx, kernel_size
				MOV eax, k1
				ADD eax, edx
				MOV k_x, eax

				; k_y = k2 + kernel_size;
				MOV edx, kernel_size
				MOV eax, k2
				ADD eax, edx
				MOV k_y, eax

				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

				; suma += img_original.data[pix_x * img_original.cols + pix_y] * kernel[k_x*(2 * kernel_size + 1) + k_y]
				; kernel[k_x*(2 * kernel_size + 1) + k_y]
				MOV esi, kernel
				MOV edx, ki
				ADD esi, edx ;-----------------> direccion del valor que se saca de kernel

				ADD edx,4
				MOV ki, edx
					
				; img_original.data[pix_x * img_original.cols + pix_y]
				MOV eax, pix_x
				MOV ebx, pix_y
				MOV ecx, img_original.cols

				IMUL eax, ecx
				ADD eax, ebx	;ya no mas operaciones

				MOV edi, img_original.data
				ADD edi, eax;----------------->direccion del valor que se saca de la imagen

					;;;;;; Hay que sacar el valor de suma, luego kenrel  y ponerlos en st
					;meter el valor de img_original en st y multiplic kernel con img
					;se hace ADD del resultaod a suma TODO ESTO ES EN FLOTANTES O SE CASTEA
					
				FLD [esi]; carga el valor de kernel
				XOR eax, eax
				MOV al, [edi]
				MOV pix_aux, eax
				FILD [pix_aux] ;carga el valor del pixel

				FMULP st(1), st(0)
				FADDP st(1), st(0)

				MOV eax, k2
				MOV ebx, kernel_size

				; CMP k2 <= kernel_size
				CMP  ebx, eax
				JNLE for_k2

				ret
		fin:
				NOP
	}
	
	
	cv::imshow("Original Image", img_original);
	cv::imshow("blur", img_blur_asm);
	//cv::imshow("blur", img_blur_asm);
}
	*/
}

