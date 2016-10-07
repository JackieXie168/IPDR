/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/


/*****************************************************
 * File                 : dynamicArray.h             *
 * Description          :                            *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 *                                                   *
 *****************************************************/

#ifndef _DYNAMICARRAY_H
#define _DYNAMICARRAY_H

/* This defines the maximum size of node */

typedef struct List {
	char *node;
	struct List* pNext;
} List;


int freeList(List** pHeadRef);
int addNode(List** pHeadRef, void *newNode);
int getLength(List* pList);
int printList(List* pList);
int appendNode(List** pHeadRef, void* newNode) ;
int getNode(List* pList, int nodeNum, void* Node);
int freeListNode(List** pHeadRef, int node);

/*
 * * * * * * * * * *
 * Sample Usage  * *
 * * * * * * * * * *
 * 
 * int main()
 * {
 *	List* pList = NULL;
 *	char *Node = NULL;
 *
 *	Node = (char *)malloc(MAXIMUM_NODE_SIZE);
 *	
 *	printf("\nAdding Node 1");
 *	appendNode(&pList, "Infosys"); 
 *
 *	printf("\nAdding Node 2");
 *	appendNode(&pList, "Boolean"); 
 *
 *	printf("\nAdding Node 3");
 *	appendNode(&pList, "Create Descriptor"); 
 *
 *	printList(pList);
 *	printf("\nList length = %d\n", getLength(pList));
 *
 *	getNode(pList, 3, Node);
 *	printf("\nNode obtained is = %s, Length = %d", Node, strlen(Node));
 *	printf("\n");
 *
 *	freeList(&pList);
 *	
 *	return 0;
 * };
 *
 */

#endif


