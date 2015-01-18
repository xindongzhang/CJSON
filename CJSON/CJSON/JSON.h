#ifndef JSON_H
#define JSON_H

/* cJSON Types: */
#define JSON_FALSE 0
#define JSON_TRUE 1
#define JSON_NULL 2
#define JSON_NUMBER 3
#define JSON_STRING 4
#define JSON_ARRAY 5
#define JSON_OBJECT 6
#define JSON_IsUsed 256

#pragma warning(disable: 4996)

/* The cJSON structure: */
typedef struct JSON 
{
	// basic structure of the JSON
	struct JSON *next, *prev;
	struct JSON *child;

	int type;					/* The type of the item, as above. */

	char *valuestring;			/* The item's string, if type==JSON_STRING */
	int valueint;				/* The item's number, if type==JSON_TRUE||JSON_FALSE */
	double valuedouble;			/* The item's number, if type==JSON_NUMBER  */

	char *string;				/* The item's name string */

} JSON;


/* Internal constructor. */
extern JSON *CreateJSON(void);

/* Parse & Print */
extern JSON *ParseJSON(const char *value);
extern JSON *ParseJSONFromFile(const char *file_name);

extern char *JSON_Print(JSON *item);
extern void PrintJSON(JSON *item);
extern void PrintJSONToFile(JSON *item, const char *file_name);

/* Create */
extern JSON *CreateNULL(void);
extern JSON *CreateTrue(void);
extern JSON *CreateFalse(void);
extern JSON *CreateBool(int b);
extern JSON *CreateNumber(double num);
extern JSON *CreateString(const char *string);
extern JSON *CreateArray(void);
extern JSON *CreateObject(void);

/* Append */
extern void AddItemToArray(JSON *array, JSON *item);
extern void AddItemToObject(JSON *object, const char *key, JSON *value);

/* Update */
extern void ReplaceItemInArray(JSON *array, int which, JSON *new_item);
extern void ReplaceItemInObject(JSON *object, const char *key, JSON *new_value);

/* Remove/Delete */
extern JSON *DetachItemFromArray(JSON *array, int which);
extern void *DeleteItemFromArray(JSON *array, int which);
extern JSON *DetachItemFromObject(JSON *object, const char *key);
extern void *DeleteItemFromObject(JSON *object, const char *key);

extern void DeleteJSON(JSON *item);

/* Duplicate */
extern JSON *Duplicate(JSON *item, int recurse);

/* Read */
extern JSON *GetItemInArray(JSON *array, int which);
extern JSON *GetItemInObject(JSON *object, const char *key);
extern JSON *GetItemInJSON(JSON *json, const char *path);


/* other function */
/* for the convenience of parsing*/
static const char *tmp;

/* for parsing */
static const char *parse_string(JSON *item,const char *str);
static const char *parse_value(JSON *item,const char *value);
static const char *parse_array(JSON *item,const char *value);
static const char *parse_object(JSON *item,const char *value);

/* for printing */
char*  StrDup(const char* str);
static char *print_value(JSON *item);
static char *print_array(JSON *item);
static char *print_object(JSON *item);
static char *print_string(JSON *item);

#endif