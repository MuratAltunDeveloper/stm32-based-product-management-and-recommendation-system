/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_host.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "string.h"


// Button pin definitions
// Button pin definitions
#define BUTTON_UP_PIN  GPIO_PIN_0
#define BUTTON_DOWN_PIN  GPIO_PIN_2
#define BUTTON_SELECT_PIN  GPIO_PIN_4
#define BUTTON_PORT GPIOD
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

I2S_HandleTypeDef hi2s3;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2S3_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);
void MX_USB_HOST_Process(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

	//global aşama seç değişkenleri tanımlanıyor:
	//burada şimdi kontrol tanımlama
    int sayac1=0;
	// Diğer değişken tanımlamaları
	int yukaributonbas;
	int asagibutonbas;
	int onaybutonbas;
	int art = 0;
	int mantik = 1;
	int screen_switch = 0;
	int menu2 = 0;
	int state = 1;
	int control = 0;
	int rtnMainPage = 1;//rtnMainpage 1 yaptım ürün ve zaman yönetimine girsin diye
	int upbuton = 0;
	int bottombuton = 0;
	int pagecroos = 0;
	int choseProduct = 0;
	char* products[] = {"pano sistem ", "dataloger", "pompa sistem","geri"};
	int productscounter[]={-1,-1,-1};//üretilen ürün sayısı dizisi
	int giris=0;
	int control_cikis=0;
	int cnttime=0;
	int  whichMonths[]={0,0,0};

	// Global KESME değişkenlerİ
	char rxBuffer[256]; // 256 baytlık veri tamponu
	extern UART_HandleTypeDef huart2;


  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_I2S3_Init();
  MX_SPI1_Init();
  MX_USB_HOST_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  ssd1306_Init();
  ssd1306_Fill(White);
  ssd1306_UpdateScreen();


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  // Önceki buton durumlarını saklamak için değişkenler
       int prevButtonState1 = 0;
       int prevButtonState2 = 0;
       int prevButtonState3 = 0;

       // LED durumlarını saklamak için değişkenler
       int ledState1 = 0;
       int ledState2 = 0;
       int ledState3 = 0;
       //MENU SAYACI
       int menu = 0;

//TAHMİN KESMESİ
       // UART kesme geri çağırma fonksiyonu
       void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
           if (huart->Instance == USART2) {
               // OLED ekranı temizle ve veri yazdır
               ssd1306_Fill(Black);
               ssd1306_SetCursor(0, 0);
               SSD1306_Puts(rxBuffer, &Font_7x10, 1);
               ssd1306_UpdateScreen();

               // Veri üzerinde tahmin yap
               PredictFunc(rxBuffer);

               // Buffer'ı temizle
               memset(rxBuffer, 0, sizeof(rxBuffer));

               // Tekrar veri alımını başlat
               HAL_UART_Receive_IT(&huart2, (uint8_t*)rxBuffer, sizeof(rxBuffer));
           }
       }




       void updateDisplay(int menu)
       {
           ssd1306_Fill(Black);
           ssd1306_SetCursor(0, 0);
           ssd1306_WriteString("is Sistemi\n", Font_7x10, White);
           if (menu == 0)
           {
               ssd1306_SetCursor(0, 20);
               ssd1306_WriteString("> tahminler\n", Font_7x10, White);
               ssd1306_SetCursor(0, 30);
               ssd1306_WriteString("urunler\n", Font_7x10, White);
           }
           else if (menu == 1)
           {
               ssd1306_SetCursor(0, 20);
               ssd1306_WriteString("tahminler\n", Font_7x10, White);
               ssd1306_SetCursor(0, 30);
               ssd1306_WriteString("> urunler\n", Font_7x10, White);
           }
           ssd1306_UpdateScreen();
       }
//burada ay şeçim işlemleri yapılıyor
       void TimeManager(int whichPrd) {
           printf("time manager       time manager\n");
           ssd1306_WriteString("Ay Secimi", Font_7x10, White);

           int twoCntOnayBtn = 0;
           int cnt = 0;
           cnttime++;
           while(cnttime > 1) {
               // Ekran özelliklerini belirle
               ssd1306_Fill(Black); // Ekranı temizle
               ssd1306_SetCursor(0, 40); // İmleci konumlandır

               // Yukarı buton basma durumu
               if(HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_UP_PIN) == GPIO_PIN_SET) {
                   cnt++;
                   if(cnt >= 9) {
                       cnt = 9;
                   }
               }

               // Aşağı buton basma durumu
               if(HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_DOWN_PIN) == GPIO_PIN_SET) {
                   cnt--;
                   if(cnt <= 0) {
                       cnt = 1;
                   }
               }

               // Onay butonu basma durumu
               if(HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_SELECT_PIN) == GPIO_PIN_SET) {
                   twoCntOnayBtn++;
                   if(twoCntOnayBtn > 1) {
                       cnttime = 0;
                       rtnMainPage = 0;
                       break;
                   }
               }

               // Ayları ekrana yazdır
               switch(cnt) {
                   case 1:
                       whichMonths[whichPrd] = 1;
                       ssd1306_WriteString("ocak", Font_7x10, White);
                       break;
                   case 2:
                       whichMonths[whichPrd] = 2;
                       ssd1306_WriteString("subat", Font_7x10, White);
                       break;
                   case 3:
                       whichMonths[whichPrd] = 3;
                       ssd1306_WriteString("mart", Font_7x10, White);
                       break;
                   case 4:
                       whichMonths[whichPrd] = 4;
                       ssd1306_WriteString("nisan", Font_7x10, White);
                       break;
                   case 5:
                       whichMonths[whichPrd] = 5;
                       ssd1306_WriteString("mayis", Font_7x10, White);
                       break;
                   case 6:
                       whichMonths[whichPrd] = 6;
                       ssd1306_WriteString("haziran", Font_7x10, White);
                       break;
                   case 7:
                       whichMonths[whichPrd] = 7;
                       ssd1306_WriteString("temmuz", Font_7x10, White);
                       break;
                   case 8:
                       whichMonths[whichPrd] = 8;
                       ssd1306_WriteString("agustos", Font_7x10, White);
                       break;
                   case 9:
                       whichMonths[whichPrd] = 9;
                       ssd1306_WriteString("eylul", Font_7x10, White);
                       break;
               }

               // Ekranı güncelle
               ssd1306_UpdateScreen();
               HAL_Delay(200);
           }
       }

       void ProductManager(int whichproduct) {
           printf("Product Manager\n");
           int uretilenproduct = productscounter[whichproduct];
           int cntrmanage = 0;

           while (control_cikis) {
               ssd1306_Fill(Black); // Ekranı temizle
               ssd1306_SetCursor(0, 16); // Başlangıç konumu

               if(productscounter[whichproduct] < 0) {
                   productscounter[whichproduct] = 0;
               }

               if (cntrmanage == 0) {
                   char buffer[20];
                   sprintf(buffer, ">artir %d     ", productscounter[whichproduct]);
                   ssd1306_WriteString(buffer, Font_7x10, White);
                   ssd1306_SetCursor(0, 32);
                   ssd1306_WriteString("azalt", Font_7x10, White);
                   ssd1306_SetCursor(0, 48);
                   ssd1306_WriteString("cikis", Font_7x10, White);
               } else if (cntrmanage == 1) {
                   ssd1306_WriteString("artir", Font_7x10, White);
                   ssd1306_SetCursor(0, 32);
                   char buffer[20];
                   sprintf(buffer, ">azalt %d     ", productscounter[whichproduct]);
                   ssd1306_WriteString(buffer, Font_7x10, White);
                   ssd1306_SetCursor(0, 48);
                   ssd1306_WriteString("cikis", Font_7x10, White);
               } else if (cntrmanage == 2) {
                   ssd1306_WriteString("artir", Font_7x10, White);
                   ssd1306_SetCursor(0, 32);
                   ssd1306_WriteString("azalt", Font_7x10, White);
                   ssd1306_SetCursor(0, 48);
                   ssd1306_WriteString(">cikis", Font_7x10, White);
               }

               ssd1306_UpdateScreen(); // Ekranı güncelle

               if (HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_UP_PIN) == GPIO_PIN_SET) { // yukaributon basma durumu
                   cntrmanage--;
                   HAL_Delay(200); // Debounce için kısa bir gecikme
               }

               if (HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_DOWN_PIN) == GPIO_PIN_SET) { // asagibuton basma durumu
                   cntrmanage++;
                   HAL_Delay(200); // Debounce için kısa bir gecikme
               }

               if (cntrmanage < 0) cntrmanage = 0;
               if (cntrmanage > 2) cntrmanage = 2;

               if (HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_SELECT_PIN) == GPIO_PIN_SET) {
                   if (cntrmanage == 0) {
                       productscounter[whichproduct]++;
                   } else if (cntrmanage == 1) {
                       productscounter[whichproduct]--;
                   } else if (cntrmanage == 2) {
                       control_cikis = 0;
                   }
                   printf("products counter: %d\n", productscounter[whichproduct]);
                   printf("hangi ürün: %d\n", whichproduct);
               }

               HAL_Delay(10); // Döngü gecikmesi
           }
       }


//burada yapay zeka ile tahmin işlemleri yapılıyor


       void Predictionfunc() {
    	   ssd1306_Fill(Black);
    	   ssd1306_SetCursor(0, 0);
           // Buton durumlarını oku
           int yukaributonbas = HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_UP_PIN);
           int asagibutonbas = HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_DOWN_PIN);
           int onaybutonbas = HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_SELECT_PIN);

           // Ekrana basılacak dizi
           char ekranArray[4][30]; // Maksimum 30 karakter içerecek şekilde ayarlandı
           // Ekran ayarlamaları

           // ekran.clearDisplay(); // Ekranı temizle
           // ekran.setTextSize(1); // Yazı tipi boyutu1
           // ekran.setTextColor(WHITE); // Renk beyaz
           // ekran.setCursor(0, 10);

           int productguess[3];
           char datagemini[100] = "bos";
           char elements[3][30];
           int count = 0;

           // whichmonths ve product counter gönder
           // esp32 ye soruyu gönderme
           char ai_data[100] = "+";
           char temp[10];

           // Serial.println("productscounter:");
           for (int x = 0; x < 3; x++) {
               sprintf(temp, "%d,", productscounter[x]);
               strcat(ai_data, temp);
           }
           strcat(ai_data, "*");
           // Serial.println("whichMonths:");
           for (int y = 0; y < 3; y++) {
               sprintf(temp, "%d,", whichMonths[y]);
               strcat(ai_data, temp);
           }
           strcat(ai_data, "+");


        //   ssd1306_WriteString(ai_data, Font_7x10, White);


           //ESP32 VERİ GÖNDERİMİ BAŞARILIMI KONTROL ET
           // Veri gönderim durumu kontrol ediliyor
              HAL_StatusTypeDef uartStatus;
              uartStatus = HAL_UART_Transmit(&huart2, (uint8_t*)ai_data, strlen(ai_data), HAL_MAX_DELAY);  // ESP32'ye veri gönder

              // Eğer veri gönderimi başarılıysa LED'i söndür
              if (uartStatus == HAL_OK) {

            	  HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);  // LED YAK

              } else {
                  HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);    // Veri gönderimi başarısız, LED'i SONDUR
              }



                             //  uint8_t received_char;
                    	    //   static char buffer[100];  // Gelen veriyi biriktirmek için bir buffer
                    	    //  static uint8_t buffer_index = 0;
                    	    //   static uint8_t buffer_size = sizeof(buffer) - 1; // Buffer boyutunu sakla



               uint8_t rx_buffer[500]; // Gelen veriyi tutacak buffer


           // ESP32'dan gelen veriyi alma
           while (1) {



        	   ssd1306_SetCursor(0, 10); // İkinci satıra set et (0,10)

        	   ssd1306_WriteString("sonuclar isleniyor....", Font_7x10, White);


        	   HAL_UART_Receive(&huart2, rx_buffer, sizeof(rx_buffer), 80000); // 1000ms timeout
        	   char received_string[100];
        	   strncpy(received_string, (char*)rx_buffer, sizeof(rx_buffer));
        	   if (strstr(received_string, "finish") != NULL) {
        	       // "finish" stringini bulduğunuzda yapılacak işlemler
        		   // String veriyi ekranda gösterin
        		   ssd1306_Fill(Black); // Ekranı temizle
        		   strcpy(datagemini, received_string);


        		   break;
        	   }




              ssd1306_UpdateScreen();
              HAL_Delay(13000);

               HAL_Delay(100);  // 1 sn bekle
           }

           // Veriyi işleme ve ekrana basma kısımları

              if (strstr(datagemini, "bos") == NULL) {

            	  //bu ekranı siyah yap
            	  ssd1306_Fill(Black); // Ekranı temizle
            	  ssd1306_SetCursor(0, 0);




                  char *token = strtok(datagemini, ",");
                  while (token != NULL && count < 3) {
                      strcpy(elements[count], token);
                      token = strtok(NULL, ",");
                      count++;
                  }

                  int intCount = 0;
                  for (int i = 0; i < count; i++) {
                      if (strcmp(elements[i], "finish") != 0) {
                          if (strcmp(elements[i], "yok") != 0) {
                              int value = atoi(elements[i]);
                              if (value >= 0) { // eksik üretilmiş
                                  sprintf(ekranArray[i], "%d.id %d eksik", i, value);
                              } else { // fazla üretilmiş
                                  value = -value;
                                  sprintf(ekranArray[i], "%d.id %d fazla", i, value);
                              }
                          } else {
                        	   sprintf(ekranArray[i], "%d.id uretim yok", i);
                        	 }

                          // Ekrana yazdırma işlemi
                          ssd1306_SetCursor(0, 10 + i * 10); // Satırları ayarlamak için y pozisyonunu artırın
                          ssd1306_WriteString(ekranArray[i], Font_7x10, White);

                          HAL_Delay(200);
                          intCount++;
                      }
                  }

                  ssd1306_UpdateScreen();
              }








           ssd1306_UpdateScreen();
           // ekran.display();
           HAL_Delay(2000);
       }

      /* void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
       {
       ssd1306_Fill(Black);
       ssd1306_SetCursor(0, 0);
       ssd1306_WriteString("HAL_UART_RxCpltCallback girdi", Font_7x10, White);

           if (huart->Instance == USART1)
           {
               // OLED ekranı temizle ve veri yazdır

               SSD1306_Puts(rxBuffer, &Font_7x10, 1);
               HAL_Delay(2000);
               // Buffer'ı temizle
               memset(rxBuffer, 0, sizeof(rxBuffer));

               // Tekrar veri alımını başlat
               HAL_UART_Receive_IT(&huart2, (uint8_t*)rxBuffer, sizeof(rxBuffer));

               // Ekranı güncelle
               }      ssd1306_UpdateScreen();

       }*/



        void SelectStage(){
        	  while (mantik) {
        	        art++;

        	        yukaributonbas = HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_UP_PIN);
        	        asagibutonbas = HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_DOWN_PIN);
        	        onaybutonbas = HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_SELECT_PIN);

        	        if (art == 1) {
        	            onaybutonbas = 0;
        	            HAL_Delay(500);
        	        }

        	        ssd1306_Fill(Black); // Ekranı temizle
        	        ssd1306_SetCursor(0, 0);

        	        if (yukaributonbas == 1) {
        	            menu2++;
        	            if (menu2 >= 1) { menu2 = 1; }
        	        }

        	        if (asagibutonbas == 1) {
        	            menu2--;
        	            if (menu2   <=0) { menu2 = 0; }
        	        }

        	        if (menu2 == 0) {
        	            ssd1306_WriteString("urun\n", Font_7x10, White);
        	            ssd1306_SetCursor(0,15);
        	            ssd1306_WriteString(">geri\n", Font_7x10, White);
        	            HAL_Delay(10);
        	        } else if (menu2 == 1) {
        	            ssd1306_WriteString(">urun\n", Font_7x10, White);
        	           ssd1306_SetCursor(0,15);
        	            ssd1306_WriteString("geri\n", Font_7x10, White);
        	            HAL_Delay(10);
        	        }
        	        ssd1306_SetCursor(0, 0);
        	        if (onaybutonbas == 1) {
        	            if (menu2 == 1) {
        	                int numProducts = sizeof(products) / sizeof(products[0]);
        	                screen_switch = 1;
        	                int currentPos = 0;
        	                int lineHeight = 16; // Her ürün arasındaki dikey boşluk

        	                while (state) {
        	                    ssd1306_Fill(Black); // Ekranı temizle
        	                    ssd1306_SetCursor(0, 0);

        	                    if (control >= 2) {
        	                        control--;
        	                        rtnMainPage = 1;
        	                    }

        	                    if (HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_UP_PIN) == 1) {
        	                        upbuton++;
        	                        currentPos -= 16;
        	                        if (currentPos < 0) {
        	                            currentPos = 0;
        	                        }
        	                        HAL_Delay(20);
        	                    }

        	                    if (HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_DOWN_PIN) == 1) {
        	                        bottombuton++;
        	                        currentPos += 16;
        	                        int maxPos = (numProducts - 1) * lineHeight;
        	                        if (currentPos > maxPos) {
        	                            currentPos = maxPos;
        	                        }
        	                        HAL_Delay(20);
        	                    }
        	                    ssd1306_WriteString("                 <-", Font_7x10, White);

        	                    for (int i = 0; i < numProducts; i++) {
                                      int yPos = i * lineHeight - currentPos;
        	                        if (yPos >= 0 && yPos < 64) {
        	                            ssd1306_SetCursor(0, yPos);

        	                            ssd1306_WriteString(products[i], Font_7x10, White);

        	                        }
        	                    }

        	                    if (HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_SELECT_PIN)== 1) {


        	                        pagecroos++;
        	                        if (pagecroos > 1 && (bottombuton >= 1 || upbuton >= 1)) {
        	                                  if (currentPos > 43) {
        	                                state = 0;
        	                                ssd1306_Fill(Black);
        	                                ssd1306_UpdateScreen();
        	                                break;
        	                            }
        	                            pagecroos = 0;
        	                            bottombuton = 0;
        	                            upbuton = 0;
        	                            choseProduct = (currentPos) / 19;

        	                            int productpage = 0;
        	                            while (1 && (rtnMainPage==1)) {

        	                            	//   HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, 1);


        	                                ssd1306_Fill(Black);
        	                                ssd1306_SetCursor(0, 0);
                                               if (productpage <= 0) {
        	                                    ssd1306_WriteString("uretim\n", Font_7x10, White);
        	                                    ssd1306_SetCursor(0,15);
        	                                    ssd1306_WriteString(">zaman secim\n", Font_7x10, White);
        	                                    HAL_Delay(10);
        	                                } else if (productpage == 1) {
        	                                    ssd1306_WriteString(">uretim\n", Font_7x10, White);
        	                                   ssd1306_SetCursor(0,15);
        	                                    ssd1306_WriteString("zaman secim\n", Font_7x10, White);
        	                                    HAL_Delay(10);
        	                                }
        	                                if (HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_UP_PIN) == 1) {
        	                                    productpage++;
        	                                    if (productpage >= 1) { productpage = 1; }
        	                                }

        	                                if (HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_DOWN_PIN) == 1) {
        	                                    productpage--;

        	                                    if (productpage <= 0) { productpage = 0; }
        	                                }



        	                                if (HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_SELECT_PIN) == 1) {
        	                                    if (productpage == 1) {

        	                                        control_cikis = 1;
        	                                        ProductManager(choseProduct);
        	                                    }
        	                                    if (productpage == 0) {

        	                                        control++;
        	                                        TimeManager(choseProduct);
        	                                    }
        	                                }

        	                                HAL_Delay(100);
        	                                ssd1306_UpdateScreen();
        	                            }
        	                        }
        	                    }
        	                    ssd1306_UpdateScreen();
        	                    HAL_Delay(100);
        	                }
        	            }
        	            if (menu2 == 0) {
        	                screen_switch = 1;
        	                HAL_Delay(500);
        	            }
        	        }

        	        if (screen_switch == 1) {
        	            screen_switch = 0;
        	            menu = 0;
        	            giris = 0;
        	            menu2 = 0;
        	            state = 1;

        	            while (1) {
        	                ssd1306_Fill(Black);
        	                ssd1306_SetCursor(0, 0);
        	                ssd1306_WriteString("is Sistemi\n", Font_7x10, White);
        	                ssd1306_SetCursor(0, 15);
        	                yukaributonbas = HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_UP_PIN);
        	                asagibutonbas = HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_DOWN_PIN);
        	                onaybutonbas = HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_SELECT_PIN);

        	                if (yukaributonbas == 1) {
        	                    menu++;
        	                    if (menu >= 1) { menu = 1; }
        	                    HAL_Delay(200);
        	                }

        	                if (asagibutonbas == 1) {
        	                    menu--;
        	                    if (menu >= 2) { menu = 0; }
        	                    HAL_Delay(200);
        	                }

        	                if (onaybutonbas == 1) {

        	                    if (menu == 1) {
        	                        HAL_Delay(30);

                                //BURADA PREDİCTFUNC
        	                        ssd1306_Fill(Black);
        	                        ssd1306_SetCursor(0, 0);
        	                        ssd1306_WriteString("Tahmin ediliyor", Font_7x10, White);
        	                        ssd1306_UpdateScreen();
                                    HAL_Delay(100);


        	                       Predictionfunc();
        	                    }
        	                    if (menu == 0) {
        	                        ssd1306_Fill(Black);
        	                        ssd1306_SetCursor(0, 15);
        	                        SelectStage();
        	                    }
        	                    HAL_Delay(100);
        	                }

        	                if (menu == 0) {
        	                    ssd1306_WriteString("tahminler\n", Font_7x10, White);
        	                   ssd1306_SetCursor(0,35);
        	                    ssd1306_WriteString(">urunler\n", Font_7x10, White);
        	                    HAL_Delay(20);
        	                    //ana ekrana tekrar giriş
        	                    if (HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_SELECT_PIN)== 1)
        	                    {

        	                    		 art = 0;
        	                    		 mantik = 1;
        	                    		 screen_switch = 0;
        	                    		 menu2 = 0;
        	                    		 state = 1;
        	                    		 control = 0;
        	                    		 rtnMainPage = 1;//rtnMainpage 1 yaptım ürün ve zaman yönetimine girsin diye
        	                    		 upbuton = 0;
        	                    		 bottombuton = 0;
        	                    		 pagecroos = 0;
        	                    		 choseProduct = 0;
        	                    		//char* products[] = {"pano sistem ", "dataloger", "pompa sistem","geri"};
        	                    		 giris=0;
        	                    		 control_cikis=0;
        	                    		 cnttime=0;
        	                    		 for(int i = 0; i < 3; i++) {
        	                    		  productscounter[i] = -1; // Her elemanı sıfırla değiştir
        	                    		   }
        	                    		 for(int i = 0; i < 3; i++) {
        	                    	     whichMonths[i] = 0; // Her elemanı sıfırla değiştir
        	                    	          }


        	                    	 SelectStage();
        	                    }


        	                } else if (menu == 1) {
        	                    ssd1306_WriteString(">tahminler\n", Font_7x10, White);
        	                    ssd1306_SetCursor(0,35);
        	                    ssd1306_WriteString("urunler\n", Font_7x10, White);
        	                    HAL_Delay(20);
        	                    //ana ekran tekrar ğiriş
        	                    if (HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_SELECT_PIN)== 1)
        	                     {

        	                                                        	 art = 0;
        	                            	                    		 mantik = 1;
        	                            	                    		 screen_switch = 0;
        	                            	                    		 menu2 = 0;
        	                            	                    		 state = 1;
        	                            	                    		 control = 0;
        	                            	                    		 rtnMainPage = 1;//rtnMainpage 1 yaptım ürün ve zaman yönetimine girsin diye
        	                            	                    		 upbuton = 0;
        	                            	                    		 bottombuton = 0;
        	                            	                    		 pagecroos = 0;
        	                            	                    		 choseProduct = 0;
        	                            	                    		//char* products[] = {"pano sistem ", "dataloger", "pompa sistem","geri"};
        	                            	                    		 for(int i = 0; i < 3; i++) {
        	                            	                    		     productscounter[i] = -1; // Her elemanı sıfırla değiştir
        	                            	                    		 }
	 giris=0;
        	                            	                    		 control_cikis=0;
        	                            	                    		 cnttime=0;
        	                            	                    		 for(int i = 0; i < 3; i++) {
        	                            	                    		     whichMonths[i] = 0; // Her elemanı sıfırla değiştir
        	                            	                    		 }





        	                         ssd1306_Fill(Black);
        	                         ssd1306_SetCursor(0, 0);
        	                        ssd1306_WriteString("Tahmin ediliyor", Font_7x10, White);
        	                       ssd1306_UpdateScreen();
        	                       HAL_Delay(100);
        	                    	  Predictionfunc();
        	                     }
        	                }

        	                ssd1306_UpdateScreen();
        	            }
        	        }

        	        ssd1306_UpdateScreen();
        	    }





        }














    while (1)
    {
    /* USER CODE END WHILE */
    MX_USB_HOST_Process();

    /* USER CODE BEGIN 3 */
      // Mevcut buton durumlarını oku
                int buttonState1 = (int)HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_UP_PIN);
                int buttonState2 = (int)HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_DOWN_PIN);
                int buttonState3 = (int)HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_SELECT_PIN);

                // Button 1 kontrolü
                if (buttonState1 == 1 && prevButtonState1 == 0) {
                    ledState1 = !ledState1; // LED durumunu değiştir
                    HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, ledState1); // LED'i yak veya söndür
                    //MENU İŞLEMLERİ  UP BUTTONU
                               menu++;
                               if (menu > 1) menu = 1;
                               updateDisplay(menu);
                               HAL_Delay(200); // Debounce delay


                }
                prevButtonState1 = buttonState1; // Buton durumunu güncelle

                // Button 2 kontrolü
                if (buttonState2 == 1 && prevButtonState2 == 0) {
                    ledState2 = !ledState2; // LED durumunu değiştir
                    HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, ledState2); // LED'i yak veya söndür

                    //MENU İŞLEMLERİ DOWN BUTTONU
                               menu--;
                               if (menu < 0) menu = 0;
                               updateDisplay(menu);
                               HAL_Delay(200); // Debounce delay
                }
                prevButtonState2 = buttonState2; // Buton durumunu güncelle

                // Button 3 kontrolü
                if (buttonState3 == 1 && prevButtonState3 == 0) {
                    ledState3 = !ledState3; // LED durumunu değiştir
                    HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, ledState3); // LED'i yak veya söndür

                    //UUATRRT İŞLEMLERİ KISMI!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!11
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
                    /*
                     *
                     * uint8_t txData[] = '+1,2,*,"mart","haziran"+'; // Gönderilecek veri
                    uint8_t rxData[10]; // Alınan veri buffer'ı
                    // Veriyi gönder
                           HAL_UART_Transmit(&huart2, (uint8_t *)txData, sizeof(txData), 100);
                           HAL_Delay(1000); // 1 saniye bekle

                           // Veriyi al
                           if (HAL_UART_Receive(&huart2, rxData, sizeof(rxData), 1000) == HAL_OK)
                           {
                               // Alınan veriyi işleme
                               HAL_UART_Transmit(&huart2, rxData, sizeof(rxData), 100);
                           }
*/








                               if (menu == 0)
                               {
                                   // Implement Predictionfunc logic

                                   //BURADA PREDİCTFUNC
           	                        ssd1306_Fill(Black);
           	                        ssd1306_SetCursor(0, 0);
           	                        ssd1306_WriteString("Tahmin ediliyor", Font_7x10, White);
           	                         ssd1306_UpdateScreen();
                                       HAL_Delay(1000);
                            	   Predictionfunc();


                               }
                               else if (menu == 1)
                               {







                                   // Implement Asamasec logic
                            	   SelectStage();
                               }
                               HAL_Delay(200); // Debounce delay
                }
                prevButtonState3 = buttonState3; // Buton durumunu güncelle

                // Biraz bekle (debouncing için)
                HAL_Delay(50); // 50 ms bekleme





    }





  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2S3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2S3_Init(void)
{

  /* USER CODE BEGIN I2S3_Init 0 */

  /* USER CODE END I2S3_Init 0 */

  /* USER CODE BEGIN I2S3_Init 1 */

  /* USER CODE END I2S3_Init 1 */
  hi2s3.Instance = SPI3;
  hi2s3.Init.Mode = I2S_MODE_MASTER_TX;
  hi2s3.Init.Standard = I2S_STANDARD_PHILIPS;
  hi2s3.Init.DataFormat = I2S_DATAFORMAT_16B;
  hi2s3.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
  hi2s3.Init.AudioFreq = I2S_AUDIOFREQ_16K;
  hi2s3.Init.CPOL = I2S_CPOL_LOW;
  hi2s3.Init.ClockSource = I2S_CLOCK_PLL;
  hi2s3.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;
  if (HAL_I2S_Init(&hi2s3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2S3_Init 2 */

  /* USER CODE END I2S3_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CS_I2C_SPI_GPIO_Port, CS_I2C_SPI_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(OTG_FS_PowerSwitchOn_GPIO_Port, OTG_FS_PowerSwitchOn_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin
                          |Audio_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : CS_I2C_SPI_Pin */
  GPIO_InitStruct.Pin = CS_I2C_SPI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(CS_I2C_SPI_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : OTG_FS_PowerSwitchOn_Pin */
  GPIO_InitStruct.Pin = OTG_FS_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(OTG_FS_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PDM_OUT_Pin */
  GPIO_InitStruct.Pin = PDM_OUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(PDM_OUT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BOOT1_Pin */
  GPIO_InitStruct.Pin = BOOT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BOOT1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : CLK_IN_Pin */
  GPIO_InitStruct.Pin = CLK_IN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(CLK_IN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD4_Pin LD3_Pin LD5_Pin LD6_Pin
                           Audio_RST_Pin */
  GPIO_InitStruct.Pin = LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin
                          |Audio_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : OTG_FS_OverCurrent_Pin */
  GPIO_InitStruct.Pin = OTG_FS_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(OTG_FS_OverCurrent_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : MEMS_INT2_Pin */
  GPIO_InitStruct.Pin = MEMS_INT2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(MEMS_INT2_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
