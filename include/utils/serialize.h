/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/



int serializeAttributeDescriptor(
			AttributeDescriptor* pAttributeDescriptor,
			char* returnString);


int deSerializeAttributeDescriptor(
			char* inputString,
			AttributeDescriptor* pAttributeDescriptor);
