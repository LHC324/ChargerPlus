#ifndef __STC_NVIC_H_
#define __STC_NVIC_H_
#include "config.h"

/*中断使能寄存器*/
typedef struct 
{
    union 
    {
        struct 
        {
            uint8_t bEA : 1;
            uint8_t bELVD : 1;
            uint8_t bEADC : 1;
            uint8_t bES  : 1;
            uint8_t bET1 : 1;
            uint8_t bEX1 : 1;
            uint8_t bET0 : 1;
            uint8_t bEX0 : 1;
        }IE_Bit;
        uint8_t IE_U8;  
    }IE;
}IE_Register;

/*中断使能寄存器2*/
typedef struct 
{
    union 
    {
        struct 
        {
            uint8_t bECAN : 1;
            uint8_t bET4  : 1;
            uint8_t bET3  : 1;
            uint8_t bES4  : 1;
            uint8_t bES3  : 1;
            uint8_t bET2  : 1;
            uint8_t bESP1 : 1;
            uint8_t bES2  : 1;
        }IE2_Bit;
        uint8_t IE2_U8;  
    }IE2;
}IE2_Register;

/*中断请求寄存器*/
/*定时器1/0模式控制寄存器*/
typedef struct 
{
	union 
	{
		struct 
		{
			uint8_t bTF1 : 1;
			uint8_t bTR1 : 1;
			uint8_t bTF0 : 1;
			uint8_t bTR0 : 1;
			uint8_t bIE1 : 1;
			uint8_t bIT1 : 1;
			uint8_t bIE0 : 1;
			uint8_t bIT0 : 1;	
		}TCON_Bit;
		uint8_t TCON_U8;
	}TCON;
}TCON_Register;

/*中断标志辅助寄存器*/
typedef struct 
{
	union 
	{
		struct 
		{
			uint8_t bNULL0  : 1;
			uint8_t bINT4IF : 1;
			uint8_t bINT3IF : 1;
			uint8_t bINT2IF : 1;
			uint8_t bNULL1  : 1;
			uint8_t bT4IF   : 1;
			uint8_t bT3IF   : 1;
			uint8_t bT2IF   : 1;
		}AUXRINTIF_Bit;
		uint8_t AUXRINTIF_U8;	
	}AUXRINTIF;	
}AUXRINTIF_Register;

/*外部中断与时钟输出寄存器*/
typedef struct 
{
	union 
	{
		struct 
		{
			uint8_t bNULL0 : 1;
			uint8_t bEX4 : 1;
			uint8_t bEX3 : 1;
			uint8_t bEX2 : 1;
			uint8_t bNULL1  : 1;
			uint8_t bT2CLKO : 1;
			uint8_t bT1CLKO : 1;
			uint8_t bT0CLKO : 1;
		}INITCLO_Bit;
		uint8_t INITCLO_U8; 	
	}INTCLKO;
}INTCLKO_Register;

/*中断优先级寄存器*/
typedef struct 
{
	union 
	{
		struct 
		{
			uint8_t bPPCA : 1;
			uint8_t bPLVD : 1;
			uint8_t bPADC : 1;
			uint8_t bPS   : 1;
			uint8_t bPT1  : 1;
			uint8_t bPX1  : 1;
			uint8_t bPT0  : 1;
			uint8_t bPX0  : 1;
		}IP_Bit;
		uint8_t IP_U8; 	
	}IP;
}IP_Register;

/*中断优先级寄存器*/
typedef struct 
{
	union 
	{
		struct 
		{
			uint8_t bPPCAH : 1;
			uint8_t bPLVDH : 1;
			uint8_t bPADCH : 1;
			uint8_t bPSH   : 1;
			uint8_t bPT1H  : 1;
			uint8_t bPX1H  : 1;
			uint8_t bPT0H  : 1;
			uint8_t bPX0H  : 1;
		}IPH_Bit;
		uint8_t IPH_U8; 	
	}IPH;
}IPH_Register;

/*中断优先级寄存器*/
typedef struct 
{
	union 
	{
		struct 
		{
			uint8_t bPCAN : 1;
			uint8_t bPI2C : 1;
			uint8_t bPCMP : 1;
			uint8_t bPX4  : 1;
			uint8_t bPPWMFD  : 1;
			uint8_t bPPWM : 1;
			uint8_t bPSPI : 1;
			uint8_t bPS2  : 1;
		}IP2_Bit;
		uint8_t IP2_U8; 	
	}IP2;
}IP2_Register;

/*中断优先级寄存器*/
typedef struct 
{
	union 
	{
		struct 
		{
			uint8_t bPCAN : 1;
			uint8_t bPI2C : 1;
			uint8_t bPCMP : 1;
			uint8_t bPX4  : 1;
			uint8_t bPPWMFD  : 1;
			uint8_t bPPWM : 1;
			uint8_t bPSPI : 1;
			uint8_t bPS2  : 1;
		}IP2H_Bit;
		uint8_t IP2H_U8; 	
	}IP2H;
}IP2H_Register;

/*定时器中断优先级*/
typedef struct 
{
	IPH_Register   IPH_Reg;
    IP_Register	   IP_Reg;
}TIMER_NVIC_TypeDef;






/*PCA/CCP/PWM*/
typedef struct 
{
    union 
    {
        struct 
        {
            uint8_t bCIDL : 1;
            uint8_t bNULL : 3;
            uint8_t bCPS  : 3;
            uint8_t bECF  : 1;
        }CMOD_Bit;
        uint8_t CMOD_U8;  
    }CMOD;
}CMOD_Register;

/*PCA/CCP/PWM*/
typedef struct 
{
    union 
    {
        struct 
        {
            uint8_t bNULL   : 1;
            uint8_t bECOM0  : 1;
            uint8_t bCCAPP0 : 1;
            uint8_t bCCAPN0 : 1;
            uint8_t bMAT0   : 1;
            uint8_t bTOG0   : 1;
            uint8_t bPWM0   : 1;
            uint8_t bECCF0  : 1;
        }CCAPM0_Bit;
        uint8_t CCAPM0_U8;  
    }CCAPM0;
}CCAPM0_Register;

/*PCA/CCP/PWM*/
typedef struct 
{
    union 
    {
        struct 
        {
            uint8_t bNULL   : 1;
            uint8_t bECOM1  : 1;
            uint8_t bCCAPP1 : 1;
            uint8_t bCCAPN1 : 1;
            uint8_t bMAT1   : 1;
            uint8_t bTOG1   : 1;
            uint8_t bPWM1   : 1;
            uint8_t bECCF1  : 1;
        }CCAPM1_Bit;
        uint8_t CCAPM1_U8;  
    }CCAPM1;
}CCAPM1_Register;

/*PCA/CCP/PWM*/
typedef struct 
{
    union 
    {
        struct 
        {
            uint8_t bNULL   : 1;
            uint8_t bECOM2  : 1;
            uint8_t bCCAPP2 : 1;
            uint8_t bCCAPN2 : 1;
            uint8_t bMAT2   : 1;
            uint8_t bTOG2   : 1;
            uint8_t bPWM2   : 1;
            uint8_t bECCF2  : 1;
        }CCAPM2_Bit;
        uint8_t CCAPM2_U8;  
    }CCAPM2;
}CCAPM2_Register;

/*PCA/CCP/PWM*/
typedef struct 
{
    union 
    {
        struct 
        {
            uint8_t bNULL   : 1;
            uint8_t bECOM3  : 1;
            uint8_t bCCAPP3 : 1;
            uint8_t bCCAPN3 : 1;
            uint8_t bMAT3   : 1;
            uint8_t bTOG3   : 1;
            uint8_t bPWM3   : 1;
            uint8_t bECCF3  : 1;
        }CCAPM3_Bit;
        uint8_t CCAPM3_U8;  
    }CCAPM3;
}CCAPM3_Register;

/*比较器控制寄存器*/
typedef struct 
{
    union 
    {
        struct 
        {
            uint8_t bCMPEN  : 1;
            uint8_t bCMPIF  : 1;
            uint8_t bPIE    : 1;
            uint8_t bNIE    : 1;
            uint8_t bPIS    : 1;
            uint8_t bNIS    : 1;
            uint8_t bCMPOE  : 1;
            uint8_t bCMPRES : 1;
        }CMPCR1_Bit;
        uint8_t CMPCR1_U8;  
    }CMPCR1;
}CMPCR1_Register;

/*PWM控制寄存器*/
typedef struct 
{
    union 
    {
        struct 
        {
            uint8_t bENPWM  : 1;
            uint8_t bECBI   : 1;
            uint8_t bNULL   : 6;
        }PWMCR_Bit;
        uint8_t PWMCR_U8;  
    }PWMCR;
}PWMCR_Register;

/*PWM异常检测控制寄存器*/
typedef struct 
{
    union 
    {
        struct 
        {
            uint8_t bINVCMP  : 1;
            uint8_t bINIO    : 1;
            uint8_t bENFD    : 1;
            uint8_t bFLTEFLO : 1;
            uint8_t bEFDI    : 1;
            uint8_t bFDCMP   : 1;
            uint8_t bFDIO    : 1;
            uint8_t bFDIF    : 1;
        }PWMFDCR_Bit;
        uint8_t PWMFDCR_U8;  
    }PWMFDCR;
}PWMFDCR_Register;

/*增强型PWM控制寄存器*/
typedef struct 
{
    union 
    {
        struct 
        {
            uint8_t bENC0O   : 1;
            uint8_t bC0INI   : 1;
            uint8_t bNULL    : 1;
            uint8_t bC0_S    : 2;
            uint8_t bEC0I    : 1;
            uint8_t bEC0T2SI : 1;
            uint8_t bEC0T1SI : 1;
        }PWMM0CR_Bit;
        uint8_t PWMM0CR_U8;  
    }PWMM0CR;
}PWMM0CR_Register;

/*增强型PWM控制寄存器*/
typedef struct 
{
    union 
    {
        struct 
        {
            uint8_t bENC1O   : 1;
            uint8_t bC1INI   : 1;
            uint8_t bNULL    : 1;
            uint8_t bC1_S    : 2;
            uint8_t bEC1I    : 1;
            uint8_t bEC1T2SI : 1;
            uint8_t bEC1T1SI : 1;
        }PWMM1CR_Bit;
        uint8_t PWMM1CR_U8;  
    }PWMM1CR;
}PWMM1CR_Register;

/*增强型PWM控制寄存器*/
typedef struct 
{
    union 
    {
        struct 
        {
            uint8_t bENC2O   : 1;
            uint8_t bC2INI   : 1;
            uint8_t bNULL    : 1;
            uint8_t bC2_S    : 2;
            uint8_t bEC2I    : 1;
            uint8_t bEC2T2SI : 1;
            uint8_t bEC2T1SI : 1;
        }PWMM2CR_Bit;
        uint8_t PWMM2CR_U8;  
    }PWMM2CR;
}PWMM2CR_Register;

/*增强型PWM控制寄存器*/
typedef struct 
{
    union 
    {
        struct 
        {
            uint8_t bENC3O   : 1;
            uint8_t bC3INI   : 1;
            uint8_t bNULL    : 1;
            uint8_t bC3_S    : 2;
            uint8_t bEC3I    : 1;
            uint8_t bEC3T2SI : 1;
            uint8_t bEC3T1SI : 1;
        }PWMM3CR_Bit;
        uint8_t PWMM3CR_U8;  
    }PWMM3CR;
}PWMM3CR_Register;

/*增强型PWM控制寄存器*/
typedef struct 
{
    union 
    {
        struct 
        {
            uint8_t bENC4O   : 1;
            uint8_t bC4INI   : 1;
            uint8_t bNULL    : 1;
            uint8_t bC4_S    : 2;
            uint8_t bEC4I    : 1;
            uint8_t bEC4T2SI : 1;
            uint8_t bEC4T1SI : 1;
        }PWMM4CR_Bit;
        uint8_t PWMM4CR_U8;  
    }PWMM4CR;
}PWMM4CR_Register;

/*增强型PWM控制寄存器*/
typedef struct 
{
    union 
    {
        struct 
        {
            uint8_t bENC5O   : 1;
            uint8_t bC5INI   : 1;
            uint8_t bNULL    : 1;
            uint8_t bC5_S    : 2;
            uint8_t bEC5I    : 1;
            uint8_t bEC5T2SI : 1;
            uint8_t bEC5T1SI : 1;
        }PWMM5CR_Bit;
        uint8_t PWMM5CR_U8;  
    }PWMM5CR;
}PWMM5CR_Register;

/*增强型PWM控制寄存器*/
typedef struct 
{
    union 
    {
        struct 
        {
            uint8_t bENC6O   : 1;
            uint8_t bC6INI   : 1;
            uint8_t bNULL    : 1;
            uint8_t bC6_S    : 2;
            uint8_t bEC6I    : 1;
            uint8_t bEC6T2SI : 1;
            uint8_t bEC6T1SI : 1;
        }PWMM6CR_Bit;
        uint8_t PWMM6CR_U8;  
    }PWMM6CR;
}PWMM6CR_Register;

/*增强型PWM控制寄存器*/
typedef struct 
{
    union 
    {
        struct 
        {
            uint8_t bENC7O   : 1;
            uint8_t bC7INI   : 1;
            uint8_t bNULL    : 1;
            uint8_t bC7_S    : 2;
            uint8_t bEC7I    : 1;
            uint8_t bEC7T2SI : 1;
            uint8_t bEC7T1SI : 1;
        }PWMM7CR_Bit;
        uint8_t PWMM7CR_U8;  
    }PWMM7CR;
}PWMM7CR_Register;

/*IIC控制寄存器*/
typedef struct 
{
    union 
    {
        struct 
        {
            uint8_t bEMSI   : 1;
            uint8_t bNULL   : 4;
            uint8_t bMSCMD  : 3;
        }I2CMSCR_Bit;
        uint8_t I2CMSCR_U8;  
    }rI2CMSCR;
}I2CMSCR_Register;

/*IIC控制寄存器*/
typedef struct 
{
    union 
    {
        struct 
        {
            uint8_t bNULL0  : 1;
            uint8_t bESTAI  : 1;
            uint8_t bERXI   : 1;
            uint8_t bETXI   : 1;
            uint8_t bESTOI  : 1;
            uint8_t bNULL1  : 2;
            uint8_t bSLRST  : 1;
        }I2CSLCR_Bit;
        uint8_t I2CSLCR_U8;  
    }rI2CSLCR;
}I2CSLCR_Register;

/*串口控制寄存器*/
typedef struct 
{
    union 
    {
        struct 
        {
            uint8_t bSM0  : 1;
            uint8_t bSM1  : 1;
            uint8_t bSM2  : 1;
            uint8_t bREN  : 1;
            uint8_t bTB8  : 1;
            uint8_t bRB8  : 1;
            uint8_t bTI   : 1;
            uint8_t bRI   : 1;
        }SCON_Bit;
        uint8_t SCON_U8;  
    }SCON;
}SCON_Register;

/*串口控制寄存器*/
typedef struct 
{
    union 
    {
        struct 
        {
            uint8_t bS2SM0  : 1;
            uint8_t bNULL   : 1;
            uint8_t bS2SM2  : 1;
            uint8_t bS2REN  : 1;
            uint8_t bS2TB8  : 1;
            uint8_t bS2RB8  : 1;
            uint8_t bS2TI   : 1;
            uint8_t bS2RI   : 1;
        }S2CON_Bit;
        uint8_t S2CON_U8;  
    }S2CON;
}S2CON_Register;

/*串口控制寄存器*/
typedef struct 
{
    union 
    {
        struct 
        {
            uint8_t bS3SM0  : 1;
            uint8_t bS3ST3  : 1;
            uint8_t bS3SM2  : 1;
            uint8_t bS3REN  : 1;
            uint8_t bS3TB8  : 1;
            uint8_t bS3RB8  : 1;
            uint8_t bS3TI   : 1;
            uint8_t bS3RI   : 1;
        }S3CON_Bit;
        uint8_t S3CON_U8;  
    }S3CON;
}S3CON_Register;

/*串口控制寄存器*/
typedef struct 
{
    union 
    {
        struct 
        {
            uint8_t bS4SM0  : 1;
            uint8_t bS4ST3  : 1;
            uint8_t bS4SM2  : 1;
            uint8_t bS4REN  : 1;
            uint8_t bS4TB8  : 1;
            uint8_t bS4RB8  : 1;
            uint8_t bS4TI   : 1;
            uint8_t bS4RI   : 1;
        }S4CON_Bit;
        uint8_t S4CON_U8;  
    }S4CON;
}S4CON_Register;
#endif