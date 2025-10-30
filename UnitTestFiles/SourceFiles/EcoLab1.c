/* Eco OS */
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"
#include "IdEcoInterfaceBus1.h"
#include "IdEcoFileSystemManagement1.h"
#include "IdEcoLab1.h"
#include "IEcoCalculatorX.h"
#include "IEcoCalculatorY.h"
#include "IdEcoCalculatorB.h"
#include "IdEcoCalculatorC.h"
#include "IdEcoCalculatorD.h"
#include "IdEcoCalculatorE.h"
#include <stdio.h>

#define TEST_PASS "OK"
#define TEST_FAIL "ERROR"
#define TEST_SKIP "MISSING"

void print_test_header(const char* test_name) {
    printf("\n=====================================================================\n");
    printf("                 %s\n", test_name);
    printf("=====================================================================\n");
}

void print_test_result(const char* test_desc, const char* result, const char* details) {
    printf("  %-40s [%s] %s\n", test_desc, result, details);
}

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

void test_component_integration(IEcoLab1* pIEcoLab1) {
    IEcoUnknown* pUnk = 0;
    IEcoCalculatorX* pIX = 0;
    IEcoCalculatorY* pIY = 0;
    IEcoCalculatorX* pIX_from_other = 0;
    IEcoCalculatorY* pIY_from_other = 0;
    IEcoLab1* pLab1_from_other = 0;
    IEcoUnknown* pUnk_from_other = 0;
    int16_t result;
    int32_t op_result;
    
    print_test_header("COMPONENT INTEGRATION VALIDATION");
    
    /* Direct Interface Access through IEcoLab1 */
    printf("\n--- Interface Access via IEcoLab1 ---\n");
    result = pIEcoLab1->pVTbl->QueryInterface(pIEcoLab1, &IID_IEcoUnknown, (void**)&pUnk);
    if (result == 0 && pUnk != 0) {
        print_test_result("IEcoUnknown interface", TEST_PASS, "Successfully obtained");
    } else {
        print_test_result("IEcoUnknown interface", TEST_FAIL, "Acquisition failed");
    }
    
    result = pIEcoLab1->pVTbl->QueryInterface(pIEcoLab1, &IID_IEcoCalculatorX, (void**)&pIX);
    if (result == 0 && pIX != 0) {
        print_test_result("IEcoCalculatorX interface", TEST_PASS, "Successfully obtained");
    } else {
        print_test_result("IEcoCalculatorX interface", TEST_FAIL, "Acquisition failed");
    }
    
    result = pIEcoLab1->pVTbl->QueryInterface(pIEcoLab1, &IID_IEcoCalculatorY, (void**)&pIY);
    if (result == 0 && pIY != 0) {
        print_test_result("IEcoCalculatorY interface", TEST_PASS, "Successfully obtained");
    } else {
        print_test_result("IEcoCalculatorY interface", TEST_FAIL, "Acquisition failed");
    }

    /* Arithmetic Operations through obtained interfaces */
    printf("\n--- Arithmetic Operations ---\n");
    if (pIX != 0) {
        op_result = pIX->pVTbl->Addition(pIX, 25, 7);
        if (op_result == 32) {
            print_test_result("Addition (25 + 7)", TEST_PASS, "Correct result: 32");
        } else {
            print_test_result("Addition (25 + 7)", TEST_FAIL, "Wrong computation");
        }
        
        op_result = pIX->pVTbl->Subtraction(pIX, 25, 7);
        if (op_result == 18) {
            print_test_result("Subtraction (25 - 7)", TEST_PASS, "Correct result: 18");
        } else {
            print_test_result("Subtraction (25 - 7)", TEST_FAIL, "Wrong computation");
        }
    } else {
        print_test_result("IEcoCalculatorX operations", TEST_SKIP, "Interface unavailable");
    }
    
    if (pIY != 0) {
        op_result = pIY->pVTbl->Multiplication(pIY, 25, 7);
        if (op_result == 175) {
            print_test_result("Multiplication (25 * 7)", TEST_PASS, "Correct result: 175");
        } else {
            print_test_result("Multiplication (25 * 7)", TEST_FAIL, "Wrong computation");
        }
        
        op_result = pIY->pVTbl->Division(pIY, 25, 5);
        if (op_result == 5) {
            print_test_result("Division (25 / 5)", TEST_PASS, "Correct result: 5");
        } else {
            print_test_result("Division (25 / 5)", TEST_FAIL, "Wrong computation");
        }
    } else {
        print_test_result("IEcoCalculatorY operations", TEST_SKIP, "Interface unavailable");
    }

    /* Cross-Interface References through IEcoLab1 interfaces */
    printf("\n--- Cross-Interface References ---\n");
    
    /* From IEcoCalculatorX */
    if (pIX != 0) {
        result = pIX->pVTbl->QueryInterface(pIX, &IID_IEcoCalculatorY, (void**)&pIY_from_other);
        if (result == 0 && pIY_from_other != 0) {
            print_test_result("IEcoCalculatorY from IEcoCalculatorX", TEST_PASS, "Cross-reference valid");
            pIY_from_other->pVTbl->Release(pIY_from_other);
        } else {
            print_test_result("IEcoCalculatorY from IEcoCalculatorX", TEST_FAIL, "Cross-reference broken");
        }
        
        result = pIX->pVTbl->QueryInterface(pIX, &IID_IEcoLab1, (void**)&pLab1_from_other);
        if (result == 0 && pLab1_from_other != 0) {
            print_test_result("IEcoLab1 from IEcoCalculatorX", TEST_PASS, "Cross-reference valid");
            pLab1_from_other->pVTbl->Release(pLab1_from_other);
        } else {
            print_test_result("IEcoLab1 from IEcoCalculatorX", TEST_FAIL, "Cross-reference broken");
        }
        
        result = pIX->pVTbl->QueryInterface(pIX, &IID_IEcoUnknown, (void**)&pUnk_from_other);
        if (result == 0 && pUnk_from_other != 0) {
            print_test_result("IEcoUnknown from IEcoCalculatorX", TEST_PASS, "Cross-reference valid");
            pUnk_from_other->pVTbl->Release(pUnk_from_other);
        } else {
            print_test_result("IEcoUnknown from IEcoCalculatorX", TEST_FAIL, "Cross-reference broken");
        }
    }
    
    /* From IEcoCalculatorY */
    if (pIY != 0) {
        result = pIY->pVTbl->QueryInterface(pIY, &IID_IEcoCalculatorX, (void**)&pIX_from_other);
        if (result == 0 && pIX_from_other != 0) {
            print_test_result("IEcoCalculatorX from IEcoCalculatorY", TEST_PASS, "Cross-reference valid");
            pIX_from_other->pVTbl->Release(pIX_from_other);
        } else {
            print_test_result("IEcoCalculatorX from IEcoCalculatorY", TEST_FAIL, "Cross-reference broken");
        }
        
        result = pIY->pVTbl->QueryInterface(pIY, &IID_IEcoLab1, (void**)&pLab1_from_other);
        if (result == 0 && pLab1_from_other != 0) {
            print_test_result("IEcoLab1 from IEcoCalculatorY", TEST_PASS, "Cross-reference valid");
            pLab1_from_other->pVTbl->Release(pLab1_from_other);
        } else {
            print_test_result("IEcoLab1 from IEcoCalculatorY", TEST_FAIL, "Cross-reference broken");
        }
        
        result = pIY->pVTbl->QueryInterface(pIY, &IID_IEcoUnknown, (void**)&pUnk_from_other);
        if (result == 0 && pUnk_from_other != 0) {
            print_test_result("IEcoUnknown from IEcoCalculatorY", TEST_PASS, "Cross-reference valid");
            pUnk_from_other->pVTbl->Release(pUnk_from_other);
        } else {
            print_test_result("IEcoUnknown from IEcoCalculatorY", TEST_FAIL, "Cross-reference broken");
        }
    }
    
    /* From IEcoUnknown */
    if (pUnk != 0) {
        result = pUnk->pVTbl->QueryInterface(pUnk, &IID_IEcoCalculatorX, (void**)&pIX_from_other);
        if (result == 0 && pIX_from_other != 0) {
            print_test_result("IEcoCalculatorX from IEcoUnknown", TEST_PASS, "Cross-reference valid");
            pIX_from_other->pVTbl->Release(pIX_from_other);
        } else {
            print_test_result("IEcoCalculatorX from IEcoUnknown", TEST_FAIL, "Cross-reference broken");
        }
        
        result = pUnk->pVTbl->QueryInterface(pUnk, &IID_IEcoCalculatorY, (void**)&pIY_from_other);
        if (result == 0 && pIY_from_other != 0) {
            print_test_result("IEcoCalculatorY from IEcoUnknown", TEST_PASS, "Cross-reference valid");
            pIY_from_other->pVTbl->Release(pIY_from_other);
        } else {
            print_test_result("IEcoCalculatorY from IEcoUnknown", TEST_FAIL, "Cross-reference broken");
        }
        
        result = pUnk->pVTbl->QueryInterface(pUnk, &IID_IEcoLab1, (void**)&pLab1_from_other);
        if (result == 0 && pLab1_from_other != 0) {
            print_test_result("IEcoLab1 from IEcoUnknown", TEST_PASS, "Cross-reference valid");
            pLab1_from_other->pVTbl->Release(pLab1_from_other);
        } else {
            print_test_result("IEcoLab1 from IEcoUnknown", TEST_FAIL, "Cross-reference broken");
        }
    }

    /* Cleanup */
    if (pUnk != 0) pUnk->pVTbl->Release(pUnk);
    if (pIX != 0) pIX->pVTbl->Release(pIX);
    if (pIY != 0) pIY->pVTbl->Release(pIY);
    printf("\n");
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
    
    test_component_integration(pIEcoLab1);
    
    result = 0;

Release:
    if (pIBus != 0) pIBus->pVTbl->Release(pIBus);
    if (pIMem != 0) pIMem->pVTbl->Release(pIMem);
    if (pIEcoLab1 != 0) pIEcoLab1->pVTbl->Release(pIEcoLab1);
    if (pISys != 0) pISys->pVTbl->Release(pISys);

    
    return result;
}