#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <ctype.h>
#include "JSON.h"

/* ����һЩ�ַ� */
static const char *skip(const char *in) 
{
	while (in && *in && (unsigned char)*in<=32) in++; 
	return in;
}


/*���ַ�*/
static const char *parse_string(JSON *item,const char *str)
{
	const char *ptr=str+1;char *ptr2;char *out;int len=0;
	if (*str!='\"') {tmp=str;return 0;}	/* û��"�����ַ��� */
	while (*ptr!='\"' && *ptr && ++len) if(*ptr++ == '\\') ptr++;	/* Skip escaped quotes. */

	out=(char*)malloc(len+1);	/* �����ַ������ȵ��ڴ� */
	if (!out) return 0;

	ptr=str+1;ptr2=out;
	while (*ptr!='\"' && *ptr)
	{
		if (*ptr!='\\') *ptr2++=*ptr++;
		else
		{
			ptr++;
			switch (*ptr)
			{
			case 'b': *ptr2++='\b';	break;
			case 'f': *ptr2++='\f';	break;
			case 'n': *ptr2++='\n';	break;
			case 'r': *ptr2++='\r';	break;
			case 't': *ptr2++='\t';	break;
			default:  *ptr2++=*ptr; break;
			}
			ptr++;
		}
	}
	*ptr2=0;
	if (*ptr=='\"') ptr++;
	item->valuestring=out;
	item->type=JSON_STRING;
	return ptr;
}


/* ������ */
static const char *parse_array(JSON *item,const char *value)
{
	JSON *child;
	if (*value!='[')	{tmp=value;return 0;}	/* û��[��ͷ��Ȼ�������� */

	item->type=JSON_ARRAY;                      /*��������Ϊ����*/
	value=skip(value+1);
	if (*value==']') return value+1;	        /* �յ����� */

	item->child=child=CreateJSON();
	if (!item->child) return 0;		            /* �ڴ�����ʧ�� */
	value=skip(parse_value(child,skip(value)));	/* ���Կո�ֻȡֵ */
	if (!value) return 0;                       /*ֻ��һ���˳�*/

	while (*value==',')
	{
		JSON *new_item;
		if (!(new_item=CreateJSON())) return 0; 	/* �ڴ�����ʧ�� */
		child->next=new_item;new_item->prev=child;child=new_item;
		value=skip(parse_value(child,skip(value+1)));
		if (!value) return 0;	                    /* ����û��] */
	}

	if (*value==']') return value+1;	            /* ������� */
	tmp=value;return 0;	                  
}


/* ��object */ 
static const char *parse_object(JSON *item,const char *value)
{
	JSON *child;
	if (*value!='{')	{tmp=value;return 0;}	/* û�С�{������һ�� object! */

	item->type=JSON_OBJECT;
	value=skip(value+1);
	if (*value=='}') return value+1;	        /* �յ�object */

	item->child=child=CreateJSON();
	if (!item->child) return 0;
	value=skip(parse_string(child,skip(value)));
	if (!value) return 0;
	child->string=child->valuestring;child->valuestring=0;
	if (*value!=':') {tmp=value;return 0;}	        /* ��ʽ���� */
	value=skip(parse_value(child,skip(value+1)));	/* ���Կո�ֻȡֵ */
	if (!value) return 0;

	while (*value==',')
	{
		JSON *new_item;
		if (!(new_item=CreateJSON()))	return 0; /* �����ڴ�ʧ�� */
		child->next=new_item;new_item->prev=child;child=new_item;
		value=skip(parse_string(child,skip(value+1)));
		if (!value) return 0;
		child->string=child->valuestring;child->valuestring=0;
		if (*value!=':') {tmp=value;return 0;}	/* ��ʽ���� */
		value=skip(parse_value(child,skip(value+1)));	/* ���Կո�ֻȡֵ */
		if (!value) return 0;
	}

	if (*value=='}') return value+1;	/* object��β */
	tmp=value;return 0;
}

/* ������ */
static const char *parse_number(JSON *item,const char *num)
{
	double n=0,sign=1,scale=0;int subscale=0,signsubscale=1;

	if (*num=='-') sign=-1,num++;	/* ������ */
	if (*num=='0') num++;			/* ��Ԫ  */
	if (*num>='1' && *num<='9')	 do n=(n*10.0)+(*num++ -'0');while (*num>='0' && *num<='9');	/* Number? */
	if (*num=='.' && num[1]>='0' && num[1]<='9') {num++;		do	n=(n*10.0)+(*num++ -'0'),scale--; while (*num>='0' && *num<='9');}	/* Fractional part? */

	n=sign*n*pow(10.0,(scale+subscale*signsubscale));	/* number = +/- number.fraction * 10^+/- exponent */
	item->valuedouble=n;
	item->valueint=(int)n;
	item->type=JSON_NUMBER;
	return num;
}


static const char *parse_value(JSON *item,const char *value)
{
	if (!value)						return 0;	/* ���򱨴� */
	if (!strncmp(value,"null",4))	{ item->type=JSON_NULL;  return value+4; }
	if (!strncmp(value,"false",5))	{ item->type=JSON_FALSE; return value+5; }
	if (!strncmp(value,"true",4))	{ item->type=JSON_TRUE; item->valueint=1;	return value+4; }
	if (*value=='\"')				{ return parse_string(item,value); }
	if (*value=='-' || (*value>='0' && *value<='9'))	{ return parse_number(item,value); }
	if (*value=='[')				{ return parse_array(item,value); }
	if (*value=='{')				{ return parse_object(item,value); }

	tmp=value;return 0;	/* failure. */
}

/* ��char*ת����JSON* */
JSON *ParseJSON(const char *value)
{
	const char *end=0;
	JSON *c=CreateJSON();
	tmp=0;
	if (!c) return 0;       /* �����ڴ�ʧ�� */

	end=parse_value(c,skip(value));
	if (!end)	{DeleteJSON(c);return 0;}	/* ת��ʧ�� */
	return c;
}


/* ���ļ��ж�ȡchar*Ȼ��ת����JSON* */
JSON *ParseJSONFromFile(const char *file_name)
{
	JSON *json;
	FILE *f=fopen(file_name,"rb");fseek(f,0,SEEK_END);long len=ftell(f);fseek(f,0,SEEK_SET);
	char *data=(char*)malloc(len+1);fread(data,1,len,f);fclose(f);

	json=ParseJSON(data);
	if (!json) 
	{
		printf("Error");
		return NULL;
	}
	else
	{
		return json;
	}
}