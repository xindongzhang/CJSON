#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <ctype.h>
#include "JSON.h"


/*比较两个字符串*/
static int strcasecmp(const char *s1,const char *s2)
{
	if (!s1) return (s1==s2)?0:1;if (!s2) return 1;
	for(; tolower(*s1) == tolower(*s2); ++s1, ++s2)	if(*s1 == 0)	return 0;
	return tolower(*(const unsigned char *)s1) - tolower(*(const unsigned char *)s2);
}


/* 构造新JSON对象 */
JSON *CreateJSON(void)
{
	// 申请内存
	JSON* node = (JSON*)malloc(sizeof(JSON));
	JSON* test = (JSON*)malloc(sizeof(JSON));
	// 申请则成功初始化如下
	if (node)
	{
		memset(node,0,sizeof(JSON));
	}
	return node;
}



/* JSON基本类型 */
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

/* 将一个JSON对象加到数组中 */
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

/* 将一个JSON对象加到object中 */
void AddItemToObject(JSON *object, const char *string, JSON *item)	
{
	if (!item) return;
	if (item->string) free(item->string);
	item->string=StrDup(string);
	AddItemToArray(object,item);
}

/* 用newitem替代数组指定位置的元素 */
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

/* 替换object中特定string的JSON元素 */
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

/* 从JSON数组中删除指定位置的元素， 不释放内存 */
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

/* 从JSON数组中删除指定位置的元素， 释放内存 */
void *DeleteItemFromArray(JSON *array,int which)	
{
	DeleteJSON(DetachItemFromArray(array,which));
	return NULL;
}

/* 从object中删除指定key的JSON元素， 不释放内存 */
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

/* 从object中删除指定key的JSON元素， 释放内存 */
void *DeleteItemFromObject(JSON *object,const char *key)
{
	DeleteJSON(DetachItemFromObject(object, key));
	return NULL;
}


/* 释放JSON对象的内存 */
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

/* 复制，可以选非深度复制及深度复制 */
JSON *Duplicate(JSON *item, int recurse)
{
	JSON *newitem,*cptr,*nptr=0,*newchild;
	/* 空指针 */
	if (!item) return 0;
	/* 申请复制后对象内存 */
	newitem=CreateJSON();
	if (!newitem) return 0;
	/* 复制所有信息 */
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
	/* 若为非深度复制，结束则返回 */
	if (!recurse) return newitem;
	/* 若为深度复制，下面为深度复制流程 */
	cptr=item->child;
	while (cptr)
	{
		newchild=Duplicate(cptr,1);		                                        /* 递归复制该位置的元素信息 */
		if (!newchild) {DeleteJSON(newitem);return 0;}
		if (nptr)	{nptr->next=newchild,newchild->prev=nptr;nptr=newchild;}	/* 如果newitem->child已经赋值 */
		else		{newitem->child=newchild;nptr=newchild;}					/* 对newitem->child赋值，然后继续 */
		cptr=cptr->next;
	}
	return newitem;
}

/* 从数组中获取指定位置的JSON元素 */
JSON *GetItemInArray(JSON *array, int which)		
{
	JSON *c=array->child; 
	while (c && which>0) which--,c=c->next;
	return c;
}

/* 从object中取指定string的JSON元素 */
JSON *GetItemInObject(JSON *object,const char *key)	
{
	JSON *c=object->child;
	while (c && strcasecmp(c->string, key)) c=c->next;
	return c;
}

/* 从object中取指定string的JSON元素 */
JSON *GetItemInJSON(JSON *object, const char *path)
{ 
	/* 将路径从const转换成非const，方便后面的操作 */
	char *key = (char*)malloc(strlen(path)+1);
	for (size_t i = 0; i< strlen(path);++i){key[i]=path[i];}key[strlen(path)] = 0;
	/* 处理异常的情况 */
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
