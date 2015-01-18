#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "JSON.h"
#include <conio.h>

#define eps 1e-10

//≤‚ ‘CreateNULL
void testNull()
{
	JSON *obj = CreateNULL();
	if (obj->type != JSON_NULL)
		printf("null wrong type\n");
	else
		printf("null right type\n");
	DeleteJSON(obj);
}
//≤‚ ‘CreateFalse
void testFalse()
{
	JSON *obj = CreateFalse();
	if (obj->type != JSON_FALSE)
		printf("false wrong type\n");
	else
		printf("false right type\n");

	if (obj->valueint != 0)
		printf("false wrong value\n");
	else
		printf("false right value\n");
	DeleteJSON(obj);
}
//≤‚ ‘CreateTrue
void testTrue()
{
	JSON *obj = CreateTrue();
	if (obj->type != JSON_TRUE)
		printf("true wrong type\n");
	else
		printf("true right type\n");

	if (obj->valueint != 1)
		printf("true wrong value\n");
	else
		printf("true right value\n");

	DeleteJSON(obj);
}
//≤‚ ‘CreateNumber
void testNumber()
{
	JSON *obj = CreateNumber(123.123);
	if (obj->type != JSON_NUMBER)
		printf("number wrong type\n");
	else
		printf("number right type\n");
	if (abs(obj->valuedouble - 123.123)>eps)
		printf("number wrong value\n");
	else
		printf("number right value\n");
	DeleteJSON(obj);
}
//≤‚ ‘CreateString
void testString()
{
	JSON *obj = CreateString("123");
	if (obj->type != JSON_STRING)
		printf("string wrong type\n");
	else
		printf("string right type\n");

	if (strcmp(obj->valuestring, "123") != 0)
		printf("string wrong value\n");
	else
		printf("string right value\n");
	DeleteJSON(obj);
}
//≤‚ ‘CreateArray
void testArray()
{
	JSON *obj = CreateArray();
	if (obj->type != JSON_ARRAY)
		printf("array wrong type\n");
	else
		printf("array right type\n");
	DeleteJSON(obj);
}
//≤‚ ‘CreateObject
void testObject()
{
	JSON *obj = CreateObject();
	if (obj->type != JSON_OBJECT)
		printf("object wrong type");
	else
		printf("object right type");
	DeleteJSON(obj);
}
//≤‚ ‘CreateBool
void testBool()
{
	JSON *obj = CreateBool(0);
	if (obj->type != JSON_FALSE)
		printf("Bool  wrong\n");
	else
		printf("Bool  right\n");
	PrintJSON(obj);
	DeleteJSON(obj);

	obj = CreateBool(1);
	if (obj->type != JSON_TRUE)
		printf("Bool  wrong\n");
	else
		printf("Bool  right\n");
	PrintJSON(obj);
	DeleteJSON(obj);
}

//≤‚ ‘AddItemToArray
void testAddItemToArray()
{
	JSON *obj = CreateArray();
	JSON *add0 = CreateFalse();
	JSON *add1 = CreateTrue();
	AddItemToArray(obj, add0);
	AddItemToArray(obj, add1);
	PrintJSON(obj);
}

//≤‚ ‘AddItemToObject
void testAddItemToObject()
{
	JSON *obj = CreateObject();
	AddItemToObject(obj, "test", CreateNumber(100));
	PrintJSON(obj);
}

//≤‚ ‘ReplaceItemInArray
void testReplaceItemInArray()
{
	JSON *obj = CreateArray();
	JSON *add0 = CreateFalse();
	JSON *add1 = CreateTrue();
	AddItemToArray(obj, add0);
	AddItemToArray(obj, add1);
	ReplaceItemInArray(obj, 0, CreateTrue());
	PrintJSON(obj);
}
//≤‚ ‘ReplaceItemInObject
void testReplaceItemInObject()
{
	JSON *obj = CreateObject();
	AddItemToObject(obj, "test", CreateNumber(100));
	ReplaceItemInObject(obj, "test", CreateNumber(99));
	PrintJSON(obj);
}

//≤‚ ‘DeleteItemInArray
void testDeleteItemInArray()
{
	JSON *obj = CreateArray();
	JSON *add0 = CreateFalse();
	JSON *add1 = CreateTrue();
	AddItemToArray(obj, add0);
	AddItemToArray(obj, add1);
	//obj = DetachItemFromArray(obj, 0);
	DeleteItemFromArray(obj, 0);
	PrintJSON(obj);
}

//≤‚ ‘Parse
void testParse()
{
	JSON *obj = ParseJSONFromFile("tests/test6");
	PrintJSON(obj);
	DeleteJSON(obj);
}

//≤‚ ‘GetItemInArray
void testGetItemInArray()
{
    JSON *obj = ParseJSONFromFile("test_array.json");
	JSON *get = GetItemInArray(obj, 1);
	JSON *get1 = GetItemInArray(obj, 0);
	PrintJSON(get);
	PrintJSON(obj);
	DeleteJSON(obj);
	DeleteJSON(get);
}

//≤‚ ‘GetItemInJSON
void testGetItemInJSON()
{
	JSON *obj = ParseJSONFromFile("tests/test1");
	char* path = "/glossary/GlossDiv/title";
	JSON *get = GetItemInJSON(obj, path);
	PrintJSON(get);
	//DeleteJSON(get);
	//DeleteJSON(obj);
}

//≤‚ ‘PrintJSONToFile
void testPrintJSONToFile()
{
	JSON *obj = ParseJSONFromFile("tests/test6");
	PrintJSON(obj);
	PrintJSONToFile(obj, "test");
	DeleteJSON(obj);
}

int main()
{
	//testNull();
	//testFalse();
	//testNumber();
	//testString();
	//testTrue();
	//testBool();
	//testGetItemInArray();
	//testGetItemInJSON();
	//testAddItemToArray();
	//testAddItemToObject();
	//testReplaceItemInArray();
	//testReplaceItemInObject();
	//testDeleteItemInArray();
	testPrintJSONToFile();
	_getch();
	return 0;
}
