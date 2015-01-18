#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <ctype.h>
#include "JSON.h"


/*复制字符串*/
char* StrDup(const char* str)
{
	size_t len;
	char* copy;
	len = strlen(str) + 1;
	if (!(copy = (char*)malloc(len))) return 0;
	memcpy(copy,str,len);
	return copy;
}


/*打印数字*/
static char *print_number(JSON *item)
{
	char *str;
	double d=item->valuedouble;
	if (fabs(((double)item->valueint)-d)<=DBL_EPSILON && d<=INT_MAX && d>=INT_MIN)
	{
		str=(char*)malloc(21);	/* 2^64+1 can be represented in 21 chars. */
		if (str) sprintf(str,"%d",item->valueint);
	}
	else
	{
		str=(char*)malloc(64);	/* This is a nice trade-off. */
		if (str)
		{
			if (fabs(floor(d)-d)<=DBL_EPSILON && fabs(d)<1.0e60)sprintf(str,"%.0f",d);
			else if (fabs(d)<1.0e-6 || fabs(d)>1.0e9)			sprintf(str,"%e",d);
			else												sprintf(str,"%f",d);
		}
	}
	return str;
}


/* 打印字符串*/
static char *print_string_ptr(const char *str)
{
	const char *ptr;char *ptr2,*out;int len=0;unsigned char token;

	if (!str) return StrDup("");
	ptr=str;while ((token=*ptr) && ++len) {if (strchr("\"\\\b\f\n\r\t",token)) len++; else if (token<32) len+=5;ptr++;}

	out=(char*)malloc(len+3);
	if (!out) return 0;

	ptr2=out;ptr=str;
	*ptr2++='\"';
	while (*ptr)
	{
		if ((unsigned char)*ptr>31 && *ptr!='\"' && *ptr!='\\') *ptr2++=*ptr++;
		else
		{
			*ptr2++='\\';
			switch (token=*ptr++)
			{
			case '\\':	*ptr2++='\\';	break;
			case '\"':	*ptr2++='\"';	break;
			case '\b':	*ptr2++='b';	break;
			case '\f':	*ptr2++='f';	break;
			case '\n':	*ptr2++='n';	break;
			case '\r':	*ptr2++='r';	break;
			case '\t':	*ptr2++='t';	break;
			default: sprintf(ptr2,"u%04x",token);ptr2+=5;	break;	/* escape and print */
			}
		}
	}
	*ptr2++='\"';*ptr2++=0;
	return out;
}

/* 打印字符串 */
static char *print_string(JSON *item)	{return print_string_ptr(item->valuestring);}

/* 打印数组 */
static char *print_array(JSON *item)
{
	char **entries;
	char *out=0,*ptr,*ret;int len=5;
	JSON *child=item->child;
	int numentries=0,i=0,fail=0;

	/* 元素个数 */
	while (child) numentries++,child=child->next;
	/*当元素个数为0*/
	if (!numentries)
	{
		out=(char*)malloc(3);
		if (out) strcpy(out,"[]");
		return out;
	}
	/* 申请内存来储存元素 */
	entries=(char**)malloc(numentries*sizeof(char*));
	if (!entries) return 0;
	memset(entries,0,numentries*sizeof(char*));
	/* 遍历所有元素 */
	child=item->child;
	while (child && !fail)
	{
		ret=print_value(child);
		entries[i++]=ret;
		if (ret) len+=strlen(ret)+2; else fail=1;
		child=child->next;
	}

	/*申请内存*/
	if (!fail) out=(char*)malloc(len);
	/* 申请内存失败*/
	if (!out) fail=1;
	if (fail)
	{
		for (i=0;i<numentries;i++) if (entries[i]) free(entries[i]);
		free(entries);
		return 0;
	}

	/* 格式化输出*/
	*out='[';
	ptr=out+1;*ptr=0;
	for (i=0;i<numentries;i++)
	{
		strcpy(ptr,entries[i]);ptr+=strlen(entries[i]);
		if (i!=numentries-1) {*ptr++=',';*ptr=0;}
		free(entries[i]);
	}
	free(entries);
	*ptr++=']';*ptr++=0;
	return out;	
}

/* 打印object */
static char *print_object(JSON *item)
{
	char **entries=0,**names=0;
	char *out=0,*ptr,*ret,*str;int len=7,i=0;
	JSON *child=item->child;
	int numentries=0,fail=0;
	/* 计算元素的格式 */
	while (child) numentries++,child=child->next;
	/* 当元素非空的时候进行下面的操作 */
	if (!numentries)
	{
		out=(char*)malloc(3);
		if (!out)	return 0;
		ptr=out;*ptr++='{';
		*ptr++='}';*ptr++=0;
		return out;
	}
	/* 为names和objects分配空间 */
	entries=(char**)malloc(numentries*sizeof(char*));
	if (!entries) return 0;
	names=(char**)malloc(numentries*sizeof(char*));
	if (!names) {free(entries);return 0;}
	memset(entries,0,sizeof(char*)*numentries);
	memset(names,0,sizeof(char*)*numentries);

	/* 将每个元素的names和value进行转化 */
	child=item->child;
	while (child)
	{
		names[i]=str=print_string_ptr(child->string);
		entries[i++]=ret=print_value(child);
		if (str && ret) len+=strlen(ret)+strlen(str)+2; else fail=1;
		child=child->next;
	}

	/* 为输出分配内存 */
	if (!fail) out=(char*)malloc(len);
	if (!out) fail=1;

	/* 格式化输出 */
	*out='{';ptr=out+1;
	*ptr=0;
	for (i=0;i<numentries;i++)
	{
		strcpy(ptr,names[i]);ptr+=strlen(names[i]);*ptr++=':';
		strcpy(ptr,entries[i]);ptr+=strlen(entries[i]);
		if (i!=numentries-1) *ptr++=',';*ptr=0;
		free(names[i]);free(entries[i]);
	}

	free(names);free(entries);
	*ptr++='}';*ptr++=0;
	return out;	
}



/* 打印value */
static char *print_value(JSON *item)
{
	char *out=0;
	if (!item) return 0;
	if (item->type == JSON_NULL)   out=StrDup("null");
	if (item->type == JSON_FALSE)  out=StrDup("false");
	if (item->type == JSON_TRUE)   out=StrDup("true");
	if (item->type == JSON_NUMBER) out=print_number(item); 
	if (item->type == JSON_STRING) out=print_string(item);  
	if (item->type == JSON_ARRAY)  out=print_array(item);  
	if (item->type == JSON_OBJECT) out=print_object(item);
	return out;
}



/* 将JSON成员无格式打印出来 */
void PrintJSON(JSON *item)
{
	char *out;
	if (!item) 
	{
		printf("Error");
	}
	else
	{
		out=print_value(item);
		printf("%s\n",out);
		free(out);
	}
}


/*将JSON成员格式化输出到文件中*/
void PrintJSONToFile(JSON *item, const char *file_name)
{
	FILE *fp;
	if((fp=fopen(file_name,"w"))==NULL)  
	{
		printf("cannot open file!\n");
		exit(0);
	}
	char *out=print_value(item);
	/*牺牲空间换时间*/
	char *result = (char*)malloc(strlen(out)*strlen(out));
	int depth = 4;
	int indent = 1;
	int i;

	*result = '{';
	char *ptr = result + 1;*ptr++ = '\n'; 
	for (i = 0; i < indent*depth;++i){*ptr++=' ';}
	out++;
	for (size_t j = 0; j < strlen(out); ++j)
	{
		if (out[j] == '{' || out[j] == '[')
		{
			++indent;
			out[j] == '{'? *ptr++ = '{': *ptr++ = '[';
			*ptr++ = '\n';
			for (i = 0; i < indent*depth;++i){*ptr++=' ';}
			continue;
		}
		if (out[j] == ',')
		{
			*ptr++ = ',';
			*ptr++ = '\n';
			for (i = 0; i < indent*depth;++i){*ptr++=' ';}
			continue;
		}

		if (out[j] == '}' || out[j] == ']')
		{
			--indent;*ptr++ = '\n';
			for (i = 0; i < indent*depth;++i){*ptr++=' ';}
			out[j] == '}'? *ptr++ = '}': *ptr++ = ']';
			continue;
		}
		if (out[j] == ':')
		{
			*ptr++ = ':';
			*ptr++ = ' ';
			continue;
		}
		*ptr++ = out[j];
	}
	*ptr++ = 0;
	fprintf( fp, "%s\n", result);
	fclose(fp);
}

