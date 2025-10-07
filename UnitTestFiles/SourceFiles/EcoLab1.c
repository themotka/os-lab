#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"
#include "IdEcoInterfaceBus1.h"
#include "IdEcoFileSystemManagement1.h"
#include "IdEcoLab1.h"
#include <stdio.h>

static void print_separator() {
    printf("===============================================================================\n");
}

static void print_header(const char* title) {
    printf("\n");
    print_separator();
    printf("| %-75s |\n", title);
    print_separator();
}

static int approx_equal_float(float a, float b, float eps) {
    float d = a - b;
    if (d < 0.0f) d = -d;
    return d <= eps * (b >= 0.0f ? (b + 1.0f) : (-b + 1.0f));
}

static int approx_equal_double(double a, double b, double eps) {
    double d = a - b;
    if (d < 0.0) d = -d;
    return d <= eps * (b >= 0.0 ? (b + 1.0) : (-b + 1.0));
}

static int approx_equal_longdouble(long double a, long double b, long double eps) {
    long double d = a - b;
    if (d < 0.0L) d = -d;
    return d <= eps * (b >= 0.0L ? (b + 1.0L) : (-b + 1.0L));
}

int16_t EcoMain(IEcoUnknown* pIUnk) {
    int16_t result = -1;
    IEcoSystem1* pISys = 0;
    IEcoInterfaceBus1* pIBus = 0;
    IEcoMemoryAllocator1* pIMem = 0;
    IEcoLab1* pIEcoLab1 = 0;


    if (pISys == 0) {
        result = pIUnk->pVTbl->QueryInterface(pIUnk, &GID_IEcoSystem, (void **)&pISys);
        if (result != 0 || pISys == 0) {
            printf("ERROR: Failed to create system interface.\n");
            goto Release;
        }
    }

    result = pISys->pVTbl->QueryInterface(pISys, &IID_IEcoInterfaceBus1, (void **)&pIBus);
    if (result != 0 || pIBus == 0) {
        printf("ERROR: Failed to get interface bus.\n");
        goto Release;
    }

    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoMemoryManager1, 0,
        &IID_IEcoMemoryAllocator1, (void**)&pIMem);
    if (result != 0 || pIMem == 0) {
        printf("ERROR: Failed to get memory manager.\n");
        goto Release;
    }

    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoLab1, 0,
        &IID_IEcoLab1, (void**)&pIEcoLab1);
    if (result != 0 || pIEcoLab1 == 0) {
        printf("ERROR: Failed to get IEcoLab1 interface.\n");
        goto Release;
    }


    print_header("TESTING GAMMA FUNCTION (float)");

    {
        float out;
        out = 0.0f;
        pIEcoLab1->pVTbl->gamma_float(pIEcoLab1, 1.0f, &out);
        printf("Gamma(1.0f) = %.6f | expected 1.0 -> %s\n", out, approx_equal_float(out, 1.0f, 1e-6f) ? "OK" : "FAIL");
    }
    {
        float out;
        out = 0.0f;
        pIEcoLab1->pVTbl->gamma_float(pIEcoLab1, 2.0f, &out);
        printf("Gamma(2.0f) = %.6f | expected 1.0 -> %s\n", out, approx_equal_float(out, 1.0f, 1e-6f) ? "OK" : "FAIL");
    }
    {
        float out;
        out = 0.0f;
        pIEcoLab1->pVTbl->gamma_float(pIEcoLab1, 3.0f, &out);
        printf("Gamma(3.0f) = %.6f | expected 2.0 -> %s\n", out, approx_equal_float(out, 2.0f, 1e-5f) ? "OK" : "FAIL");
    }
    {
        float out;
        out = 0.0f;
        pIEcoLab1->pVTbl->gamma_float(pIEcoLab1, 6.0f, &out);
        printf("Gamma(6.0f) = %.2f | expected 120.0 -> %s\n", out, approx_equal_float(out, 120.0f, 1e-5f) ? "OK" : "FAIL");
    }

    print_header("TESTING GAMMA FUNCTION (double)");

    {
        double out;
        double expected;
        out = 0.0;
        expected = 1.7724538509055160273; /* sqrt(pi) */
        pIEcoLab1->pVTbl->gamma_double(pIEcoLab1, 0.5, &out);
        printf("Gamma(0.5) = %.12f | expected ~1.772453850905 -> %s\n", out, approx_equal_double(out, expected, 1e-10) ? "OK" : "FAIL");
    }
    {
        double out;
        double expected;
        out = 0.0;
        expected = 0.8862269254527580137; /* 0.5*sqrt(pi) */
        pIEcoLab1->pVTbl->gamma_double(pIEcoLab1, 1.5, &out);
        printf("Gamma(1.5) = %.12f | expected ~0.886226925453 -> %s\n", out, approx_equal_double(out, expected, 1e-10) ? "OK" : "FAIL");
    }
    {
        double out;
        double expected;
        out = 0.0;
        expected = 362880.0; /* 9! */
        pIEcoLab1->pVTbl->gamma_double(pIEcoLab1, 10.0, &out);
        printf("Gamma(10.0) = %.2f | expected 362880.0 -> %s\n", out, approx_equal_double(out, expected, 1e-12) ? "OK" : "FAIL");
    }

    print_header("TESTING GAMMA FUNCTION (long double)");

    {
        long double out;
        long double expected;
        out = 0.0L;
        expected = 1.7724538509055160272981674833411L; /* sqrt(pi) */
        pIEcoLab1->pVTbl->gamma_longdouble(pIEcoLab1, 0.5L, &out);
        printf("Gamma(0.5L) = %.15Lf | expected ~1.772453850905516 -> %s\n", out, approx_equal_longdouble(out, expected, 1e-12L) ? "OK" : "FAIL");
    }
    {
        long double out;
        long double expected;
        out = 0.0L;
        expected = 24.0L; /* 4! */
        pIEcoLab1->pVTbl->gamma_longdouble(pIEcoLab1, 5.0L, &out);
        printf("Gamma(5.0L) = %.0Lf | expected 24 -> %s\n", out, approx_equal_longdouble(out, expected, 1e-12L) ? "OK" : "FAIL");
    }
    {
        long double out;
        long double expected;
        out = 0.0L;
        expected = 39916800.0L; /* 11! */
        pIEcoLab1->pVTbl->gamma_longdouble(pIEcoLab1, 12.0L, &out);
        printf("Gamma(12.0L) = %.0Lf | expected 39916800 -> %s\n", out, approx_equal_longdouble(out, expected, 1e-12L) ? "OK" : "FAIL");
    }
    result = 0;

Release:
    if (pIBus != 0) pIBus->pVTbl->Release(pIBus);
    if (pIMem != 0) pIMem->pVTbl->Release(pIMem);
    if (pIEcoLab1 != 0) pIEcoLab1->pVTbl->Release(pIEcoLab1);
    if (pISys != 0) pISys->pVTbl->Release(pISys);

    
    return result;
}