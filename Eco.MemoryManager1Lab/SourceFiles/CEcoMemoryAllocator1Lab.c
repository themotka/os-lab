/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   IEcoMemoryAllocator1
 * </сводка>
 *
 * <описание>
 *   Данный исходный код описывает реализацию интерфейсов IEcoMemoryAllocator1
 * </описание>
 *
 * <автор>
 *   Copyright (c) 2016 Vladimir Bashev. All rights reserved.
 * </автор>
 *
 */

#include "IEcoSystem1.h"
#include "CEcoMemoryManager1Lab.h"

/* Поиск свободного чанка в пуле */
static MemChunk* findFreeChunk(MemChunk* pPool, uint32_t uSize) {
    MemChunk* pChunk;
    uint32_t i;
    
    if (pPool == 0) {
        return 0;
    }
    
    pChunk = pPool;
    for (i = 0; i < uSize; i++) {
        if (pChunk->bInUse == 0) {
            return pChunk;
        }
        pChunk++;
    }
    return 0;
}

/* Инициализация чанка памяти */
static void initChunk(MemChunk* pChunk, char_t* pStart, uint32_t uSize, MemChunk* pPrev, MemChunk* pNext) {
    if (pChunk == 0) {
        return;
    }
    pChunk->bInUse = 1;
    pChunk->pBegin = pStart;
    pChunk->pEnd = pStart + uSize;
    pChunk->pPrev = pPrev;
    pChunk->pNext = pNext;
}

/* Выделение памяти по алгоритму Next Fit */
static void* nextFitAllocate(uint32_t uSize, MemChunk** ppLastFit, MemChunk** ppListHead, 
                             MemChunk* pPool, uint32_t uPoolSize, 
                             char_t* pBase, char_t* pLimit) {
    MemChunk* pCurrent;
    MemChunk* pNewChunk;
    MemChunk* pListHead;
    char_t bWrapped;
    uint32_t uGap;
    
    pListHead = *ppListHead;
    pCurrent = 0;
    bWrapped = 0;
    
    /* Если список пуст */
    if (pListHead == 0) {
        if ((uint32_t)(pLimit - pBase) >= uSize) {
            pNewChunk = findFreeChunk(pPool, uPoolSize);
            if (pNewChunk == 0) {
                return 0;
            }
            initChunk(pNewChunk, pBase, uSize, 0, 0);
            *ppListHead = pNewChunk;
            *ppLastFit = pNewChunk;
            return pNewChunk->pBegin;
        }
        return 0;
    }
    
    /* Поиск подходящего места начиная с последнего выделенного блока */
    if (*ppLastFit != 0) {
        pCurrent = *ppLastFit;
    } else {
        pCurrent = pListHead;
    }
    
    while (bWrapped == 0 || pCurrent != *ppLastFit) {
        /* Проверка промежутка между текущим и следующим блоком */
        if (pCurrent->pNext != 0) {
            uGap = (uint32_t)(pCurrent->pNext->pBegin - pCurrent->pEnd);
            if (uGap >= uSize) {
                pNewChunk = findFreeChunk(pPool, uPoolSize);
                if (pNewChunk == 0) {
                    return 0;
                }
                initChunk(pNewChunk, pCurrent->pEnd, uSize, pCurrent, pCurrent->pNext);
                pCurrent->pNext->pPrev = pNewChunk;
                pCurrent->pNext = pNewChunk;
                *ppLastFit = pNewChunk;
                return pNewChunk->pBegin;
            }
            pCurrent = pCurrent->pNext;
            continue;
        }
        
        /* Проверка места после последнего блока */
        uGap = (uint32_t)(pLimit - pCurrent->pEnd);
        if (uGap >= uSize) {
            pNewChunk = findFreeChunk(pPool, uPoolSize);
            if (pNewChunk == 0) {
                return 0;
            }
            initChunk(pNewChunk, pCurrent->pEnd, uSize, pCurrent, 0);
            pCurrent->pNext = pNewChunk;
            *ppLastFit = pNewChunk;
            return pNewChunk->pBegin;
        }
        
        /* Переход к началу списка */
        bWrapped = 1;
        pCurrent = pListHead;
        
        /* Проверка места перед первым блоком */
        uGap = (uint32_t)(pCurrent->pBegin - pBase);
        if (uGap >= uSize) {
            pNewChunk = findFreeChunk(pPool, uPoolSize);
            if (pNewChunk == 0) {
                return 0;
            }
            initChunk(pNewChunk, pBase, uSize, 0, pCurrent);
            pCurrent->pPrev = pNewChunk;
            *ppListHead = pNewChunk;
            *ppLastFit = pNewChunk;
            return pNewChunk->pBegin;
        }
    }
    
    return 0;
}

/* Освобождение памяти */
static void nextFitFree(char_t* pPtr, MemChunk** ppListHead) {
    MemChunk* pChunk;
    
    if (ppListHead == 0 || *ppListHead == 0) {
        return;
    }
    
    pChunk = *ppListHead;
    
    /* Если освобождается первый блок */
    if (pChunk->pBegin == pPtr) {
        pChunk->bInUse = 0;
        if (pChunk->pNext != 0) {
            pChunk->pNext->pPrev = 0;
            *ppListHead = pChunk->pNext;
        } else {
            *ppListHead = 0;
        }
        return;
    }
    
    /* Поиск блока для освобождения */
    while (pChunk != 0) {
        if (pChunk->pBegin == pPtr) {
            pChunk->bInUse = 0;
            if (pChunk->pNext != 0) {
                pChunk->pNext->pPrev = pChunk->pPrev;
            }
            if (pChunk->pPrev != 0) {
                pChunk->pPrev->pNext = pChunk->pNext;
            }
            break;
        }
        pChunk = pChunk->pNext;
    }
}

/*
 *
 * <сводка>
 *   Функция QueryInterface
 * </сводка>
 *
 * <описание>
 *   Функция QueryInterface для интерфейса IEcoMemoryAllocator1
 * </описание>
 *
 */
int16_t ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_QueryInterface(/* in */ IEcoMemoryAllocator1Ptr_t me, /* in */ const UGUID* riid, /* out */ voidptr_t* ppv) {
    CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));

    if (me == 0 || ppv == 0) {
        return ERR_ECO_POINTER;
    }

    /* Проверка и получение запрошенного интерфейса */
    if ( IsEqualUGUID(riid, &IID_IEcoMemoryManager1) ) {
        *ppv = &pCMe->m_pVTblIMgr;
        pCMe->m_pVTblIMgr->AddRef((IEcoMemoryManager1*)pCMe);
    }
    else if ( IsEqualUGUID(riid, &IID_IEcoMemoryAllocator1) ) {
        *ppv = &pCMe->m_pVTblIMem;
        pCMe->m_pVTblIMgr->AddRef((IEcoMemoryManager1*)pCMe);
    }
    else if ( IsEqualUGUID(riid, &IID_IEcoVirtualMemory1) ) {
        *ppv = &pCMe->m_pVTblIVirtual;
        pCMe->m_pVTblIMgr->AddRef((IEcoMemoryManager1*)pCMe);
    }
    else if ( IsEqualUGUID(riid, &IID_IEcoUnknown) ) {
        *ppv = &pCMe->m_pVTblIMgr;
        pCMe->m_pVTblIMgr->AddRef((IEcoMemoryManager1*)pCMe);
    }
    else {
        *ppv = 0;
        return ERR_ECO_NOINTERFACE;
    }

    return ERR_ECO_SUCCESES;
}

/*
 *
 * <сводка>
 *   Функция AddRef
 * </сводка>
 *
 * <описание>
 *   Функция AddRef для интерфейса IEcoMemoryAllocator1
 * </описание>
 *
 */
uint32_t ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_AddRef(/* in */ IEcoMemoryAllocator1Ptr_t me) {
    CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));

    if (me == 0 ) {
        return -1;
    }

    return atomicincrement_int32_t(&pCMe->m_cRef);
}

/*
 *
 * <сводка>
 *   Функция Release
 * </сводка>
 *
 * <описание>
 *   Функция Release для интерфейса IEcoMemoryAllocator1
 * </описание>
 *
 */
uint32_t ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_Release(/* in */ IEcoMemoryAllocator1Ptr_t me) {
    CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));

    if (me == 0 ) {
        return -1;
    }

    /* Уменьшение счетчика ссылок на компонент */
    atomicdecrement_int32_t(&pCMe->m_cRef);

    /* В случае обнуления счетчика, освобождение данных экземпляра */
    if ( pCMe->m_cRef == 0 ) {
        deleteCEcoMemoryManager1Lab_623E1838((IEcoMemoryManager1*)pCMe);
        return 0;
    }
    return pCMe->m_cRef;
}

/*
 *
 * <сводка>
 *   Функция Alloc
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
void* ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_Alloc(/* in */ IEcoMemoryAllocator1Ptr_t me, /* in */ uint32_t size) {
    CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));

    if (me == 0 || size == 0) {
        return 0;
    }

    return nextFitAllocate(size, &(pCMe->m_pLastFit), &(pCMe->m_pChunkList), 
                          pCMe->m_pChunkPool, pCMe->m_uChunkPoolSize,
                          pCMe->m_pMemBase, pCMe->m_pMemLimit);
}

/*
 *
 * <сводка>
 *   Функция Free
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
void ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_Free(/* in */ IEcoMemoryAllocator1Ptr_t me, /* in */ voidptr_t pv) {
    CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));
    
    if (me == 0 || pv == 0) {
        return;
    }
    
    nextFitFree((char_t*)pv, &(pCMe->m_pChunkList));
}

/*
 *
 * <сводка>
 *   Функция Realloc
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
void* ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_Realloc(/* in */ IEcoMemoryAllocator1Ptr_t me, /* in */ voidptr_t pv, /* in */ uint32_t size) {
    /*CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));*/

    return 0;
}

/*
 *
 * <сводка>
 *   Функция Copy
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
void* ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_Copy(/* in */ IEcoMemoryAllocator1Ptr_t me, /* in */ voidptr_t pvDst, /* in */ voidptr_t pvSrc, /* in */ uint32_t size) {
    /*CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));*/

    return 0;
}

/*
 *
 * <сводка>
 *   Функция Fill
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
void* ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_Fill(/* in */ IEcoMemoryAllocator1Ptr_t me, /* in */ voidptr_t pvDst, /* in */ char_t Fill, /* in */ uint32_t size) {
    /*CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));*/

    return 0;
}

/*
 *
 * <сводка>
 *   Функция Compare
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
int16_t ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_Compare(/* in */ IEcoMemoryAllocator1Ptr_t me, /* in */ voidptr_t pv1, /* in */ voidptr_t pv2, /* in */ uint32_t size) {
    /*CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));*/

    return 0;
}
