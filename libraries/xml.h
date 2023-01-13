#ifndef XML_HM
#define XML_HM

#include <stdbool.h>

struct XML_Document;
struct XML_Node;

// Functions Definition
struct XML_Document *xml_load_document(char *filepath);
struct XML_Node *xml_parse(struct XML_Document *doc);
void xml_free_document(struct XML_Document *doc);
void xml_free_node(struct XML_Node *node, bool free_doc);

#endif