#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <ctype.h>
#include "JSON.h"


/*�Ƚ������ַ���*/
static int strcasecmp(const char *s1,const char *s2)
{
	if (!s1) return (s1==s2)?0:1;if (!s2) return 1;
	for(; tolower(*s1) == tolower(*s2); ++s1, ++s2)	if(*s1 == 0)	return 0;
	return tolower(*(const unsigned char *)s1) - tolower(*(const unsigned char *)s2);
}


/* ������JSON���� */
JSON *CreateJSON(void)
{
	// �����ڴ�
	JSON* node = (JSON*)malloc(sizeof(JSON));
	JSON* test = (JSON*)malloc(sizeof(JSON));
	// ������ɹ���ʼ������
	if (node)
	{
		memset(node,0,sizeof(JSON));
	}
	return node;
}



/* JSON�������� */
JSON *CreateNULL(void)					
{
	JSON *item=CreateJSON();
	if(item)item->type=JSON_NULL;
	return item;
}
JSON *CreateTrue(void)
{
	JSON *item=CreateJSON();
	if(item)item->type=JSON_TRUE, item->valueint=1;
	return item;
}
JSON *CreateFalse(void)					
{
	JSON *item=CreateJSON();
	if(item)item->type=JSON_FALSE, item->valueint=0;
	return item;
}
JSON *CreateBool(int b)					
{
	JSON *item=CreateJSON();
	if(item)item->type=b?JSON_TRUE:JSON_FALSE;
	return item;
}
JSON *CreateNumber(double num)		
{
	JSON *item=CreateJSON();
	if(item)
	{
		item->type=JSON_NUMBER;
		item->valuedouble=num;
		item->valueint=(int)num;
	}
	return item;
}
JSON *CreateString(const char *string)
{
	JSON *item=CreateJSON();
	if(item)
	{
		item->type=JSON_STRING;
		item->valuestring=StrDup(string);
	}
	return item;
}
JSON *CreateArray(void)					
{
	JSON *item=CreateJSON();
	if(item)item->type=JSON_ARRAY;
	return item;
}
JSON *CreateObject(void)			
{
	JSON *item=CreateJSON();
	if(item)item->type=JSON_OBJECT;
	return item;
}

/* ��һ��JSON����ӵ������� */
void AddItemToArray(JSON *array, JSON *item)		
{
	JSON *c=array->child;
	if (!item) return;
	if (!c) 
	{
		array->child=item;
	}
	else
	{
		while (c && c->next) 
			c=c->next;
		//suffix
		c->next = item;
		item->prev = c;
	}
}

/* ��һ��JSON����ӵ�object�� */
void AddItemToObject(JSON *object, const char *string, JSON *item)	
{
	if (!item) return;
	if (item->string) free(item->string);
	item->string=StrDup(string);
	AddItemToArray(object,item);
}

/* ��newitem�������ָ��λ�õ�Ԫ�� */
void ReplaceItemInArray(JSON *array, int which, JSON *newitem)
{
	JSON *c=array->child;
	while (c && which>0) c=c->next,which--;
	if (!c) return;
	newitem->next=c->next;
	newitem->prev=c->prev;
	if (newitem->next) newitem->next->prev=newitem;
	if (c==array->child) array->child=newitem;
	else newitem->prev->next=newitem;
	c->next=c->prev=0;DeleteJSON(c);
}

/* �滻object���ض�string��JSONԪ�� */
void ReplaceItemInObject(JSON *object,const char *string, JSON *newitem)
{
	int i=0;
	JSON *c=object->child;
	while(c && strcasecmp(c->string,string))i++,c=c->next;
	if(c)
	{
		newitem->string=StrDup(string);
		ReplaceItemInArray(object,i,newitem);
	}
}

/* ��JSON������ɾ��ָ��λ�õ�Ԫ�أ� ���ͷ��ڴ� */
JSON *DetachItemFromArray(JSON *array,int which)	
{
	JSON *c=array->child;
	while (c && which>0) c=c->next,which--;
	if (!c) return 0;
	if (c->prev)
		c->prev->next=c->next;
	if (c->next) 
		c->next->prev=c->prev;
	if (c==array->child) 
		array->child=c->next;
	c->prev=c->next=0;
	return c;
}

/* ��JSON������ɾ��ָ��λ�õ�Ԫ�أ� �ͷ��ڴ� */
void *DeleteItemFromArray(JSON *array,int which)	
{
	DeleteJSON(DetachItemFromArray(array,which));
	return NULL;
}

/* ��object��ɾ��ָ��key��JSONԪ�أ� ���ͷ��ڴ� */
JSON *DetachItemFromObject(JSON *object,const char *key)
{
	int i=0;
	JSON *c=object->child;
	while (c && strcasecmp(c->string, key)) 
		i++,c=c->next;
	if (c) 
		return DetachItemFromArray(object,i);
	return 0;
}

/* ��object��ɾ��ָ��key��JSONԪ�أ� �ͷ��ڴ� */
void *DeleteItemFromObject(JSON *object,const char *key)
{
	DeleteJSON(DetachItemFromObject(object, key));
	return NULL;
}


/* �ͷ�JSON������ڴ� */
void DeleteJSON(JSON *item)
{
	JSON *next;
	while (item)
	{
		next=item->next;
		if (!(item->type&JSON_IsUsed) && item->child) 
			DeleteJSON(item->child);
		if (!(item->type&JSON_IsUsed) && item->valuestring) 
			free(item->valuestring);
		if (item->string) 
			free(item->string);
		free(item);
		item=next;
	}
}

/* ���ƣ�����ѡ����ȸ��Ƽ���ȸ��� */
JSON *Duplicate(JSON *item, int recurse)
{
	JSON *newitem,*cptr,*nptr=0,*newchild;
	/* ��ָ�� */
	if (!item) return 0;
	/* ���븴�ƺ�����ڴ� */
	newitem=CreateJSON();
	if (!newitem) return 0;
	/* ����������Ϣ */
	newitem->type=item->type&(~JSON_IsUsed),newitem->valueint=item->valueint,newitem->valuedouble=item->valuedouble;
	if (item->valuestring)	
	{
		newitem->valuestring=StrDup(item->valuestring);
		if (!newitem->valuestring)	{DeleteJSON(newitem);return 0;}
	}
	if (item->string)		
	{
		newitem->string=StrDup(item->string);	
		if (!newitem->string){DeleteJSON(newitem);return 0;}
	}
	/* ��Ϊ����ȸ��ƣ������򷵻� */
	if (!recurse) return newitem;
	/* ��Ϊ��ȸ��ƣ�����Ϊ��ȸ������� */
	cptr=item->child;
	while (cptr)
	{
		newchild=Duplicate(cptr,1);		                                        /* �ݹ鸴�Ƹ�λ�õ�Ԫ����Ϣ */
		if (!newchild) {DeleteJSON(newitem);return 0;}
		if (nptr)	{nptr->next=newchild,newchild->prev=nptr;nptr=newchild;}	/* ���newitem->child�Ѿ���ֵ */
		else		{newitem->child=newchild;nptr=newchild;}					/* ��newitem->child��ֵ��Ȼ����� */
		cptr=cptr->next;
	}
	return newitem;
}

/* �������л�ȡָ��λ�õ�JSONԪ�� */
JSON *GetItemInArray(JSON *array, int which)		
{
	JSON *c=array->child; 
	while (c && which>0) which--,c=c->next;
	return c;
}

/* ��object��ȡָ��string��JSONԪ�� */
JSON *GetItemInObject(JSON *object,const char *key)	
{
	JSON *c=object->child;
	while (c && strcasecmp(c->string, key)) c=c->next;
	return c;
}

/* ��object��ȡָ��string��JSONԪ�� */
JSON *GetItemInJSON(JSON *object, const char *path)
{ 
	/* ��·����constת���ɷ�const���������Ĳ��� */
	char *key = (char*)malloc(strlen(path)+1);
	for (size_t i = 0; i< strlen(path);++i){key[i]=path[i];}key[strlen(path)] = 0;
	/* �����쳣����� */
	if (object==NULL) return NULL; 
	if (key == NULL|| *key == 0) return object; 
	else if (object->type == JSON_ARRAY)
	{ 
		int which = 0; 
		char *pos = key+1; 
		while (*pos != 0&& *pos != '/')
		{ 
			which = which*10 + *pos-'0'; 
			++pos; 
		} 
		return GetItemInJSON(GetItemInArray(object, which), pos); 
	} 
	else if (object->type == JSON_OBJECT)
	{ 
		char *op = key+1, *ed = op; 
		for( ; *ed != 0&& *ed != '/'; ++ed); 
		char *query_string = (char*)calloc(ed-op+1, sizeof(char)); 
		memcpy(query_string, op, sizeof(char)*(ed-op)); 
		return GetItemInJSON(GetItemInObject(object, query_string), ed); 
	} 
	else 
		return NULL; 
} 
