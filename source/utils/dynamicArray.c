/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/


/*****************************************************
 * File                 : dynamicArray.c             *
 * Description          :                            *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 *                                                   *
 *****************************************************/

#include <stdio.h>
#include <string.h>
#include "utils/dynamicArray.h"
#include "utils/errorCode.h"


int freeList(List** pHeadRef)
{
	List* pCurrent;
	List* pNext;

	if (pHeadRef == NULL) {
		return(IPDR_ERROR);
	}

	pCurrent = *pHeadRef;
    while (pCurrent != NULL) {
        pNext = pCurrent->pNext;
		
		free(pCurrent->node);
        free(pCurrent);
        pCurrent = pNext;
    }
    free(pCurrent);
	return (IPDR_OK);
}

int addNode(List** pHeadRef, void *newNode) 
{
        unsigned int length = 0;

	List* pList = (List *) malloc (sizeof(List));

	if (pList == NULL) {
		return(IPDR_ERROR);
	}

        length = strlen((char *) newNode) + 1; 	
	pList->node = (char *)malloc(length);

	if (pList == NULL) {
		return(IPDR_ERROR);
	}
	
	strcpy(pList->node, newNode);

	pList->pNext = *pHeadRef; 
	*pHeadRef = pList; 

	return(IPDR_OK);
}

int getLength(List* pList)
{
	int count = 0;
	List* pCurrent = pList;

	if (pList == NULL) {
		return(IPDR_ERROR);
	}

	for (pCurrent = pList; pCurrent != NULL; pCurrent = pCurrent->pNext)
		count++;
	return (count);
}


int printList(List* pList)
{
	List* pCurrent = pList;
	int count = 0;

	if(pCurrent == NULL)  {
	    printf("\nLinked List is pointing to NULL !!!");
	    return (-1);
	}
	while (pCurrent != NULL) {
		#ifdef DEBUG_IPDR
	    printf("\n\nDisplaying Node %d", count + 1);
	    printf("\nNode Info = %s", pCurrent->node);
		#endif
	    
	    pCurrent = pCurrent->pNext;	
	    count++;
	}
	return(IPDR_OK);
}


int appendNode(List** pHeadRef, void* newNode) 
{
	List* pCurrent = *pHeadRef;

	if (pHeadRef == NULL) {
		return(IPDR_ERROR);
	}

	if (newNode == NULL) {
		return(IPDR_ERROR);
	}

	
	if (pCurrent == NULL) {
	    addNode(pHeadRef, newNode);
	} else {
		        while (pCurrent->pNext != NULL) {
		            pCurrent = pCurrent->pNext;
				}
	            addNode(&(pCurrent->pNext), newNode);
			}
	return(IPDR_OK);
}


int getNode(List* pList, int nodeNum, void* Node)
{
	List* pCurrent = pList;
	int count = 0;

	if(pCurrent == NULL){
		return(IPDR_ERROR);
	}

	if ((nodeNum > getLength(pList)) || (nodeNum <= 0))
	{
		return(IPDR_ERROR);
	}

	while (pCurrent != NULL) {
		if (count == nodeNum - 1) {
			strcpy(Node, pCurrent->node);
		}	    
	    pCurrent = pCurrent->pNext;	
	    count++;
	}
	return(IPDR_OK);
}


int freeListNode(List** pHeadRef, int node)
{
	
	List *pCurrent = NULL;
	List *pPrevious = NULL;
	int recordCount = 1;

	for (pCurrent = *pHeadRef, recordCount = 1; pCurrent != NULL; pCurrent = pCurrent->pNext, recordCount++) {
		if (recordCount == node) 
			break;

		pPrevious = pCurrent;
	}

	if (pCurrent == NULL)
		return (IPDR_OK);

	if (pPrevious != NULL) {
        pPrevious->pNext = pCurrent->pNext;
    } else {
        *pHeadRef = pCurrent->pNext;
    }

	free(pCurrent->node);
	free(pCurrent);

	return (IPDR_OK);
}
